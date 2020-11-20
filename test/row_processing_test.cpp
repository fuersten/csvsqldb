//
//  csvsqldb test
//
//  BSD 3-Clause License
//  Copyright (c) 2015-2020 Lars-Christian Fürstenberg
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


#include "csvsqldb/operatornode.h"
#include "csvsqldb/sql_astdump.h"
#include "csvsqldb/sql_parser.h"

#include "temporary_directory.h"

#include <catch2/catch.hpp>

#include <sstream>


namespace
{
  class DummyScanOperatorNode
  : public csvsqldb::ScanOperatorNode
  , public csvsqldb::BlockProvider
  {
  public:
    DummyScanOperatorNode(const csvsqldb::OperatorContext& context, const csvsqldb::SymbolTablePtr& symbolTable,
                          const csvsqldb::SymbolInfo& tableInfo)
    : csvsqldb::ScanOperatorNode(context, symbolTable, tableInfo)
    , _block(nullptr)
    {
    }

    const csvsqldb::Values* getNextRow()
    {
      if (!_block) {
        prepareBuffer();
      }

      return _iterator->getNextRow();
    }

    virtual csvsqldb::BlockPtr getNextBlock()
    {
      return _block;
    }

    virtual void dump(std::ostream& stream) const
    {
      stream << "DummyScanOperator\n";
    }

  private:
    void prepareBuffer()
    {
      _block = getBlockManager().createBlock();

      if (_tableInfo._identifier == "EMPLOYEES") {
        _block->addValue(csvsqldb::Variant(4711));
        _block->addValue(csvsqldb::Variant(csvsqldb::Date(1970, csvsqldb::Date::September, 23)));
        _block->addValue(csvsqldb::Variant("Lars"));
        _block->addValue(csvsqldb::Variant("Fürstenberg"));
        _block->addValue(csvsqldb::Variant("M"));
        _block->addValue(csvsqldb::Variant(csvsqldb::Date(2012, csvsqldb::Date::February, 1)));
        _block->nextRow();
        _block->addValue(csvsqldb::Variant(815));
        _block->addValue(csvsqldb::Variant(csvsqldb::Date(1969, csvsqldb::Date::May, 17)));
        _block->addValue(csvsqldb::Variant("Mark"));
        _block->addValue(csvsqldb::Variant("Fürstenberg"));
        _block->addValue(csvsqldb::Variant("M"));
        _block->addValue(csvsqldb::Variant(csvsqldb::Date(2003, csvsqldb::Date::April, 15)));
        _block->nextRow();
        _block->addValue(csvsqldb::Variant(9227));
        _block->addValue(csvsqldb::Variant(csvsqldb::Date(1963, csvsqldb::Date::March, 6)));
        _block->addValue(csvsqldb::Variant("Angelica"));
        _block->addValue(csvsqldb::Variant("Tello de Fürstenberg"));
        _block->addValue(csvsqldb::Variant("F"));
        _block->addValue(csvsqldb::Variant(csvsqldb::Date(2003, csvsqldb::Date::June, 15)));
        _block->nextRow();
      } else if (_tableInfo._identifier == "SALARIES") {
        _block->addValue(csvsqldb::Variant(4711));
        _block->addValue(csvsqldb::Variant(12000.0));
        _block->addValue(csvsqldb::Variant(csvsqldb::Date(2012, csvsqldb::Date::February, 1)));
        _block->addValue(csvsqldb::Variant(csvsqldb::Date(2015, csvsqldb::Date::February, 1)));
        _block->nextRow();
        _block->addValue(csvsqldb::Variant(9227));
        _block->addValue(csvsqldb::Variant(450.0));
        _block->addValue(csvsqldb::Variant(csvsqldb::Date(2003, csvsqldb::Date::June, 15)));
        _block->addValue(csvsqldb::Variant(csvsqldb::Date(2015, csvsqldb::Date::February, 1)));
        _block->nextRow();
        _block->addValue(csvsqldb::Variant(815));
        _block->addValue(csvsqldb::Variant(6000.0));
        _block->addValue(csvsqldb::Variant(csvsqldb::Date(2003, csvsqldb::Date::April, 15)));
        _block->addValue(csvsqldb::Variant(csvsqldb::Date(2015, csvsqldb::Date::February, 1)));
        _block->nextRow();
      }
      _block->endBlocks();

      _iterator = std::make_shared<csvsqldb::BlockIterator>(_types, *this, getBlockManager());
    }

    csvsqldb::BlockPtr _block;
    csvsqldb::BlockIteratorPtr _iterator;
  };
}


