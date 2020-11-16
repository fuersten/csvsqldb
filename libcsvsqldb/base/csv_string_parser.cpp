//
//  csv_string_parser.cpp
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

#include "csv_string_parser.h"

#include "exception.h"


namespace csvsqldb
{
  namespace csv
  {
    CSVStringParser::CSVStringParser(BufferType& buffer, const size_t bufferSize, ReadFunction readFunction)
    : _buffer(buffer)
    , _bufferSize(bufferSize)
    , _readFunction(readFunction)
    , _currentBufferSize(_buffer.capacity())
    , _currentState(START)
    {
      initializeTransitionTable();
    }

    size_t CSVStringParser::parseToBuffer()
    {
      size_t pos = 0;
      _currentState = START;
      // bring the state machine into a final state
      const auto* newState = &_transitionTable[NO_QUOTE_STRING][OTHER];
      char c = _readFunction(false);
      while (c) {
        auto cat = charCategory(c);
        newState = &_transitionTable[_currentState][cat];
        _currentState = newState->_state;
        if (newState->_copy) {
          if (pos >= _currentBufferSize) {
            _currentBufferSize += _bufferSize;
            _buffer.resize(_currentBufferSize);
          }
          _buffer[pos] = c;
          ++pos;
        }
        c = _readFunction(newState->_ignoreDelimiter);
      }
      _buffer[pos] = '\0';
      if (_currentState == ERROR || !newState->_final) {
        throw Exception("wrong delimiters in string");
      }

      return pos;
    }

    void CSVStringParser::initializeTransitionTable()
    {
      // clang-format off
            std::vector<std::vector<State>> transitionTable
            {
              { // START
                  { SINGLE_QUOTE_STRING, false, false, true }, // SINGLE_QUOTE
                  { DOUBLE_QUOTE_STRING, false, false, true }, // DOUBLE_QUOTE
                  { NO_QUOTE_STRING, true, true, false }       // OTHER
              },
              { // DOUBLE_QUOTE_STRING
                  { DOUBLE_QUOTE_STRING, true, false, true },
                  { DOUBLE_QUOTE_STRING_END, false, true, false },
                  { DOUBLE_QUOTE_STRING, true, false, true }
              },
              { // DOUBLE_QUOTE_STRING_END
                  { ERROR, false, true, false },
                  { DOUBLE_QUOTE_STRING, true, false, true },
                  { ERROR, false, true, false }
              },
              { // SINGLE_QUOTE_STRING
                  { SINGLE_QUOTE_STRING_END, false, true, false },
                  { SINGLE_QUOTE_STRING, true, false, true },
                  { SINGLE_QUOTE_STRING, true, false, true }
              },
              { // NO_QUOTE_STRING
                  { NO_QUOTE_STRING, true, true, false },
                  { NO_QUOTE_STRING, true, true, false },
                  { NO_QUOTE_STRING, true, true, false }
              },
              { // ERROR
                  { ERROR, false, true, false },
                  { ERROR, false, true, false },
                  { ERROR, false, true, false }
              },
              { // SINGLE_QUOTE_STRING_END
                  { SINGLE_QUOTE_STRING, true, false, true },
                  { ERROR, false, true, false },
                  { ERROR, false, true, false }
              }
            };
            _transitionTable.swap(transitionTable);
      // clang-format on
    }
  }
}
