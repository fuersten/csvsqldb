//
//  json_parser.cpp
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

#include "json_parser.h"

#include "exception.h"

#include <cctype>
#include <sstream>


namespace csvsqldb
{
  namespace json
  {
    enum Token {
      NONE,
      LEFT_CURLY_BRACKET,
      RIGHT_CURLY_BRACKET,
      LEFT_SQUARE_BRACKET,
      RIGHT_SQUARE_BRACKET,
      COLON,
      COMMA,
      TRUE,
      FALSE,
      NIL,
      NUMBER,
      STRING
    };

    struct Lexeme {
      std::string _string;
      double _number;
      Token _token;
    };

    class Tokenizer
    {
    public:
      Tokenizer(std::istream& stream)
      : _stream(stream)
      {
        nextChar();
      }

      bool eatWhitespace()
      {
        while (_currentChar == ' ' || _currentChar == '\t' || _currentChar == '\r' || _currentChar == '\n') {
          if (nextChar() == EOF) {
            return false;
          }
        }

        return true;
      }

      Lexeme next()
      {
        Lexeme lex;
        lex._token = NONE;

        if (_currentChar == EOF) {
          return lex;
        }

        if (eatWhitespace()) {
          switch (_currentChar) {
            case '{':
              lex._token = LEFT_CURLY_BRACKET;
              break;
            case '[':
              lex._token = LEFT_SQUARE_BRACKET;
              break;
            case '}':
              lex._token = RIGHT_CURLY_BRACKET;
              break;
            case ']':
              lex._token = RIGHT_SQUARE_BRACKET;
              break;
            case ':':
              lex._token = COLON;
              break;
            case ',':
              lex._token = COMMA;
              break;
            case '"':
              lex._token = STRING;
              break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '-':
              lex._token = NUMBER;
              break;
            case 'f':
              lex._token = FALSE;
              break;
            case 't':
              lex._token = TRUE;
              break;
            case 'n':
              lex._token = NIL;
              break;
            default:
              CSVSQLDB_THROW(JsonException, "illegal token '" << _currentChar << "' found");
          }

          if (lex._token == STRING) {
            // TODO LCF: needs to respect unicode-point representation of JSON strings
            while (safeNextChar() != '"') {
              if (_currentChar == '\\') {
                switch (safeNextChar()) {
                  case '\"':
                    lex._string.append(1, '\"');
                    break;
                  case '\\':
                    lex._string.append(1, '\\');
                    break;
                  case '/':
                    lex._string.append(1, '/');
                    break;
                  case 'b':
                    lex._string.append(1, '\b');
                    break;
                  case 'f':
                    lex._string.append(1, '\f');
                    break;
                  case 'n':
                    lex._string.append(1, '\n');
                    break;
                  case 'r':
                    lex._string.append(1, '\r');
                    break;
                  case 't':
                    lex._string.append(1, '\t');
                    break;
                  default:
                    CSVSQLDB_THROW(JsonException, "illegal escape sequence '\\" << _currentChar << "' found");
                }
              } else {
                lex._string.append(1, _currentChar);
              }
            }
            nextChar();
          } else if (lex._token == NUMBER) {
            std::stringstream ss;
            if (_currentChar == '-') {
              ss << _currentChar;
              safeNextChar();
            }
            if (_currentChar == '0') {
              lex._number = 0.0;
              safeNextChar();
            } else {
              while (std::isdigit(_currentChar)) {
                ss << _currentChar;
                safeNextChar();
              }
              if (_currentChar == '.') {
                ss << _currentChar;
                safeNextChar();
                if (!std::isdigit(_currentChar)) {
                  CSVSQLDB_THROW(JsonException, "bad number format");
                }
                ss << _currentChar;
                safeNextChar();
                while (std::isdigit(_currentChar)) {
                  ss << _currentChar;
                  safeNextChar();
                }
              }
              if (_currentChar == 'e' || _currentChar == 'E') {
                ss << _currentChar;
                safeNextChar();
                if (_currentChar == '+' || _currentChar == '-') {
                  ss << _currentChar;
                  safeNextChar();
                }
                while (std::isdigit(_currentChar)) {
                  ss << _currentChar;
                  safeNextChar();
                }
              }
              lex._number = std::stod(ss.str());
            }
          } else if (lex._token == FALSE) {
            if (safeNextChar() != 'a' || safeNextChar() != 'l' || safeNextChar() != 's' || safeNextChar() != 'e') {
              CSVSQLDB_THROW(JsonException, "illegal token '" << _currentChar << "' found, false expected");
            }
            nextChar();
          } else if (lex._token == TRUE) {
            if (safeNextChar() != 'r' || safeNextChar() != 'u' || safeNextChar() != 'e') {
              CSVSQLDB_THROW(JsonException, "illegal token '" << _currentChar << "' found, true expected");
            }
            nextChar();
          } else if (lex._token == NIL) {
            if (safeNextChar() != 'u' || safeNextChar() != 'l' || safeNextChar() != 'l') {
              CSVSQLDB_THROW(JsonException, "illegal token '" << _currentChar << "' found, null expected");
            }
            nextChar();
          } else {
            nextChar();
          }
        }

        return lex;
      }

