//
//  operatornode.cpp
//  csvsqldb
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

#include "operatornode.h"
#include "sql_astexpressionvisitor.h"

#include "compat/regex.h"

#include <fstream>


namespace csvsqldb
{
    
    OutputRowOperatorNode::OutputRowOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, std::ostream& stream)
    : RootOperatorNode(context, symbolTable)
    , _stream(stream)
    , _firstCall(true)
    {}
    
    int64_t OutputRowOperatorNode::process()
    {
        if(_firstCall && _context._showHeaderLine) {
            _outputBuffer << "#";
            
            SymbolInfos infos;
            _input->getColumnInfos(infos);
            
            bool firstName(true);
            for(const auto& info : infos) {
                if(!firstName) {
                    _outputBuffer << ",";
                } else {
                    firstName = false;
                }
                
                _outputBuffer << info->_name;
            }
            _outputBuffer << "\n";
            _firstCall = false;
        }
        int64_t count = 0;
        const Values* row = nullptr;
        while( (row = _input->getNextRow()) ) {
            bool first = true;
            for(const auto value : *row) {
                if(!first) {
                    _outputBuffer << ",";
                } else {
                    first = false;
                }
                
                if(value->getType() == STRING && !value->isNull()) {
                    _outputBuffer << "'";
                }
                value->toStream(_outputBuffer);
                if(value->getType() == STRING && !value->isNull()) {
                    _outputBuffer << "'";
                }
            }
            ++count;
            _outputBuffer << "\n";
            if(count % 1000 == 0) {
                _stream << _outputBuffer.str();
                _outputBuffer.str(std::string());
            }
        }
        _stream << _outputBuffer.str();
        return count;
    }
    
    bool OutputRowOperatorNode::connect(const RowOperatorNodePtr& input)
    {
        _input = input;
        return true;
    }
    
    void OutputRowOperatorNode::getColumnInfos(SymbolInfos& outputSymbols)
    {
    }
    
    void OutputRowOperatorNode::dump(std::ostream& stream) const
    {
        stream << "OutputRowOperator (";
        SymbolInfos outputSymbols;
        _input->getColumnInfos(outputSymbols);
        bool first(true);
        for(const auto& entry : outputSymbols) {
            if(first) {
                first = false;
            } else {
                stream << ",";
            }
            stream << entry->_name;
        }
        stream << ")\n-->";
        _input->dump(stream);
    }
    
    
    LimitOperatorNode::LimitOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const ASTExprNodePtr& limit,
                                         const ASTExprNodePtr& offset)
    : RowOperatorNode(context, symbolTable)
    , _limit(-1)
    , _offset(0)
    {
        {
            StackMachine sm;
            VariableStore store;
            StackMachine::VariableMapping mapping;
            
            ASTInstructionStackVisitor visitor(sm, mapping);
            limit->accept(visitor);
            // as we test from 1 to the limit we have to add one here
            _limit = sm.evaluate(store, _context._functions).asInt() + 1;
        }
        if(offset) {
            StackMachine sm;
            VariableStore store;
            StackMachine::VariableMapping mapping;
            
            ASTInstructionStackVisitor visitor(sm, mapping);
            offset->accept(visitor);
            _offset = sm.evaluate(store, _context._functions).asInt();
        }
    }
    
    const Values* LimitOperatorNode::getNextRow()
    {
        if(_offset) {
            const Values* row = _input->getNextRow();
            while(row && --_offset) {
                row = _input->getNextRow();
            }
            if(!row) {
                return nullptr;
            }
        }
        if(_limit > 0 && --_limit == 0) {
            return nullptr;
        }
        return _input->getNextRow();
    }
    
    bool LimitOperatorNode::connect(const RowOperatorNodePtr& input)
    {
        _input = input;
        return true;
    }
    
    void LimitOperatorNode::getColumnInfos(SymbolInfos& outputSymbols)
    {
        remapOutputSymbols(outputSymbols);
        _input->getColumnInfos(outputSymbols);
    }
    
    void LimitOperatorNode::dump(std::ostream& stream) const
    {
        stream << "LimitOperator (";
        stream << _offset << " -> " << (_limit - 1);
        stream << ")\n-->";
        _input->dump(stream);
    }
    
    
    SortOperatorNode::SortOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, OrderExpressions orderExpressions)
    : RowOperatorNode(context, symbolTable)
    , _orderExpressions(orderExpressions)
    {
    }
    
    const Values* SortOperatorNode::getNextRow()
    {
        return _iterator->getNextRow();
    }
    
    bool SortOperatorNode::connect(const RowOperatorNodePtr& input)
    {
        _input = input;
        _input->getColumnInfos(_inputSymbols);
        
        SortingBlockIterator::SortOrders sortOrders;
        
        for(const auto& orderExp : _orderExpressions) {
            ASTIdentifierPtr ident = std::dynamic_pointer_cast<ASTIdentifier>(orderExp.first);
            if(ident) {
                bool found = false;
                for(size_t n = 0; !found && n < _inputSymbols.size(); ++n) {
                    const SymbolInfoPtr& info = _inputSymbols[n];
                    
                    if( (!ident->_info->_name.empty() && (ident->_info->_name == info->_name)) ||
                       (!ident->_info->_qualifiedIdentifier.empty() && (ident->_info->_qualifiedIdentifier == info->_qualifiedIdentifier)) ||
                       (ident->_info->_prefix.empty() && ident->_info->_identifier == info->_identifier) ) {
                        sortOrders.push_back( { n, orderExp.second } );
                        found = true;
                    }
                }
                if(!found) {
                    CSVSQLDB_THROW(csvsqldb::Exception, "order expression '" << ident->_info->_name << "' not found in context");
                }
            } else {
                CSVSQLDB_THROW(csvsqldb::Exception, "complex order expression not supported yet");
            }
        }
        
        for(const auto& info : _inputSymbols) {
            _types.push_back(info->_type);
        }
        _iterator = std::make_shared<SortingBlockIterator>(_types, sortOrders, *_input, getBlockManager());
        
        return true;
    }
    
    void SortOperatorNode::getColumnInfos(SymbolInfos& outputSymbols)
    {
        remapOutputSymbols(_inputSymbols);
        outputSymbols = _inputSymbols;
    }
    
    void SortOperatorNode::dump(std::ostream& stream) const
    {
        stream << "SortOperator (";
        bool first(true);
        for(const auto& entry : _orderExpressions) {
            if(first) {
                first = false;
            } else {
                stream << ",";
            }
            ASTExpressionVisitor visitor;
            entry.first->accept(visitor);
            stream << visitor.toString();
            stream << " " << orderToString(entry.second);
        }
        stream << ")\n-->";
        _input->dump(stream);
    }
    
    
    GroupingOperatorNode::GroupingOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const Expressions& nodes,
                                               const Identifiers& groupByIdentifiers)
    : RowOperatorNode(context, symbolTable)
    , _nodes(nodes)
    , _groupByIdentifiers(groupByIdentifiers)
    {
    }
    
    const Values* GroupingOperatorNode::getNextRow()
    {
        return _iterator->getNextRow();
    }
    
    void GroupingOperatorNode::addPathThrough(const ASTIdentifierPtr& ident, csvsqldb::IndexVector& groupingIndices, csvsqldb::IndexVector& outputColumns,
                                              bool suppress)
    {
        bool found = false;
        for(size_t n = 0; !found && n < _inputSymbols.size(); ++n) {
            const SymbolInfoPtr& info = _inputSymbols[n];
            
            if( (!ident->_info->_name.empty() && (ident->_info->_name == info->_name)) ||
               (!ident->_info->_qualifiedIdentifier.empty() && (ident->_info->_qualifiedIdentifier == info->_qualifiedIdentifier)) ||
               (ident->_info->_prefix.empty() && ident->_info->_identifier == info->_identifier)) {
                groupingIndices.push_back(n);
                outputColumns.push_back(n);
                if(!suppress) {
                    _outputSymbols.push_back(info);
                    _types.push_back(info->_type);
                }
                _aggregateFunctions.push_back(std::make_shared<PaththroughAggregationFunction>(suppress));
                found = true;
            }
        }
        if(!found) {
            CSVSQLDB_THROW(csvsqldb::Exception, "group expression '" << ident->_info->_qualifiedIdentifier << "' not found in context");
        }
    }
    
    bool GroupingOperatorNode::connect(const RowOperatorNodePtr& input)
    {
        _input = input;
        _input->getColumnInfos(_inputSymbols);
        
        csvsqldb::IndexVector groupingIndices;
        csvsqldb::IndexVector outputColumns;
        Identifiers foundGroupByIdentifiers;
        
        for(const auto& groupExp : _nodes) {
            ASTIdentifierPtr ident = std::dynamic_pointer_cast<ASTIdentifier>(groupExp);
            if(ident) {
                addPathThrough(ident, groupingIndices, outputColumns, false);
                auto result = std::find_if(_groupByIdentifiers.begin(), _groupByIdentifiers.end(), [&](const ASTIdentifierPtr& identifier) {
                    return ident->_info->_name == identifier->_identifier;
                });
                if(result == std::end(_groupByIdentifiers)) {
                    CSVSQLDB_THROW(csvsqldb::Exception, "all elements of the select list of a group by have to be aggregations or contained in the group by expressions");
                }
                foundGroupByIdentifiers.push_back(*result);
            } else if(std::dynamic_pointer_cast<ASTAggregateFunctionNode>(groupExp)) {
                ASTAggregateFunctionNodePtr aggr = std::dynamic_pointer_cast<ASTAggregateFunctionNode>(groupExp);
                size_t rowValueId = 0;
                eType type = INT;
                
                if(aggr->_aggregateFunction != COUNT_STAR) {
                    if(aggr->_parameters.size() != 1) {
                        CSVSQLDB_THROW(csvsqldb::Exception, "not exactly one aggregation parameter found");
                    }
                    if(!std::dynamic_pointer_cast<ASTIdentifier>(aggr->_parameters[0]._exp)) {
                        CSVSQLDB_THROW(csvsqldb::Exception, "currently only identifier allowed as aggregation parameter");
                    }
                    std::string param = std::dynamic_pointer_cast<ASTIdentifier>(aggr->_parameters[0]._exp)->getQualifiedIdentifier();
                    bool found = false;
                    for(const auto& info : _inputSymbols) {
                        if(info->_name == param) {
                            found = true;
                            type = info->_type;
                            break;
                        }
                        ++rowValueId;
                    }
                    if(!found) {
                        CSVSQLDB_THROW(csvsqldb::Exception, "aggregation parameter '" << param << "'");
                    }
                }
                
                _aggregateFunctions.push_back(AggregationFunction::create(aggr->_aggregateFunction, type));
                if(aggr->_aggregateFunction == COUNT || aggr->_aggregateFunction == COUNT_STAR) {
                    type = INT;
                }
                
                _types.push_back(type);
                const SymbolInfoPtr& outputInfo = getSymbolTable().findSymbol(groupExp->_symbolName);
                _outputSymbols.push_back(outputInfo);
                outputColumns.push_back(rowValueId);
            } else {
                CSVSQLDB_THROW(csvsqldb::Exception, "only grouping values and aggregate function allowed in group by");
            }
        }
        for(const auto& groupBy : _groupByIdentifiers) {
            auto result = std::find_if(foundGroupByIdentifiers.begin(), foundGroupByIdentifiers.end(), [&](const ASTIdentifierPtr& identifier) {
                return groupBy->_identifier == identifier->_identifier;
            });
            if(result == std::end(foundGroupByIdentifiers)) {
                addPathThrough(groupBy, groupingIndices, outputColumns, true);
            }
        }
        
        _iterator = std::make_shared<GroupingBlockIterator>(_types, groupingIndices, outputColumns, _aggregateFunctions, *_input, getBlockManager());
        
        return true;
    }
    
    void GroupingOperatorNode::getColumnInfos(SymbolInfos& outputSymbols)
    {
        remapOutputSymbols(_outputSymbols);
        outputSymbols = _outputSymbols;
    }
    
    void GroupingOperatorNode::dump(std::ostream& stream) const
    {
        stream << "GroupingOperator (";
        bool first(true);
        for(const auto& entry : _aggregateFunctions) {
            if(first) {
                first = false;
            } else {
                stream << ",";
            }
            stream << entry->toString();
        }
        stream << " -> ";
        first = true;
        for(const auto& group : _groupByIdentifiers) {
            if(first) {
                first = false;
            } else {
                stream << ",";
            }
            stream << group->_identifier;
        }
        stream << ")\n";
        stream << "-->";
        _input->dump(stream);
    }
    
    
    AggregationOperatorNode::AggregationOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const Expressions& nodes)
    : RowOperatorNode(context, symbolTable)
    , _nodes(nodes)
    , _block(nullptr)
    {}
    
    AggregationOperatorNode::~AggregationOperatorNode()
    {
    }
    
    const Values* AggregationOperatorNode::getNextRow()
    {
        if(!_block) {
            _block = _context._blockManager.createBlock();
            _iterator = std::make_shared<BlockIterator>(_types, *this, getBlockManager());
        }
        return _iterator->getNextRow();
    }
    
    bool AggregationOperatorNode::connect(const RowOperatorNodePtr& input)
    {
        _input = input;
        eType type = INT;
        SymbolInfos infos;
        _input->getColumnInfos(infos);
        
        for(const auto& exp : _nodes) {
            if(std::dynamic_pointer_cast<ASTAggregateFunctionNode>(exp)) {
                ASTAggregateFunctionNodePtr aggr = std::dynamic_pointer_cast<ASTAggregateFunctionNode>(exp);
                
                if(aggr->_aggregateFunction != COUNT_STAR) {
                    if(aggr->_parameters.size() != 1) {
                        CSVSQLDB_THROW(csvsqldb::Exception, "not exactly one aggregation parameter found");
                    }
                    
                    type = aggr->_parameters[0]._exp->type();
                    
                    StackMachine sm;
                    IdentifierSet expressionVariables;
                    
                    StackMachine::VariableMapping mapping;
                    {
                        ASTInstructionStackVisitor visitor(sm, mapping);
                        aggr->_parameters[0]._exp->accept(visitor);
                    }
                    
                    {
                        ASTExpressionVariableVisitor visitor(expressionVariables);
                        aggr->_parameters[0]._exp->accept(visitor);
                    }
                    
                    VariableMapping varMapping;
                    for(const auto& variable : expressionVariables) {
                        bool found = false;
                        for(size_t n = 0; !found && n < infos.size(); ++n) {
                            const SymbolInfoPtr& info = infos[n];
                            
                            if(variable._info->_name == info->_name) {
                                varMapping.push_back(std::make_pair(getMapping(variable.getQualifiedIdentifier(), mapping), n));
                                found = true;
                            }
                        }
                        if(!found) {
                            CSVSQLDB_THROW(csvsqldb::Exception, "variable '" << variable.getQualifiedIdentifier() << "' not found");
                        }
                    }
                    _sms.push_back(StackMachineType(sm, varMapping));
                } else {
                    // we just need a dummy value
                    StackMachine sm;
                    VariableMapping varMapping;
                    sm.addInstruction(StackMachine::Instruction(StackMachine::PUSH, Variant(BOOLEAN)));
                    _sms.push_back(StackMachineType(sm, varMapping));
                }
                
                _aggregateFunctions.push_back(AggregationFunction::create(aggr->_aggregateFunction, type));
                if(aggr->_aggregateFunction == COUNT || aggr->_aggregateFunction == COUNT_STAR) {
                    type = INT;
                }
                
                _types.push_back(type);
                const SymbolInfoPtr& outputInfo = getSymbolTable().findSymbol(exp->_symbolName);
                _outputSymbols.push_back(outputInfo);
            } else {
                CSVSQLDB_THROW(csvsqldb::Exception, "no aggregation on other than aggregation functions");
            }
        }
        
        return true;
    }
    
    void AggregationOperatorNode::getColumnInfos(SymbolInfos& outputSymbols)
    {
        remapOutputSymbols(_outputSymbols);
        outputSymbols = _outputSymbols;
    }
    
    BlockPtr AggregationOperatorNode::getNextBlock()
    {
        for(auto& aggrFunc : _aggregateFunctions) {
            aggrFunc->init();
        }
        
        size_t smIndex = 0;
        size_t n = 0;
        const Values* row = nullptr;
        while( (row = _input->getNextRow()) ) {
            for(auto& aggrFunc : _aggregateFunctions) {
                fillVariableStore(_sms[smIndex]._store, _sms[smIndex]._variableMappings, *row);
                Variant& variant = _sms[smIndex]._sm.evaluate(_sms[smIndex]._store, _context._functions);
                aggrFunc->step(variant);
                ++n;
                ++smIndex;
            }
            n = 0;
            smIndex = 0;
        }
        
        n = 0;
        for(auto& aggrFunc : _aggregateFunctions) {
            _block->addValue(aggrFunc->finalize());
        }
        _block->nextRow();
        _block->endBlocks();
        
        return _block;
    }
    
    void AggregationOperatorNode::dump(std::ostream& stream) const
    {
        stream << "AggregationOperator (";
        bool first(true);
        for(const auto& entry : _aggregateFunctions) {
            if(first) {
                first = false;
            } else {
                stream << ",";
            }
            stream << entry->toString();
        }
        stream << ")\n-->";
        _input->dump(stream);
    }
    
    
    ExtendedProjectionOperatorNode::ExtendedProjectionOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const Expressions& nodes)
    : RowOperatorNode(context, symbolTable)
    , _nodes(nodes)
    , _block(nullptr)
    {
    }
    
    ExtendedProjectionOperatorNode::~ExtendedProjectionOperatorNode()
    {
    }
    
    const Values* ExtendedProjectionOperatorNode::getNextRow()
    {
        return _iterator->getNextRow();
    }
    
    bool ExtendedProjectionOperatorNode::connect(const RowOperatorNodePtr& input)
    {
        _input = input;
        _input->getColumnInfos(_inputSymbols);
        
        size_t index = 0;
        for(const auto& exp : _nodes) {
            if(std::dynamic_pointer_cast<ASTIdentifier>(exp)) {
                ASTIdentifierPtr ident = std::dynamic_pointer_cast<ASTIdentifier>(exp);
                
                bool found = false;
                for(size_t n = 0; !found && n < _inputSymbols.size(); ++n) {
                    const SymbolInfoPtr& info = _inputSymbols[n];
                    
                    if( (!ident->_info->_name.empty() && (ident->_info->_name == info->_name)) ||
                       (!ident->_info->_qualifiedIdentifier.empty() && (ident->_info->_qualifiedIdentifier == info->_qualifiedIdentifier)) ||
                       (ident->_info->_prefix.empty() && (ident->_info->_identifier == info->_identifier)) ) {
                        _outputSymbols.push_back(ident->_info);
                        _types.push_back(ident->_info->_type);
                        _outputInputMapping.insert(std::make_pair(index, n));
                        found = true;
                    }
                }
                if(!found) {
                    CSVSQLDB_THROW(csvsqldb::Exception, "select list expression '" << ident->_info->_name << "' not found in context");
                }
            } else if(std::dynamic_pointer_cast<ASTQualifiedAsterisk>(exp)) {
                std::string prefixName = std::dynamic_pointer_cast<ASTQualifiedAsterisk>(exp)->_prefix;
                
                SymbolInfoPtr table;
                if(!prefixName.empty() && getSymbolTable().hasTableSymbol(prefixName)) {
                    table = getSymbolTable().findTableSymbol(prefixName);
                }
                
                if(table) {
                    if(!_context._database.hasTable(table->_identifier)) {
                        CSVSQLDB_THROW(csvsqldb::Exception, "cannot find table '" << table->_identifier << "'");
                    }
                    // TODO LCF: this is not quite ok, as we could have a different sorting, so better so go through the _inputSymbols and find the right
                    // table symbols
                    const TableData& tableData = _context._database.getTable(table->_identifier);
                    
                    for(size_t n = 0; n < tableData.columnCount(); ++n) {
                        const SymbolInfoPtr& info = getSymbolTable().findSymbolNameForTable(table->_identifier, tableData.getColumn(n)._name);
                        auto result = std::find(_inputSymbols.begin(), _inputSymbols.end(), info);
                        if(result == _inputSymbols.end()) {
                            CSVSQLDB_THROW(csvsqldb::Exception, "could not find output symbol '" << info->_identifier << "' in input symbols");
                        }
                        _outputSymbols.push_back(*result);
                        _types.push_back(info->_type);
                    }
                } else {
                    for(const auto& info : _inputSymbols) {
                        _outputSymbols.push_back(info);
                        _types.push_back(info->_type);
                    }
                }
            } else {
                const SymbolInfoPtr& outputInfo = getSymbolTable().findSymbol(exp->_symbolName);
                _outputSymbols.push_back(outputInfo);
                _types.push_back(outputInfo->_type);
                
                StackMachine sm;
                IdentifierSet expressionVariables;
                
                StackMachine::VariableMapping mapping;
                {
                    ASTInstructionStackVisitor visitor(sm, mapping);
                    exp->accept(visitor);
                }
                
                {
                    ASTExpressionVariableVisitor visitor(expressionVariables);
                    exp->accept(visitor);
                }
                
                VariableMapping varMapping;
                for(const auto& variable : expressionVariables) {
                    bool found = false;
                    for(size_t n = 0; !found && n < _inputSymbols.size(); ++n) {
                        const SymbolInfoPtr& info = _inputSymbols[n];
                        
                        if(variable._info->_name == info->_name) {
                            varMapping.push_back(std::make_pair(getMapping(variable.getQualifiedIdentifier(), mapping), n));
                            found = true;
                        }
                    }
                    if(!found) {
                        CSVSQLDB_THROW(csvsqldb::Exception, "variable '" << variable.getQualifiedIdentifier() << "' not found");
                    }
                }
                _sms.push_back(StackMachineType(sm, varMapping));
            }
            ++index;
        }
        
        _block = _context._blockManager.createBlock();
        _iterator = std::make_shared<BlockIterator>(_types, *this, getBlockManager());
        
        return true;
    }
    
    void ExtendedProjectionOperatorNode::getColumnInfos(SymbolInfos& outputSymbols)
    {
        remapOutputSymbols(_outputSymbols);
        outputSymbols = _outputSymbols;
    }
    
    BlockPtr ExtendedProjectionOperatorNode::getNextBlock()
    {
        BlockPtr previousBlock = prepareNextBuffer();
        return previousBlock? previousBlock : _block;
    }
    
    BlockPtr ExtendedProjectionOperatorNode::prepareNextBuffer()
    {
        const Values* row = nullptr;
        BlockPtr previousBlock = nullptr;
        
        while( !previousBlock && (row = _input->getNextRow()) ) {
            size_t smIndex = 0;
            size_t index = 0;
            for(const auto& exp : _nodes) {
                if(std::dynamic_pointer_cast<ASTIdentifier>(exp)) {
                    OutputInputMapping::const_iterator iter = _outputInputMapping.find(index);
                    if(iter == _outputInputMapping.end()) {
                        CSVSQLDB_THROW(csvsqldb::Exception, "selection expression no. " << index << " not found in output input mappings");
                    }
                    const Values& rrow = *row;
                    if(!_block->addValue(*(rrow[iter->second]))) {
                        _block->markNextBlock();
                        previousBlock = _block;
                        _block = _context._blockManager.createBlock();
                        _block->addValue(*(rrow[iter->second]));
                    }
                } else if(std::dynamic_pointer_cast<ASTQualifiedAsterisk>(exp)) {
                    std::string prefixName = std::dynamic_pointer_cast<ASTQualifiedAsterisk>(exp)->_prefix;
                    
                    SymbolInfoPtr table;
                    if(!prefixName.empty() && getSymbolTable().hasTableSymbol(prefixName)) {
                        table = getSymbolTable().findTableSymbol(prefixName);
                    }
                    
                    if(table) {
                        if(!_context._database.hasTable(table->_identifier)) {
                            CSVSQLDB_THROW(csvsqldb::Exception, "cannot find table '" << table->_identifier << "'");
                        }
                        // TODO LCF: this is not quite ok, as we could have a different sorting, so better so go through the _inputSymbols and find the right
                        // table symbols
                        const TableData& tableData = _context._database.getTable(table->_identifier);
                        
                        const Values& rrow = *row;
                        for(size_t n = 0; n < tableData.columnCount(); ++n) {
                            if(!_block->addValue(*(rrow[n]))) {
                                _block->markNextBlock();
                                previousBlock = _block;
                                _block = _context._blockManager.createBlock();
                                _block->addValue(*(rrow[n]));
                            }
                        }
                    } else {
                        const Values& rrow = *row;
                        for(size_t n = 0; n < _inputSymbols.size(); ++n) {
                            if(!_block->addValue(*(rrow[n]))) {
                                _block->markNextBlock();
                                previousBlock = _block;
                                _block = _context._blockManager.createBlock();
                                _block->addValue(*(rrow[n]));
                            }
                        }
                    }
                } else {
                    fillVariableStore(_sms[smIndex]._store, _sms[smIndex]._variableMappings, *row);
                    
                    if(!_block->addValue(_sms[smIndex]._sm.evaluate(_sms[smIndex]._store, _context._functions))) {
                        _block->markNextBlock();
                        previousBlock = _block;
                        _block = _context._blockManager.createBlock();
                        _block->addValue(_sms[smIndex]._sm.evaluate(_sms[smIndex]._store, _context._functions));
                    }
                    ++smIndex;
                }
                ++index;
            }
            _block->nextRow();
        }
        if(!row) {
            _block->endBlocks();
        }
        
        return previousBlock;
    }
    
    void ExtendedProjectionOperatorNode::dump(std::ostream& stream) const
    {
        stream << "ExtendedProjectionOperator (";
        bool first(true);
        for(const auto& entry : _outputSymbols) {
            if(first) {
                first = false;
            } else {
                stream << ",";
            }
            stream << entry->_name;
        }
        stream << ")\n-->";
        _input->dump(stream);
    }
    
    
    CrossJoinOperatorNode::CrossJoinOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable)
    : RowOperatorNode(context, symbolTable)
    , _currentLhs(nullptr)
    {}
    
    const Values* CrossJoinOperatorNode::getNextRow()
    {
        if(!_currentLhs) {
            _currentLhs = _lhsInput->getNextRow();
            if(!_currentLhs) {
                return nullptr;
            }
            for(size_t n = 0; n < _inputLhsSymbols.size(); ++n) {
                const Values& values = *(_currentLhs);
                _row[n] = values[n];
            }
        }
        
        const Values* row = _rhsIterator->getNextRow();
        if(!row) {
            _currentLhs = _lhsInput->getNextRow();
            if(!_currentLhs) {
                return nullptr;
            }
            for(size_t n = 0; n < _inputLhsSymbols.size(); ++n) {
                const Values& values = *(_currentLhs);
                _row[n] = values[n];
            }
            _rhsIterator->rewind();
            row = _rhsIterator->getNextRow();
        }
        size_t m = 0;
        for(size_t n = _inputLhsSymbols.size(); n < _inputRhsSymbols.size() + _inputLhsSymbols.size(); ++n, ++m) {
            const Values& values = *(row);
            _row[n] = values[m];
        }
        
        return &_row;
    }
    
    bool CrossJoinOperatorNode::connect(const RowOperatorNodePtr& input)
    {
        if(!_lhsInput) {
            _lhsInput = input;
            _lhsInput->getColumnInfos(_inputLhsSymbols);
            _outputSymbols.insert(_outputSymbols.end(), _inputLhsSymbols.begin(), _inputLhsSymbols.end());
            return false;
        } else if(!_rhsInput) {
            _rhsInput = input;
            _rhsInput->getColumnInfos(_inputRhsSymbols);
            _outputSymbols.insert(_outputSymbols.end(), _inputRhsSymbols.begin(), _inputRhsSymbols.end());
            Types types;
            for(const auto& info : _inputRhsSymbols) {
                types.push_back(info->_type);
            }
            _rhsIterator = std::make_shared<CachingBlockIterator>(types, *_rhsInput, getBlockManager());
            _row.resize(_outputSymbols.size());
        } else {
            CSVSQLDB_THROW(csvsqldb::Exception, "all inputs already set");
        }
        
        return true;
    }
    
    void CrossJoinOperatorNode::getColumnInfos(SymbolInfos& outputSymbols)
    {
        outputSymbols = _outputSymbols;
    }
    
    void CrossJoinOperatorNode::dump(std::ostream& stream) const
    {
        stream << "CrossJoinOperator\n";
        stream << "-->";
        _lhsInput->dump(stream);
        stream << "-->";
        _rhsInput->dump(stream);
    }
    
    
    InnerJoinOperatorNode::InnerJoinOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const ASTExprNodePtr& exp)
    : CrossJoinOperatorNode(context, symbolTable)
    , _exp(exp)
    {
    }
    
    const Values* InnerJoinOperatorNode::getNextRow()
    {
        VariableStore store;
        const Values* row = nullptr;
        bool match = false;
        
        do {
            row = CrossJoinOperatorNode::getNextRow();
            
            if(row) {
                fillVariableStore(store, _sm._variableMappings, *row);
                match = _sm._sm.evaluate(store, _context._functions).asBool();
            }
        } while(row && !match);
        
        return row;
    }
    
    bool InnerJoinOperatorNode::connect(const RowOperatorNodePtr& input)
    {
        if(CrossJoinOperatorNode::connect(input)) {
            SymbolInfos outputSymbols;
            CrossJoinOperatorNode::getColumnInfos(outputSymbols);
            
            StackMachine sm;
            IdentifierSet expressionVariables;
            
            StackMachine::VariableMapping mapping;
            {
                ASTInstructionStackVisitor visitor(sm, mapping);
                _exp->accept(visitor);
            }
            
            {
                ASTExpressionVariableVisitor visitor(expressionVariables);
                _exp->accept(visitor);
            }
            
            VariableMapping variableMapping;
            for(const auto& variable : expressionVariables) {
                bool found = false;
                for(size_t n = 0; !found && n < outputSymbols.size(); ++n) {
                    const SymbolInfoPtr& info = outputSymbols[n];
                    
                    if(variable._info->_name == info->_name) {
                        variableMapping.push_back(std::make_pair(getMapping(variable.getQualifiedIdentifier(), mapping), n));
                        found = true;
                    }
                }
                if(!found) {
                    CSVSQLDB_THROW(csvsqldb::Exception, "variable '" << variable.getQualifiedIdentifier() << "' not found in context");
                }
            }
            
            _sm = StackMachineType(sm, variableMapping);
        }
        
        return true;
    }
    
    void InnerJoinOperatorNode::getColumnInfos(SymbolInfos& outputSymbols)
    {
        CrossJoinOperatorNode::getColumnInfos(outputSymbols);
    }
    
    void InnerJoinOperatorNode::dump(std::ostream& stream) const
    {
        stream << "InnerJoinOperatorNode\n";
        CrossJoinOperatorNode::dump(stream);
    }
    
    
    InnerHashJoinOperatorNode::InnerHashJoinOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const ASTExprNodePtr& exp)
    : RowOperatorNode(context, symbolTable)
    , _currentLhs(nullptr)
    , _exp(exp)
    , _hashTableKeyPosition(0)
    {}
    
    const Values* InnerHashJoinOperatorNode::getNextRow()
    {
        const Values* row = nullptr;
        
        do {
            row = _rhsIterator->getNextKeyValueRow();
            if(!row) {
                _currentLhs = _lhsInput->getNextRow();
                if(!_currentLhs) {
                    // free all resources, as we have delivered the last row
                    _rhsIterator->reset();
                    return nullptr;
                }
                for(size_t n = 0; n < _inputLhsSymbols.size(); ++n) {
                    const Values& values = *(_currentLhs);
                    _row[n] = values[n];
                }
                _rhsIterator->setContextForKeyValue(*_row[_hashTableKeyPosition]);
            }
        } while(!row);
        
        size_t m = 0;
        for(size_t n = _inputLhsSymbols.size(); n < _inputRhsSymbols.size() + _inputLhsSymbols.size(); ++n, ++m) {
            const Values& values = *(row);
            _row[n] = values[m];
        }
        return &_row;
    }
    
    bool InnerHashJoinOperatorNode::connect(const RowOperatorNodePtr& input)
    {
        if(!_lhsInput) {
            _lhsInput = input;
            _lhsInput->getColumnInfos(_inputLhsSymbols);
            _outputSymbols.insert(_outputSymbols.end(), _inputLhsSymbols.begin(), _inputLhsSymbols.end());
            return false;
        } else if(!_rhsInput) {
            _rhsInput = input;
            _rhsInput->getColumnInfos(_inputRhsSymbols);
            _outputSymbols.insert(_outputSymbols.end(), _inputRhsSymbols.begin(), _inputRhsSymbols.end());
            Types types;
            for(const auto& info : _inputRhsSymbols) {
                types.push_back(info->_type);
            }
            
            StackMachine sm;
            IdentifierSet expressionVariables;
            
            StackMachine::VariableMapping mapping;
            {
                ASTInstructionStackVisitor visitor(sm, mapping);
                _exp->accept(visitor);
            }
            
            {
                ASTExpressionVariableVisitor visitor(expressionVariables);
                _exp->accept(visitor);
            }
            
            VariableMapping variableMapping;
            size_t hashTableKeyPosition = 0;
            for(const auto& variable : expressionVariables) {
                bool found = false;
                for(size_t n = 0; !found && n < _outputSymbols.size(); ++n) {
                    const SymbolInfoPtr& info = _outputSymbols[n];
                    
                    if(variable._info->_name == info->_name) {
                        variableMapping.push_back(std::make_pair(getMapping(variable.getQualifiedIdentifier(), mapping), n));
                        found = true;
                    }
                }
                if(!found) {
                    CSVSQLDB_THROW(csvsqldb::Exception, "variable '" << variable.getQualifiedIdentifier() << "' not found in context");
                }
                // find the rhs expresion variable for the hash table key
                found = false;
                for(size_t n = 0; !found && n < _inputRhsSymbols.size(); ++n) {
                    const SymbolInfoPtr& info = _inputRhsSymbols[n];
                    
                    if(variable._info->_name == info->_name) {
                        hashTableKeyPosition = n;
                        found = true;
                    }
                }
                // find the lhs expresion variable for the hash table key
                found = false;
                for(size_t n = 0; !found && n < _inputLhsSymbols.size(); ++n) {
                    const SymbolInfoPtr& info = _inputLhsSymbols[n];
                    
                    if(variable._info->_name == info->_name) {
                        _hashTableKeyPosition = n;
                        found = true;
                    }
                }
            }
            
            _rhsIterator = std::make_shared<HashingBlockIterator>(types, *_rhsInput, getBlockManager(), hashTableKeyPosition);
            _row.resize(_outputSymbols.size());
        } else {
            CSVSQLDB_THROW(csvsqldb::Exception, "all inputs already set");
        }
        
        return true;
    }
    
    void InnerHashJoinOperatorNode::getColumnInfos(SymbolInfos& outputSymbols)
    {
        outputSymbols = _outputSymbols;
    }
    
    void InnerHashJoinOperatorNode::dump(std::ostream& stream) const
    {
        stream << "InnerHashJoinOperator\n";
        stream << "-->";
        _lhsInput->dump(stream);
        stream << "-->";
        _rhsInput->dump(stream);
    }
    
    
    UnionOperatorNode::UnionOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable)
    : RowOperatorNode(context, symbolTable)
    {}
    
    const Values* UnionOperatorNode::getNextRow()
    {
        const Values* row;
        
        row = _currentInput->getNextRow();
        if(!row && _firstInput) {
            _currentInput = _secondInput;
            row = _currentInput->getNextRow();
            _firstInput.reset();
        }
        
        return row;
    }
    
    bool UnionOperatorNode::connect(const RowOperatorNodePtr& input)
    {
        if(!_firstInput) {
            _firstInput = input;
            _firstInput->getColumnInfos(_inputSymbols);
            _currentInput = _firstInput;
        } else if(!_secondInput) {
            _secondInput = input;
        } else {
            CSVSQLDB_THROW(csvsqldb::Exception, "all inputs already set");
        }
        
        return true;
    }
    
    void UnionOperatorNode::getColumnInfos(SymbolInfos& outputSymbols)
    {
        outputSymbols = _inputSymbols;
    }
    
    void UnionOperatorNode::dump(std::ostream& stream) const
    {
        stream << "UnionOperatorNode\n";
        stream << "-->";
        _firstInput->dump(stream);
        stream << "-->";
        _secondInput->dump(stream);
    }
    
    
    SelectOperatorNode::SelectOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const ASTExprNodePtr& exp)
    : RowOperatorNode(context, symbolTable)
    {
        {
            ASTInstructionStackVisitor visitor(_sm, _mapping);
            exp->accept(visitor);
        }
        
        {
            ASTExpressionVariableVisitor visitor(_expressionVariables);
            exp->accept(visitor);
        }
    }
    
    const Values* SelectOperatorNode::getNextRow()
    {
        const Values* row = _input->getNextRow();
        while(row) {
            fillVariableStore(_store, _variableMapping, *row);
            if(_sm.evaluate(_store, _context._functions).asBool()) {
                return row;
            }
            row = _input->getNextRow();
        }
        return nullptr;
    }
    
    bool SelectOperatorNode::connect(const RowOperatorNodePtr& input)
    {
        _input = input;
        _input->getColumnInfos(_inputSymbols);
        
        for(const auto& variable : _expressionVariables) {
            bool found = false;
            for(size_t n = 0; !found && n < _inputSymbols.size(); ++n) {
                const SymbolInfoPtr& info = _inputSymbols[n];
                
                if( (info->_identifier == variable._info->_name) ||
                   (info->_qualifiedIdentifier == variable._info->_name) ) {
                    _variableMapping.push_back(std::make_pair(getMapping(variable.getQualifiedIdentifier(), _mapping), n));
                    found = true;
                }
            }
            if(!found) {
                CSVSQLDB_THROW(csvsqldb::Exception, "variable '" << variable.getQualifiedIdentifier() << "' not found in context");
            }
        }
        
        return true;
    }
    
    void SelectOperatorNode::getColumnInfos(SymbolInfos& outputSymbols)
    {
        remapOutputSymbols(_inputSymbols);
        outputSymbols = _inputSymbols;
    }
    
    void SelectOperatorNode::dump(std::ostream& stream) const
    {
        stream << "SelectOperator\n";
        stream << "-->";
        _input->dump(stream);
    }
    
    
    ScanOperatorNode::ScanOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const SymbolInfo& tableInfo)
    : RowOperatorNode(context, symbolTable)
    , _tableData(_context._database.getTable(tableInfo._identifier))
    , _tableInfo(tableInfo)
    {
        for(size_t n = 0; n < _tableData.columnCount(); ++n) {
            _types.push_back(_tableData.getColumn(n)._type);
        }
    }
    
    void ScanOperatorNode::getColumnInfos(SymbolInfos& outputSymbols)
    {
        outputSymbols.clear();
        
        for(size_t n = 0; n < _tableData.columnCount(); ++n) {
            if(getSymbolTable().hasSymbolNameForTable(_tableInfo._name, _tableData.getColumn(n)._name)) {
                const SymbolInfoPtr& info = getSymbolTable().findSymbolNameForTable(_tableInfo._name, _tableData.getColumn(n)._name);
                outputSymbols.push_back(info);
            }
        }
    }
    
    
    SystemTableScanOperatorNode::SystemTableScanOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const SymbolInfo& tableInfo)
    : ScanOperatorNode(context, symbolTable, tableInfo)
    , _currentBlock(nullptr)
    {
    }
    
    void SystemTableScanOperatorNode::dump(std::ostream& stream) const
    {
        stream << "SystemTableScanOperatorNode(" << _tableInfo._identifier << ")\n";
    }
    
    const Values* SystemTableScanOperatorNode::getNextRow()
    {
        if(!_currentBlock) {
            _iterator = std::make_shared<BlockIterator>(_types, *this, _context._blockManager);
        }
        return _iterator->getNextRow();
    }
    
    BlockPtr SystemTableScanOperatorNode::getNextBlock()
    {
        _currentBlock = _context._blockManager.createBlock();
        
        _currentBlock->addValue(Variant(false));
        _currentBlock->nextRow();
        _currentBlock->endBlocks();
        
        return _currentBlock;
    }
    
    
    TableScanOperatorNode::TableScanOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const SymbolInfo& tableInfo)
    : ScanOperatorNode(context, symbolTable, tableInfo)
    , _blockReader(_context._blockManager)
    {}
    
    const Values* TableScanOperatorNode::getNextRow()
    {
        if(!_blockReader.valid()) {
            initializeBlockReader();
        }
        
        return _iterator->getNextRow();
    }
    
    
    BlockReader::BlockReader(BlockManager& blockManager)
    : _blockManager(blockManager)
    , _block(_blockManager.createBlock())
    , _continue(true)
    {
    }
    
    BlockReader::~BlockReader()
    {
        _continue = false;
        if(_readThread.joinable()) {
            _readThread.join();
        }
    }
    
    void BlockReader::initialize(CSVParserPtr csvparser)
    {
        _csvparser = csvparser;
        _readThread = std::thread(std::bind(&BlockReader::readBlocks, this));
    }
    
    BlockPtr BlockReader::getNextBlock()
    {
        std::unique_lock<std::mutex> lk(_queueMutex);
        if(_blocks.empty() && !_block) {
            return nullptr;
        }
        _cv.wait(lk, [this]{ return !_blocks.empty(); });
        
        BlockPtr block = nullptr;
        if(!_blocks.empty()) {
            block = _blocks.front();
            _blocks.pop();
        }
        
        return block;
    }
    
    void BlockReader::readBlocks()
    {
        bool moreLines = _csvparser->parseLine();
        _block->nextRow();
        
        while(_continue && moreLines) {
            moreLines = _csvparser->parseLine();
            _block->nextRow();
        }
        {
            std::unique_lock<std::mutex> lk(_queueMutex);
            _block->endBlocks();
            _blocks.push(_block);
            _block = nullptr;
        }
        _cv.notify_all();
    }
    
    void BlockReader::onLong(int64_t num, bool isNull)
    {
        if(!_block->addInt(num, isNull)) {
            std::unique_lock<std::mutex> lk(_queueMutex);
            _block->markNextBlock();
            _blocks.push(_block);
            _cv.notify_all();
            _block = _blockManager.createBlock();
            _block->addInt(num, isNull);
        }
    }
    
    void BlockReader::onDouble(double num, bool isNull)
    {
        if(!_block->addReal(num, isNull)) {
            std::unique_lock<std::mutex> lk(_queueMutex);
            _block->markNextBlock();
            _blocks.push(_block);
            _cv.notify_all();
            _block = _blockManager.createBlock();
            _block->addReal(num, isNull);
        }
    }
    
    void BlockReader::onString(const char* s, size_t len, bool isNull)
    {
        if(!_block->addString(s, len, isNull)) {
            std::unique_lock<std::mutex> lk(_queueMutex);
            _block->markNextBlock();
            _blocks.push(_block);
            _cv.notify_all();
            _block = _blockManager.createBlock();
            _block->addString(s, len, isNull);
        }
    }
    
    void BlockReader::onDate(const csvsqldb::Date& date, bool isNull)
    {
        if(!_block->addDate(date, isNull)) {
            std::unique_lock<std::mutex> lk(_queueMutex);
            _block->markNextBlock();
            _blocks.push(_block);
            _cv.notify_all();
            _block = _blockManager.createBlock();
            _block->addDate(date, isNull);
        }
    }
    
    void BlockReader::onTime(const csvsqldb::Time& time, bool isNull)
    {
        if(!_block->addTime(time, isNull)) {
            std::unique_lock<std::mutex> lk(_queueMutex);
            _block->markNextBlock();
            _blocks.push(_block);
            _cv.notify_all();
            _block = _blockManager.createBlock();
            _block->addTime(time, isNull);
        }
    }
    
    void BlockReader::onTimestamp(const csvsqldb::Timestamp& timestamp, bool isNull)
    {
        if(!_block->addTimestamp(timestamp, isNull)) {
            std::unique_lock<std::mutex> lk(_queueMutex);
            _block->markNextBlock();
            _blocks.push(_block);
            _cv.notify_all();
            _block = _blockManager.createBlock();
            _block->addTimestamp(timestamp, isNull);
        }
    }
    
    void BlockReader::onBoolean(bool boolean, bool isNull)
    {
        if(!_block->addBool(boolean, isNull)) {
            std::unique_lock<std::mutex> lk(_queueMutex);
            _block->markNextBlock();
            _blocks.push(_block);
            _cv.notify_all();
            _block = _blockManager.createBlock();
            _block->addBool(boolean, isNull);
        }
    }
    
    
    BlockPtr TableScanOperatorNode::getNextBlock()
    {
        return _blockReader.getNextBlock();
    }
    
    void TableScanOperatorNode::initializeBlockReader()
    {
        _iterator = std::make_shared<BlockIterator>(_types, *this, getBlockManager());
        
        csvsqldb::csv::Types types;
        for(auto& type : _types) {
            switch(type) {
                case INT:
                    types.push_back(csvsqldb::csv::LONG);
                    break;
                case REAL:
                    types.push_back(csvsqldb::csv::DOUBLE);
                    break;
                case DATE:
                    types.push_back(csvsqldb::csv::DATE);
                    break;
                case TIME:
                    types.push_back(csvsqldb::csv::TIME);
                    break;
                case TIMESTAMP:
                    types.push_back(csvsqldb::csv::TIMESTAMP);
                    break;
                case STRING:
                    types.push_back(csvsqldb::csv::STRING);
                    break;
                case BOOLEAN:
                    types.push_back(csvsqldb::csv::BOOLEAN);
                    break;
                default:
                    CSVSQLDB_THROW(csvsqldb::Exception, "type not implemented yet");
            }
        }
        
        Mapping mapping = _context._database.getMappingForTable(_tableInfo._identifier);
        std::string filePattern = mapping._mapping;
        filePattern = R"(.*)" + filePattern;
        boost::regex r(filePattern);
        
        fs::path pathToCsvFile("");
        
        for(const auto& file : _context._files) {
            boost::smatch match;
            if(regex_match(file, match, r)) {
                pathToCsvFile = file;
                break;
            }
        }
        if(pathToCsvFile.string().empty()) {
            CSVSQLDB_THROW(MappingException, "no file found for mapping '" << filePattern << "'");
        }
        
        _stream = std::make_shared<std::fstream>(pathToCsvFile.string());
        if(!_stream || _stream->fail()) {
            std::cerr << csvsqldb::errnoText() << std::endl;
            CSVSQLDB_THROW(csvsqldb::FilesystemException, "could not open file '" << pathToCsvFile << "'");
        }
        
        _csvContext._skipFirstLine = true;
        _csvContext._delimiter = mapping._delimiter;
        _csvparser = std::make_shared<csvsqldb::csv::CSVParser>(_csvContext, *_stream, types, _blockReader);
        _blockReader.initialize(_csvparser);
    }
    
    void TableScanOperatorNode::dump(std::ostream& stream) const
    {
        stream << "TableScanOperator (" << _tableInfo._identifier << ")\n";
    }
    
}
