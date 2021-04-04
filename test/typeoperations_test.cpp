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

namespace
{
  struct Expected {
    bool _first{true};
    bool _second{false};
  };

  void executeOperation(const std::vector<csvsqldb::Variant>& values, csvsqldb::eOperationType op, Expected expected)
  {
    csvsqldb::Variant result;
    csvsqldb::Variant lhs;
    csvsqldb::Variant rhs;

    auto iter = values.begin();
    while (iter != values.end()) {
      lhs = *iter++;
      rhs = *iter++;
      result = binaryOperation(op, lhs, rhs);
      CHECK(csvsqldb::BOOLEAN == result.getType());
      if (expected._first) {
        CHECK(result.asBool());
      } else {
        CHECK_FALSE(result.asBool());
      }

      if (iter == values.end()) {
        break;
      }

      lhs = *iter++;
      rhs = *iter++;
      result = binaryOperation(op, lhs, rhs);
      CHECK(csvsqldb::BOOLEAN == result.getType());
      if (expected._second) {
        CHECK(result.asBool());
      } else {
        CHECK_FALSE(result.asBool());
      }
    }
  }
}


TEST_CASE("Type operations Test", "[operations]")
{
  SECTION("add operations")
  {
    csvsqldb::Variant result = binaryOperation(csvsqldb::OP_ADD, csvsqldb::Variant{4711}, csvsqldb::Variant{815});
    CHECK(csvsqldb::INT == result.getType());
    CHECK(5526 == result.asInt());

    result = binaryOperation(csvsqldb::OP_ADD, csvsqldb::Variant{4711.0}, csvsqldb::Variant{815.0});
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(5526.0 == Approx(result.asDouble()));

    result = binaryOperation(csvsqldb::OP_ADD, csvsqldb::Variant{4711.0}, csvsqldb::Variant{815});
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(5526.0 == Approx(result.asDouble()));

    result = binaryOperation(csvsqldb::OP_ADD, csvsqldb::Variant{815}, csvsqldb::Variant{4711.0});
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(5526.0 == Approx(result.asDouble()));
  }

  SECTION("sub operations")
  {
    csvsqldb::Variant result = binaryOperation(csvsqldb::OP_SUB, csvsqldb::Variant{4711}, csvsqldb::Variant{815});
    CHECK(csvsqldb::INT == result.getType());
    CHECK(3896 == result.asInt());

    result = binaryOperation(csvsqldb::OP_SUB, csvsqldb::Variant{4711.0}, csvsqldb::Variant{815.0});
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(3896.0 == Approx(result.asDouble()));

    result = binaryOperation(csvsqldb::OP_SUB, csvsqldb::Variant{4711.0}, csvsqldb::Variant{815});
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(3896.0 == Approx(result.asDouble()));

    result = binaryOperation(csvsqldb::OP_SUB, csvsqldb::Variant{815}, csvsqldb::Variant{4711.0});
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(-3896.0 == Approx(result.asDouble()));

    result = binaryOperation(csvsqldb::OP_SUB, csvsqldb::Variant{csvsqldb::Date{2020, csvsqldb::Date::December, 2}},
                             csvsqldb::Variant{csvsqldb::Date{2020, csvsqldb::Date::November, 20}});
    CHECK(csvsqldb::INT == result.getType());
    CHECK(12 == result.asInt());

    result =
      binaryOperation(csvsqldb::OP_SUB, csvsqldb::Variant{csvsqldb::Time{20, 0, 0}}, csvsqldb::Variant{csvsqldb::Time{10, 0, 0}});
    CHECK(csvsqldb::INT == result.getType());
    CHECK(36000000 == result.asInt());

    result =
      binaryOperation(csvsqldb::OP_SUB, csvsqldb::Variant{csvsqldb::Timestamp{2020, csvsqldb::Date::December, 2, 20, 0, 0}},
                      csvsqldb::Variant{csvsqldb::Timestamp{2020, csvsqldb::Date::November, 20, 10, 0, 0}});
    CHECK(csvsqldb::INT == result.getType());
    CHECK(1072800 == result.asInt());
  }

  SECTION("mul operations")
  {
    csvsqldb::Variant result = binaryOperation(csvsqldb::OP_MUL, csvsqldb::Variant{4711}, csvsqldb::Variant{10});
    CHECK(csvsqldb::INT == result.getType());
    CHECK(47110 == result.asInt());

    result = binaryOperation(csvsqldb::OP_MUL, csvsqldb::Variant{4711.0}, csvsqldb::Variant{10.0});
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(47110.0 == Approx(result.asDouble()));

    result = binaryOperation(csvsqldb::OP_MUL, csvsqldb::Variant{4711.0}, csvsqldb::Variant{10});
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(47110.0 == Approx(result.asDouble()));

    result = binaryOperation(csvsqldb::OP_MUL, csvsqldb::Variant{4711}, csvsqldb::Variant{10.0});
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(47110.0 == Approx(result.asDouble()));
  }

  SECTION("div operations")
  {
    csvsqldb::Variant result = binaryOperation(csvsqldb::OP_DIV, csvsqldb::Variant{4711}, csvsqldb::Variant{10});
    CHECK(csvsqldb::INT == result.getType());
    CHECK(471 == result.asInt());

    result = binaryOperation(csvsqldb::OP_DIV, csvsqldb::Variant{4711.0}, csvsqldb::Variant{10.0});
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(471.1 == Approx(result.asDouble()));

    result = binaryOperation(csvsqldb::OP_DIV, csvsqldb::Variant{4711.0}, csvsqldb::Variant{10});
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(471.1 == Approx(result.asDouble()));

    result = binaryOperation(csvsqldb::OP_DIV, csvsqldb::Variant{4711}, csvsqldb::Variant{10.0});
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(471.1 == Approx(result.asDouble()));
  }

  SECTION("mod operations")
  {
    csvsqldb::Variant result = binaryOperation(csvsqldb::OP_MOD, csvsqldb::Variant{4711}, csvsqldb::Variant{9});
    CHECK(csvsqldb::INT == result.getType());
    CHECK(4 == result.asInt());

    result = binaryOperation(csvsqldb::OP_MOD, csvsqldb::Variant{4711.0}, csvsqldb::Variant{9.0});
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(4.0 == Approx(result.asDouble()));

    result = binaryOperation(csvsqldb::OP_MOD, csvsqldb::Variant{4711.0}, csvsqldb::Variant{9});
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(4.0 == Approx(result.asDouble()));

    result = binaryOperation(csvsqldb::OP_MOD, csvsqldb::Variant{4711}, csvsqldb::Variant{9.0});
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(4.0 == Approx(result.asDouble()));
  }

  SECTION("value concat")
  {
    csvsqldb::Variant i(4711);
    csvsqldb::Variant d(47.11);

    csvsqldb::Variant result = binaryOperation(csvsqldb::OP_ADD, i, d);
    CHECK("4758.110000" == result.toString());

    csvsqldb::Variant s1(std::string("Lars "));
    csvsqldb::Variant s2(std::string("Fürstenberg"));

    result = binaryOperation(csvsqldb::OP_CONCAT, s1, s2);
    CHECK("Lars Fürstenberg" == result.toString());

    result = binaryOperation(csvsqldb::OP_CONCAT, d, result);
    CHECK("Lars Fürstenberg47.110000" == result.toString());

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
}

TEST_CASE("Type compare operations Test", "[operations]")
{
  // clang-format off
  std::vector<csvsqldb::Variant> values{
    4711, 9,
    9, 4711,
    4711.0, 9.0,
    9.0, 4711.0,
    4711.0, 9,
    9.0, 4711,
    4711, 9.0,
    9, 4711.0,
    "Test", "Check",
    "Check", "Test",
    csvsqldb::Date{2020, csvsqldb::Date::December, 3}, csvsqldb::Date{2020, csvsqldb::Date::November, 3},
    csvsqldb::Date{2020, csvsqldb::Date::November, 3}, csvsqldb::Date{2020, csvsqldb::Date::December, 3},
    csvsqldb::Date{2020, csvsqldb::Date::December, 3}, "2020-11-03",
    "2020-11-03", csvsqldb::Date{2020, csvsqldb::Date::December, 3},
    csvsqldb::Time{18, 56, 13}, csvsqldb::Time{9, 46, 5},
    csvsqldb::Time{9, 46, 5}, csvsqldb::Time{18, 56, 13},
    csvsqldb::Time{18, 56, 13}, "09:46:05",
    "09:46:05", csvsqldb::Time{18, 56, 13},
    csvsqldb::Timestamp{2020, csvsqldb::Date::December, 3, 18, 56, 13}, csvsqldb::Timestamp{2020, csvsqldb::Date::November, 3, 18, 56, 13},
    csvsqldb::Timestamp{2020, csvsqldb::Date::November, 3, 18, 56, 13}, csvsqldb::Timestamp{2020, csvsqldb::Date::December, 3, 18, 56, 13},
    "2020-12-03T18:56:13", csvsqldb::Timestamp{2020, csvsqldb::Date::November, 3, 18, 56, 13},
    csvsqldb::Timestamp{2020, csvsqldb::Date::November, 3, 18, 56, 13}, "2020-12-03T18:56:13"};
  // clang-format on

  // clang-format off
  std::vector<csvsqldb::Variant> eq_values{
    true, true,
    true, false,
    false, false,
    false, true,
    true, 4711,
    false, 4711,
    false, 0,
    true, 0,
    4711, true,
    4711, false,
    0, false,
    0, true,
    true, 47.11,
    false, 47.11,
    false, 0.0,
    true, 0.0,
    47.11, true,
    47.11, false,
    0.0, false,
    0.0, true,
    4711, 4711,
    9, 4711,
    4711, 4711.0,
    9, 47.11,
    4711, 4711.0,
    9.0, 4711,
    47.11, 47.11,
    9.0, 47.11,
    "Check", "Check",
    "Check", "Test",
    "", "",
    "", "Test",
    csvsqldb::Date{2020, csvsqldb::Date::December, 3}, csvsqldb::Date{2020, csvsqldb::Date::December, 3},
    csvsqldb::Date{2020, csvsqldb::Date::November, 3}, csvsqldb::Date{2020, csvsqldb::Date::December, 3},
    csvsqldb::Date{2020, csvsqldb::Date::December, 3}, "2020-12-03",
    csvsqldb::Date{2020, csvsqldb::Date::December, 3}, "2020-11-03",
    "2020-12-03", csvsqldb::Date{2020, csvsqldb::Date::December, 3},
    "2020-11-03", csvsqldb::Date{2020, csvsqldb::Date::December, 3},
  };
  // clang-format on

  SECTION("gt operations")
  {
    Expected expected{true, false};
    executeOperation(values, csvsqldb::OP_GT, expected);
  }

  SECTION("ge operations")
  {
    Expected expected{true, false};
    executeOperation(values, csvsqldb::OP_GE, expected);
  }

  SECTION("lt operations")
  {
    Expected expected{false, true};
    executeOperation(values, csvsqldb::OP_LT, expected);
  }

  SECTION("le operations")
  {
    Expected expected{false, true};
    executeOperation(values, csvsqldb::OP_LT, expected);
  }

  SECTION("eq operations")
  {
    Expected expected{true, false};
    executeOperation(eq_values, csvsqldb::OP_EQ, expected);
  }

  SECTION("neq operations")
  {
    Expected expected{false, true};
    executeOperation(eq_values, csvsqldb::OP_NEQ, expected);
  }
}
