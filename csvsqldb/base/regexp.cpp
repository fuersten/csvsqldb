//
//  regexp.cpp
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

#include "regexp.h"

#include "exception.h"

#include <cstring>
#include <list>
#include <vector>


namespace csvsqldb
{
  enum eToken { STAR, PLUS, PIPE, QUEST, LPAREN, RPAREN, CHAR, CHARCLASS, CHARSET, EOP };

  using Groups = std::vector<std::string>;

  struct CharacterSet {
    bool _negate;
    Groups _groups;
  };

  using CharacterSets = std::list<CharacterSet>;

  struct Token {
    Token() = default;

    std::string tostring() const
    {
      switch (_token) {
        case STAR:
          return "*";
        case PLUS:
          return "+";
        case PIPE:
          return "|";
        case QUEST:
          return "?";
        case LPAREN:
          return "(";
        case RPAREN:
          return ")";
        case CHAR:
        case CHARCLASS:
          return std::string(1, _lexeme);
        case CHARSET:
          return "[]";
        case EOP:
          return "eop";
      }
    }

    eToken _token{EOP};
    char _lexeme;
    CharacterSet _charSet;
  };


  class CharacterSetParser
  {
  public:
    CharacterSetParser(std::string::iterator& iter, std::string::iterator end, bool& negate, Groups& groups)
    : _negate(negate)
    , _groups(groups)
    , _iter(iter)
    , _end(end)
    {
      _negate = false;
    }

    void parse()
    {
      char c = nextChar();
      if (c == '^') {
        _negate = true;
        c = nextChar();
      }
      if (c == '-') {
        _groups.push_back("-");
        c = nextChar();
      }

      findSets(c);
    }

    bool& _negate;
    Groups& _groups;

  private:
    void findSets(char c)
    {
      std::string group;

      while (c != ']') {
        if (c == '\\') {
          c = nextChar();
        }
        char c2 = nextChar();
        if (c2 == '-') {
          char c3 = nextChar();
          if (c3 == ']') {
            group += c;
            group += '-';
            _groups.push_back(group);
            return;
          } else {
            _groups.push_back(std::string("R") + c + c3);
          }
        } else {
          group += c;
          if (c2 == ']') {
            _groups.push_back(group);
            return;
          }
          group += c2;
        }
        c = nextChar();
      }
      _groups.push_back(group);
    }

    char nextChar()
    {
      ++_iter;
      if (_iter == _end) {
        CSVSQLDB_THROW(RegExpException, "invalid expression");
      }

      return *_iter;
    }

    std::string::iterator& _iter;
    std::string::iterator _end;
  };

  class Lexer
  {
  public:
    Lexer(std::string&& s)
    : _s(std::move(s))
    {
      _iter = _s.begin();
    }

    Token nextToken()
    {
      Token token;

      if (_iter == _s.end()) {
        return token;
      }

      if (*_iter == '*') {
        token._token = STAR;
      } else if (*_iter == '+') {
        token._token = PLUS;
      } else if (*_iter == '|') {
        token._token = PIPE;
      } else if (*_iter == '?') {
        token._token = QUEST;
      } else if (*_iter == '(') {
        token._token = LPAREN;
      } else if (*_iter == ')') {
        token._token = RPAREN;
      } else if ((*_iter >= '0' && *_iter <= '9') || (*_iter >= 'A' && *_iter <= 'Z') || (*_iter >= 'a' && *_iter <= 'z') ||
                 (*_iter == ' ') || (*_iter == '!') || (*_iter == '"') || (*_iter == '#') || (*_iter == '$') || (*_iter == '%') ||
                 (*_iter == '&') || (*_iter == '\'') || (*_iter == ',') || (*_iter == '-') || (*_iter == '/') ||
                 (*_iter == ':') || (*_iter == ';') || (*_iter == '<') || (*_iter == '=') || (*_iter == '>') || (*_iter == '@') ||
                 (*_iter == '_') || (*_iter == '`') || (*_iter == '{') || (*_iter == '}') || (*_iter == '~')) {
        token._token = CHAR;
        token._lexeme = *_iter;
      } else if (*_iter == '.') {
        token._token = CHARCLASS;
        token._lexeme = *_iter;
      } else if (*_iter == '\0') {
        token._token = EOP;
      } else if (*_iter == '\\') {
        ++_iter;
        if (_iter == _s.end()) {
          CSVSQLDB_THROW(RegExpException, "invalid expression");
        }
        token._token = CHARCLASS;
        if (*_iter == 'w') {
          token._lexeme = 'W';
        } else if (*_iter == 'd') {
          token._lexeme = 'D';
        } else if (*_iter == 's') {
          token._lexeme = 'S';
        } else {
          token._lexeme = *_iter;
          token._token = CHAR;
        }
      } else if (*_iter == '[') {
        token._token = CHARSET;
        CharacterSetParser parser(_iter, _s.end(), token._charSet._negate, token._charSet._groups);
        parser.parse();
      } else {
        CSVSQLDB_THROW(RegExpException, "unrecognized sequence " << *_iter);
      }
      ++_iter;

      return token;
    }

