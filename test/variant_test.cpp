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


#include "libcsvsqldb/base/float_helper.h"
#include "libcsvsqldb/variant.h"

#include <catch2/catch.hpp>


TEST_CASE("Variant Test", "[variant]")
{
  SECTION("variant")
  {
    csvsqldb::Variant v1(4711);
    CHECK(4711 == v1.asInt());

    csvsqldb::Variant v2(47.11);
    CHECK(csvsqldb::compare(47.11, v2.asDouble()));

    csvsqldb::Variant v3(csvsqldb::Date(1970, csvsqldb::Date::September, 23));
    CHECK("1970-09-23" == v3.asDate().format("%F"));

    csvsqldb::Variant v4(true);
    CHECK(true == v4.asBool());

    std::string cool("This is cool!");
    csvsqldb::Variant v5(cool);
    CHECK("This is cool!" == std::string(v5.asString()));

    csvsqldb::Variant v6(csvsqldb::INT);
    CHECK(v6.isNull());

    csvsqldb::Variant v7(csvsqldb::Time(8, 9, 11));
    CHECK("08:09:11" == v7.asTime().format("%H:%M:%S"));

    csvsqldb::Variant v8(csvsqldb::Timestamp(1970, csvsqldb::Date::September, 23, 8, 9, 11));
    CHECK("1970-09-23T08:09:11" == v8.asTimestamp().format("%F"));
  }

  SECTION("assignement")
  {
    csvsqldb::Variant v1(csvsqldb::Date(1970, csvsqldb::Date::September, 23));
    CHECK("1970-09-23" == v1.asDate().format("%F"));

    csvsqldb::Variant v2(4711);
    CHECK(4711 == v2.asInt());

    v2 = v1;
    CHECK("1970-09-23" == v1.asDate().format("%F"));

    csvsqldb::Variant v3(csvsqldb::Date(2015, csvsqldb::Date::January, 1));
    v3 = v1;
    CHECK("1970-09-23" == v3.asDate().format("%F"));

    csvsqldb::Variant v4("This is a test");
    CHECK("This is a test" == std::string(v4.asString()));

    v1 = v4;
    CHECK("This is a test" == std::string(v1.asString()));

    csvsqldb::Variant v5(csvsqldb::Time(8, 9, 11));
    v1 = v5;
    CHECK("08:09:11" == v1.asTime().format("%H:%M:%S"));

    csvsqldb::Variant v6(csvsqldb::Timestamp(1970, csvsqldb::Date::September, 23, 8, 9, 11));
    v1 = v6;
    CHECK("1970-09-23T08:09:11" == v1.asTimestamp().format("%F"));
  }

  SECTION("copy construction")
  {
    csvsqldb::Variant v1(csvsqldb::Date(1970, csvsqldb::Date::September, 23));
    csvsqldb::Variant v2(4711);
    csvsqldb::Variant v3("This is a test");
    csvsqldb::Variant v7(csvsqldb::Time(8, 9, 11));
    csvsqldb::Variant v9(csvsqldb::Timestamp(1970, csvsqldb::Date::September, 23, 8, 9, 11));

    csvsqldb::Variant v4(v1);
    CHECK("1970-09-23" == v4.asDate().format("%F"));
    csvsqldb::Variant v5(v2);
    CHECK(4711 == v5.asInt());
    csvsqldb::Variant v6(v3);
    CHECK("This is a test" == std::string(v6.asString()));
    csvsqldb::Variant v8(v7);
    CHECK("08:09:11" == v8.asTime().format("%H:%M:%S"));
    csvsqldb::Variant v10(v9);
    CHECK("1970-09-23T08:09:11" == v10.asTimestamp().format("%F"));
  }

  SECTION("comparison")
  {
    csvsqldb::Variant v1(csvsqldb::Date(1970, csvsqldb::Date::September, 23));
    csvsqldb::Variant v2(4711);
    csvsqldb::Variant v3("This is a test");
    csvsqldb::Variant v7(csvsqldb::Time(8, 9, 11));
    csvsqldb::Variant v9(csvsqldb::Timestamp(1970, csvsqldb::Date::September, 23, 8, 9, 11));

    csvsqldb::Variant v4(v1);
    CHECK(v1 == v4);
    csvsqldb::Variant v5(v2);
    CHECK(v2 == v5);
    csvsqldb::Variant v6(v3);
    CHECK(v3 == v6);
    csvsqldb::Variant v8(v7);
    CHECK(v8 == v7);
    csvsqldb::Variant v10(v9);
    CHECK(v10 == v9);

    v4 = csvsqldb::Variant(csvsqldb::Date(1963, csvsqldb::Date::March, 6));
    CHECK(v1 != v4);
    v5 = csvsqldb::Variant(815);
    CHECK(v2 != v5);
    v6 = csvsqldb::Variant("OK");
    CHECK(v3 != v6);
    v8 = csvsqldb::Variant(csvsqldb::Time(12, 0, 0));
    CHECK(v7 != v8);
    v10 = csvsqldb::Variant(csvsqldb::Timestamp(1963, csvsqldb::Date::March, 6, 12, 0, 0));
    CHECK(v9 != v10);
  }

  SECTION("type")
  {
    csvsqldb::Variant v;
    CHECK_FALSE(v.isNull());
    CHECK(csvsqldb::NONE == v.getType());

    csvsqldb::Variant v1(4711);
    CHECK(csvsqldb::INT == v1.getType());

    csvsqldb::Variant v2(47.11);
    CHECK(csvsqldb::REAL == v2.getType());

    csvsqldb::Variant v3(csvsqldb::Date(1970, csvsqldb::Date::September, 23));
    CHECK(csvsqldb::DATE == v3.getType());

    csvsqldb::Variant v4(true);
    CHECK(csvsqldb::BOOLEAN == v4.getType());

    std::string cool("This is cool!");
    csvsqldb::Variant v5(cool);
    CHECK(csvsqldb::STRING == v5.getType());

    csvsqldb::Variant v6(csvsqldb::INT);
    CHECK(v6.isNull());
    CHECK(csvsqldb::INT == v6.getType());

    csvsqldb::Variant v7(csvsqldb::Time(8, 9, 11));
    CHECK(csvsqldb::TIME == v7.getType());

    csvsqldb::Variant v8(csvsqldb::Timestamp(1970, csvsqldb::Date::September, 23, 8, 9, 11));
    CHECK(csvsqldb::TIMESTAMP == v8.getType());
  }

  SECTION("to string")
  {
    csvsqldb::Variant v1(4711);
    CHECK("4711" == v1.toString());

    csvsqldb::Variant v2(47.11);
    CHECK("47.110000" == v2.toString());

    csvsqldb::Variant v3(csvsqldb::Date(1970, csvsqldb::Date::September, 23));
    CHECK("1970-09-23" == v3.toString());

    csvsqldb::Variant v4(true);
    CHECK("1" == v4.toString());

    std::string cool("This is cool!");
    csvsqldb::Variant v5(cool);
    CHECK("This is cool!" == v5.toString());

    csvsqldb::Variant v6(csvsqldb::INT);
    CHECK("NULL" == v6.toString());

    csvsqldb::Variant v7;
    CHECK("NONE" == v7.toString());

    csvsqldb::Variant v8(csvsqldb::Time(8, 9, 11));
    CHECK("08:09:11" == v8.toString());

    csvsqldb::Variant v9(csvsqldb::Timestamp(1970, csvsqldb::Date::September, 23, 8, 9, 11));
    CHECK("1970-09-23T08:09:11" == v9.toString());
  }

  SECTION("arithmetic operations")
  {
    csvsqldb::Variant v1(4711);
    csvsqldb::Variant v2(47.11);

    v1 += 4711;
    CHECK(csvsqldb::INT == v1.getType());
    CHECK(9422 == v1.asInt());

    // TODO LCF: not sure if it would be better to actually change the type to REAL if adding a double
    v1 += 4711.0;
    CHECK(csvsqldb::INT == v1.getType());
    CHECK(14133 == v1.asInt());

    CHECK_THROWS_AS(v2 += v1, csvsqldb::VariantException);

    v2 += csvsqldb::Variant(47.11);
    CHECK(csvsqldb::REAL == v2.getType());
    CHECK(csvsqldb::compare(94.22, v2.asDouble()));

    v1 /= 2;
    CHECK(csvsqldb::INT == v1.getType());
    CHECK(7066 == v1.asInt());

    CHECK_THROWS_AS(v2 += csvsqldb::Variant(csvsqldb::REAL), csvsqldb::VariantException);
  }
}
