//
//  tokens.cpp
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


#include "tokens.h"

#include "base/string_helper.h"


namespace csvsqldb
{
  std::string tokenToString(eToken token)
  {
    switch (token) {
      case TOK_EOI:
        return "eoi";
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

  void Token::setIdentifier(const std::string& s)
  {
    _value = toupper_copy(s);
  }

  void Token::setQuotedIdentifier(const std::string& s)
  {
    //_value = toupper_copy(s.substr(1, s.length()-2));
    _value = s.substr(1, s.length()-2);
  }

  void Token::setString(const std::string& s)
  {
    _value = s.substr(1, s.length()-2);
  }
}
