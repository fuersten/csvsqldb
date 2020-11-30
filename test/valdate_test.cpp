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


TEST_CASE("ValDate Test", "[values]")
{
  SECTION("construct empty")
  {
    csvsqldb::ValDate d;
    CHECK(csvsqldb::DATE == d.getType());
    CHECK(d.isNull());
    CHECK(csvsqldb::Date{} == d.asDate());
  }

  SECTION("construct from date")
  {
    csvsqldb::ValDate d{csvsqldb::Date(2020, csvsqldb::Date::November, 30)};
    CHECK(csvsqldb::DATE == d.getType());
    CHECK_FALSE(d.isNull());
    CHECK(csvsqldb::Date(2020, csvsqldb::Date::November, 30) == d.asDate());
  }

  SECTION("copy from ValDate")
  {
    csvsqldb::ValDate d{csvsqldb::Date(1970, csvsqldb::Date::January, 1)};

    csvsqldb::ValDate d1{d};
    CHECK_FALSE(d1.isNull());
    CHECK(csvsqldb::Date(1970, csvsqldb::Date::January, 1) == d1.asDate());

    csvsqldb::ValDate d2;
    d2 = d1;
    CHECK_FALSE(d2.isNull());
    CHECK(csvsqldb::Date(1970, csvsqldb::Date::January, 1) == d2.asDate());
  }

  SECTION("compare less")
  {
    CHECK_FALSE(csvsqldb::ValDate{} < csvsqldb::ValDate{});
    CHECK_FALSE(csvsqldb::ValDate{} < csvsqldb::ValDate{csvsqldb::Date(1983, csvsqldb::Date::May, 5)});
    CHECK_FALSE(csvsqldb::ValDate{csvsqldb::Date(1983, csvsqldb::Date::May, 5)} < csvsqldb::ValDate{});

    CHECK(csvsqldb::ValDate{csvsqldb::Date(1983, csvsqldb::Date::May, 5)} <
          csvsqldb::ValDate{csvsqldb::Date(1990, csvsqldb::Date::October, 15)});
    CHECK_FALSE(csvsqldb::ValDate{csvsqldb::Date(1990, csvsqldb::Date::October, 15)} <
                csvsqldb::ValDate{csvsqldb::Date(1983, csvsqldb::Date::May, 5)});

    CHECK_THROWS(csvsqldb::ValDate{csvsqldb::Date(1983, csvsqldb::Date::May, 5)} < csvsqldb::ValDouble{43.0});
  }

  SECTION("compare equal")
  {
    CHECK_FALSE(csvsqldb::ValDate{} == csvsqldb::ValDate{});
    CHECK_FALSE(csvsqldb::ValDate{} == csvsqldb::ValDate{csvsqldb::Date(1983, csvsqldb::Date::May, 5)});
    CHECK_FALSE(csvsqldb::ValDate{csvsqldb::Date(1983, csvsqldb::Date::May, 5)} == csvsqldb::ValDate{});

    CHECK_FALSE(csvsqldb::ValDate{csvsqldb::Date(1983, csvsqldb::Date::May, 5)} ==
                csvsqldb::ValDate{csvsqldb::Date(1990, csvsqldb::Date::October, 15)});
    CHECK_FALSE(csvsqldb::ValDate{csvsqldb::Date(1990, csvsqldb::Date::October, 15)} ==
                csvsqldb::ValDate{csvsqldb::Date(1983, csvsqldb::Date::May, 5)});
    CHECK(csvsqldb::ValDate{csvsqldb::Date(1983, csvsqldb::Date::May, 5)} ==
          csvsqldb::ValDate{csvsqldb::Date(1983, csvsqldb::Date::May, 5)});

    CHECK_THROWS(csvsqldb::ValDate{csvsqldb::Date(1983, csvsqldb::Date::May, 5)} == csvsqldb::ValDouble{42.0});
  }

  SECTION("to stream")
  {
    {
      std::ostringstream os;
      csvsqldb::ValDate{csvsqldb::Date(1983, csvsqldb::Date::May, 5)}.toStream(os);
      CHECK("1983-05-05" == os.str());
    }
    {
      std::ostringstream os;
      csvsqldb::ValDate{}.toStream(os);
      CHECK("NULL" == os.str());
    }
  }

  SECTION("to string")
  {
    CHECK("1983-05-05" == csvsqldb::ValDate{csvsqldb::Date(1983, csvsqldb::Date::May, 5)}.toString());
    CHECK("NULL" == csvsqldb::ValDate{}.toString());
  }

  SECTION("size")
  {
    csvsqldb::ValDate d{};
    CHECK(d.getBaseSize() == d.getSize());

    csvsqldb::ValDate d1{csvsqldb::Date(1983, csvsqldb::Date::May, 5)};
    CHECK(d1.getBaseSize() == d1.getSize());

    CHECK(d.getSize() == d1.getSize());
  }

  SECTION("hash")
  {
    CHECK(csvsqldb::ValDate{}.getHash() != csvsqldb::ValDate{csvsqldb::Date(1983, csvsqldb::Date::May, 5)}.getHash());
    CHECK(csvsqldb::ValDate{csvsqldb::Date(1990, csvsqldb::Date::October, 15)}.getHash() !=
          csvsqldb::ValDate{csvsqldb::Date(1983, csvsqldb::Date::May, 5)}.getHash());
    CHECK(csvsqldb::ValDate{csvsqldb::Date(1990, csvsqldb::Date::October, 15)}.getHash() ==
          csvsqldb::ValDate{csvsqldb::Date(1990, csvsqldb::Date::October, 15)}.getHash());
  }
}