TEST_CASE("Row Processing Test", "[engine]")
{
  csvsqldb::FunctionRegistry functions;
  csvsqldb::SQLParser parser(functions);
  TemporaryDirectoryGuard tmpDir;
  auto path = tmpDir.temporaryDirectoryPath();

  SECTION("processing")
  {
    csvsqldb::ASTNodePtr node = parser.parse(
      "CREATE TABLE employees(emp_no INTEGER,birth_date DATE NOT NULL,first_name VARCHAR(25) NOT NULL,last_name VARCHAR(50) "
      "NOT NULL,gender CHAR,hire_date DATE,PRIMARY KEY(emp_no))");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node));
    csvsqldb::ASTCreateTableNodePtr createNode = std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node);

    csvsqldb::TableData tabledata = csvsqldb::TableData::fromCreateAST(createNode);
    csvsqldb::FileMapping mapping;

    csvsqldb::Database database(path, mapping);
    database.addTable(std::move(tabledata), false);

    node = parser.parse(
      "SELECT emp.emp_no as id,(first_name || ' ' || last_name) as name,birth_date birthday, 7 * 5 / 4 as calc FROM employees "
      "emp WHERE \"emp_no\" BETWEEN 100 AND 9999 AND emp.birth_date > DATE'1960-01-01'");

    //        ASTNodeDumpVisitor visitor;
    //        std::cout << std::endl;
    //        node->accept(visitor);

    csvsqldb::SymbolTablePtr symbolTable = node->symbolTable();
    //        symbolTable->dump();
    //        std::cout << std::endl;

    symbolTable->typeSymbolTable(database);

    //        symbolTable->dump();
    //        std::cout << std::endl;

    csvsqldb::ASTQueryNodePtr query = std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node);
    csvsqldb::ASTExprNodePtr exp =
      std::dynamic_pointer_cast<csvsqldb::ASTQuerySpecificationNode>(query->_query)->_tableExpression->_where->_exp;
    csvsqldb::Expressions& selectList = std::dynamic_pointer_cast<csvsqldb::ASTQuerySpecificationNode>(query->_query)->_nodes;
    csvsqldb::SymbolInfoPtr tableInfo = std::dynamic_pointer_cast<csvsqldb::ASTTableIdentifierNode>(
                                          std::dynamic_pointer_cast<csvsqldb::ASTQuerySpecificationNode>(query->_query)
                                            ->_tableExpression->_from->_tableReferences[0])
                                          ->_factor->_info;

    csvsqldb::BlockManager blockManager;
    csvsqldb::StringVector files;
    csvsqldb::OperatorContext context(database, functions, blockManager, files);

    std::stringstream ss;
    csvsqldb::RootOperatorNodePtr output = std::make_shared<csvsqldb::OutputRowOperatorNode>(context, symbolTable, ss);
    csvsqldb::RowOperatorNodePtr projection =
      std::make_shared<csvsqldb::ExtendedProjectionOperatorNode>(context, symbolTable, selectList);
    csvsqldb::RowOperatorNodePtr select = std::make_shared<csvsqldb::SelectOperatorNode>(context, symbolTable, exp);
    csvsqldb::RowOperatorNodePtr scan = std::make_shared<DummyScanOperatorNode>(context, symbolTable, *tableInfo);

    select->connect(scan);
    projection->connect(select);
    output->connect(projection);

    CHECK(3 == output->process());
    CHECK(
      "#ID,NAME,BIRTHDAY,CALC\n4711,'Lars Fürstenberg',1970-09-23,8\n815,'Mark Fürstenberg',1969-05-17,8\n9227,'Angelica Tello "
      "de Fürstenberg',1963-03-06,8\n" == ss.str());
  }

  SECTION("processing asterisk")
  {
    csvsqldb::ASTNodePtr node = parser.parse(
      "CREATE TABLE employees(emp_no INTEGER,birth_date DATE NOT NULL,first_name VARCHAR(25) NOT NULL,last_name VARCHAR(50) "
      "NOT NULL,gender CHAR,hire_date DATE,PRIMARY KEY(emp_no))");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node));
    csvsqldb::ASTCreateTableNodePtr createNode = std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node);

    csvsqldb::TableData tabledata = csvsqldb::TableData::fromCreateAST(createNode);
    csvsqldb::FileMapping mapping;

    csvsqldb::Database database(path, mapping);
    database.addTable(std::move(tabledata), false);

    node = parser.parse("SELECT * FROM employees emp WHERE emp_no BETWEEN 100 AND 9999 AND emp.birth_date > DATE'1960-01-01'");

    csvsqldb::SymbolTablePtr symbolTable = node->symbolTable();
    //        symbolTable->dump();
    //        std::cout << std::endl;

    symbolTable->typeSymbolTable(database);

    //        symbolTable->dump();
    //        std::cout << std::endl;

    csvsqldb::ASTQueryNodePtr query = std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node);
    csvsqldb::ASTExprNodePtr exp =
      std::dynamic_pointer_cast<csvsqldb::ASTQuerySpecificationNode>(query->_query)->_tableExpression->_where->_exp;
    csvsqldb::Expressions& selectList = std::dynamic_pointer_cast<csvsqldb::ASTQuerySpecificationNode>(query->_query)->_nodes;
    csvsqldb::SymbolInfoPtr tableInfo = std::dynamic_pointer_cast<csvsqldb::ASTTableIdentifierNode>(
                                          std::dynamic_pointer_cast<csvsqldb::ASTQuerySpecificationNode>(query->_query)
                                            ->_tableExpression->_from->_tableReferences[0])
                                          ->_factor->_info;

    csvsqldb::BlockManager blockManager;
    csvsqldb::StringVector files;
    csvsqldb::OperatorContext context(database, functions, blockManager, files);

    std::stringstream ss;
    csvsqldb::RootOperatorNodePtr output = std::make_shared<csvsqldb::OutputRowOperatorNode>(context, symbolTable, ss);
    csvsqldb::RowOperatorNodePtr projection =
      std::make_shared<csvsqldb::ExtendedProjectionOperatorNode>(context, symbolTable, selectList);
    csvsqldb::RowOperatorNodePtr select = std::make_shared<csvsqldb::SelectOperatorNode>(context, symbolTable, exp);
    csvsqldb::RowOperatorNodePtr scan = std::make_shared<DummyScanOperatorNode>(context, symbolTable, *tableInfo);

    select->connect(scan);
    projection->connect(select);
    output->connect(projection);

    CHECK(3 == output->process());

    std::string expected = R"(#EMP_NO,EMP.BIRTH_DATE,EMP.FIRST_NAME,EMP.LAST_NAME,EMP.GENDER,EMP.HIRE_DATE
