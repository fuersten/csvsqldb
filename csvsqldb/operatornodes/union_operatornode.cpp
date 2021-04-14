//
//  union_operatornode.cpp
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

#include "union_operatornode.h"


namespace csvsqldb
{
  UnionOperatorNode::UnionOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable)
  : RowOperatorNode(context, symbolTable)
  {
  }

  const Values* UnionOperatorNode::getNextRow()
  {
    const Values* row;

    row = _currentInput->getNextRow();
    if (!row && _firstInput) {
      _currentInput = _secondInput;
      row = _currentInput->getNextRow();
      _firstInput.reset();
    }

    return row;
  }

  bool UnionOperatorNode::connect(const RowOperatorNodePtr& input)
  {
    if (!_firstInput) {
      _firstInput = input;
      _firstInput->getColumnInfos(_inputSymbols);
      _currentInput = _firstInput;
    } else if (!_secondInput) {
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
}
