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
#include "base/string_helper.h"


namespace csvsqldb
{
  std::string tokenToString(eToken token)
  {
    switch (static_cast<csvsqldb::lexer::eToken>(token)) {
      case csvsqldb::lexer::UNDEFINED:
        return "UNDEFINED";
      case csvsqldb::lexer::NEWLINE:
        return "NEWLINE";
      case csvsqldb::lexer::WHITESPACE:
        return "WHITESPACE";
      case csvsqldb::lexer::EOI:
        return "EOI";
    }

    switch (token) {
      case TOK_NONE:
        return "none";
      case TOK_IDENTIFIER:
        return "identifier";
      case TOK_QUOTED_IDENTIFIER:
        return "quoted identifier";
      case TOK_CONST_STRING:
        return "string constant";
      case TOK_CONST_INTEGER:
        return "integer constant";
      case TOK_CONST_BOOLEAN:
        return "boolean constant";
      case TOK_CONST_DATE:
        return "date constant";
      case TOK_CONST_REAL:
        return "real constant";
      case TOK_CONST_CHAR:
        return "char constant";
      case TOK_DOT:
        return ".";
      case TOK_DESCRIBE:
        return "DESCRIBE";
      case TOK_AST:
        return "AST";
      case TOK_SMALLER:
        return "<";
      case TOK_GREATER:
        return ">";
      case TOK_EQUAL:
        return "=";
      case TOK_NOTEQUAL:
        return "<>";
      case TOK_GREATEREQUAL:
        return ">=";
      case TOK_SMALLEREQUAL:
        return "<=";
      case TOK_CONCAT:
        return "||";
      case TOK_ADD:
        return "+";
      case TOK_SUB:
        return "-";
      case TOK_DIV:
        return "/";
      case TOK_MOD:
        return "%";
      case TOK_ASTERISK:
        return "*";
      case TOK_COMMA:
        return ",";
      case TOK_SEMICOLON:
        return ";";
      case TOK_LEFT_PAREN:
        return "(";
      case TOK_RIGHT_PAREN:
        return ")";
      case TOK_COMMENT:
        return "comment";
      case TOK_UNION:
        return "UNION";
      case TOK_INTERSECT:
        return "INTERSECT";
      case TOK_EXCEPT:
        return "EXCEPT";
      case TOK_SELECT:
        return "SELECT";
      case TOK_AS:
        return "AS";
      case TOK_ALL:
        return "ALL";
      case TOK_CAST:
        return "CAST";
      case TOK_DISTINCT:
        return "DISTINCT";
      case TOK_FROM:
        return "FROM";
      case TOK_WHERE:
        return "WHERE";
      case TOK_ON:
        return "ON";
      case TOK_USING:
        return "USING";
      case TOK_NATURAL:
        return "NATURAL";
      case TOK_LEFT:
        return "LEFT";
      case TOK_RIGHT:
        return "RIGHT";
      case TOK_INNER:
        return "INNER";
      case TOK_OUTER:
        return "OUTER";
      case TOK_CROSS:
        return "CROSS";
      case TOK_FULL:
        return "FULL";
      case TOK_JOIN:
        return "JOIN";
      case TOK_LIKE:
        return "LIKE";
      case TOK_AND:
        return "AND";
      case TOK_OR:
        return "OR";
      case TOK_NOT:
        return "NOT";
      case TOK_IS:
        return "IS";
      case TOK_NULL:
        return "NULL";
      case TOK_BETWEEN:
        return "BETWEEN";
      case TOK_IN:
        return "IN";
      case TOK_EXISTS:
        return "EXISTS";
      case TOK_GROUP:
        return "GROUP";
      case TOK_ORDER:
        return "ORDER";
      case TOK_BY:
        return "BY";
      case TOK_COLLATE:
        return "COLLATE";
      case TOK_ASC:
        return "ASC";
      case TOK_DESC:
        return "DESC";
      case TOK_HAVING:
        return "HAVING";
      case TOK_LIMIT:
        return "LIMIT";
      case TOK_OFFSET:
        return "OFFSET";
      case TOK_CREATE:
        return "CREATE";
      case TOK_TABLE:
        return "TABLE";
      case TOK_IF:
        return "IF";
      case TOK_CONSTRAINT:
        return "CONSTRAINT";
      case TOK_PRIMARY:
        return "PRIMARY";
      case TOK_KEY:
        return "KEY";
      case TOK_UNIQUE:
        return "UNIQUE";
      case TOK_DEFAULT:
        return "DEFAULT";
      case TOK_CHECK:
        return "CHECK";
      case TOK_BOOL:
        return "BOOLEAN";
      case TOK_INT:
        return "INTEGER";
      case TOK_REAL:
        return "REAL";
      case TOK_STRING:
        return "STRING";
      case TOK_CHAR:
        return "CHARACTER";
      case TOK_DATE:
        return "DATE";
      case TOK_TIME:
        return "TIME";
      case TOK_TIMESTAMP:
        return "TIMESTAMP";
      case TOK_ALTER:
        return "ALTER";
      case TOK_COLUMN:
        return "COLUMN";
      case TOK_DROP:
        return "DROP";
      case TOK_ADD_KEYWORD:
        return "ADD";
      case TOK_SUM:
        return "SUM";
      case TOK_COUNT:
        return "COUNT";
      case TOK_AVG:
        return "AVG";
      case TOK_MAX:
        return "MAX";
      case TOK_MIN:
        return "MIN";
      case TOK_CURRENT_DATE:
        return "CURRENT_DATE";
      case TOK_CURRENT_TIME:
        return "CURRENT_TIME";
      case TOK_CURRENT_TIMESTAMP:
        return "CURRENT_TIMESTAMP";
      case TOK_VARYING:
        return "VARYING";
      case TOK_EXTRACT:
        return "EXTRACT";
      case TOK_SECOND:
        return "SECOND";
      case TOK_MINUTE:
        return "MINUTE";
      case TOK_HOUR:
        return "HOUR";
      case TOK_YEAR:
        return "YEAR";
      case TOK_MONTH:
        return "MONTH";
      case TOK_DAY:
        return "DAY";
      case TOK_EXPLAIN:
        return "EXPLAIN";
      case TOK_SHOW:
        return "SHOW";
      case TOK_MAPPING:
        return "MAPPING";
      case TOK_EXEC:
        return "EXEC";
      case TOK_ARBITRARY:
        return "ARBITRARY";
    }
    throw std::runtime_error("just to make VC2013 happy");
  }

