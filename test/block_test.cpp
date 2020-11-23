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


#include "csvsqldb/block.h"

#include <catch2/catch.hpp>


TEST_CASE("Block Test", "[block]")
{
  csvsqldb::Block block{42, 256};

  SECTION("construct")
  {
    csvsqldb::Block block1{4711, 256};
    CHECK(4711 == block1.getBlockNumber());
  }
  SECTION("has size")
  {
    csvsqldb::Block block1{42, 32};
    CHECK(block1.hasSizeFor(0));
    CHECK(block1.hasSizeFor(16));
    CHECK(block1.hasSizeFor(29));
    CHECK_FALSE(block1.hasSizeFor(30));
    CHECK_FALSE(block1.hasSizeFor(31));
    CHECK_FALSE(block1.hasSizeFor(32));
    CHECK_FALSE(block1.hasSizeFor(33));
    CHECK_FALSE(block1.hasSizeFor(128));
    CHECK_FALSE(block1.hasSizeFor(std::numeric_limits<size_t>::max()));
  }
  SECTION("add int")
  {
    auto* val = block.addInt(4711, false);
    CHECK(dynamic_cast<csvsqldb::ValInt*>(val));
    CHECK(csvsqldb::ValInt{4711} == *val);
    CHECK(block.offset() == sizeof(csvsqldb::ValInt) + 1);
    CHECK(block.isValue(0));

    csvsqldb::ValInt i{815};
    val = block.addValue(i);
    CHECK(dynamic_cast<csvsqldb::ValInt*>(val));
    CHECK(csvsqldb::ValInt{815} == *val);
    CHECK(block.offset() == (sizeof(csvsqldb::ValInt) + 1) * 2);
    CHECK(block.isValue(sizeof(csvsqldb::ValInt) + 1));

    csvsqldb::Variant vi{42};
    val = block.addValue(vi);
    CHECK(dynamic_cast<csvsqldb::ValInt*>(val));
    CHECK(csvsqldb::ValInt{42} == *val);
    CHECK(block.offset() == (sizeof(csvsqldb::ValInt) + 1) * 3);
    CHECK(block.isValue((sizeof(csvsqldb::ValInt) + 1) * 2));

    val = block.addInt(0, true);
    CHECK(dynamic_cast<csvsqldb::ValInt*>(val));
    CHECK(val->isNull());
    CHECK(block.offset() == (sizeof(csvsqldb::ValInt) + 1) * 4);
    CHECK(block.isValue((sizeof(csvsqldb::ValInt) + 1) * 3));
  }
  SECTION("add real")
  {
    auto* val = block.addReal(47.11, false);
    CHECK(dynamic_cast<csvsqldb::ValDouble*>(val));
    CHECK(csvsqldb::ValDouble{47.11} == *val);
    CHECK(block.offset() == sizeof(csvsqldb::ValDouble) + 1);
    CHECK(block.isValue(0));

    csvsqldb::ValDouble d{8.15};
    val = block.addValue(d);
    CHECK(dynamic_cast<csvsqldb::ValDouble*>(val));
    CHECK(csvsqldb::ValDouble{8.15} == *val);
    CHECK(block.offset() == (sizeof(csvsqldb::ValDouble) + 1) * 2);
    CHECK(block.isValue(sizeof(csvsqldb::ValDouble) + 1));

    csvsqldb::Variant vd{42.815};
    val = block.addValue(vd);
    CHECK(dynamic_cast<csvsqldb::ValDouble*>(val));
    CHECK(csvsqldb::ValDouble{42.815} == *val);
    CHECK(block.offset() == (sizeof(csvsqldb::ValDouble) + 1) * 3);
    CHECK(block.isValue((sizeof(csvsqldb::ValDouble) + 1) * 2));

    val = block.addReal(0.0, true);
    CHECK(dynamic_cast<csvsqldb::ValDouble*>(val));
    CHECK(val->isNull());
    CHECK(block.offset() == (sizeof(csvsqldb::ValDouble) + 1) * 4);
    CHECK(block.isValue((sizeof(csvsqldb::ValDouble) + 1) * 3));
  }
  SECTION("add string")
  {
    const auto* s1 = csvsqldb::ValueCreator<std::string>::createValue(std::string("Hello world!"));

    auto* val = block.addString("Hello world!", 12, false);
    CHECK(dynamic_cast<csvsqldb::ValString*>(val));
    CHECK(*s1 == *val);
    CHECK(block.offset() == sizeof(csvsqldb::ValString) + s1->length() + 2);
    CHECK(block.isValue(0));

    csvsqldb::Variant s2("Hutzli");
    val = block.addString("Hutzli", 6, false);
    CHECK(dynamic_cast<csvsqldb::ValString*>(val));
    CHECK(s2.asString() == val->toString());
    CHECK(block.offset() == sizeof(csvsqldb::ValString) + s1->length() + 2 + sizeof(csvsqldb::ValString) + 6 + 2);
    CHECK(block.isValue(sizeof(csvsqldb::ValString) + s1->length() + 2));

    const auto* s3 = csvsqldb::ValueCreator<std::string>::createValue(std::string("Schnucki"));
    val = block.addValue(csvsqldb::Variant{"Schnucki"});
    CHECK(dynamic_cast<csvsqldb::ValString*>(val));
    CHECK(*s3 == *val);
    CHECK(block.offset() == sizeof(csvsqldb::ValString) + s1->length() + 2 + sizeof(csvsqldb::ValString) + 6 + 2 +
                              sizeof(csvsqldb::ValString) + s3->length() + 2);
    CHECK(block.isValue(sizeof(csvsqldb::ValString) + s1->length() + 2 + sizeof(csvsqldb::ValString) + 6 + 2));

    val = block.addString(nullptr, 0, true);
    CHECK(dynamic_cast<csvsqldb::ValString*>(val));
    CHECK(val->isNull());
    CHECK(block.offset() == sizeof(csvsqldb::ValString) + s1->length() + 2 + sizeof(csvsqldb::ValString) + 6 + 2 +
                              sizeof(csvsqldb::ValString) + s3->length() + 2 + sizeof(csvsqldb::ValString) + 2);
    CHECK(block.isValue(sizeof(csvsqldb::ValString) + s1->length() + 2 + sizeof(csvsqldb::ValString) + 6 + 2 +
                        sizeof(csvsqldb::ValString) + s3->length() + 2));
  }
  SECTION("add bool")
  {
    auto* val = block.addBool(true, false);
    CHECK(dynamic_cast<csvsqldb::ValBool*>(val));
    CHECK(csvsqldb::ValBool{true} == *val);
    CHECK(block.offset() == sizeof(csvsqldb::ValBool) + 1);
    CHECK(block.isValue(0));

    csvsqldb::ValBool b{false};
    val = block.addValue(b);
    CHECK(dynamic_cast<csvsqldb::ValBool*>(val));
    CHECK(csvsqldb::ValBool{false} == *val);
    CHECK(block.offset() == (sizeof(csvsqldb::ValBool) + 1) * 2);
    CHECK(block.isValue(sizeof(csvsqldb::ValBool) + 1));

    csvsqldb::Variant vb{true};
    val = block.addValue(vb);
    CHECK(dynamic_cast<csvsqldb::ValBool*>(val));
    CHECK(csvsqldb::ValBool{true} == *val);
    CHECK(block.offset() == (sizeof(csvsqldb::ValBool) + 1) * 3);
    CHECK(block.isValue((sizeof(csvsqldb::ValBool) + 1) * 2));

    val = block.addBool(false, true);
    CHECK(dynamic_cast<csvsqldb::ValBool*>(val));
    CHECK(val->isNull());
    CHECK(block.offset() == (sizeof(csvsqldb::ValBool) + 1) * 4);
    CHECK(block.isValue((sizeof(csvsqldb::ValBool) + 1) * 3));
  }
  SECTION("add date")
  {
    auto* val = block.addDate(csvsqldb::Date{1995, csvsqldb::Date::May, 16}, false);
    CHECK(dynamic_cast<csvsqldb::ValDate*>(val));
    CHECK(csvsqldb::ValDate{csvsqldb::Date{1995, csvsqldb::Date::May, 16}} == *val);
    CHECK(block.offset() == sizeof(csvsqldb::ValDate) + 1);
    CHECK(block.isValue(0));

    csvsqldb::ValDate d{csvsqldb::Date{2000, csvsqldb::Date::August, 8}};
    val = block.addValue(d);
    CHECK(dynamic_cast<csvsqldb::ValDate*>(val));
    CHECK(csvsqldb::ValDate{csvsqldb::Date{2000, csvsqldb::Date::August, 8}} == *val);
    CHECK(block.offset() == (sizeof(csvsqldb::ValDate) + 1) * 2);
    CHECK(block.isValue(sizeof(csvsqldb::ValDate) + 1));

    csvsqldb::Variant vd{csvsqldb::Date{2020, csvsqldb::Date::November, 21}};
    val = block.addValue(vd);
    CHECK(dynamic_cast<csvsqldb::ValDate*>(val));
    CHECK(csvsqldb::ValDate{csvsqldb::Date{2020, csvsqldb::Date::November, 21}} == *val);
    CHECK(block.offset() == (sizeof(csvsqldb::ValDate) + 1) * 3);
    CHECK(block.isValue((sizeof(csvsqldb::ValDate) + 1) * 2));

    val = block.addDate(csvsqldb::Date{}, true);
    CHECK(dynamic_cast<csvsqldb::ValDate*>(val));
    CHECK(val->isNull());
    CHECK(block.offset() == (sizeof(csvsqldb::ValDate) + 1) * 4);
    CHECK(block.isValue((sizeof(csvsqldb::ValDate) + 1) * 3));
  }
  SECTION("add time")
  {
    auto* val = block.addTime(csvsqldb::Time{17, 6, 27}, false);
    CHECK(dynamic_cast<csvsqldb::ValTime*>(val));
    CHECK(csvsqldb::ValTime{csvsqldb::Time{17, 6, 27}} == *val);
    CHECK(block.offset() == sizeof(csvsqldb::ValTime) + 1);
    CHECK(block.isValue(0));

    csvsqldb::ValTime d{csvsqldb::Time{9, 9, 9}};
    val = block.addValue(d);
    CHECK(dynamic_cast<csvsqldb::ValTime*>(val));
    CHECK(csvsqldb::ValTime{csvsqldb::Time{9, 9, 9}} == *val);
    CHECK(block.offset() == (sizeof(csvsqldb::ValTime) + 1) * 2);
    CHECK(block.isValue(sizeof(csvsqldb::ValTime) + 1));

    csvsqldb::Variant vd{csvsqldb::Time{21, 47, 59}};
    val = block.addValue(vd);
    CHECK(dynamic_cast<csvsqldb::ValTime*>(val));
    CHECK(csvsqldb::ValTime{csvsqldb::Time{21, 47, 59}} == *val);
    CHECK(block.offset() == (sizeof(csvsqldb::ValTime) + 1) * 3);
    CHECK(block.isValue((sizeof(csvsqldb::ValTime) + 1) * 2));

    val = block.addTime(csvsqldb::Time{}, true);
    CHECK(dynamic_cast<csvsqldb::ValTime*>(val));
    CHECK(val->isNull());
    CHECK(block.offset() == (sizeof(csvsqldb::ValTime) + 1) * 4);
    CHECK(block.isValue((sizeof(csvsqldb::ValTime) + 1) * 3));
  }
  SECTION("add timestamp")
  {
    auto* val = block.addTimestamp(csvsqldb::Timestamp{1995, csvsqldb::Date::May, 16, 17, 6, 27}, false);
    CHECK(dynamic_cast<csvsqldb::ValTimestamp*>(val));
    CHECK(csvsqldb::ValTimestamp{csvsqldb::Timestamp{1995, csvsqldb::Date::May, 16, 17, 6, 27}} == *val);
    CHECK(block.offset() == sizeof(csvsqldb::ValTimestamp) + 1);
    CHECK(block.isValue(0));

    csvsqldb::ValTimestamp d{csvsqldb::Timestamp{2000, csvsqldb::Date::August, 8, 9, 9, 9}};
    val = block.addValue(d);
    CHECK(dynamic_cast<csvsqldb::ValTimestamp*>(val));
    CHECK(csvsqldb::ValTimestamp{csvsqldb::Timestamp{2000, csvsqldb::Date::August, 8, 9, 9, 9}} == *val);
    CHECK(block.offset() == (sizeof(csvsqldb::ValTimestamp) + 1) * 2);
    CHECK(block.isValue(sizeof(csvsqldb::ValTimestamp) + 1));

    csvsqldb::Variant vd{csvsqldb::Timestamp{2020, csvsqldb::Date::November, 21, 21, 47, 59}};
    val = block.addValue(vd);
    CHECK(dynamic_cast<csvsqldb::ValTimestamp*>(val));
    CHECK(csvsqldb::ValTimestamp{csvsqldb::Timestamp{2020, csvsqldb::Date::November, 21, 21, 47, 59}} == *val);
    CHECK(block.offset() == (sizeof(csvsqldb::ValTimestamp) + 1) * 3);
    CHECK(block.isValue((sizeof(csvsqldb::ValTimestamp) + 1) * 2));

    val = block.addTimestamp(csvsqldb::Timestamp{}, true);
    CHECK(dynamic_cast<csvsqldb::ValTimestamp*>(val));
    CHECK(val->isNull());
    CHECK(block.offset() == (sizeof(csvsqldb::ValTimestamp) + 1) * 4);
    CHECK(block.isValue((sizeof(csvsqldb::ValTimestamp) + 1) * 3));
  }
  SECTION("add variant null")
  {
    auto* val = block.addValue(csvsqldb::Variant{csvsqldb::INT});
    CHECK(val->isNull());
    val = block.addValue(csvsqldb::Variant{csvsqldb::REAL});
    CHECK(val->isNull());
    val = block.addValue(csvsqldb::Variant{csvsqldb::STRING});
    CHECK(val->isNull());
    val = block.addValue(csvsqldb::Variant{csvsqldb::BOOLEAN});
    CHECK(val->isNull());
    val = block.addValue(csvsqldb::Variant{csvsqldb::DATE});
    CHECK(val->isNull());
    val = block.addValue(csvsqldb::Variant{csvsqldb::TIME});
    CHECK(val->isNull());
    val = block.addValue(csvsqldb::Variant{csvsqldb::TIMESTAMP});
    CHECK(val->isNull());
  }
  SECTION("add value null")
  {
    auto* val = block.addValue(csvsqldb::ValInt{});
    CHECK(val->isNull());
    val = block.addValue(csvsqldb::ValDouble{});
    CHECK(val->isNull());
    val = block.addValue(csvsqldb::ValString{});
    CHECK(val->isNull());
    val = block.addValue(csvsqldb::ValBool{});
    CHECK(val->isNull());
    val = block.addValue(csvsqldb::ValDate{});
    CHECK(val->isNull());
    val = block.addValue(csvsqldb::ValTime{});
    CHECK(val->isNull());
    val = block.addValue(csvsqldb::ValTimestamp{});
    CHECK(val->isNull());
  }
  SECTION("not enough size")
  {
    csvsqldb::Block block1{4711, 10};
    CHECK_FALSE(block1.addInt(42, false));
  }
  SECTION("marker")
  {
    csvsqldb::Block block1{4711, 1024};

    for (auto n = 0; n < 3; ++n) {
      for (auto m = 0; m < 10; ++m) {
        CHECK(block1.addInt(m, false));
      }
      block1.nextRow();
    }
    block1.endBlocks();
    CHECK(block1.isRow((sizeof(csvsqldb::ValInt) + 1) * 10));
    CHECK(block1.isRow((sizeof(csvsqldb::ValInt) + 1) * 20) + 1);
    CHECK(block1.isRow((sizeof(csvsqldb::ValInt) + 1) * 30) + 2);
    CHECK(block1.isEnd((sizeof(csvsqldb::ValInt) + 1) * 30) + 3);
  }
  SECTION("mark next block")
  {
    for (auto m = 0; m < 10; ++m) {
      CHECK(block.addInt(m, false));
    }
    block.markNextBlock();
    CHECK(block.isBlock((sizeof(csvsqldb::ValInt) + 1) * 10) + 1);
  }
  SECTION("allocate")
  {
    struct Test {
      Test()
      {
      }

      Test(int32_t i)
      : _i{i}
      {
      }

      int32_t _i{4711};
    };

    auto* t = block.allocate<Test>();
    CHECK(t);
    REQUIRE(dynamic_cast<Test*>(t));
    CHECK(4711 == dynamic_cast<Test*>(t)->_i);

    t = block.allocate<Test>(42);
    CHECK(t);
    REQUIRE(dynamic_cast<Test*>(t));
    CHECK(42 == dynamic_cast<Test*>(t)->_i);
  }
  SECTION("allocate too much")
  {
    csvsqldb::Block block1{4711, 5};
    CHECK_FALSE(block1.allocate<int64_t>());
  }
  SECTION("add value error")
  {
    CHECK_THROWS_AS(block.addValue(csvsqldb::Variant()), csvsqldb::Exception);
  }
  SECTION("out of range offset")
  {
    CHECK_THROWS_WITH(block.getRawBuffer(256), "block offset out of range");
    CHECK_THROWS_WITH(block.getRawBuffer(257), "block offset out of range");
    CHECK_THROWS_WITH(block.getRawBuffer(1024), "block offset out of range");
    CHECK_THROWS_WITH(block.getRawBuffer(std::numeric_limits<size_t>::max()), "block offset out of range");

    CHECK_THROWS_WITH(block.isValue(256), "block offset out of range");
    CHECK_THROWS_WITH(block.isValue(257), "block offset out of range");
    CHECK_THROWS_WITH(block.isValue(1024), "block offset out of range");
    CHECK_THROWS_WITH(block.isValue(std::numeric_limits<size_t>::max()), "block offset out of range");

    CHECK_THROWS_WITH(block.isRow(256), "block offset out of range");
    CHECK_THROWS_WITH(block.isRow(257), "block offset out of range");
    CHECK_THROWS_WITH(block.isRow(1024), "block offset out of range");
    CHECK_THROWS_WITH(block.isRow(std::numeric_limits<size_t>::max()), "block offset out of range");

    CHECK_THROWS_WITH(block.isBlock(256), "block offset out of range");
    CHECK_THROWS_WITH(block.isBlock(257), "block offset out of range");
    CHECK_THROWS_WITH(block.isBlock(1024), "block offset out of range");
    CHECK_THROWS_WITH(block.isBlock(std::numeric_limits<size_t>::max()), "block offset out of range");

    CHECK_THROWS_WITH(block.isEnd(256), "block offset out of range");
    CHECK_THROWS_WITH(block.isEnd(257), "block offset out of range");
    CHECK_THROWS_WITH(block.isEnd(1024), "block offset out of range");
    CHECK_THROWS_WITH(block.isEnd(std::numeric_limits<size_t>::max()), "block offset out of range");
  }
}
