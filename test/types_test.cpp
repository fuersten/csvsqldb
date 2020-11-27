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


#include <csvsqldb/types.h>

#include <catch2/catch.hpp>


TEST_CASE("Types Test", "[types]")
{
  SECTION("string to bool")
  {
    CHECK(csvsqldb::stringToBool("true"));
    CHECK_FALSE(csvsqldb::stringToBool("false"));
    CHECK(csvsqldb::stringToBool("TRUE"));
    CHECK_FALSE(csvsqldb::stringToBool("FaLsE"));
    CHECK(csvsqldb::stringToBool("1"));
    CHECK_FALSE(csvsqldb::stringToBool("0"));
    CHECK(csvsqldb::stringToBool("4711"));

    CHECK_THROWS_WITH(csvsqldb::stringToBool("This is no bool"), "Not a boolean value");
  }

  SECTION("description type to string")
  {
    CHECK("AST" == csvsqldb::descriptionTypeToString(csvsqldb::AST));
    CHECK("EXEC" == csvsqldb::descriptionTypeToString(csvsqldb::EXEC));
    CHECK_THROWS_AS(csvsqldb::descriptionTypeToString(static_cast<csvsqldb::eDescriptionType>(42)), std::runtime_error);
  }

  SECTION("order to string")
  {
    CHECK("ASC" == csvsqldb::orderToString(csvsqldb::ASC));
    CHECK("DESC" == csvsqldb::orderToString(csvsqldb::DESC));
    CHECK_THROWS_AS(csvsqldb::orderToString(static_cast<csvsqldb::eOrder>(42)), std::runtime_error);
  }

  SECTION("type to string")
  {
    CHECK("NONE" == csvsqldb::typeToString(csvsqldb::NONE));
    CHECK("BOOLEAN" == csvsqldb::typeToString(csvsqldb::BOOLEAN));
    CHECK("INTEGER" == csvsqldb::typeToString(csvsqldb::INT));
    CHECK("REAL" == csvsqldb::typeToString(csvsqldb::REAL));
    CHECK("VARCHAR" == csvsqldb::typeToString(csvsqldb::STRING));
    CHECK("DATE" == csvsqldb::typeToString(csvsqldb::DATE));
    CHECK("TIME" == csvsqldb::typeToString(csvsqldb::TIME));
    CHECK("TIMESTAMP" == csvsqldb::typeToString(csvsqldb::TIMESTAMP));

    CHECK_THROWS(csvsqldb::typeToString(static_cast<csvsqldb::eType>(4711)));
  }

  SECTION("string to type")
  {
    CHECK(csvsqldb::BOOLEAN == csvsqldb::stringToType("BOOLEAN"));
    CHECK(csvsqldb::INT == csvsqldb::stringToType("INTEGER"));
    CHECK(csvsqldb::REAL == csvsqldb::stringToType("REAL"));
    CHECK(csvsqldb::STRING == csvsqldb::stringToType("VARCHAR"));
    CHECK(csvsqldb::DATE == csvsqldb::stringToType("DATE"));
    CHECK(csvsqldb::TIME == csvsqldb::stringToType("TIME"));
    CHECK(csvsqldb::TIMESTAMP == csvsqldb::stringToType("TIMESTAMP"));

    CHECK_THROWS(csvsqldb::stringToType("NONE"));
    CHECK_THROWS(csvsqldb::stringToType("Goose Fra Ba"));
  }

  SECTION("aggregate function to string")
  {
    CHECK("SUM" == csvsqldb::aggregateFunctionToString(csvsqldb::SUM));
    CHECK("COUNT" == csvsqldb::aggregateFunctionToString(csvsqldb::COUNT));
    CHECK("COUNT" == csvsqldb::aggregateFunctionToString(csvsqldb::COUNT_STAR));
    CHECK("AVG" == csvsqldb::aggregateFunctionToString(csvsqldb::AVG));
    CHECK("MIN" == csvsqldb::aggregateFunctionToString(csvsqldb::MIN));
    CHECK("MAX" == csvsqldb::aggregateFunctionToString(csvsqldb::MAX));
    CHECK("ARBITRARY" == csvsqldb::aggregateFunctionToString(csvsqldb::ARBITRARY));

    CHECK_THROWS(csvsqldb::aggregateFunctionToString(static_cast<csvsqldb::eAggregateFunction>(4711)));
  }

  SECTION("natural join to string")
  {
    CHECK("NATURAL" == csvsqldb::naturalJoinToString(csvsqldb::eNaturalJoinType::NATURAL));
    CHECK("NATURAL LEFT OUTER" == csvsqldb::naturalJoinToString(csvsqldb::eNaturalJoinType::LEFT));
    CHECK("NATURAL RIGHT OUTER" == csvsqldb::naturalJoinToString(csvsqldb::eNaturalJoinType::RIGHT));
    CHECK("NATURAL FULL OUTER" == csvsqldb::naturalJoinToString(csvsqldb::eNaturalJoinType::FULL));

    CHECK_THROWS(csvsqldb::naturalJoinToString(static_cast<csvsqldb::eNaturalJoinType>(4711)));
  }

  SECTION("operation type to string")
  {
    CHECK("||" == csvsqldb::operationTypeToString(csvsqldb::OP_CONCAT));
    CHECK("+" == csvsqldb::operationTypeToString(csvsqldb::OP_ADD));
    CHECK("-" == csvsqldb::operationTypeToString(csvsqldb::OP_SUB));
    CHECK("*" == csvsqldb::operationTypeToString(csvsqldb::OP_MUL));
    CHECK("/" == csvsqldb::operationTypeToString(csvsqldb::OP_DIV));
    CHECK("%" == csvsqldb::operationTypeToString(csvsqldb::OP_MOD));
    CHECK(">" == csvsqldb::operationTypeToString(csvsqldb::OP_GT));
    CHECK(">=" == csvsqldb::operationTypeToString(csvsqldb::OP_GE));
    CHECK("<" == csvsqldb::operationTypeToString(csvsqldb::OP_LT));
    CHECK("<=" == csvsqldb::operationTypeToString(csvsqldb::OP_LE));
    CHECK("=" == csvsqldb::operationTypeToString(csvsqldb::OP_EQ));
    CHECK("<>" == csvsqldb::operationTypeToString(csvsqldb::OP_NEQ));
    CHECK("AND" == csvsqldb::operationTypeToString(csvsqldb::OP_AND));
    CHECK("OR" == csvsqldb::operationTypeToString(csvsqldb::OP_OR));
    CHECK("NOT" == csvsqldb::operationTypeToString(csvsqldb::OP_NOT));
    CHECK("-" == csvsqldb::operationTypeToString(csvsqldb::OP_MINUS));
    CHECK("+" == csvsqldb::operationTypeToString(csvsqldb::OP_PLUS));
    CHECK("CAST" == csvsqldb::operationTypeToString(csvsqldb::OP_CAST));
    CHECK("LIKE" == csvsqldb::operationTypeToString(csvsqldb::OP_LIKE));
    CHECK("BETWEEN" == csvsqldb::operationTypeToString(csvsqldb::OP_BETWEEN));
    CHECK("IN" == csvsqldb::operationTypeToString(csvsqldb::OP_IN));
    CHECK("IS" == csvsqldb::operationTypeToString(csvsqldb::OP_IS));
    CHECK("IS NOT" == csvsqldb::operationTypeToString(csvsqldb::OP_ISNOT));

    CHECK_THROWS(csvsqldb::operationTypeToString(static_cast<csvsqldb::eOperationType>(4711)));
  }

  SECTION("c type to eType")
  {
    CHECK(csvsqldb::NONE == csvsqldb::ctype2eType<uint>());
    CHECK(csvsqldb::BOOLEAN == csvsqldb::ctype2eType<bool>());
    CHECK(csvsqldb::INT == csvsqldb::ctype2eType<int64_t>());
    CHECK(csvsqldb::REAL == csvsqldb::ctype2eType<double>());
    CHECK(csvsqldb::STRING == csvsqldb::ctype2eType<std::string>());
    CHECK(csvsqldb::STRING == csvsqldb::ctype2eType<csvsqldb::StringType>());
    CHECK(csvsqldb::DATE == csvsqldb::ctype2eType<csvsqldb::Date>());
    CHECK(csvsqldb::TIME == csvsqldb::ctype2eType<csvsqldb::Time>());
    CHECK(csvsqldb::TIMESTAMP == csvsqldb::ctype2eType<csvsqldb::Timestamp>());
  }
}