  SQLLexer::SQLLexer(const std::string& input)
  : _lexer(std::bind(&SQLLexer::inspectToken, this, std::placeholders::_1))
  {
    initDefinitions();
    initKeywords();

    _lexer.setInput(input);
  }

  void SQLLexer::setInput(const std::string& input)
  {
    _lexer.setInput(input);
  }

  void SQLLexer::initDefinitions()
  {
    _lexer.addDefinition("bool", R"([tT][rR][uU][eE])", TOK_CONST_BOOLEAN);
    _lexer.addDefinition("bool", R"([fF][aA][lL][sS][eE])", TOK_CONST_BOOLEAN);
    _lexer.addDefinition("bool", R"([uU][nN][kK][nN][oO][wW][nN])", TOK_CONST_BOOLEAN);
    _lexer.addDefinition("identifier", R"([_a-zA-Z][_a-zA-Z0-9]*)", TOK_IDENTIFIER);
    _lexer.addDefinition("quoted identifier", R"(\"([^\"]*)\")", TOK_QUOTED_IDENTIFIER);
    _lexer.addDefinition("char", R"('([^'])')", TOK_CONST_CHAR);
    _lexer.addDefinition("string", R"('([^']*)')", TOK_CONST_STRING);
    _lexer.addDefinition("concat", R"(\|\|)", TOK_CONCAT);
    _lexer.addDefinition("add", R"(\+)", TOK_ADD);
    _lexer.addDefinition("comment", R"(--([^\r\n]*))", TOK_COMMENT);
    _lexer.addDefinition("sub", R"(-)", TOK_SUB);
    _lexer.addDefinition("point", R"(\.)", TOK_DOT);
    _lexer.addDefinition("real", R"((?:0|[1-9]\d*)\.\d+)", TOK_CONST_REAL);
    _lexer.addDefinition("integer", R"(0|[1-9]\d*)", TOK_CONST_INTEGER);
    _lexer.addDefinition("string", R"('([^']*)')", TOK_CONST_STRING);
    _lexer.addDefinition("equal", R"(=)", TOK_EQUAL);
    _lexer.addDefinition("greater equal", R"(>=)", TOK_GREATEREQUAL);
    _lexer.addDefinition("smaller equal", R"(<=)", TOK_SMALLEREQUAL);
    _lexer.addDefinition("notequal", R"(<>)", TOK_NOTEQUAL);
    _lexer.addDefinition("smaller", R"(<)", TOK_SMALLER);
    _lexer.addDefinition("greater", R"(>)", TOK_GREATER);
    _lexer.addDefinition("comma", R"(,)", TOK_COMMA);
    _lexer.addDefinition("semicolon", R"(;)", TOK_SEMICOLON);
    _lexer.addDefinition("asterisk", R"(\*)", TOK_ASTERISK);
    _lexer.addDefinition("left_paren", R"(\()", TOK_LEFT_PAREN);
    _lexer.addDefinition("right_paren", R"(\))", TOK_RIGHT_PAREN);
    _lexer.addDefinition("comment", R"(/\*((.|[\r\n])*?)\*/)", TOK_COMMENT);
    _lexer.addDefinition("div", R"(/)", TOK_DIV);
    _lexer.addDefinition("mod", R"(%)", TOK_MOD);
  }

