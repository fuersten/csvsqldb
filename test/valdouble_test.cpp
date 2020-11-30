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


TEST_CASE("ValDouble Test", "[values]")
{
  SECTION("construct empty")
  {
    csvsqldb::ValDouble d;
    CHECK(csvsqldb::REAL == d.getType());
    CHECK(d.isNull());
    CHECK(std::numeric_limits<double>::max() == Approx(d.asDouble()));
  }

  SECTION("construct from double")
  {
    csvsqldb::ValDouble dPositive{47.11};
    CHECK(csvsqldb::REAL == dPositive.getType());
    CHECK_FALSE(dPositive.isNull());
    CHECK(47.11 == Approx(dPositive.asDouble()));

    csvsqldb::ValDouble dNegative{-42};
    CHECK_FALSE(dNegative.isNull());
    CHECK(-42 == Approx(dNegative.asDouble()));

    csvsqldb::ValDouble dBig{std::numeric_limits<double>::max()};
    CHECK_FALSE(dBig.isNull());
    CHECK(std::numeric_limits<double>::max() == Approx(dBig.asDouble()));

    csvsqldb::ValDouble dSmall{std::numeric_limits<double>::min()};
    CHECK_FALSE(dSmall.isNull());
    CHECK(std::numeric_limits<double>::min() == Approx(dSmall.asDouble()));
  }

  SECTION("copy from ValDouble")
  {
    csvsqldb::ValDouble d{47.11};

    csvsqldb::ValDouble d1{d};
    CHECK_FALSE(d1.isNull());
    CHECK(47.11 == Approx(d1.asDouble()));

    csvsqldb::ValDouble d2;
    d2 = d1;
    CHECK_FALSE(d2.isNull());
    CHECK(47.11 == Approx(d2.asDouble()));
  }

  SECTION("compare less")
  {
    CHECK_FALSE(csvsqldb::ValDouble{} < csvsqldb::ValDouble{});
    CHECK_FALSE(csvsqldb::ValDouble{} < csvsqldb::ValDouble{42});
    CHECK_FALSE(csvsqldb::ValDouble{42} < csvsqldb::ValDouble{});

    CHECK(csvsqldb::ValDouble{42} < csvsqldb::ValDouble{47.11});
    CHECK_FALSE(csvsqldb::ValDouble{47.11} < csvsqldb::ValDouble{42});
    CHECK(csvsqldb::ValDouble{-42} < csvsqldb::ValDouble{47.11});
    CHECK_FALSE(csvsqldb::ValDouble{-42} < csvsqldb::ValDouble{-47.11});
    CHECK(csvsqldb::ValDouble{-47.11} < csvsqldb::ValDouble{-42});

    CHECK_THROWS(csvsqldb::ValInt{42} < csvsqldb::ValDouble{43.0});
  }

  SECTION("compare equal")
  {
    CHECK_FALSE(csvsqldb::ValDouble{} == csvsqldb::ValDouble{});
    CHECK_FALSE(csvsqldb::ValDouble{} == csvsqldb::ValDouble{42});
    CHECK_FALSE(csvsqldb::ValDouble{42} == csvsqldb::ValDouble{});

    CHECK_FALSE(csvsqldb::ValDouble{42} == csvsqldb::ValDouble{47.11});
    CHECK_FALSE(csvsqldb::ValDouble{47.11} == csvsqldb::ValDouble{42});
    CHECK(csvsqldb::ValDouble{42} == csvsqldb::ValDouble{42});
    CHECK(csvsqldb::ValDouble{-42} == csvsqldb::ValDouble{-42});

    CHECK_THROWS(csvsqldb::ValInt{42} == csvsqldb::ValDouble{42.0});
  }

  SECTION("to stream")
  {
    {
      std::ostringstream os;
      csvsqldb::ValDouble{47.11}.toStream(os);
      CHECK("47.110000" == os.str());
    }
    {
      std::ostringstream os;
      csvsqldb::ValDouble{-42}.toStream(os);
      CHECK("-42.000000" == os.str());
    }
    {
      std::ostringstream os;
      csvsqldb::ValDouble{}.toStream(os);
      CHECK("NULL" == os.str());
    }
  }

  SECTION("to string")
  {
    CHECK("47.110000" == csvsqldb::ValDouble{47.11}.toString());
    CHECK("-800.150000" == csvsqldb::ValDouble{-800.15}.toString());
    CHECK("NULL" == csvsqldb::ValDouble{}.toString());
  }

  SECTION("size")
  {
    csvsqldb::ValDouble d{};
    CHECK(d.getBaseSize() == d.getSize());

    csvsqldb::ValDouble d1{47.11};
    CHECK(d1.getBaseSize() == d1.getSize());

    CHECK(d.getSize() == d1.getSize());
  }

  SECTION("hash")
  {
    CHECK(csvsqldb::ValDouble{}.getHash() != csvsqldb::ValDouble{42}.getHash());
    CHECK(csvsqldb::ValDouble{47.11}.getHash() != csvsqldb::ValDouble{42}.getHash());
    CHECK(csvsqldb::ValDouble{47.11}.getHash() == csvsqldb::ValDouble{47.11}.getHash());

    CHECK(csvsqldb::ValDouble{}.getHash() != csvsqldb::ValDouble{-42}.getHash());
    CHECK(csvsqldb::ValDouble{-47.11}.getHash() != csvsqldb::ValDouble{-42}.getHash());
    CHECK(csvsqldb::ValDouble{-47.11}.getHash() == csvsqldb::ValDouble{-47.11}.getHash());

    CHECK(csvsqldb::ValDouble{47.11}.getHash() == std::hash<csvsqldb::Value>()(csvsqldb::ValDouble{47.11}));
  }
}
