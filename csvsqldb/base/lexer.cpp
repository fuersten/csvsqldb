//
//  lexer.cpp
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

#include "lexer.h"

#include "exception.h"


namespace csvsqldb
{
  namespace lexer
  {
    Lexer::Lexer(InspectTokenCallback callback)
    : _callback(callback)
    , _pos(_input.cbegin())
    , _end(_input.cend())
    , _lineStart(_pos)
    {
      addDefinition("newline", R"(\r|\n)", NEWLINE);
      addDefinition("whitespace", R"([ \t\f]+)", WHITESPACE);
    }

    void Lexer::addDefinition(std::string name, std::string r, int32_t token)
    {
      _tokenDefinitions.push_back(TokenDefinition(std::move(name), std::regex(std::move(r)), token));
    }

    void Lexer::setInput(std::string input)
    {
      _input = std::move(input);
      _pos = _input.cbegin();
      _end = _input.cend();
      _lineCount = 1;
      _lineStart = _pos;
    }

    Token Lexer::next()
    {
      if (_pos < _end) {
        std::smatch match;

        for (const auto& r : _tokenDefinitions) {
          if (regex_search(_pos, _end, match, r._rx)) {
            if (_pos == _pos + match.position()) {
              Token token;
              token._name = r._name;
              token._token = r._token;
              if (match.size() > 1) {
                token._value = match[1].str();
              } else {
                token._value = match[0].str();
              }
              token._lineCount = _lineCount;
              token._charCount = static_cast<uint16_t>(std::distance(_lineStart, _pos)) + 1;

              _pos = _pos + match.length();

              if (token._token == WHITESPACE) {
                return next();
              } else if (token._token == NEWLINE) {
                ++_lineCount;
                _lineStart = _pos;
                return next();
              } else {
                std::regex nl(R"(\r|\n)");

                std::sregex_iterator it(token._value.begin(), token._value.end(), nl);
                std::sregex_iterator it_end;

                while (it != it_end) {
                  ++_lineCount;
                  _lineStart = _pos;
                  ++it;
                }
              }
              if (_callback) {
                _callback(token);
              }
              return token;
            }
          }
        }
      } else {
        Token ret;
        ret._token = EOI;
        return ret;
      }
      // didn't find any matching regex
      CSVSQLDB_THROW(LexicalAnalysisException,
                     "could not match any regex at line " << _lineCount << ":" << (std::distance(_lineStart, _pos) + 1));
    }
  }
}
