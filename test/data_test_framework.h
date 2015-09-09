//
//  csvsqldb test
//
//  BSD 3-Clause License
//  Copyright (c) 2015, Lars-Christian Fürstenberg
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted
//  provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of 
//  conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of 
//  conditions and the following disclaimer in the documentation and/or other materials provided 
//  with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors may be used to 
//  endorse or promote products derived from this software without specific prior written 
//  permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
//  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
//  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
//  POSSIBILITY OF SUCH DAMAGE.
//

#ifndef csvsqldb_data_test_framework_h
#define csvsqldb_data_test_framework_h

#include "libcsvsqldb/execution_engine.h"

#include "libcsvsqldb/base/string_helper.h"

#include <initializer_list>


struct TableElement
{
    const std::string _name;
    csvsqldb::eType _type;
};


class TableInitializer
{
public:
    TableInitializer(const std::string& table, std::initializer_list<TableElement> elements)
    : _table(csvsqldb::toupper_copy(table))
    {
        for(const auto& element : elements) {
            _table.addColumn(csvsqldb::toupper_copy(element._name), element._type, false, false, false, csvsqldb::Any(), nullptr, 0);
        }
    }
    
    const csvsqldb::TableData& getTable() const
    {
        return _table;
    }
    
private:
    csvsqldb::TableData _table;
};


class DatabaseTestWrapper
{
public:
    DatabaseTestWrapper()
    : _database("/tmp", csvsqldb::FileMapping())
    {}
    
    void addTable(TableInitializer table)
    {
        _database.addTable(table.getTable());
    }
    
    csvsqldb::Database& getDatabase()
    {
        return _database;
    }
    
private:
    csvsqldb::Database _database;
};


template<typename TestRowProvider>
class TestScanOperatorNode : public csvsqldb::ScanOperatorNode, public csvsqldb::BlockProvider
{
public:
    TestScanOperatorNode(const csvsqldb::OperatorContext& context, const csvsqldb::SymbolTablePtr& symbolTable, const csvsqldb::SymbolInfo& tableInfo)
    : csvsqldb::ScanOperatorNode(context, symbolTable, tableInfo)
    , _block(nullptr)
    {}
    
    const csvsqldb::Values* getNextRow()
    {
        if(!_block) {
            prepareBuffer();
        }
        
        return _iterator->getNextRow();
    }
    
    virtual csvsqldb::BlockPtr getNextBlock()
    {
        return _block;
    }
    
    void addRow(std::initializer_list<csvsqldb::Variant> values)
    {
        for(const auto& value : values) {
            _block->addValue(value);
        }
        _block->nextRow();
    }
    
    virtual void dump(std::ostream& stream) const
    {
        stream << "TestScanOperatorNode\n";
    }
    
private:
    void addRows()
    {
        TestRowProvider::addRows(_tableInfo._identifier, *this);
    }
    
    void prepareBuffer()
    {
        _block = getBlockManager().createBlock();
        
        addRows();
        
        _block->endBlocks();
        _iterator = std::make_shared<csvsqldb::BlockIterator>(_types, *this, getBlockManager());
    }
    
    csvsqldb::BlockPtr _block;
    csvsqldb::BlockIteratorPtr _iterator;
};


struct TestRowProvider
{
    typedef std::initializer_list<csvsqldb::Variant> Row;
    typedef std::vector<Row> Rows;
    typedef std::map<std::string, Rows> TableRows;
    
    
    static void setRows(const std::string& tableName, Rows rows)
    {
        getRows(csvsqldb::toupper_copy(tableName)) = rows;
    }
    
    static void addRows(const std::string& tableName, TestScanOperatorNode<TestRowProvider>& scanOperator)
    {
        for(const auto& row : getRows(csvsqldb::toupper_copy(tableName))) {
            scanOperator.addRow(row);
        }
    }
    
    static Rows& getRows(const std::string& tableName)
    {
        static TableRows s_rows;
        return s_rows[csvsqldb::toupper_copy(tableName)];
    }
};


class TestOperatorNodeFactory
{
public:
    
    static csvsqldb::RootOperatorNodePtr createOutputRowOperatorNode(csvsqldb::OperatorContext& context, const csvsqldb::SymbolTablePtr& symbolTable,
                                                                     std::ostream& stream)
    {
        return std::make_shared<csvsqldb::OutputRowOperatorNode>(context, symbolTable, stream);
    }
    
