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


#include "data_test_framework.h"

#include <catch2/catch.hpp>


TEST_CASE("Aggreagation Functions Test", "[engine]")
{
  SECTION("count")
  {
    csvsqldb::CountAggregationFunction count;
    CHECK("COUNT" == count.toString());
    CHECK_FALSE(count.suppress());
    count.init();
    csvsqldb::Variant v(4711);
    count.step(v);
    v = 815;
    count.step(v);
    v = 4711;
    count.step(v);

    REQUIRE(count.finalize().getType() == csvsqldb::INT);
    CHECK(count.finalize() == csvsqldb::Variant(3));
  }
  SECTION("count with null")
  {
    csvsqldb::CountAggregationFunction count;
    count.init();
    csvsqldb::Variant v(4711);
    count.step(v);
    v = csvsqldb::Variant{csvsqldb::INT};
    count.step(v);
    v = 4711;
    count.step(v);

    REQUIRE(count.finalize().getType() == csvsqldb::INT);
    CHECK(count.finalize() == csvsqldb::Variant(2));
  }
  SECTION("count with no step")
  {
    csvsqldb::CountAggregationFunction count;
    count.init();

    REQUIRE(count.finalize().getType() == csvsqldb::INT);
    CHECK(count.finalize().isNull());
  }
  SECTION("row count")
  {
    csvsqldb::RowCountAggregationFunction count;
    CHECK("COUNT_STAR" == count.toString());
    CHECK_FALSE(count.suppress());
    count.init();
    csvsqldb::Variant v(4711);
    count.step(v);
    v = 815;
    count.step(v);
    v = 4711;
    count.step(v);

    REQUIRE(count.finalize().getType() == csvsqldb::INT);
    CHECK(count.finalize() == csvsqldb::Variant(3));
  }
  SECTION("row count with null")
  {
    csvsqldb::RowCountAggregationFunction count;
    count.init();
    csvsqldb::Variant v("Hutzli");
    count.step(v);
    v = csvsqldb::Variant{csvsqldb::STRING};
    count.step(v);
    v = "Putzli";
    count.step(v);

    REQUIRE(count.finalize().getType() == csvsqldb::INT);
    CHECK(count.finalize() == csvsqldb::Variant(3));
  }
  SECTION("row count with no step")
  {
    csvsqldb::RowCountAggregationFunction count;
    count.init();

    REQUIRE(count.finalize().getType() == csvsqldb::INT);
    CHECK(count.finalize() == csvsqldb::Variant(0));
  }
  SECTION("path through")
  {
    csvsqldb::PaththroughAggregationFunction pathThrough{true};
    CHECK("PATH_THROUGH" == pathThrough.toString());
    CHECK(pathThrough.suppress());
    pathThrough.init();
    csvsqldb::Variant v(4711);
    pathThrough.step(v);
    v = 815;
    pathThrough.step(v);
    v = 42;
    pathThrough.step(v);

    REQUIRE(pathThrough.finalize().getType() == csvsqldb::INT);
    CHECK(pathThrough.finalize() == csvsqldb::Variant(4711));
  }
  SECTION("path through with string")
  {
    csvsqldb::PaththroughAggregationFunction pathThrough{false};
    CHECK_FALSE(pathThrough.suppress());
    pathThrough.init();
    const char* s = "Check";
    csvsqldb::Variant v(s);
    pathThrough.step(v);

    CHECK(pathThrough.finalize().getType() == csvsqldb::STRING);
    CHECK(pathThrough.finalize() == csvsqldb::Variant("Check"));
    CHECK(pathThrough.finalize().isDisconnected());
  }
  SECTION("path through with no step")
  {
    csvsqldb::PaththroughAggregationFunction pathThrough{false};
    pathThrough.init();

    CHECK(pathThrough.finalize().getType() == csvsqldb::NONE);
  }
  SECTION("sum")
  {
    csvsqldb::SumAggregationFunction sum{csvsqldb::INT};
    CHECK("SUM" == sum.toString());
    CHECK_FALSE(sum.suppress());
    sum.init();
    csvsqldb::Variant v(4711);
    sum.step(v);
    v = 815;
    sum.step(v);
    v = 42;
    sum.step(v);

    REQUIRE(sum.finalize().getType() == csvsqldb::INT);
    CHECK(sum.finalize() == csvsqldb::Variant(5568));
  }
  SECTION("sum with no step")
  {
    csvsqldb::SumAggregationFunction sum{csvsqldb::REAL};
    sum.init();

    REQUIRE(sum.finalize().getType() == csvsqldb::REAL);
    CHECK(sum.finalize().isNull());
  }
  SECTION("sum with wrong type")
  {
    CHECK_THROWS_AS(csvsqldb::SumAggregationFunction(csvsqldb::STRING), csvsqldb::Exception);
    CHECK_THROWS_AS(csvsqldb::SumAggregationFunction(csvsqldb::BOOLEAN), csvsqldb::Exception);
    CHECK_THROWS_AS(csvsqldb::SumAggregationFunction(csvsqldb::DATE), csvsqldb::Exception);
    CHECK_THROWS_AS(csvsqldb::SumAggregationFunction(csvsqldb::TIME), csvsqldb::Exception);
    CHECK_THROWS_AS(csvsqldb::SumAggregationFunction(csvsqldb::TIMESTAMP), csvsqldb::Exception);
  }
  SECTION("avg")
  {
    csvsqldb::AvgAggregationFunction avg{csvsqldb::INT};
    CHECK("AVG" == avg.toString());
    CHECK_FALSE(avg.suppress());
    avg.init();
    csvsqldb::Variant v(4711);
    avg.step(v);
    v = 815;
    avg.step(v);
    v = 42;
    avg.step(v);

    auto res = avg.finalize();
    REQUIRE(res.getType() == csvsqldb::INT);
    CHECK(res.asInt() == csvsqldb::Variant(5568 / 3).asInt());
  }
  SECTION("avg with no step")
  {
    csvsqldb::AvgAggregationFunction avg{csvsqldb::REAL};
    avg.init();

    REQUIRE(avg.finalize().getType() == csvsqldb::REAL);
    CHECK(avg.finalize().isNull());
  }
  SECTION("avg with wrong type")
  {
    CHECK_THROWS_AS(csvsqldb::AvgAggregationFunction(csvsqldb::STRING), csvsqldb::Exception);
    CHECK_THROWS_AS(csvsqldb::AvgAggregationFunction(csvsqldb::BOOLEAN), csvsqldb::Exception);
    CHECK_THROWS_AS(csvsqldb::AvgAggregationFunction(csvsqldb::DATE), csvsqldb::Exception);
    CHECK_THROWS_AS(csvsqldb::AvgAggregationFunction(csvsqldb::TIME), csvsqldb::Exception);
    CHECK_THROWS_AS(csvsqldb::AvgAggregationFunction(csvsqldb::TIMESTAMP), csvsqldb::Exception);
  }
  SECTION("min")
  {
    csvsqldb::MinAggregationFunction min{csvsqldb::INT};
    CHECK("MIN" == min.toString());
    CHECK_FALSE(min.suppress());
    min.init();
    csvsqldb::Variant v(4711);
    min.step(v);
    v = 815;
    min.step(v);
    v = 42;
    min.step(v);

    REQUIRE(min.finalize().getType() == csvsqldb::INT);
    CHECK(min.finalize() == csvsqldb::Variant(42));
  }
  SECTION("min with strings")
  {
    csvsqldb::MinAggregationFunction min{csvsqldb::INT};
    CHECK("MIN" == min.toString());
    CHECK_FALSE(min.suppress());
    min.init();
    const char* s1 = "One";
    csvsqldb::Variant v(s1);
    min.step(v);
    const char* s2 = "Two";
    v = s2;
    min.step(v);
    const char* s3 = "Three";
    v = s3;
    min.step(v);

    REQUIRE(min.finalize().getType() == csvsqldb::STRING);
    CHECK(min.finalize() == csvsqldb::Variant("One"));
    CHECK(min.finalize().isDisconnected());
  }
  SECTION("min with no step")
  {
    csvsqldb::MinAggregationFunction min{csvsqldb::REAL};
    min.init();

    REQUIRE(min.finalize().getType() == csvsqldb::REAL);
    CHECK(min.finalize().isNull());
  }
  SECTION("max")
  {
    csvsqldb::MaxAggregationFunction max{csvsqldb::INT};
    CHECK("MAX" == max.toString());
    CHECK_FALSE(max.suppress());
    max.init();
    csvsqldb::Variant v(4711);
    max.step(v);
    v = 815;
    max.step(v);
    v = 42;
    max.step(v);

    REQUIRE(max.finalize().getType() == csvsqldb::INT);
    CHECK(max.finalize() == csvsqldb::Variant(4711));
  }
  SECTION("max with strings")
  {
    csvsqldb::MaxAggregationFunction max{csvsqldb::INT};
    CHECK("MAX" == max.toString());
    CHECK_FALSE(max.suppress());
    max.init();
    const char* s1 = "One";
    csvsqldb::Variant v(s1);
    max.step(v);
    const char* s2 = "Two";
    v = s2;
    max.step(v);
    const char* s3 = "Three";
    v = s3;
    max.step(v);

    REQUIRE(max.finalize().getType() == csvsqldb::STRING);
    CHECK(max.finalize() == csvsqldb::Variant("Two"));
    CHECK(max.finalize().isDisconnected());
  }
  SECTION("max with no step")
  {
    csvsqldb::MaxAggregationFunction max{csvsqldb::REAL};
    max.init();

    REQUIRE(max.finalize().getType() == csvsqldb::REAL);
    CHECK(max.finalize().isNull());
  }
  SECTION("arbitrary")
  {
    csvsqldb::ArbitraryAggregationFunction arbitrary{csvsqldb::INT};
    CHECK("ARBITRARY" == arbitrary.toString());
    CHECK_FALSE(arbitrary.suppress());
    arbitrary.init();
    csvsqldb::Variant v(4711);
    arbitrary.step(v);
    v = 815;
    arbitrary.step(v);
    v = 42;
    arbitrary.step(v);

    REQUIRE(arbitrary.finalize().getType() == csvsqldb::INT);
    CHECK(arbitrary.finalize() == csvsqldb::Variant(4711));
  }
  SECTION("arbitrary through with string")
  {
    csvsqldb::ArbitraryAggregationFunction arbitrary{csvsqldb::INT};
    arbitrary.init();
    const char* s1 = "Check";
    csvsqldb::Variant v(s1);
    arbitrary.step(v);
    const char* s2 = "No check";
    v = s2;
    arbitrary.step(v);

    CHECK(arbitrary.finalize().getType() == csvsqldb::STRING);
    CHECK(arbitrary.finalize() == csvsqldb::Variant("Check"));
    CHECK(arbitrary.finalize().isDisconnected());
  }
  SECTION("arbitrary through with no step")
  {
    csvsqldb::ArbitraryAggregationFunction arbitrary{csvsqldb::STRING};
    arbitrary.init();

    CHECK(arbitrary.finalize().isNull());
  }
}

