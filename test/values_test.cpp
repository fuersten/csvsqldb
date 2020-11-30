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
#include <csvsqldb/values.h>

#include <catch2/catch.hpp>

#include <array>


TEST_CASE("Values Test", "[values]")
{
  SECTION("value creator")
  {
    csvsqldb::Value* v1 = csvsqldb::ValueCreator<std::string>::createValue(std::string("Lars"));
    csvsqldb::Value* v2 = csvsqldb::ValueCreator<std::string>::createValue(std::string("Fürstenberg"));

    CHECK("Lars" == v1->toString());
    CHECK("Fürstenberg" == v2->toString());
    CHECK(csvsqldb::STRING == v1->getType());

    delete v1;
    delete v2;

    v1 = csvsqldb::ValueCreator<int64_t>::createValue(4711);
    CHECK("4711" == v1->toString());
    CHECK(csvsqldb::INT == v1->getType());
    csvsqldb::ValInt* pi = static_cast<csvsqldb::ValInt*>(v1);
    CHECK(4711 == pi->asInt());
    delete v1;

    v1 = csvsqldb::ValueCreator<double>::createValue(47.11);
    CHECK(csvsqldb::REAL == v1->getType());
    CHECK("47.110000" == v1->toString());
    csvsqldb::ValDouble* pd = static_cast<csvsqldb::ValDouble*>(v1);
    CHECK(47.11 == Approx(pd->asDouble()));
    delete v1;

    v1 = csvsqldb::ValueCreator<bool>::createValue(true);
    CHECK(csvsqldb::BOOLEAN == v1->getType());
    CHECK("1" == v1->toString());
    csvsqldb::ValBool* pb = static_cast<csvsqldb::ValBool*>(v1);
    CHECK(pb->asBool());
    delete v1;

    v1 = csvsqldb::ValueCreator<csvsqldb::Date>::createValue(csvsqldb::Date(1970, csvsqldb::Date::September, 23));
    CHECK(csvsqldb::DATE == v1->getType());
    CHECK("1970-09-23" == v1->toString());
    csvsqldb::ValDate* pdt = static_cast<csvsqldb::ValDate*>(v1);
    CHECK(csvsqldb::Date(1970, csvsqldb::Date::September, 23) == pdt->asDate());
    delete v1;

    v1 = csvsqldb::ValueCreator<csvsqldb::Time>::createValue(csvsqldb::Time(8, 9, 11));
    CHECK(csvsqldb::TIME == v1->getType());
    CHECK("08:09:11" == v1->toString());
    csvsqldb::ValTime* pt = static_cast<csvsqldb::ValTime*>(v1);
    CHECK(csvsqldb::Time(8, 9, 11) == pt->asTime());
    delete v1;

    v1 = csvsqldb::ValueCreator<csvsqldb::Timestamp>::createValue(
      csvsqldb::Timestamp(1970, csvsqldb::Date::September, 23, 8, 9, 11));
    CHECK(csvsqldb::TIMESTAMP == v1->getType());
    CHECK("1970-09-23T08:09:11" == v1->toString());
    csvsqldb::ValTimestamp* pts = static_cast<csvsqldb::ValTimestamp*>(v1);
    CHECK(csvsqldb::Timestamp(1970, csvsqldb::Date::September, 23, 8, 9, 11) == pts->asTimestamp());
    delete v1;
  }

  SECTION("store")
  {
    using StoreType = std::array<char, 4096>;

    StoreType store;
    size_t offset = 0;

    {
      csvsqldb::ValInt* pi = new (&store[0]) csvsqldb::ValInt(4711l);
      offset = pi->getSize();
      csvsqldb::ValDouble* pd = new (&store[0] + offset) csvsqldb::ValDouble(47.11);
      offset += pd->getSize();
      csvsqldb::ValBool* pb = new (&store[0] + offset) csvsqldb::ValBool(true);
      offset += pb->getSize();
      csvsqldb::ValDate* pdt = new (&store[0] + offset) csvsqldb::ValDate(csvsqldb::Date(1970, csvsqldb::Date::September, 23));
      offset += pdt->getSize();
      csvsqldb::ValTime* pt = new (&store[0] + offset) csvsqldb::ValTime(csvsqldb::Time(8, 9, 11));
      offset += pt->getSize();
      csvsqldb::ValTimestamp* pts =
        new (&store[0] + offset) csvsqldb::ValTimestamp(csvsqldb::Timestamp(1970, csvsqldb::Date::September, 23, 8, 9, 11));
      offset += pts->getSize();
      char* psd = &store[0] + offset + sizeof(csvsqldb::ValString);
      ::strcpy(psd, "Lars Fürstenberg");
      csvsqldb::ValString* ps = new (&store[0] + offset) csvsqldb::ValString(psd);

      CHECK("4711" == pi->toString());
      CHECK("47.110000" == pd->toString());
      CHECK("1" == pb->toString());
      CHECK("1970-09-23" == pdt->toString());
      CHECK("1970-09-23T08:09:11" == pts->toString());
      CHECK("Lars Fürstenberg" == ps->toString());
      CHECK("08:09:11" == pt->toString());
    }

    {
      csvsqldb::ValInt* pi = reinterpret_cast<csvsqldb::ValInt*>(&store[0]);
      CHECK("4711" == pi->toString());

      csvsqldb::ValString* ps = reinterpret_cast<csvsqldb::ValString*>(&store[0] + offset);
      CHECK("Lars Fürstenberg" == ps->toString());
    }
  }
}