    static csvsqldb::RowOperatorNodePtr createLimitOperatorNode(csvsqldb::OperatorContext& context, const csvsqldb::SymbolTablePtr& symbolTable,
                                                                const csvsqldb::ASTExprNodePtr& limit, const csvsqldb::ASTExprNodePtr& offset) {
        return std::make_shared<csvsqldb::LimitOperatorNode>(context, symbolTable, limit, offset);
    }
    
    static csvsqldb::RowOperatorNodePtr createSortOperatorNode(csvsqldb::OperatorContext& context, const csvsqldb::SymbolTablePtr& symbolTable,
                                                               csvsqldb::OrderExpressions orderExpressions)
    {
        return std::make_shared<csvsqldb::SortOperatorNode>(context, symbolTable, orderExpressions);
    }
    
    static csvsqldb::RowOperatorNodePtr createGroupingOperatorNode(csvsqldb::OperatorContext& context, const csvsqldb::SymbolTablePtr& symbolTable,
                                                                   const csvsqldb::Expressions& nodes, const csvsqldb::Identifiers& groupByIdentifiers)
    {
        return std::make_shared<csvsqldb::GroupingOperatorNode>(context, symbolTable, nodes, groupByIdentifiers);
    }
    
    static csvsqldb::RowOperatorNodePtr createAggregationOperatorNode(csvsqldb::OperatorContext& context, const csvsqldb::SymbolTablePtr& symbolTable,
                                                                      const csvsqldb::Expressions& nodes)
    {
        return std::make_shared<csvsqldb::AggregationOperatorNode>(context, symbolTable, nodes);
    }
    
    static csvsqldb::RowOperatorNodePtr createExtendedProjectionOperatorNode(csvsqldb::OperatorContext& context,
                                                                             const csvsqldb::SymbolTablePtr& symbolTable,
                                                                             const csvsqldb::Expressions& nodes)
    {
        return std::make_shared<csvsqldb::ExtendedProjectionOperatorNode>(context, symbolTable, nodes);
    }
    
    static csvsqldb::RowOperatorNodePtr createCrossJoinOperatorNode(csvsqldb::OperatorContext& context, const csvsqldb::SymbolTablePtr& symbolTable)
    {
        return std::make_shared<csvsqldb::CrossJoinOperatorNode>(context, symbolTable);
    }

    static csvsqldb::RowOperatorNodePtr createInnerJoinOperatorNode(csvsqldb::OperatorContext& context, const csvsqldb::SymbolTablePtr& symbolTable,
                                                                    const csvsqldb::ASTExprNodePtr& exp)
    {
        return std::make_shared<csvsqldb::InnerJoinOperatorNode>(context, symbolTable, exp);
    }

    static csvsqldb::RowOperatorNodePtr createInnerHashJoinOperatorNode(csvsqldb::OperatorContext& context, const csvsqldb::SymbolTablePtr& symbolTable,
                                                                        const csvsqldb::ASTExprNodePtr& exp)
    {
        return std::make_shared<csvsqldb::InnerHashJoinOperatorNode>(context, symbolTable, exp);
    }
    
    static csvsqldb::RowOperatorNodePtr createUnionOperatorNode(csvsqldb::OperatorContext& context, const csvsqldb::SymbolTablePtr& symbolTable)
    {
        return std::make_shared<csvsqldb::UnionOperatorNode>(context, symbolTable);
    }
    
    static csvsqldb::RowOperatorNodePtr createSelectOperatorNode(csvsqldb::OperatorContext& context, const csvsqldb::SymbolTablePtr& symbolTable,
                                                                 const csvsqldb::ASTExprNodePtr& exp)
    {
        return std::make_shared<csvsqldb::SelectOperatorNode>(context, symbolTable, exp);
    }
    
    static csvsqldb::RowOperatorNodePtr createScanOperatorNode(csvsqldb::OperatorContext& context, const csvsqldb::SymbolTablePtr& symbolTable,
                                                               const csvsqldb::SymbolInfo& tableInfo)
    {
        return std::make_shared<TestScanOperatorNode<TestRowProvider>>(context, symbolTable, tableInfo);
    }
};

#endif
