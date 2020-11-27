//
//  types.h
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
// \file csvsqldb/types.h

#pragma once

#include <csvsqldb/inc.h>

#include <csvsqldb/base/date.h>
#include <csvsqldb/base/exception.h>
#include <csvsqldb/base/time.h>
#include <csvsqldb/base/timestamp.h>
#include <csvsqldb/base/types.h>

#include <any>
#include <string>
#include <vector>


namespace csvsqldb
{
  CSVSQLDB_DECLARE_EXCEPTION(SqlException, csvsqldb::Exception);
  CSVSQLDB_DECLARE_EXCEPTION(SqlParserException, SqlException);


  enum eDescriptionType { AST, EXEC };

  CSVSQLDB_EXPORT std::string descriptionTypeToString(eDescriptionType type);

  enum eOrder { ASC, DESC };

  CSVSQLDB_EXPORT std::string orderToString(eOrder order);

  enum eQuantifier { DISTINCT, ALL };

  enum eType { NONE, BOOLEAN, INT, REAL, STRING, DATE, TIME, TIMESTAMP };

  CSVSQLDB_EXPORT std::string typeToString(eType type);
  CSVSQLDB_EXPORT eType stringToType(const std::string& s);

  using Types = std::vector<eType>;

  using StringType = char*;

  enum eAggregateFunction { SUM, COUNT, COUNT_STAR, AVG, MIN, MAX, ARBITRARY };

  CSVSQLDB_EXPORT std::string aggregateFunctionToString(eAggregateFunction aggregate);

  enum eJoinType {
    CROSS_JOIN,
    INNER_JOIN,
    NATURAL_JOIN,
    LEFT_JOIN,
    RIGHT_JOIN,
    FULL_JOIN,
    NATURAL_LEFT_JOIN,
    NATURAL_RIGHT_JOIN,
    NATURAL_FULL_JOIN
  };

  enum eNaturalJoinType { NATURAL, LEFT, RIGHT, FULL };

  CSVSQLDB_EXPORT std::string naturalJoinToString(eNaturalJoinType joinType);

  struct CSVSQLDB_EXPORT TypedValue {
    TypedValue() = default;

    TypedValue(eType type, const std::any& value)
    : _type(type)
    , _value(value)
    {
    }

    TypedValue(const TypedValue& rhs) = default;
    TypedValue& operator=(const TypedValue& rhs) = default;
    TypedValue(TypedValue&& rhs) = default;
    TypedValue& operator=(TypedValue&& rhs) = default;

    static TypedValue createValue(eType type, const std::string& value);

    eType _type{NONE};
    std::any _value;
  };

  using TypedValues = std::vector<TypedValue>;

  CSVSQLDB_EXPORT std::string typedValueToString(const TypedValue& typedValue);

  enum eOperationType {
    OP_CONCAT,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_GT,
    OP_GE,
    OP_LT,
    OP_LE,
    OP_EQ,
    OP_NEQ,
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_MINUS,
    OP_PLUS,
    OP_CAST,
    OP_LIKE,
    OP_BETWEEN,
    OP_IN,
    OP_IS,
    OP_ISNOT
  };

  CSVSQLDB_EXPORT std::string operationTypeToString(eOperationType type);

  using NoneType = int2type<NONE>;

  template<typename ctype>
  inline eType ctype2eType()
  {
    return NONE;
  }

  template<>
  inline eType ctype2eType<NoneType>()
  {
    return NONE;
  }

  template<>
  inline eType ctype2eType<char>()
  {
    return STRING;
  }

  template<>
  inline eType ctype2eType<bool>()
  {
    return BOOLEAN;
  }

  template<>
  inline eType ctype2eType<int64_t>()
  {
    return INT;
  }

  template<>
  inline eType ctype2eType<double>()
  {
    return REAL;
  }

  template<>
  inline eType ctype2eType<std::string>()
  {
    return STRING;
  }

  template<>
  inline eType ctype2eType<StringType>()
  {
    return STRING;
  }

  template<>
  inline eType ctype2eType<csvsqldb::Date>()
  {
    return DATE;
  }

  template<>
  inline eType ctype2eType<csvsqldb::Time>()
  {
    return TIME;
  }

  template<>
  inline eType ctype2eType<csvsqldb::Timestamp>()
  {
    return TIMESTAMP;
  }
}