    private:
      char nextChar()
      {
        _currentChar = static_cast<char>(_stream.get());
        if (!_stream.good() && !_stream.eof()) {
          CSVSQLDB_THROW(JsonException, "unexpected end of input");
        }

        return _currentChar;
      }

      char safeNextChar()
      {
        if (nextChar() == EOF) {
          CSVSQLDB_THROW(JsonException, "unexpected end of input");
        }
        return _currentChar;
      }

      std::istream& _stream;
      char _currentChar;
    };

    struct Parser::Private {
      Private(const std::string& json, const Callback::Ptr& callback)
      : _ss(json)
      , _tokenizer(_ss)
      , _callback(callback)
      {
      }

      Private(std::istream& stream, const Callback::Ptr& callback)
      : _tokenizer(stream)
      , _callback(callback)
      {
      }

      ~Private()
      {
      }

      void parseObject()
      {
        if (_callback) {
          _callback->startObject();
        }
        Lexeme lex = _tokenizer.next();

        if (lex._token != RIGHT_CURLY_BRACKET) {
          if (lex._token != STRING) {
            CSVSQLDB_THROW(JsonException, "expected a string got " << lex._token);
          }
          bool first(true);
          while (lex._token != RIGHT_CURLY_BRACKET) {
            if (!first) {
              if (lex._token != COMMA) {
                CSVSQLDB_THROW(JsonException, "expected a comma got " << lex._token);
              }
              lex = _tokenizer.next();
            } else {
              first = false;
            }
            if (lex._token != STRING) {
              CSVSQLDB_THROW(JsonException, "expected a string got " << lex._token);
            }
            if (_callback) {
              _callback->key(lex._string);
            }
            lex = _tokenizer.next();
            if (lex._token != COLON) {
              CSVSQLDB_THROW(JsonException, "expected a colon got " << lex._token);
            }
            lex = _tokenizer.next();
            parseValue(lex);
            lex = _tokenizer.next();
          }
        }
        if (_callback) {
          _callback->endObject();
        }
      }

      void parseArray()
      {
        if (_callback) {
          _callback->startArray();
        }
        Lexeme lex = _tokenizer.next();
        if (lex._token != RIGHT_SQUARE_BRACKET) {
          parseValue(lex);
          lex = _tokenizer.next();
          while (lex._token != RIGHT_SQUARE_BRACKET) {
            if (lex._token != COMMA) {
              CSVSQLDB_THROW(JsonException, "expected a comma got " << lex._token);
            }
            lex = _tokenizer.next();
            parseValue(lex);
            lex = _tokenizer.next();
          }
        }
        if (_callback) {
          _callback->endArray();
        }
      }

      void parseValue(Lexeme& lex)
      {
        switch (lex._token) {
          case LEFT_CURLY_BRACKET:
            parseObject();
            break;
          case LEFT_SQUARE_BRACKET:
            parseArray();
            break;
          case STRING:
            if (_callback) {
              _callback->stringValue(lex._string);
            }
            break;
          case NUMBER:
            if (_callback) {
              _callback->numberValue(lex._number);
            }
            break;
          case FALSE:
            if (_callback) {
              _callback->booleanValue(false);
            }
            break;
          case TRUE:
            if (_callback) {
              _callback->booleanValue(true);
            }
            break;
          case NIL:
            if (_callback) {
              _callback->nullValue();
            }
            break;
          case COLON:
          case COMMA:
          case NONE:
          case RIGHT_CURLY_BRACKET:
          case RIGHT_SQUARE_BRACKET:
            CSVSQLDB_THROW(JsonException, "illegal token found " << lex._token);
        }
      }

      std::istringstream _ss;
      Tokenizer _tokenizer;
      Callback::Ptr _callback;
    };

    Parser::Parser(const std::string& json, const Callback::Ptr& callback)
    : _p(new Private(json, callback))
    {
    }

    Parser::Parser(std::istream& stream, const Callback::Ptr& callback)
    : _p(new Private(stream, callback))
    {
    }

    Parser::~Parser()
    {
    }

    bool Parser::parse()
    {
      try {
        Lexeme lex = _p->_tokenizer.next();
        while (lex._token != NONE) {
          if (lex._token == LEFT_CURLY_BRACKET) {
            _p->parseObject();
          } else if (lex._token == LEFT_SQUARE_BRACKET) {
            _p->parseArray();
          } else {
            CSVSQLDB_THROW(JsonException, "illegal token found " << lex._token);
          }
          lex = _p->_tokenizer.next();
        }
      } catch (JsonException&) {
        if (_p->_callback) {
          throw;
        } else {
          return false;
        }
      }
      return true;
    }
  }
}