4711,1970-09-23,'Lars','Fürstenberg','M',2012-02-01
815,1969-05-17,'Mark','Fürstenberg','M',2003-04-15
9227,1963-03-06,'Angelica','Tello de Fürstenberg','F',2003-06-15
)";
    CHECK(expected == ss.str());
  }

  SECTION("processing aggregation")
  {
    csvsqldb::ASTNodePtr node = parser.parse(
      "CREATE TABLE employees(emp_no INTEGER,birth_date DATE NOT NULL,first_name VARCHAR(25) NOT NULL,last_name VARCHAR(50) "
      "NOT NULL,gender CHAR,hire_date DATE,PRIMARY KEY(emp_no))");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node));
    csvsqldb::ASTCreateTableNodePtr createNode = std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node);

    csvsqldb::TableData tabledata = csvsqldb::TableData::fromCreateAST(createNode);
    csvsqldb::FileMapping mapping;

    csvsqldb::Database database(path, mapping);
    database.addTable(std::move(tabledata), false);

    node = parser.parse("SELECT count(*) FROM employees");

    //        csvsqldb::ASTNodeDumpVisitor visitor;
    //        std::cout << std::endl;
    //        node->accept(visitor);

    csvsqldb::SymbolTablePtr symbolTable = node->symbolTable();
    symbolTable->typeSymbolTable(database);

    //        symbolTable->dump();
    //        std::cout << std::endl;

    csvsqldb::ASTQueryNodePtr query = std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node);
    csvsqldb::Expressions& selectList = std::dynamic_pointer_cast<csvsqldb::ASTQuerySpecificationNode>(query->_query)->_nodes;
    csvsqldb::SymbolInfoPtr tableInfo = std::dynamic_pointer_cast<csvsqldb::ASTTableIdentifierNode>(
                                          std::dynamic_pointer_cast<csvsqldb::ASTQuerySpecificationNode>(query->_query)
                                            ->_tableExpression->_from->_tableReferences[0])
                                          ->_factor->_info;

    csvsqldb::BlockManager manager;
    csvsqldb::StringVector files;
    csvsqldb::OperatorContext context(database, functions, manager, files);

    std::stringstream ss;
    csvsqldb::RootOperatorNodePtr output = std::make_shared<csvsqldb::OutputRowOperatorNode>(context, symbolTable, ss);
    csvsqldb::RowOperatorNodePtr projection =
      std::make_shared<csvsqldb::AggregationOperatorNode>(context, symbolTable, selectList);
    csvsqldb::RowOperatorNodePtr scan = std::make_shared<DummyScanOperatorNode>(context, symbolTable, *tableInfo);

    projection->connect(scan);
    output->connect(projection);

    CHECK(1 == output->process());
    CHECK("#$alias_1\n3\n" == ss.str());
  }
}