TEST_CASE("Typed Values Test", "[types]")
{
  SECTION("typed value to string")
  {
    CHECK("NULL" == csvsqldb::typedValueToString(csvsqldb::TypedValue()));
    CHECK("NULL" == csvsqldb::typedValueToString({csvsqldb::NONE, 0}));
    CHECK("TRUE" == csvsqldb::typedValueToString({csvsqldb::BOOLEAN, true}));
    CHECK("-4711" == csvsqldb::typedValueToString({csvsqldb::INT, int64_t{-4711}}));
    CHECK("42" == csvsqldb::typedValueToString({csvsqldb::INT, int64_t{42}}));
    CHECK("47.110000" == csvsqldb::typedValueToString({csvsqldb::REAL, 47.11}));
    CHECK("Hello world!" == csvsqldb::typedValueToString({csvsqldb::STRING, std::string{"Hello world!"}}));
    CHECK("DATE'2020-11-27'" ==
          csvsqldb::typedValueToString({csvsqldb::DATE, csvsqldb::Date{2020, csvsqldb::Date::November, 27}}));
    CHECK("TIME'11:54:46'" == csvsqldb::typedValueToString({csvsqldb::TIME, csvsqldb::Time{11, 54, 46}}));
    CHECK(
      "TIMESTAMP'2020-11-27T11:54:46'" ==
      csvsqldb::typedValueToString({csvsqldb::TIMESTAMP, csvsqldb::Timestamp{2020, csvsqldb::Date::November, 27, 11, 54, 46}}));
  }

  SECTION("create typed value")
  {
    CHECK(csvsqldb::TypedValue::createValue(csvsqldb::NONE, "")._type == csvsqldb::NONE);

    csvsqldb::TypedValue value;

    value = csvsqldb::TypedValue::createValue(csvsqldb::BOOLEAN, "TRUE");
    CHECK(csvsqldb::BOOLEAN == value._type);
    CHECK(std::any_cast<bool>(value._value));

    value = csvsqldb::TypedValue::createValue(csvsqldb::BOOLEAN, "FALSE");
    CHECK(csvsqldb::BOOLEAN == value._type);
    CHECK_FALSE(std::any_cast<bool>(value._value));

    value = csvsqldb::TypedValue::createValue(csvsqldb::BOOLEAN, "UNKNOWN");
    CHECK(csvsqldb::BOOLEAN == value._type);
    CHECK_FALSE(value._value.has_value());

    value = csvsqldb::TypedValue::createValue(csvsqldb::INT, "4711");
    CHECK(csvsqldb::INT == value._type);
    CHECK(4711 == std::any_cast<int64_t>(value._value));

    value = csvsqldb::TypedValue::createValue(csvsqldb::REAL, "47.11");
    CHECK(csvsqldb::REAL == value._type);
    CHECK(Approx(47.11) == std::any_cast<double>(value._value));

    value = csvsqldb::TypedValue::createValue(csvsqldb::STRING, "Hello world!");
    CHECK(csvsqldb::STRING == value._type);
    CHECK("Hello world!" == std::any_cast<std::string>(value._value));

    value = csvsqldb::TypedValue::createValue(csvsqldb::DATE, "2020-11-27");
    CHECK(csvsqldb::DATE == value._type);
    CHECK(csvsqldb::Date{2020, csvsqldb::Date::November, 27} == std::any_cast<csvsqldb::Date>(value._value));

    value = csvsqldb::TypedValue::createValue(csvsqldb::TIME, "11:54:46");
    CHECK(csvsqldb::TIME == value._type);
    CHECK(csvsqldb::Time{11, 54, 46} == std::any_cast<csvsqldb::Time>(value._value));

    value = csvsqldb::TypedValue::createValue(csvsqldb::TIMESTAMP, "2020-11-27T11:54:46");
    CHECK(csvsqldb::TIMESTAMP == value._type);
    CHECK(csvsqldb::Timestamp{2020, csvsqldb::Date::November, 27, 11, 54, 46} ==
          std::any_cast<csvsqldb::Timestamp>(value._value));
  }

  SECTION("create typed value error")
  {
    CHECK_THROWS(csvsqldb::TypedValue::createValue(csvsqldb::BOOLEAN, "Puschel"));
    CHECK_THROWS(csvsqldb::TypedValue::createValue(csvsqldb::INT, "Puschel"));
    CHECK_THROWS(csvsqldb::TypedValue::createValue(csvsqldb::REAL, "Puschel"));
    CHECK_THROWS(csvsqldb::TypedValue::createValue(csvsqldb::DATE, "11:54:46"));
    CHECK_THROWS(csvsqldb::TypedValue::createValue(csvsqldb::TIME, "2020-11-27"));
    CHECK_THROWS(csvsqldb::TypedValue::createValue(csvsqldb::TIMESTAMP, "Puschel"));
  }
}
