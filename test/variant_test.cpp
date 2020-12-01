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


#include <csvsqldb/variant.h>

#include <catch2/catch.hpp>


TEST_CASE("Variant Test", "[variant]")
{
  SECTION("construction")
  {
    csvsqldb::Variant v1(4711);
    CHECK(csvsqldb::INT == v1.getType());
    CHECK(4711 == v1.asInt());
    CHECK(v1.asBool());

    CHECK_THROWS(v1.asDouble());
    CHECK_THROWS(v1.asDate());
    CHECK_THROWS(v1.asTime());
    CHECK_THROWS(v1.asTimestamp());
    CHECK_THROWS(v1.asString());

    csvsqldb::Variant v2(47.11);
    CHECK(csvsqldb::REAL == v2.getType());
    CHECK(47.11 == Approx(v2.asDouble()));
    CHECK(v2.asBool());
    CHECK_THROWS(v2.asInt());

    csvsqldb::Variant v3(csvsqldb::Date(1970, csvsqldb::Date::September, 23));
    CHECK(csvsqldb::DATE == v3.getType());
    CHECK("1970-09-23" == v3.asDate().format("%F"));
    CHECK(v3.asBool());
    CHECK_THROWS(v3.asInt());

    csvsqldb::Variant v4(true);
    CHECK(csvsqldb::BOOLEAN == v4.getType());
    CHECK(v4.asBool());
    CHECK_THROWS(v4.asInt());

    std::string cool("This is cool!");
    csvsqldb::Variant v5(cool);
    CHECK(csvsqldb::STRING == v5.getType());
    CHECK(cool == v5.asString());
    CHECK(v5.asBool());
    CHECK_THROWS(v5.asInt());

    csvsqldb::Variant v6(csvsqldb::INT);
    CHECK(csvsqldb::INT == v6.getType());
    CHECK(v6.isNull());
    CHECK_THROWS(v6.asInt());

    csvsqldb::Variant v7(csvsqldb::Time(8, 9, 11));
    CHECK(csvsqldb::TIME == v7.getType());
    CHECK("08:09:11" == v7.asTime().format("%H:%M:%S"));
    CHECK(v7.asBool());
    CHECK_THROWS(v7.asInt());

    csvsqldb::Variant v8(csvsqldb::Timestamp(1970, csvsqldb::Date::September, 23, 8, 9, 11));
    CHECK(csvsqldb::TIMESTAMP == v8.getType());
    CHECK("1970-09-23T08:09:11" == v8.asTimestamp().format("%F"));
    CHECK(v8.asBool());
    CHECK_THROWS(v8.asInt());

    CHECK_THROWS_WITH(csvsqldb::Variant{}.asBool(), "variant is null");
  }

  SECTION("construction empty")
  {
    csvsqldb::Variant v;
    CHECK(csvsqldb::NONE == v.getType());
    CHECK(v.isNull());
  }

  SECTION("null variants")
  {
    csvsqldb::Variant i{csvsqldb::INT};
    CHECK_THROWS(i.asInt());

    csvsqldb::Variant b{csvsqldb::BOOLEAN};
    CHECK_THROWS(b.asBool());

    csvsqldb::Variant f{csvsqldb::REAL};
    CHECK_THROWS(f.asDouble());

    csvsqldb::Variant d{csvsqldb::DATE};
    CHECK_THROWS(d.asDate());

    csvsqldb::Variant t{csvsqldb::TIME};
    CHECK_THROWS(t.asTime());

    csvsqldb::Variant ts{csvsqldb::TIMESTAMP};
    CHECK_THROWS(ts.asTimestamp());

    csvsqldb::Variant s{csvsqldb::STRING};
    CHECK_THROWS(s.asString());
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

    v1 = csvsqldb::Variant{csvsqldb::STRING};
    CHECK(v1.isNull());
    CHECK(csvsqldb::STRING == v1.getType());

    CHECK_THROWS(v1 = csvsqldb::Variant{});
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

    csvsqldb::Variant v11{csvsqldb::Variant{csvsqldb::INT}};
    CHECK(v11.isNull());
    CHECK(csvsqldb::INT == v11.getType());
  }

  SECTION("compare equality")
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

    CHECK(csvsqldb::Variant{47.11} == csvsqldb::Variant{47.11});
    CHECK_FALSE(csvsqldb::Variant{8.15} == csvsqldb::Variant{47.11});

    CHECK(csvsqldb::Variant{true} == csvsqldb::Variant{true});
    CHECK_FALSE(csvsqldb::Variant{false} == csvsqldb::Variant{true});
    CHECK(csvsqldb::Variant{false} == csvsqldb::Variant{false});

    CHECK_FALSE(csvsqldb::Variant{csvsqldb::INT} == csvsqldb::Variant{csvsqldb::INT});
    CHECK_FALSE(csvsqldb::Variant{4711} == csvsqldb::Variant{csvsqldb::INT});
    CHECK_FALSE(csvsqldb::Variant{csvsqldb::INT} == csvsqldb::Variant{4711});

    CHECK_THROWS_WITH(v1 == v2, "comparing Variants with different types (DATE:INTEGER)");

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

    CHECK_FALSE(csvsqldb::Variant{47.11} != csvsqldb::Variant{47.11});
    CHECK(csvsqldb::Variant{8.15} != csvsqldb::Variant{47.11});

    CHECK_FALSE(csvsqldb::Variant{true} != csvsqldb::Variant{true});
    CHECK(csvsqldb::Variant{false} != csvsqldb::Variant{true});
    CHECK_FALSE(csvsqldb::Variant{false} != csvsqldb::Variant{false});

    CHECK_FALSE(csvsqldb::Variant{csvsqldb::INT} != csvsqldb::Variant{csvsqldb::INT});
    CHECK_FALSE(csvsqldb::Variant{4711} != csvsqldb::Variant{csvsqldb::INT});
    CHECK_FALSE(csvsqldb::Variant{csvsqldb::INT} != csvsqldb::Variant{4711});

    CHECK_THROWS_WITH(v1 != v2, "comparing Variants with different types (DATE:INTEGER)");
  }

  SECTION("compare less")
  {
    CHECK(csvsqldb::Variant{815} < csvsqldb::Variant{4711});
    CHECK_FALSE(csvsqldb::Variant{4711} < csvsqldb::Variant{815});

    CHECK(csvsqldb::Variant{-4711} < csvsqldb::Variant{-815});
    CHECK_FALSE(csvsqldb::Variant{-815} < csvsqldb::Variant{-4711});

    CHECK(csvsqldb::Variant{false} < csvsqldb::Variant{true});
    CHECK_FALSE(csvsqldb::Variant{true} < csvsqldb::Variant{false});

    CHECK(csvsqldb::Variant{8.15} < csvsqldb::Variant{47.11});
    CHECK_FALSE(csvsqldb::Variant{47.11} < csvsqldb::Variant{8.15});

    CHECK(csvsqldb::Variant{csvsqldb::Date{2020, csvsqldb::Date::November, 1}} <
          csvsqldb::Variant{csvsqldb::Date{2020, csvsqldb::Date::December, 1}});
    CHECK_FALSE(csvsqldb::Variant{csvsqldb::Date{2020, csvsqldb::Date::December, 1}} <
                csvsqldb::Variant{csvsqldb::Date{2020, csvsqldb::Date::November, 1}});

    CHECK(csvsqldb::Variant{csvsqldb::Time{9, 0, 15}} < csvsqldb::Variant{csvsqldb::Time{18, 3, 45}});
    CHECK_FALSE(csvsqldb::Variant{csvsqldb::Time{18, 3, 45}} < csvsqldb::Variant{csvsqldb::Time{9, 0, 15}});

    CHECK(csvsqldb::Variant{csvsqldb::Timestamp{2020, csvsqldb::Date::November, 1, 9, 0, 15}} <
          csvsqldb::Variant{csvsqldb::Timestamp{2020, csvsqldb::Date::December, 1, 18, 3, 45}});
    CHECK_FALSE(csvsqldb::Variant{csvsqldb::Timestamp{2020, csvsqldb::Date::December, 1, 18, 3, 45}} <
                csvsqldb::Variant{csvsqldb::Timestamp{2020, csvsqldb::Date::November, 1, 9, 0, 15}});

    CHECK(csvsqldb::Variant{"Check this out"} < csvsqldb::Variant{"Hello world!"});
    CHECK_FALSE(csvsqldb::Variant{"Hello world!"} < csvsqldb::Variant{"Check this out"});

    CHECK_THROWS_WITH(csvsqldb::Variant{815} < csvsqldb::Variant{true},
                      "comparing Variants with different types (INTEGER:BOOLEAN)");

    CHECK_FALSE(csvsqldb::Variant{csvsqldb::INT} < csvsqldb::Variant{csvsqldb::INT});
    CHECK_FALSE(csvsqldb::Variant{csvsqldb::INT} < csvsqldb::Variant{4711});
    CHECK_FALSE(csvsqldb::Variant{4711} < csvsqldb::Variant{csvsqldb::INT});
  }

  SECTION("type")
  {
    csvsqldb::Variant v;
    CHECK(v.isNull());
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
    CHECK_THROWS(v7.toString());

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

    CHECK_THROWS_WITH(v2 += v1, "adding Variants with different types (REAL:INTEGER)");

    v2 += csvsqldb::Variant(47.11);
    CHECK(csvsqldb::REAL == v2.getType());
    CHECK(94.22 == Approx(v2.asDouble()));

    v1 /= 2;
    CHECK(csvsqldb::INT == v1.getType());
    CHECK(7066 == v1.asInt());

    CHECK_THROWS_WITH(v2 += csvsqldb::Variant(csvsqldb::REAL), "cannot add to null");

    CHECK_THROWS_WITH(v1 += csvsqldb::Variant{true}, "cannot add to non numeric types");

    v2 += 10;
    CHECK(104.22 == Approx(v2.asDouble()));

    csvsqldb::Variant v3{csvsqldb::Variant{csvsqldb::INT}};
    CHECK_THROWS_WITH(v3 += 10, "cannot add to null");

    csvsqldb::Variant v4{csvsqldb::Variant{"Test"}};
    CHECK_THROWS_WITH(v4 += 10, "cannot add to non numeric types");
  }

  SECTION("arithmetic operations int64")
  {
    csvsqldb::Variant v1(4711);
    csvsqldb::Variant v2(47.11);

    int64_t i = 10;
    v1 += i;
    CHECK(4721 == v1.asInt());

    v2 += i;
    CHECK(57.11 == Approx(v2.asDouble()));

    csvsqldb::Variant v3{csvsqldb::INT};
    CHECK_THROWS_WITH(v3 += i, "cannot add to null");

    csvsqldb::Variant v4{"Test"};
    CHECK_THROWS_WITH(v4 += i, "cannot add to non numeric types");
  }

  SECTION("arithmetic operations double")
  {
    csvsqldb::Variant v1(4711);
    csvsqldb::Variant v2(47.11);

    double d = 10;
    v1 += d;
    CHECK(4721 == v1.asInt());

    v2 += d;
    CHECK(57.11 == Approx(v2.asDouble()));

    csvsqldb::Variant v3{csvsqldb::INT};
    CHECK_THROWS_WITH(v3 += d, "cannot add to null");

    csvsqldb::Variant v4{"Test"};
    CHECK_THROWS_WITH(v4 += d, "cannot add to non numeric types");
  }

  SECTION("devide by")
  {
    csvsqldb::Variant v1(4711);
    csvsqldb::Variant v2(47.11);

    v1 /= 10;
    CHECK(471 == v1.asInt());

    v1 /= 10.0;
    CHECK(47 == v1.asInt());

    v2 /= 10;
    CHECK(4.711 == Approx(v2.asDouble()));

    v2 /= 10.0;
    CHECK(0.4711 == Approx(v2.asDouble()));

    csvsqldb::Variant v3{csvsqldb::INT};
    CHECK_THROWS_WITH(v3 /= 10, "cannot devide with null");

    csvsqldb::Variant v4{"Test"};
    CHECK_THROWS_WITH(v4 /= 10, "cannot divide non numeric types");

    CHECK_THROWS_WITH(v1 /= 0, "cannot devide by null");
    CHECK_THROWS_WITH(v1 /= 0.0, "cannot devide by null");

    CHECK_THROWS_WITH(v2 /= 0, "cannot devide by null");
    CHECK_THROWS_WITH(v2 /= 0.0, "cannot devide by null");
  }
}
