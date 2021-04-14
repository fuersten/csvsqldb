//
//  inner_join_operatornode.cpp
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

#include "inner_join_operatornode.h"

#include <csvsqldb/block_iterator.h>
#include <csvsqldb/sql_astexpressionvisitor.h>
#include <csvsqldb/visitor.h>


namespace csvsqldb
{
  InnerJoinOperatorNode::InnerJoinOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable,
                                               const ASTExprNodePtr& exp)
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

      if (row) {
        fillVariableStore(store, _sm._variableMappings, *row);
        match = _sm._sm.evaluate(store, _context._functions).asBool();
      }
    } while (row && !match);

    return row;
  }

  bool InnerJoinOperatorNode::connect(const RowOperatorNodePtr& input)
  {
    if (CrossJoinOperatorNode::connect(input)) {
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
      for (const auto& variable : expressionVariables) {
        bool found = false;
        for (size_t n = 0; !found && n < outputSymbols.size(); ++n) {
          const SymbolInfoPtr& info = outputSymbols[n];

          if (variable._info->_name == info->_name) {
            variableMapping.push_back(std::make_pair(getMapping(variable.getQualifiedIdentifier(), mapping), n));
            found = true;
          }
        }
        if (!found) {
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
}
