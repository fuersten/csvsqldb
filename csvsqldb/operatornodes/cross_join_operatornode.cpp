//
//  cross_join_operatornode.cpp
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

#include "cross_join_operatornode.h"


namespace csvsqldb
{
  CrossJoinOperatorNode::CrossJoinOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable)
  : RowOperatorNode(context, symbolTable)
  {
  }

  const Values* CrossJoinOperatorNode::getNextRow()
  {
    if (!_currentLhs) {
      _currentLhs = _lhsInput->getNextRow();
      if (!_currentLhs) {
        return nullptr;
      }
      for (size_t n = 0; n < _inputLhsSymbols.size(); ++n) {
        const Values& values = *(_currentLhs);
        _row[n] = values[n];
      }
    }

    const Values* row = _rhsIterator->getNextRow();
    if (!row) {
      _currentLhs = _lhsInput->getNextRow();
      if (!_currentLhs) {
        return nullptr;
      }
      for (size_t n = 0; n < _inputLhsSymbols.size(); ++n) {
        const Values& values = *(_currentLhs);
        _row[n] = values[n];
      }
      _rhsIterator->rewind();
      row = _rhsIterator->getNextRow();
    }
    size_t m = 0;
    for (size_t n = _inputLhsSymbols.size(); n < _inputRhsSymbols.size() + _inputLhsSymbols.size(); ++n, ++m) {
      const Values& values = *(row);
      _row[n] = values[m];
    }

    return &_row;
  }

  bool CrossJoinOperatorNode::connect(const RowOperatorNodePtr& input)
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
}