TEST_CASE("ValInt Test", "[values]")
{
  SECTION("construct empty")
  {
    csvsqldb::ValInt i;
    CHECK(csvsqldb::INT == i.getType());
    CHECK(i.isNull());
    CHECK(std::numeric_limits<int64_t>::max() == i.asInt());
  }

  SECTION("construct from int")
  {
    csvsqldb::ValInt iPositive{4711};
    CHECK(csvsqldb::INT == iPositive.getType());
    CHECK_FALSE(iPositive.isNull());
    CHECK(4711 == iPositive.asInt());

    csvsqldb::ValInt iNegative{-42};
    CHECK_FALSE(iNegative.isNull());
    CHECK(-42 == iNegative.asInt());

    csvsqldb::ValInt iBig{std::numeric_limits<int64_t>::max()};
    CHECK_FALSE(iBig.isNull());
    CHECK(std::numeric_limits<int64_t>::max() == iBig.asInt());

    csvsqldb::ValInt iSmall{std::numeric_limits<int64_t>::min()};
    CHECK_FALSE(iSmall.isNull());
    CHECK(std::numeric_limits<int64_t>::min() == iSmall.asInt());
  }

  SECTION("copy from ValInt")
  {
    csvsqldb::ValInt i{4711};

    csvsqldb::ValInt i1{i};
    CHECK_FALSE(i1.isNull());
    CHECK(4711 == i1.asInt());

    csvsqldb::ValInt i2;
    i2 = i1;
    CHECK_FALSE(i2.isNull());
    CHECK(4711 == i2.asInt());
  }

  SECTION("compare less")
  {
    CHECK_FALSE(csvsqldb::ValInt{} < csvsqldb::ValInt{});
    CHECK_FALSE(csvsqldb::ValInt{} < csvsqldb::ValInt{42});
    CHECK_FALSE(csvsqldb::ValInt{42} < csvsqldb::ValInt{});

    CHECK(csvsqldb::ValInt{42} < csvsqldb::ValInt{4711});
    CHECK_FALSE(csvsqldb::ValInt{4711} < csvsqldb::ValInt{42});
    CHECK(csvsqldb::ValInt{-42} < csvsqldb::ValInt{4711});
    CHECK_FALSE(csvsqldb::ValInt{-42} < csvsqldb::ValInt{-4711});
    CHECK(csvsqldb::ValInt{-4711} < csvsqldb::ValInt{-42});

    CHECK_THROWS(csvsqldb::ValInt{42} < csvsqldb::ValDouble{43.0});
  }

  SECTION("compare equal")
  {
    CHECK_FALSE(csvsqldb::ValInt{} == csvsqldb::ValInt{});
    CHECK_FALSE(csvsqldb::ValInt{} == csvsqldb::ValInt{42});
    CHECK_FALSE(csvsqldb::ValInt{42} == csvsqldb::ValInt{});

    CHECK_FALSE(csvsqldb::ValInt{42} == csvsqldb::ValInt{4711});
    CHECK_FALSE(csvsqldb::ValInt{4711} == csvsqldb::ValInt{42});
    CHECK(csvsqldb::ValInt{42} == csvsqldb::ValInt{42});
    CHECK(csvsqldb::ValInt{-42} == csvsqldb::ValInt{-42});

    CHECK_THROWS(csvsqldb::ValInt{42} == csvsqldb::ValDouble{42.0});
  }

  SECTION("to stream")
  {
    {
      std::ostringstream os;
      csvsqldb::ValInt{4711}.toStream(os);
      CHECK("4711" == os.str());
    }
    {
      std::ostringstream os;
      csvsqldb::ValInt{-42}.toStream(os);
      CHECK("-42" == os.str());
    }
    {
      std::ostringstream os;
      csvsqldb::ValInt{}.toStream(os);
      CHECK("NULL" == os.str());
    }
  }

  SECTION("to string")
  {
    CHECK("4711" == csvsqldb::ValInt{4711}.toString());
    CHECK("-815" == csvsqldb::ValInt{-815}.toString());
    CHECK("NULL" == csvsqldb::ValInt{}.toString());
  }

  SECTION("size")
  {
    csvsqldb::ValInt i{};
    CHECK(i.getBaseSize() == i.getSize());

    csvsqldb::ValInt i1{4711};
    CHECK(i1.getBaseSize() == i1.getSize());

    CHECK(i.getSize() == i1.getSize());
  }

  SECTION("hash")
  {
    CHECK(csvsqldb::ValInt{}.getHash() != csvsqldb::ValInt{42}.getHash());
    CHECK(csvsqldb::ValInt{4711}.getHash() != csvsqldb::ValInt{42}.getHash());
    CHECK(csvsqldb::ValInt{4711}.getHash() == csvsqldb::ValInt{4711}.getHash());

    CHECK(csvsqldb::ValInt{}.getHash() != csvsqldb::ValInt{-42}.getHash());
    CHECK(csvsqldb::ValInt{-4711}.getHash() != csvsqldb::ValInt{-42}.getHash());
    CHECK(csvsqldb::ValInt{-4711}.getHash() == csvsqldb::ValInt{-4711}.getHash());

    CHECK(csvsqldb::ValInt{4711}.getHash() == std::hash<csvsqldb::Value>()(csvsqldb::ValInt{4711}));
  }
}
