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


TEST_CASE("ValBool Test", "[values]")
{
  SECTION("construct empty")
  {
    csvsqldb::ValBool b;
    CHECK(csvsqldb::BOOLEAN == b.getType());
    CHECK(b.isNull());
    CHECK_FALSE(b.asBool());
  }

  SECTION("construct from bool")
  {
    csvsqldb::ValBool bTrue{true};
    CHECK(csvsqldb::BOOLEAN == bTrue.getType());
    CHECK_FALSE(bTrue.isNull());
    CHECK(bTrue.asBool());

    csvsqldb::ValBool bFalse{false};
    CHECK_FALSE(bFalse.isNull());
    CHECK_FALSE(bFalse.asBool());
  }

  SECTION("copy from ValBool")
  {
    csvsqldb::ValBool b{true};

    csvsqldb::ValBool b1{b};
    CHECK_FALSE(b1.isNull());
    CHECK(b1.asBool());

    csvsqldb::ValBool b2;
    b2 = b1;
    CHECK_FALSE(b2.isNull());
    CHECK(b2.asBool());
  }

  SECTION("compare less")
  {
    CHECK_FALSE(csvsqldb::ValBool{} < csvsqldb::ValBool{});
    CHECK_FALSE(csvsqldb::ValBool{} < csvsqldb::ValBool{true});
    CHECK_FALSE(csvsqldb::ValBool{true} < csvsqldb::ValBool{});

    CHECK_FALSE(csvsqldb::ValBool{true} < csvsqldb::ValBool{false});
    CHECK(csvsqldb::ValBool{false} < csvsqldb::ValBool{true});

    CHECK_THROWS(csvsqldb::ValBool{true} < csvsqldb::ValDouble{43.0});
  }

  SECTION("compare equal")
  {
    CHECK_FALSE(csvsqldb::ValBool{} == csvsqldb::ValBool{});
    CHECK_FALSE(csvsqldb::ValBool{} == csvsqldb::ValBool{true});
    CHECK_FALSE(csvsqldb::ValBool{true} == csvsqldb::ValBool{});

    CHECK_FALSE(csvsqldb::ValBool{true} == csvsqldb::ValBool{false});
    CHECK_FALSE(csvsqldb::ValBool{false} == csvsqldb::ValBool{true});
    CHECK(csvsqldb::ValBool{true} == csvsqldb::ValBool{true});
    CHECK(csvsqldb::ValBool{false} == csvsqldb::ValBool{false});

    CHECK_THROWS(csvsqldb::ValBool{true} == csvsqldb::ValDouble{42.0});
  }

  SECTION("to stream")
  {
    {
      std::ostringstream os;
      csvsqldb::ValBool{true}.toStream(os);
      CHECK("1" == os.str());
    }
    {
      std::ostringstream os;
      csvsqldb::ValBool{false}.toStream(os);
      CHECK("0" == os.str());
    }
    {
      std::ostringstream os;
      csvsqldb::ValBool{}.toStream(os);
      CHECK("NULL" == os.str());
    }
  }

  SECTION("to string")
  {
    CHECK("1" == csvsqldb::ValBool{true}.toString());
    CHECK("0" == csvsqldb::ValBool{false}.toString());
    CHECK("NULL" == csvsqldb::ValBool{}.toString());
  }

  SECTION("size")
  {
    csvsqldb::ValBool b{};
    CHECK(b.getBaseSize() == b.getSize());

    csvsqldb::ValBool b1{true};
    CHECK(b1.getBaseSize() == b1.getSize());

    CHECK(b.getSize() == b1.getSize());
  }

  SECTION("hash")
  {
    CHECK(csvsqldb::ValBool{}.getHash() != csvsqldb::ValBool{true}.getHash());
    CHECK(csvsqldb::ValBool{false}.getHash() != csvsqldb::ValBool{true}.getHash());
    CHECK(csvsqldb::ValBool{false}.getHash() == csvsqldb::ValBool{false}.getHash());
    CHECK(csvsqldb::ValBool{true}.getHash() == csvsqldb::ValBool{true}.getHash());
  }
}
