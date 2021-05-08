//
//  output_row_operatornode.cpp
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

#include "output_row_operatornode.h"


namespace csvsqldb
{
  OutputRowOperatorNode::OutputRowOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable,
                                               std::ostream& stream)
  : RootOperatorNode(context, symbolTable)
  , _stream(stream)
  {
  }

  int64_t OutputRowOperatorNode::process()
  {
    if (_firstCall && _context._showHeaderLine) {
      _outputBuffer << "#";

      SymbolInfos infos;
      _input->getColumnInfos(infos);

      bool firstName(true);
      for (const auto& info : infos) {
        if (!firstName) {
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
    while ((row = _input->getNextRow())) {
      bool first = true;
      for (const auto& value : *row) {
        if (!first) {
          _outputBuffer << ",";
        } else {
          first = false;
        }

        if (value->getType() == STRING && !value->isNull()) {
          _outputBuffer << "'";
        }
        value->toStream(_outputBuffer);
        if (value->getType() == STRING && !value->isNull()) {
          _outputBuffer << "'";
        }
      }
      ++count;
      _outputBuffer << "\n";
      if (count % 1000 == 0) {
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

  void OutputRowOperatorNode::getColumnInfos(SymbolInfos&)
  {
  }

  void OutputRowOperatorNode::dump(std::ostream& stream) const
  {
    stream << "OutputRowOperator (";
    SymbolInfos outputSymbols;
    _input->getColumnInfos(outputSymbols);
    bool first(true);
    for (const auto& entry : outputSymbols) {
      if (first) {
        first = false;
      } else {
        stream << ",";
      }
      stream << entry->_name;
    }
    stream << ")\n-->";
    _input->dump(stream);
  }
}