  private:
    std::string _s;
    std::string::iterator _iter;
  };


  struct Transition {
    enum eType { Char, CharClass, CharSet, Epsilon };

    Transition(eType type, char c)
    : _c(c)
    , _type(type)
    {
    }

    Transition(const CharacterSet* charSet)
    : _charSet(charSet)
    , _type(CharSet)
    {
    }

    char _c;
    const CharacterSet* _charSet{nullptr};
    eType _type;
  };

  class State
  {
  public:
    enum eType { Accept, Final };

    State(eType type = Accept, char c = 0, Transition::eType tranType = Transition::Char)
    : _type(type)
    , _tran(tranType, c)
    {
    }

    State(const CharacterSet* charSet)
    : _type(Accept)
    , _tran(charSet)
    {
    }

    bool accept() const
    {
      return _type == Accept;
    }

    bool final() const
    {
      return _type == Final;
    }

    bool epsilon() const
    {
      return _tran._type == Transition::Epsilon;
    }

    bool match(char c) const
    {
      switch (_tran._type) {
        case Transition::Char: {
          return _tran._c == c;
        }
        case Transition::CharClass: {
          if (_tran._c == '.') {
            return true;
          } else if (_tran._c == 'W') {
            return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
          } else if (_tran._c == 'D') {
            return (c >= '0' && c <= '9');
          } else if (_tran._c == 'S') {
            return groupS.find_first_of(c) != std::string::npos;
          }
          break;
        }
        case Transition::CharSet: {
          Groups groups = _tran._charSet->_groups;
          bool negate = _tran._charSet->_negate;

          while (!groups.empty()) {
            std::string group = groups.back();
            groups.pop_back();

            if (group[0] == 'R') {
              char left = group[1];
              char right = group[2];
              if (c >= left && c <= right) {
                return !negate;
              }
            } else {
              for (const auto& tmp : group) {
                if (c == tmp) {
                  return !negate;
                }
              }
            }
          }

          return negate;
        }
        case Transition::Epsilon: {
          CSVSQLDB_THROW(RegExpException, "wrong state");
        }
      }
      return false;
    }

    void setEpsilon()
    {
      _tran._type = Transition::Epsilon;
    }

    void setAccept()
    {
      _type = Accept;
      _tran._type = Transition::Epsilon;
    }

    State* _out1{nullptr};
    State* _out2{nullptr};
    State* _end{nullptr};

  private:
    eType _type;
    Transition _tran;
    static const std::string groupS;
  };

  const std::string State::groupS = " \t\r\n";


  class Parser
  {
  public:
    using States = std::list<State>;
    using WorkStates = std::vector<State*>;

    Parser(std::string&& s)
    : _lexer(std::move(s))
    {
    }

    void parse()
    {
      _tok = _lexer.nextToken();
      _start = expression();
    }

    bool match(const char* s) const
    {
      if (_states.empty()) {
        return ::strlen(s) == 0;
      }

      WorkStates workStates;
      eClosure(workStates, _start);

      WorkStates tmpStates;
      while (*s) {
        while (!workStates.empty()) {
          State* current = workStates.back();
          workStates.pop_back();

          if (current->accept() && current->match(*s)) {
            eClosure(tmpStates, current->_out1);
            eClosure(tmpStates, current->_out2);
          } else if (current->epsilon()) {
            eClosure(workStates, current);
          }
        }
        std::swap(workStates, tmpStates);
        ++s;
      }
      while (!workStates.empty()) {
        State* current = workStates.back();
        workStates.pop_back();

        if (current->final()) {
          return true;
        }
      }

      return false;
    }

    bool match(const std::string& s) const
    {
      return match(s.c_str());
    }

  private:
    void eClosure(WorkStates& states, State* state) const
    {
      if (state && state->epsilon()) {
        if (state->_out1->epsilon()) {
          eClosure(states, state->_out1);
        } else {
          states.emplace(states.end(), state->_out1);
        }
        if (state->_out2 && state->_out2->epsilon()) {
          eClosure(states, state->_out2);
        } else if (state->_out2) {
          states.emplace(states.end(), state->_out2);
        }
      } else if (state) {
        if (state) {
          states.emplace(states.end(), state);
        }
      }
    }

