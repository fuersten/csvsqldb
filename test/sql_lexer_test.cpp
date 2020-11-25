//
//  csvsqldb test
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


#include <csvsqldb/sql_lexer.h>

#include <catch2/catch.hpp>


TEST_CASE("SQL Lexer Test", "[sql]")
{
  SECTION("token to string")
  {
    CHECK("UNDEFINED" == csvsqldb::tokenToString(static_cast<csvsqldb::eToken>(csvsqldb::lexer::UNDEFINED)));
    CHECK("NEWLINE" == csvsqldb::tokenToString(static_cast<csvsqldb::eToken>(csvsqldb::lexer::NEWLINE)));
    CHECK("WHITESPACE" == csvsqldb::tokenToString(static_cast<csvsqldb::eToken>(csvsqldb::lexer::WHITESPACE)));
    CHECK("EOI" == csvsqldb::tokenToString(static_cast<csvsqldb::eToken>(csvsqldb::lexer::EOI)));

    CHECK("+" == csvsqldb::tokenToString(csvsqldb::TOK_ADD));
    CHECK("ADD" == csvsqldb::tokenToString(csvsqldb::TOK_ADD_KEYWORD));
    CHECK("ALTER" == csvsqldb::tokenToString(csvsqldb::TOK_ALTER));
    CHECK("ALL" == csvsqldb::tokenToString(csvsqldb::TOK_ALL));
    CHECK("AND" == csvsqldb::tokenToString(csvsqldb::TOK_AND));
    CHECK("ARBITRARY" == csvsqldb::tokenToString(csvsqldb::TOK_ARBITRARY));
    CHECK("AS" == csvsqldb::tokenToString(csvsqldb::TOK_AS));
    CHECK("ASC" == csvsqldb::tokenToString(csvsqldb::TOK_ASC));
    CHECK("AST" == csvsqldb::tokenToString(csvsqldb::TOK_AST));
    CHECK("*" == csvsqldb::tokenToString(csvsqldb::TOK_ASTERISK));
    CHECK("AVG" == csvsqldb::tokenToString(csvsqldb::TOK_AVG));
    CHECK("BETWEEN" == csvsqldb::tokenToString(csvsqldb::TOK_BETWEEN));
    CHECK("BOOLEAN" == csvsqldb::tokenToString(csvsqldb::TOK_BOOL));
    CHECK("BY" == csvsqldb::tokenToString(csvsqldb::TOK_BY));
    CHECK("CAST" == csvsqldb::tokenToString(csvsqldb::TOK_CAST));
    CHECK("CHARACTER" == csvsqldb::tokenToString(csvsqldb::TOK_CHAR));
    CHECK("CHECK" == csvsqldb::tokenToString(csvsqldb::TOK_CHECK));
    CHECK("COLLATE" == csvsqldb::tokenToString(csvsqldb::TOK_COLLATE));
    CHECK("COLUMN" == csvsqldb::tokenToString(csvsqldb::TOK_COLUMN));
    CHECK("," == csvsqldb::tokenToString(csvsqldb::TOK_COMMA));
    CHECK("comment" == csvsqldb::tokenToString(csvsqldb::TOK_COMMENT));
    CHECK("||" == csvsqldb::tokenToString(csvsqldb::TOK_CONCAT));
    CHECK("CONSTRAINT" == csvsqldb::tokenToString(csvsqldb::TOK_CONSTRAINT));
    CHECK("boolean constant" == csvsqldb::tokenToString(csvsqldb::TOK_CONST_BOOLEAN));
    CHECK("char constant" == csvsqldb::tokenToString(csvsqldb::TOK_CONST_CHAR));
    CHECK("date constant" == csvsqldb::tokenToString(csvsqldb::TOK_CONST_DATE));
    CHECK("integer constant" == csvsqldb::tokenToString(csvsqldb::TOK_CONST_INTEGER));
    CHECK("real constant" == csvsqldb::tokenToString(csvsqldb::TOK_CONST_REAL));
    CHECK("string constant" == csvsqldb::tokenToString(csvsqldb::TOK_CONST_STRING));
    CHECK("COUNT" == csvsqldb::tokenToString(csvsqldb::TOK_COUNT));
    CHECK("CREATE" == csvsqldb::tokenToString(csvsqldb::TOK_CREATE));
    CHECK("CROSS" == csvsqldb::tokenToString(csvsqldb::TOK_CROSS));
    CHECK("CURRENT_DATE" == csvsqldb::tokenToString(csvsqldb::TOK_CURRENT_DATE));
    CHECK("CURRENT_TIME" == csvsqldb::tokenToString(csvsqldb::TOK_CURRENT_TIME));
    CHECK("CURRENT_TIMESTAMP" == csvsqldb::tokenToString(csvsqldb::TOK_CURRENT_TIMESTAMP));
    CHECK("DATE" == csvsqldb::tokenToString(csvsqldb::TOK_DATE));
    CHECK("DAY" == csvsqldb::tokenToString(csvsqldb::TOK_DAY));
    CHECK("DEFAULT" == csvsqldb::tokenToString(csvsqldb::TOK_DEFAULT));
    CHECK("DESC" == csvsqldb::tokenToString(csvsqldb::TOK_DESC));
    CHECK("DESCRIBE" == csvsqldb::tokenToString(csvsqldb::TOK_DESCRIBE));
    CHECK("DISTINCT" == csvsqldb::tokenToString(csvsqldb::TOK_DISTINCT));
    CHECK("/" == csvsqldb::tokenToString(csvsqldb::TOK_DIV));
    CHECK("." == csvsqldb::tokenToString(csvsqldb::TOK_DOT));
    CHECK("DROP" == csvsqldb::tokenToString(csvsqldb::TOK_DROP));
    CHECK("=" == csvsqldb::tokenToString(csvsqldb::TOK_EQUAL));
    CHECK("EXCEPT" == csvsqldb::tokenToString(csvsqldb::TOK_EXCEPT));
    CHECK("EXEC" == csvsqldb::tokenToString(csvsqldb::TOK_EXEC));
    CHECK("EXISTS" == csvsqldb::tokenToString(csvsqldb::TOK_EXISTS));
    CHECK("EXPLAIN" == csvsqldb::tokenToString(csvsqldb::TOK_EXPLAIN));
    CHECK("EXTRACT" == csvsqldb::tokenToString(csvsqldb::TOK_EXTRACT));
    CHECK("FROM" == csvsqldb::tokenToString(csvsqldb::TOK_FROM));
    CHECK("FULL" == csvsqldb::tokenToString(csvsqldb::TOK_FULL));
    CHECK(">" == csvsqldb::tokenToString(csvsqldb::TOK_GREATER));
    CHECK(">=" == csvsqldb::tokenToString(csvsqldb::TOK_GREATEREQUAL));
    CHECK("GROUP" == csvsqldb::tokenToString(csvsqldb::TOK_GROUP));
    CHECK("HAVING" == csvsqldb::tokenToString(csvsqldb::TOK_HAVING));
    CHECK("HOUR" == csvsqldb::tokenToString(csvsqldb::TOK_HOUR));
    CHECK("identifier" == csvsqldb::tokenToString(csvsqldb::TOK_IDENTIFIER));
    CHECK("IF" == csvsqldb::tokenToString(csvsqldb::TOK_IF));
    CHECK("IN" == csvsqldb::tokenToString(csvsqldb::TOK_IN));
    CHECK("INNER" == csvsqldb::tokenToString(csvsqldb::TOK_INNER));
    CHECK("INTEGER" == csvsqldb::tokenToString(csvsqldb::TOK_INT));
    CHECK("INTERSECT" == csvsqldb::tokenToString(csvsqldb::TOK_INTERSECT));
    CHECK("IS" == csvsqldb::tokenToString(csvsqldb::TOK_IS));
    CHECK("JOIN" == csvsqldb::tokenToString(csvsqldb::TOK_JOIN));
    CHECK("KEY" == csvsqldb::tokenToString(csvsqldb::TOK_KEY));
    CHECK("LEFT" == csvsqldb::tokenToString(csvsqldb::TOK_LEFT));
    CHECK("(" == csvsqldb::tokenToString(csvsqldb::TOK_LEFT_PAREN));
    CHECK("LIKE" == csvsqldb::tokenToString(csvsqldb::TOK_LIKE));
    CHECK("LIMIT" == csvsqldb::tokenToString(csvsqldb::TOK_LIMIT));
    CHECK("MAPPING" == csvsqldb::tokenToString(csvsqldb::TOK_MAPPING));
    CHECK("MAX" == csvsqldb::tokenToString(csvsqldb::TOK_MAX));
    CHECK("MIN" == csvsqldb::tokenToString(csvsqldb::TOK_MIN));
    CHECK("MINUTE" == csvsqldb::tokenToString(csvsqldb::TOK_MINUTE));
    CHECK("%" == csvsqldb::tokenToString(csvsqldb::TOK_MOD));
    CHECK("MONTH" == csvsqldb::tokenToString(csvsqldb::TOK_MONTH));
    CHECK("NATURAL" == csvsqldb::tokenToString(csvsqldb::TOK_NATURAL));
    CHECK("none" == csvsqldb::tokenToString(csvsqldb::TOK_NONE));
    CHECK("NOT" == csvsqldb::tokenToString(csvsqldb::TOK_NOT));
    CHECK("<>" == csvsqldb::tokenToString(csvsqldb::TOK_NOTEQUAL));
    CHECK("NULL" == csvsqldb::tokenToString(csvsqldb::TOK_NULL));
    CHECK("OFFSET" == csvsqldb::tokenToString(csvsqldb::TOK_OFFSET));
    CHECK("ON" == csvsqldb::tokenToString(csvsqldb::TOK_ON));
    CHECK("OR" == csvsqldb::tokenToString(csvsqldb::TOK_OR));
    CHECK("ORDER" == csvsqldb::tokenToString(csvsqldb::TOK_ORDER));
    CHECK("OUTER" == csvsqldb::tokenToString(csvsqldb::TOK_OUTER));
    CHECK("PRIMARY" == csvsqldb::tokenToString(csvsqldb::TOK_PRIMARY));
    CHECK("quoted identifier" == csvsqldb::tokenToString(csvsqldb::TOK_QUOTED_IDENTIFIER));
    CHECK("REAL" == csvsqldb::tokenToString(csvsqldb::TOK_REAL));
    CHECK("RIGHT" == csvsqldb::tokenToString(csvsqldb::TOK_RIGHT));
    CHECK(")" == csvsqldb::tokenToString(csvsqldb::TOK_RIGHT_PAREN));
    CHECK("SECOND" == csvsqldb::tokenToString(csvsqldb::TOK_SECOND));
    CHECK("SELECT" == csvsqldb::tokenToString(csvsqldb::TOK_SELECT));
    CHECK(";" == csvsqldb::tokenToString(csvsqldb::TOK_SEMICOLON));
    CHECK("SHOW" == csvsqldb::tokenToString(csvsqldb::TOK_SHOW));
    CHECK("<" == csvsqldb::tokenToString(csvsqldb::TOK_SMALLER));
    CHECK("<=" == csvsqldb::tokenToString(csvsqldb::TOK_SMALLEREQUAL));
    CHECK("STRING" == csvsqldb::tokenToString(csvsqldb::TOK_STRING));
    CHECK("-" == csvsqldb::tokenToString(csvsqldb::TOK_SUB));
    CHECK("SUM" == csvsqldb::tokenToString(csvsqldb::TOK_SUM));
    CHECK("TABLE" == csvsqldb::tokenToString(csvsqldb::TOK_TABLE));
    CHECK("TIME" == csvsqldb::tokenToString(csvsqldb::TOK_TIME));
    CHECK("TIMESTAMP" == csvsqldb::tokenToString(csvsqldb::TOK_TIMESTAMP));
    CHECK("UNION" == csvsqldb::tokenToString(csvsqldb::TOK_UNION));
    CHECK("UNIQUE" == csvsqldb::tokenToString(csvsqldb::TOK_UNIQUE));
    CHECK("USING" == csvsqldb::tokenToString(csvsqldb::TOK_USING));
    CHECK("VARYING" == csvsqldb::tokenToString(csvsqldb::TOK_VARYING));
    CHECK("WHERE" == csvsqldb::tokenToString(csvsqldb::TOK_WHERE));
    CHECK("YEAR" == csvsqldb::tokenToString(csvsqldb::TOK_YEAR));

    CHECK_THROWS(csvsqldb::tokenToString(static_cast<csvsqldb::eToken>(512)));
  }
}
