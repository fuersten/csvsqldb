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


#include <csvsqldb/values.h>

#include <catch2/catch.hpp>


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
