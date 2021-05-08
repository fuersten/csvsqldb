//
//  aggregation_operatornode.cpp
//  csvsqldb
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

#include "aggregation_operatornode.h"

#include <csvsqldb/sql_astexpressionvisitor.h>
#include <csvsqldb/visitor.h>


namespace csvsqldb
{
  AggregationOperatorNode::AggregationOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable,
                                                   const Expressions& nodes)
  : RowOperatorNode(context, symbolTable)
  , _nodes(nodes)
  , _block(nullptr)
  {
  }

  const Values* AggregationOperatorNode::getNextRow()
  {
    if (!_block) {
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

    for (const auto& exp : _nodes) {
      if (std::dynamic_pointer_cast<ASTAggregateFunctionNode>(exp)) {
        ASTAggregateFunctionNodePtr aggr = std::dynamic_pointer_cast<ASTAggregateFunctionNode>(exp);

        if (aggr->_aggregateFunction != COUNT_STAR) {
          if (aggr->_parameters.size() != 1) {
            CSVSQLDB_THROW(csvsqldb::Exception, "not exactly one aggregation parameter found");
          }

          type = aggr->_parameters[0]._exp->type();

          StackMachine sm;
          IdentifierSet expressionVariables;

          VariableStore::VariableMapping mapping;
          {
            ASTInstructionStackVisitor visitor(sm, mapping);
            aggr->_parameters[0]._exp->accept(visitor);
          }

          {
            ASTExpressionVariableVisitor visitor(expressionVariables);
            aggr->_parameters[0]._exp->accept(visitor);
          }

          VariableIndexMapping varMapping;
          for (const auto& variable : expressionVariables) {
            bool found = false;
            for (size_t n = 0; !found && n < infos.size(); ++n) {
              const SymbolInfoPtr& info = infos[n];

              if (variable._info->_name == info->_name) {
                varMapping.push_back(std::make_pair(VariableStore::getMapping(variable.getQualifiedIdentifier(), mapping), n));
                found = true;
              }
            }
            if (!found) {
              CSVSQLDB_THROW(csvsqldb::Exception, "variable '" << variable.getQualifiedIdentifier() << "' not found");
            }
          }
          _sms.push_back(StackMachineType(sm, varMapping));
        } else {
          // we just need a dummy value
          StackMachine sm;
          VariableIndexMapping varMapping;
          sm.addInstruction(StackMachine::Instruction(StackMachine::PUSH, Variant(BOOLEAN)));
          _sms.push_back(StackMachineType(sm, varMapping));
        }

        _aggregateFunctions.push_back(AggregationFunction::create(aggr->_aggregateFunction, type));
        if (aggr->_aggregateFunction == COUNT || aggr->_aggregateFunction == COUNT_STAR) {
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
    for (auto& aggrFunc : _aggregateFunctions) {
      aggrFunc->init();
    }

    size_t smIndex = 0;
    size_t n = 0;
    const Values* row = nullptr;
    while ((row = _input->getNextRow())) {
      for (auto& aggrFunc : _aggregateFunctions) {
        _sms[smIndex]._store.fillVariableStore(_sms[smIndex]._variableMappings, *row);
        const Variant& variant = _sms[smIndex]._sm.evaluate(_sms[smIndex]._store, _context._functions);
        aggrFunc->step(variant);
        ++n;
        ++smIndex;
      }
      n = 0;
      smIndex = 0;
    }

    for (auto& aggrFunc : _aggregateFunctions) {
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
    for (const auto& entry : _aggregateFunctions) {
      if (first) {
        first = false;
      } else {
        stream << ",";
      }
      stream << entry->toString();
    }
    stream << ")\n-->";
    _input->dump(stream);
  }
}