  void SQLLexer::initKeywords()
  {
    _keywords["UNION"] = eToken(TOK_UNION);
    _keywords["SELECT"] = eToken(TOK_SELECT);
    _keywords["FROM"] = eToken(TOK_FROM);
    _keywords["WHERE"] = eToken(TOK_WHERE);
    _keywords["USING"] = eToken(TOK_USING);
    _keywords["INTERSECT"] = eToken(TOK_INTERSECT);
    _keywords["EXCEPT"] = eToken(TOK_EXCEPT);
    _keywords["AS"] = eToken(TOK_AS);
    _keywords["ALL"] = eToken(TOK_ALL);
    _keywords["CAST"] = eToken(TOK_CAST);
    _keywords["DISTINCT"] = eToken(TOK_DISTINCT);
    _keywords["ON"] = eToken(TOK_ON);
    _keywords["NATURAL"] = eToken(TOK_NATURAL);
    _keywords["LEFT"] = eToken(TOK_LEFT);
    _keywords["RIGHT"] = eToken(TOK_RIGHT);
    _keywords["INNER"] = eToken(TOK_INNER);
    _keywords["OUTER"] = eToken(TOK_OUTER);
    _keywords["CROSS"] = eToken(TOK_CROSS);
    _keywords["FULL"] = eToken(TOK_FULL);
    _keywords["JOIN"] = eToken(TOK_JOIN);
    _keywords["LIKE"] = eToken(TOK_LIKE);
    _keywords["AND"] = eToken(TOK_AND);
    _keywords["OR"] = eToken(TOK_OR);
    _keywords["NOT"] = eToken(TOK_NOT);
    _keywords["IS"] = eToken(TOK_IS);
    _keywords["NULL"] = eToken(TOK_NULL);
    _keywords["BETWEEN"] = eToken(TOK_BETWEEN);
    _keywords["IN"] = eToken(TOK_IN);
    _keywords["EXISTS"] = eToken(TOK_EXISTS);
    _keywords["GROUP"] = eToken(TOK_GROUP);
    _keywords["ORDER"] = eToken(TOK_ORDER);
    _keywords["BY"] = eToken(TOK_BY);
    _keywords["COLLATE"] = eToken(TOK_COLLATE);
    _keywords["ASC"] = eToken(TOK_ASC);
    _keywords["DESC"] = eToken(TOK_DESC);
    _keywords["HAVING"] = eToken(TOK_HAVING);
    _keywords["LIMIT"] = eToken(TOK_LIMIT);
    _keywords["OFFSET"] = eToken(TOK_OFFSET);
    _keywords["CREATE"] = eToken(TOK_CREATE);
    _keywords["TABLE"] = eToken(TOK_TABLE);
    _keywords["IF"] = eToken(TOK_IF);
    _keywords["BOOLEAN"] = eToken(TOK_BOOL);
    _keywords["BOOL"] = eToken(TOK_BOOL);
    _keywords["INT"] = eToken(TOK_INT);
    _keywords["INTEGER"] = eToken(TOK_INT);
    _keywords["REAL"] = eToken(TOK_REAL);
    _keywords["FLOAT"] = eToken(TOK_REAL);
    _keywords["DOUBLE"] = eToken(TOK_REAL);
    _keywords["VARCHAR"] = eToken(TOK_STRING);
    _keywords["CHAR"] = eToken(TOK_CHAR);
    _keywords["CHARACTER"] = eToken(TOK_CHAR);
    _keywords["DATE"] = eToken(TOK_DATE);
    _keywords["TIME"] = eToken(TOK_TIME);
    _keywords["TIMESTAMP"] = eToken(TOK_TIMESTAMP);
    _keywords["CONSTRAINT"] = eToken(TOK_CONSTRAINT);
    _keywords["PRIMARY"] = eToken(TOK_PRIMARY);
    _keywords["KEY"] = eToken(TOK_KEY);
    _keywords["UNIQUE"] = eToken(TOK_UNIQUE);
    _keywords["DEFAULT"] = eToken(TOK_DEFAULT);
    _keywords["CHECK"] = eToken(TOK_CHECK);
    _keywords["ALTER"] = eToken(TOK_ALTER);
    _keywords["COLUMN"] = eToken(TOK_COLUMN);
    _keywords["DROP"] = eToken(TOK_DROP);
    _keywords["ADD"] = eToken(TOK_ADD_KEYWORD);
    _keywords["SUM"] = eToken(TOK_SUM);
    _keywords["COUNT"] = eToken(TOK_COUNT);
    _keywords["AVG"] = eToken(TOK_AVG);
    _keywords["MAX"] = eToken(TOK_MAX);
    _keywords["MIN"] = eToken(TOK_MIN);
    _keywords["CURRENT_DATE"] = eToken(TOK_CURRENT_DATE);
    _keywords["CURRENT_TIME"] = eToken(TOK_CURRENT_TIME);
    _keywords["CURRENT_TIMESTAMP"] = eToken(TOK_CURRENT_TIMESTAMP);
    _keywords["VARYING"] = eToken(TOK_VARYING);
    _keywords["DESCRIBE"] = eToken(TOK_DESCRIBE);
    _keywords["AST"] = eToken(TOK_AST);
    _keywords["EXTRACT"] = eToken(TOK_EXTRACT);
    _keywords["SECOND"] = eToken(TOK_SECOND);
    _keywords["MINUTE"] = eToken(TOK_MINUTE);
    _keywords["HOUR"] = eToken(TOK_HOUR);
    _keywords["YEAR"] = eToken(TOK_YEAR);
    _keywords["MONTH"] = eToken(TOK_MONTH);
    _keywords["DAY"] = eToken(TOK_DAY);
    _keywords["EXPLAIN"] = eToken(TOK_EXPLAIN);
    _keywords["SHOW"] = eToken(TOK_SHOW);
    _keywords["MAPPING"] = eToken(TOK_MAPPING);
    _keywords["EXEC"] = eToken(TOK_EXEC);
    _keywords["ARBITRARY"] = eToken(TOK_ARBITRARY);
  }

  void SQLLexer::inspectToken(csvsqldb::lexer::Token& token)
  {
    CSVSQLDB_CLASSLOG(SQLLexer, 2, "Intercepted : " << token._value);

    if (token._token == TOK_IDENTIFIER) {
      Keywords::const_iterator iter = _keywords.find(csvsqldb::toupper(token._value));
      if (iter != _keywords.end()) {
        token._token = iter->second;
        CSVSQLDB_CLASSLOG(SQLLexer, 2, "Intercepted : adapted token");
      }
    }
  }

  csvsqldb::lexer::Token SQLLexer::next()
  {
    csvsqldb::lexer::Token tok = _lexer.next();
    while (tok._token == TOK_COMMENT) {
      tok = _lexer.next();
    }
    return tok;
  }
}