TEST_CASE("Aggreagation Function Creator Test", "[engine]")
{
  SECTION("Count")
  {
    auto aggFunc = csvsqldb::AggregationFunction::create(csvsqldb::COUNT, csvsqldb::INT);
    CHECK(aggFunc);
    CHECK("COUNT" == aggFunc->toString());
  }
  SECTION("Row Count")
  {
    auto aggFunc = csvsqldb::AggregationFunction::create(csvsqldb::COUNT_STAR, csvsqldb::INT);
    CHECK(aggFunc);
    CHECK("COUNT_STAR" == aggFunc->toString());
  }
  SECTION("Sum")
  {
    auto aggFunc = csvsqldb::AggregationFunction::create(csvsqldb::SUM, csvsqldb::INT);
    CHECK(aggFunc);
    CHECK("SUM" == aggFunc->toString());
  }
  SECTION("Avg")
  {
    auto aggFunc = csvsqldb::AggregationFunction::create(csvsqldb::AVG, csvsqldb::INT);
    CHECK(aggFunc);
    CHECK("AVG" == aggFunc->toString());
  }
  SECTION("Min")
  {
    auto aggFunc = csvsqldb::AggregationFunction::create(csvsqldb::MIN, csvsqldb::INT);
    CHECK(aggFunc);
    CHECK("MIN" == aggFunc->toString());
  }
  SECTION("Max")
  {
    auto aggFunc = csvsqldb::AggregationFunction::create(csvsqldb::MAX, csvsqldb::INT);
    CHECK(aggFunc);
    CHECK("MAX" == aggFunc->toString());
  }
  SECTION("Arbitrary")
  {
    auto aggFunc = csvsqldb::AggregationFunction::create(csvsqldb::ARBITRARY, csvsqldb::INT);
    CHECK(aggFunc);
    CHECK("ARBITRARY" == aggFunc->toString());
  }
}
