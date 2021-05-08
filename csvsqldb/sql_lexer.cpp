//
//  sql_lexer.cpp
//  csv db
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


#include "sql_lexer.h"

#include "base/logging.h"
#include "types.h"

// clang-format off
#include "base/pragma_push.h"
#include "base/pragma_conversion.h"
#include "base/pragma_sign.h"
#include "base/pragma_shadow.h"
#include "base/pragma_non_virtual_dtor.h"
#include "lex.yy.h"
#include "base/pragma_pop.h"
// clang-format on


namespace csvsqldb
{
  class SQLLexer::Impl
  {
  public:
    Impl() = default;

    void setInput(std::string_view input)
    {
      _stream.clear();
      _stream << input;
      _lexer.reset(new Lexer{_stream});
    }

    csvsqldb::Token next()
    {
      return _lexer->lex();
    }

  private:
    std::stringstream _stream;
    std::unique_ptr<Lexer> _lexer;
  };

  SQLLexer::SQLLexer()
  : _impl{new Impl}
  {
  }

  SQLLexer::~SQLLexer()
  {
  }

  void SQLLexer::setInput(std::string_view input)
  {
    _impl->setInput(input);
  }

  csvsqldb::Token SQLLexer::next()
  {
    csvsqldb::Token tok = _impl->next();
    while (tok._token == TOK_COMMENT) {
      tok = _impl->next();
    }

    return tok;
  }
}
