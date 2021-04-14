//
//  select_operatornode.cpp
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

#include "select_operatornode.h"

#include <csvsqldb/sql_astexpressionvisitor.h>
#include <csvsqldb/visitor.h>


namespace csvsqldb
{
  SelectOperatorNode::SelectOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable,
                                         const ASTExprNodePtr& exp)
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
    while (row) {
      fillVariableStore(_store, _variableMapping, *row);
      if (_sm.evaluate(_store, _context._functions).asBool()) {
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

    for (const auto& variable : _expressionVariables) {
      bool found = false;
      for (size_t n = 0; !found && n < _inputSymbols.size(); ++n) {
        const SymbolInfoPtr& info = _inputSymbols[n];

        if ((info->_identifier == variable._info->_name) || (info->_qualifiedIdentifier == variable._info->_name)) {
          _variableMapping.push_back(std::make_pair(getMapping(variable.getQualifiedIdentifier(), _mapping), n));
          found = true;
        }
      }
      if (!found) {
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
}