    State* expression()
    {
      State* state = factor();
      if (_tok._token == PIPE) {
        _tok = _lexer.nextToken();
        State* rhs = expression();

        State s(State::Accept);
        State f(State::Final);
        _states.push_back(f);

        s.setEpsilon();
        s._out1 = state;
        s._out2 = rhs;
        s._end = &_states.back();

        state->_end->_out1 = s._end;
        state->_end->setAccept();
        rhs->_end->_out1 = s._end;
        rhs->_end->setAccept();

        _states.push_back(s);

        state = &_states.back();
      }

      if (_tok._token == STAR) {
        _tok = _lexer.nextToken();

        State s(State::Accept);
        State f(State::Final);
        _states.push_back(f);

        s.setEpsilon();
        s._out1 = state;
        s._out2 = &_states.back();
        s._end = &_states.back();

        state->_end->_out1 = state;
        state->_end->_out2 = s._end;
        state->_end->setAccept();

        _states.push_back(s);

        state = &_states.back();
      } else if (_tok._token == PLUS) {
        _tok = _lexer.nextToken();

        State s(State::Accept);
        State f(State::Final);
        _states.push_back(f);

        s.setEpsilon();
        s._out1 = state;
        s._end = &_states.back();

        state->_end->_out1 = state;
        state->_end->_out2 = s._end;
        state->_end->setAccept();

        _states.push_back(s);

        state = &_states.back();
      } else if (_tok._token == QUEST) {
        _tok = _lexer.nextToken();

        State s(State::Accept);
        State f(State::Final);
        _states.push_back(f);

        s.setEpsilon();
        s._out1 = state;
        s._out2 = &_states.back();
        s._end = &_states.back();

        state->_end->_out1 = s._end;
        state->_end->setAccept();

        _states.push_back(s);

        state = &_states.back();
      }

      if (_tok._token != RPAREN && _tok._token != EOP) {
        State* rhs = expression();

        state->_end->_out1 = rhs;
        state->_end->setAccept();
        state->_end = rhs->_end;
      }

      return state;
    }

    State* factor()
    {
      State* state = 0;

      if (_tok._token == LPAREN) {
        _tok = _lexer.nextToken();
        state = expression();
        if (_tok._token == RPAREN) {
          _tok = _lexer.nextToken();
        } else {
          CSVSQLDB_THROW(RegExpException, "invalid expression");
        }
      } else if (_tok._token == CHAR) {
        State f(State::Final);
        _states.push_back(f);

        State s(State::Accept, _tok._lexeme, Transition::Char);
        s._out1 = &_states.back();
        s._end = &_states.back();
        _states.push_back(s);
        state = &_states.back();

        _tok = _lexer.nextToken();
      } else if (_tok._token == CHARCLASS) {
        State f(State::Final);
        _states.push_back(f);

        State s(State::Accept, _tok._lexeme, Transition::CharClass);
        s._out1 = &_states.back();
        s._end = &_states.back();
        _states.push_back(s);
        state = &_states.back();

        _tok = _lexer.nextToken();
      } else if (_tok._token == CHARSET) {
        State f(State::Final);
        _states.push_back(f);

        _charSets.push_back(_tok._charSet);

        State s(&_charSets.back());
        s._out1 = &_states.back();
        s._end = &_states.back();
        _states.push_back(s);
        state = &_states.back();

        _tok = _lexer.nextToken();
      }

      return state;
    }

    Lexer _lexer;
    Token _tok;
    States _states;
    State* _start{nullptr};
    CharacterSets _charSets;
  };


  struct RegExp::Private {
    Private(std::string s)
    : _parser(std::move(s))
    {
    }

    Parser _parser;
  };


  RegExp::RegExp()
  : _m(new Private(""))
  {
    _m->_parser.parse();
  }

  RegExp::~RegExp()
  {
    // Needs to be specified due to the unique_ptr impl.
  }

  RegExp::RegExp(RegExp&& r)
  : _m(std::move(r._m))
  {
  }

  RegExp& RegExp::operator=(RegExp&& r)
  {
    _m = std::move(r._m);
    return *this;
  }

  RegExp::RegExp(std::string s)
  : _m(new Private(std::move(s)))
  {
    _m->_parser.parse();
  }

  RegExp& RegExp::operator=(const std::string& s)
  {
    _m.reset(new Private(s));
    _m->_parser.parse();
    return *this;
  }

  RegExp& RegExp::operator=(std::string&& s)
  {
    _m.reset(new Private(std::move(s)));
    _m->_parser.parse();
    return *this;
  }

  bool RegExp::match(const std::string& s) const
  {
    return _m->_parser.match(s);
  }

  bool RegExp::match(const char* s) const
  {
    return _m->_parser.match(s);
  }
}
