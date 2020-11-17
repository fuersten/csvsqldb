//
//  types.cpp
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
// \file libcsvsqldb/types.h

#include "types.h"

#include "base/string_helper.h"
#include "base/time_helper.h"


namespace csvsqldb
{
  CSVSQLDB_IMPLEMENT_EXCEPTION(SqlException, Exception);
  CSVSQLDB_IMPLEMENT_EXCEPTION(SqlParserException, SqlException);


  std::string orderToString(eOrder order)
  {
    switch (order) {
      case ASC:
        return "ASC";
      case DESC:
        return "DESC";
    }
    throw std::runtime_error("just to make VC2013 happy");
  }

  std::string typeToString(eType type)
  {
    switch (type) {
      case NONE:
        return "none";
      case BOOLEAN:
        return "BOOLEAN";
      case INT:
        return "INTEGER";
      case REAL:
        return "REAL";
      case STRING:
        return "VARCHAR";
      case DATE:
        return "DATE";
      case TIME:
        return "TIME";
      case TIMESTAMP:
        return "TIMESTAMP";
    }
    throw std::runtime_error("just to make VC2013 happy");
  }

  eType stringToType(const std::string& s)
  {
    if (s == "BOOLEAN") {
      return BOOLEAN;
    } else if (s == "INTEGER") {
      return INT;
    } else if (s == "REAL") {
      return REAL;
    } else if (s == "VARCHAR") {
      return STRING;
    } else if (s == "DATE") {
      return DATE;
    } else if (s == "TIME") {
      return TIME;
    } else if (s == "TIMESTAMP") {
      return TIMESTAMP;
    } else {
      CSVSQLDB_THROW(SqlException, "unknown type string '" << s << "'");
    }
  }

  std::string printType(eType type, const std::any& value)
  {
    std::stringstream ss;
    switch (type) {
      case NONE:
        ss << "NULL";
        break;
      case BOOLEAN:
        ss << (std::any_cast<bool>(value) ? "TRUE" : "FALSE");
        break;
      case REAL:
        ss << std::fixed << std::showpoint << std::any_cast<double>(value);
        break;
      case INT:
        ss << std::any_cast<int64_t>(value);
        break;
      case STRING:
        ss << std::any_cast<std::string>(value);
        break;
      case DATE:
        ss << "DATE'" << std::any_cast<csvsqldb::Date>(value).format("%Y-%m-%d") << "'";
        break;
      case TIME:
        ss << "TIME'" << std::any_cast<csvsqldb::Time>(value).format("%H:%M:%S") << "'";
        break;
      case TIMESTAMP:
        ss << "TIMESTAMP'" << std::any_cast<csvsqldb::Timestamp>(value).format("%Y-%m-%dT%H:%M:%S") << "'";
        break;
    }
    return ss.str();
  }

  std::string printType(const TypedValue& value)
  {
    return printType(value._type, value._value);
  }

  TypedValue TypedValue::createValue(eType type, const std::string& value)
  {
    std::any any;

    switch (type) {
      case NONE:
        break;
      case BOOLEAN:
        if (csvsqldb::toupper_copy(value) != "UNKNOWN") {
          any = csvsqldb::stringToBool(value);
        }
        break;
      case REAL:
        any = std::stod(value);
        break;
      case INT:
        any = static_cast<int64_t>(std::stol(value));
        break;
      case STRING:
        any = value;
        break;
      case DATE:
        any = csvsqldb::dateFromString(value);
        break;
      case TIME:
        any = csvsqldb::timeFromString(value);
        break;
      case TIMESTAMP:
        any = csvsqldb::timestampFromString(value);
        break;
    }

    return TypedValue(type, any);
  }

  std::string aggregateFunctionToString(eAggregateFunction aggregate)
  {
    switch (aggregate) {
      case SUM:
        return "SUM";
      case AVG:
        return "AVG";
      case COUNT:
        return "COUNT";
      case COUNT_STAR:
        return "COUNT";
      case MAX:
        return "MAX";
      case MIN:
        return "MIN";
      case ARBITRARY:
        return "ARBITRARY";
    }
    throw std::runtime_error("just to make VC2013 happy");
  }

  std::string naturalJoinToString(eNaturalJoinType joinType)
  {
    switch (joinType) {
      case NATURAL:
        return "NATURAL";
      case LEFT:
        return "NATURAL LEFT OUTER";
      case RIGHT:
        return "NATURAL RIGHT OUTER";
      case FULL:
        return "NATURAL FULL OUTER";
    }
    throw std::runtime_error("just to make VC2013 happy");
  }

  std::string operationTypeToString(eOperationType type)
  {
    switch (type) {
      case OP_CONCAT:
        return "||";
      case OP_ADD:
        return "+";
      case OP_SUB:
        return "-";
      case OP_MUL:
        return "*";
      case OP_DIV:
        return "/";
      case OP_MOD:
        return "%";
      case OP_GT:
        return ">";
      case OP_GE:
        return ">=";
      case OP_LT:
        return "<=";
      case OP_LE:
        return "<";
      case OP_EQ:
        return "=";
      case OP_NEQ:
        return "<>";
      case OP_AND:
        return "AND";
      case OP_OR:
        return "OR";
      case OP_NOT:
        return "NOT";
      case OP_MINUS:
        return "-";
      case OP_PLUS:
        return "+";
      case OP_CAST:
        return "CAST";
      case OP_LIKE:
        return "LIKE";
      case OP_BETWEEN:
        return "BETWEEN";
      case OP_IN:
        return "IN";
      case OP_IS:
        return "IS";
      case OP_ISNOT:
        return "IS NOT";
    }
    throw std::runtime_error("just to make VC2013 happy");
  }
}
