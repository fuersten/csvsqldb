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


#include <csvsqldb/base/float_helper.h>
#include <csvsqldb/typeoperations.h>

#include <catch2/catch.hpp>


TEST_CASE("Type operations Test", "[operations]")
{
  SECTION("operations")
  {
    csvsqldb::Variant lhs(4711);
    csvsqldb::Variant rhs(815);

    csvsqldb::Variant result = binaryOperation(csvsqldb::OP_ADD, lhs, rhs);
    CHECK(csvsqldb::INT == result.getType());
    CHECK(5526 == result.asInt());

    csvsqldb::Variant lhs1(4711.0);
    csvsqldb::Variant rhs1(815.0);

    csvsqldb::Variant result1 = binaryOperation(csvsqldb::OP_ADD, lhs1, rhs1);
    CHECK(csvsqldb::REAL == result1.getType());
    CHECK(5526.0 == Approx(result1.asDouble()));

    result1 = binaryOperation(csvsqldb::OP_ADD, lhs1, rhs);
    CHECK(csvsqldb::REAL == result1.getType());
    CHECK(5526.0 == Approx(result1.asDouble()));

    result1 = binaryOperation(csvsqldb::OP_ADD, lhs, rhs1);
    CHECK(csvsqldb::REAL == result1.getType());
    CHECK(5526.0 == Approx(result1.asDouble()));
  }

  SECTION("value concat")
  {
    csvsqldb::Variant i(4711);
    csvsqldb::Variant d(47.11);

    csvsqldb::Variant ret = binaryOperation(csvsqldb::OP_ADD, i, d);
    CHECK("4758.110000" == ret.toString());

    csvsqldb::Variant s1(std::string("Lars "));
    csvsqldb::Variant s2(std::string("Fürstenberg"));

    csvsqldb::Variant result = binaryOperation(csvsqldb::OP_CONCAT, s1, s2);
    CHECK("Lars Fürstenberg" == result.toString());

    csvsqldb::Variant ret2 = binaryOperation(csvsqldb::OP_CONCAT, d, result);
    CHECK("Lars Fürstenberg47.110000" == ret2.toString());

    csvsqldb::Variant t(csvsqldb::Time(8, 9, 11));
    s1 = "The time is ";
    result = binaryOperation(csvsqldb::OP_CONCAT, s1, t);
    CHECK("The time is 08:09:11" == result.toString());

    csvsqldb::Variant dt(csvsqldb::Date(2015, csvsqldb::Date::June, 29));
    s1 = "The date is ";
    result = binaryOperation(csvsqldb::OP_CONCAT, s1, dt);
    CHECK("The date is 2015-06-29" == result.toString());

    csvsqldb::Variant ts(csvsqldb::Timestamp(2015, csvsqldb::Date::June, 29, 8, 9, 11));
    s1 = "The timestamp is ";
    result = binaryOperation(csvsqldb::OP_CONCAT, s1, ts);
    CHECK("The timestamp is 2015-06-29T08:09:11" == result.toString());
  }

  SECTION("null operations")
  {
    csvsqldb::Variant i(4711);
    csvsqldb::Variant b_true(true);
    csvsqldb::Variant b_false(false);
    csvsqldb::Variant null(csvsqldb::INT);

    csvsqldb::Variant result = binaryOperation(csvsqldb::OP_ADD, i, null);
    CHECK(result.isNull());

    result = binaryOperation(csvsqldb::OP_AND, b_true, null);
    CHECK(result.isNull());
    result = binaryOperation(csvsqldb::OP_AND, null, b_false);
    CHECK(!result.isNull());
    CHECK(!result.asBool());

    result = binaryOperation(csvsqldb::OP_OR, b_true, null);
    CHECK(!result.isNull());
    CHECK(result.asBool());
    result = binaryOperation(csvsqldb::OP_OR, b_false, null);
    CHECK(result.isNull());
    result = binaryOperation(csvsqldb::OP_OR, null, b_true);
    CHECK(!result.isNull());
    CHECK(result.asBool());
  }

  SECTION("cast operations")
  {
    csvsqldb::Variant lhs(4711);

    csvsqldb::Variant result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::REAL, lhs);
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(4711.0 == Approx(result.asDouble()));

    lhs = csvsqldb::Variant("4711.0");
    result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::REAL, lhs);
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(4711.0 == Approx(result.asDouble()));

    result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::INT, lhs);
    CHECK(csvsqldb::INT == result.getType());
    CHECK(4711 == result.asInt());

    result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::BOOLEAN, lhs);
    CHECK(csvsqldb::BOOLEAN == result.getType());
    CHECK(result.asBool());

    lhs = csvsqldb::Variant("08:09:11");
    result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::TIME, lhs);
    CHECK(csvsqldb::TIME == result.getType());

    lhs = csvsqldb::Variant("2015-06-29");
    result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::DATE, lhs);
    CHECK(csvsqldb::DATE == result.getType());

    lhs = csvsqldb::Variant("2015-06-29T08:09:11");
    result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::TIMESTAMP, lhs);
    CHECK(csvsqldb::TIMESTAMP == result.getType());
  }

  SECTION("null cast operations")
  {
    csvsqldb::Variant result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::TIMESTAMP, csvsqldb::Variant(csvsqldb::NONE));
    CHECK(csvsqldb::TIMESTAMP == result.getType());
    CHECK(result.isNull());
    result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::TIME, csvsqldb::Variant(csvsqldb::NONE));
    CHECK(csvsqldb::TIME == result.getType());
    CHECK(result.isNull());
    result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::DATE, csvsqldb::Variant(csvsqldb::NONE));
    CHECK(csvsqldb::DATE == result.getType());
    CHECK(result.isNull());
    result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::INT, csvsqldb::Variant(csvsqldb::NONE));
    CHECK(csvsqldb::INT == result.getType());
    CHECK(result.isNull());
    result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::REAL, csvsqldb::Variant(csvsqldb::NONE));
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(result.isNull());
    result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::STRING, csvsqldb::Variant(csvsqldb::NONE));
    CHECK(csvsqldb::STRING == result.getType());
    CHECK(result.isNull());
    result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::BOOLEAN, csvsqldb::Variant(csvsqldb::NONE));
    CHECK(csvsqldb::BOOLEAN == result.getType());
    CHECK(result.isNull());
  }

  SECTION("compare operations")
  {
    csvsqldb::Variant lhs(csvsqldb::Time(8, 9, 11));
    csvsqldb::Variant lhs2("08:09:11");
    csvsqldb::Variant rhs(csvsqldb::Time(12, 0, 0));
    csvsqldb::Variant rhs2("12:00:00");

    csvsqldb::Variant result = binaryOperation(csvsqldb::OP_GT, lhs, rhs);
    CHECK_FALSE(result.asBool());
    result = binaryOperation(csvsqldb::OP_GT, rhs, lhs);
    CHECK(result.asBool());
    result = binaryOperation(csvsqldb::OP_GT, rhs2, lhs);
    CHECK(result.asBool());
    result = binaryOperation(csvsqldb::OP_LT, lhs, rhs);
    CHECK(result.asBool());
    result = binaryOperation(csvsqldb::OP_EQ, lhs, lhs);
    CHECK(result.asBool());
    result = binaryOperation(csvsqldb::OP_EQ, lhs, lhs2);
    CHECK(result.asBool());
  }
}
