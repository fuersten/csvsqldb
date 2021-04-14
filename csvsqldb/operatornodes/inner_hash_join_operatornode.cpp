//
//  inner_hash_join_operatornode.cpp
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

#include "inner_hash_join_operatornode.h"

#include <csvsqldb/block_iterator.h>
#include <csvsqldb/sql_astexpressionvisitor.h>
#include <csvsqldb/visitor.h>


namespace csvsqldb
{
  InnerHashJoinOperatorNode::InnerHashJoinOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable,
                                                       const ASTExprNodePtr& exp)
  : RowOperatorNode(context, symbolTable)
  , _exp(exp)
  {
  }

  const Values* InnerHashJoinOperatorNode::getNextRow()
  {
    const Values* row = nullptr;

    do {
      row = _rhsIterator->getNextKeyValueRow();
      if (!row) {
        _currentLhs = _lhsInput->getNextRow();
        if (!_currentLhs) {
          // free all resources, as we have delivered the last row
          _rhsIterator->reset();
          return nullptr;
        }
        for (size_t n = 0; n < _inputLhsSymbols.size(); ++n) {
          const Values& values = *(_currentLhs);
          _row[n] = values[n];
        }
        _rhsIterator->setContextForKeyValue(*_row[_hashTableKeyPosition]);
      }
    } while (!row);

    size_t m = 0;
    for (size_t n = _inputLhsSymbols.size(); n < _inputRhsSymbols.size() + _inputLhsSymbols.size(); ++n, ++m) {
      const Values& values = *(row);
      _row[n] = values[m];
    }
    return &_row;
  }

  bool InnerHashJoinOperatorNode::connect(const RowOperatorNodePtr& input)
  {
    if (!_lhsInput) {
      _lhsInput = input;
      _lhsInput->getColumnInfos(_inputLhsSymbols);
      _outputSymbols.insert(_outputSymbols.end(), _inputLhsSymbols.begin(), _inputLhsSymbols.end());
      return false;
    } else if (!_rhsInput) {
      _rhsInput = input;
      _rhsInput->getColumnInfos(_inputRhsSymbols);
      _outputSymbols.insert(_outputSymbols.end(), _inputRhsSymbols.begin(), _inputRhsSymbols.end());
      Types types;
      for (const auto& info : _inputRhsSymbols) {
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
      for (const auto& variable : expressionVariables) {
        bool found = false;
        for (size_t n = 0; !found && n < _outputSymbols.size(); ++n) {
          const SymbolInfoPtr& info = _outputSymbols[n];

          if (variable._info->_name == info->_name) {
            variableMapping.push_back(std::make_pair(getMapping(variable.getQualifiedIdentifier(), mapping), n));
            found = true;
          }
        }
        if (!found) {
          CSVSQLDB_THROW(csvsqldb::Exception, "variable '" << variable.getQualifiedIdentifier() << "' not found in context");
        }
        // find the rhs expresion variable for the hash table key
        found = false;
        for (size_t n = 0; !found && n < _inputRhsSymbols.size(); ++n) {
          const SymbolInfoPtr& info = _inputRhsSymbols[n];

          if (variable._info->_name == info->_name) {
            hashTableKeyPosition = n;
            found = true;
          }
        }
        // find the lhs expresion variable for the hash table key
        found = false;
        for (size_t n = 0; !found && n < _inputLhsSymbols.size(); ++n) {
          const SymbolInfoPtr& info = _inputLhsSymbols[n];

          if (variable._info->_name == info->_name) {
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
}
