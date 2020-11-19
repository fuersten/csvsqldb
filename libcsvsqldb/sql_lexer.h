//
//  sql_lexer.h
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

#pragma once

#include "libcsvsqldb/inc.h"

#include "base/lexer.h"

#include <map>


namespace csvsqldb
{
  enum eToken {
    TOK_ADD,
    TOK_ADD_KEYWORD,
    TOK_ALL,
    TOK_ALTER,
    TOK_AND,
    TOK_ARBITRARY,
    TOK_AS,
    TOK_ASC,
    TOK_AST,
    TOK_ASTERISK,
    TOK_AVG,
    TOK_BETWEEN,
    TOK_BOOL,
    TOK_BY,
    TOK_CAST,
    TOK_CHAR,
    TOK_CHECK,
    TOK_COLLATE,
    TOK_COLUMN,
    TOK_COMMA,
    TOK_COMMENT,
    TOK_CONCAT,
    TOK_CONSTRAINT,
    TOK_CONST_BOOLEAN,
    TOK_CONST_CHAR,
    TOK_CONST_DATE,
    TOK_CONST_INTEGER,
    TOK_CONST_REAL,
    TOK_CONST_STRING,
    TOK_COUNT,
    TOK_CREATE,
    TOK_CROSS,
    TOK_CURRENT_DATE,
    TOK_CURRENT_TIME,
    TOK_CURRENT_TIMESTAMP,
    TOK_DATE,
    TOK_DAY,
    TOK_DEFAULT,
    TOK_DESC,
    TOK_DESCRIBE,
    TOK_DISTINCT,
    TOK_DIV,
    TOK_DOT,
    TOK_DROP,
    TOK_EQUAL,
    TOK_EXCEPT,
    TOK_EXEC,
    TOK_EXISTS,
    TOK_EXPLAIN,
    TOK_EXTRACT,
    TOK_FROM,
    TOK_FULL,
    TOK_GREATER,
    TOK_GREATEREQUAL,
    TOK_GROUP,
    TOK_HAVING,
    TOK_HOUR,
    TOK_IDENTIFIER,
    TOK_IF,
    TOK_IN,
    TOK_INNER,
    TOK_INT,
    TOK_INTERSECT,
    TOK_IS,
    TOK_JOIN,
    TOK_KEY,
    TOK_LEFT,
    TOK_LEFT_PAREN,
    TOK_LIKE,
    TOK_LIMIT,
    TOK_MAPPING,
    TOK_MAX,
    TOK_MIN,
    TOK_MINUTE,
    TOK_MOD,
    TOK_MONTH,
    TOK_NATURAL,
    TOK_NONE,
    TOK_NOT,
    TOK_NOTEQUAL,
    TOK_NULL,
    TOK_OFFSET,
    TOK_ON,
    TOK_OR,
    TOK_ORDER,
    TOK_OUTER,
    TOK_PRIMARY,
    TOK_QUOTED_IDENTIFIER,
    TOK_REAL,
    TOK_RIGHT,
    TOK_RIGHT_PAREN,
    TOK_SECOND,
    TOK_SELECT,
    TOK_SEMICOLON,
    TOK_SHOW,
    TOK_SMALLER,
    TOK_SMALLEREQUAL,
    TOK_STRING,
    TOK_SUB,
    TOK_SUM,
    TOK_TABLE,
    TOK_TIME,
    TOK_TIMESTAMP,
    TOK_UNION,
    TOK_UNIQUE,
    TOK_USING,
    TOK_VARYING,
    TOK_WHERE,
    TOK_YEAR
  };

  std::string tokenToString(eToken token);

  class CSVSQLDB_EXPORT SQLLexer
  {
  public:
    SQLLexer(const std::string& input);

    SQLLexer(const SQLLexer&) = delete;
    SQLLexer& operator=(const SQLLexer&) = delete;
    SQLLexer(SQLLexer&&) = delete;
    SQLLexer& operator=(SQLLexer&&) = delete;

    csvsqldb::lexer::Token next();

    void setInput(const std::string& input);

  private:
    typedef std::map<std::string, eToken> Keywords;

    void initDefinitions();
    void initKeywords();
    void inspectToken(csvsqldb::lexer::Token& token);

    csvsqldb::lexer::Lexer _lexer;
    Keywords _keywords;
  };
}