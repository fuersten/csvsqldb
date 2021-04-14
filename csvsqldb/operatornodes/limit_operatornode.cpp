//
//  limit_operatornode.cpp
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

#include "limit_operatornode.h"

#include <csvsqldb/visitor.h>


namespace csvsqldb
{
  LimitOperatorNode::LimitOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable,
                                       const ASTExprNodePtr& limit, const ASTExprNodePtr& offset)
  : RowOperatorNode(context, symbolTable)
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
    if (offset) {
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
    if (_offset) {
      const Values* row = _input->getNextRow();
      while (row && --_offset) {
        row = _input->getNextRow();
      }
      if (!row) {
        return nullptr;
      }
    }
    if (_limit > 0 && --_limit == 0) {
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
}
