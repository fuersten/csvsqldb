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


TEST_CASE("ValTimestamp Test", "[values]")
{
  SECTION("construct empty")
  {
    csvsqldb::ValTimestamp d;
    CHECK(csvsqldb::TIMESTAMP == d.getType());
    CHECK(d.isNull());
    CHECK(csvsqldb::Timestamp{} == d.asTimestamp());
  }

  SECTION("construct from timestamp")
  {
    csvsqldb::ValTimestamp d{csvsqldb::Timestamp(2020, csvsqldb::Date::November, 30, 20, 11, 30)};
    CHECK(csvsqldb::TIMESTAMP == d.getType());
    CHECK_FALSE(d.isNull());
    CHECK(csvsqldb::Timestamp(2020, csvsqldb::Date::November, 30, 20, 11, 30) == d.asTimestamp());
  }

  SECTION("copy from ValTimestamp")
  {
    csvsqldb::ValTimestamp d{csvsqldb::Timestamp(1970, csvsqldb::Date::January, 1, 19, 17, 1)};

    csvsqldb::ValTimestamp d1{d};
    CHECK_FALSE(d1.isNull());
    CHECK(csvsqldb::Timestamp(1970, csvsqldb::Date::January, 1, 19, 17, 1) == d1.asTimestamp());

    csvsqldb::ValTimestamp d2;
    d2 = d1;
    CHECK_FALSE(d2.isNull());
    CHECK(csvsqldb::Timestamp(1970, csvsqldb::Date::January, 1, 19, 17, 1) == d2.asTimestamp());
  }

  SECTION("compare less")
  {
    CHECK_FALSE(csvsqldb::ValTimestamp{} < csvsqldb::ValTimestamp{});
    CHECK_FALSE(csvsqldb::ValTimestamp{} < csvsqldb::ValTimestamp{csvsqldb::Timestamp(1983, csvsqldb::Date::May, 5, 8, 35, 55)});
    CHECK_FALSE(csvsqldb::ValTimestamp{csvsqldb::Timestamp(1983, csvsqldb::Date::May, 5, 8, 35, 55)} < csvsqldb::ValTimestamp{});

    CHECK(csvsqldb::ValTimestamp{csvsqldb::Timestamp(1983, csvsqldb::Date::May, 5, 8, 35, 55)} <
          csvsqldb::ValTimestamp{csvsqldb::Timestamp(1990, csvsqldb::Date::October, 15, 9, 11, 15)});
    CHECK_FALSE(csvsqldb::ValTimestamp{csvsqldb::Timestamp(1990, csvsqldb::Date::October, 15, 9, 11, 15)} <
                csvsqldb::ValTimestamp{csvsqldb::Timestamp(1983, csvsqldb::Date::May, 5, 8, 35, 55)});

    CHECK_THROWS(csvsqldb::ValTimestamp{csvsqldb::Timestamp(1983, csvsqldb::Date::May, 5, 8, 35, 55)} <
                 csvsqldb::ValDouble{43.0});
  }

  SECTION("compare equal")
  {
    CHECK_FALSE(csvsqldb::ValTimestamp{} == csvsqldb::ValTimestamp{});
    CHECK_FALSE(csvsqldb::ValTimestamp{} == csvsqldb::ValTimestamp{csvsqldb::Timestamp(1983, csvsqldb::Date::May, 5, 8, 35, 55)});
    CHECK_FALSE(csvsqldb::ValTimestamp{csvsqldb::Timestamp(1983, csvsqldb::Date::May, 5, 8, 35, 55)} == csvsqldb::ValTimestamp{});

    CHECK_FALSE(csvsqldb::ValTimestamp{csvsqldb::Timestamp(1983, csvsqldb::Date::May, 5, 8, 35, 55)} ==
                csvsqldb::ValTimestamp{csvsqldb::Timestamp(1990, csvsqldb::Date::October, 15, 9, 11, 15)});
    CHECK_FALSE(csvsqldb::ValTimestamp{csvsqldb::Timestamp(1990, csvsqldb::Date::October, 15, 9, 11, 15)} ==
                csvsqldb::ValTimestamp{csvsqldb::Timestamp(1983, csvsqldb::Date::May, 5, 8, 35, 55)});
    CHECK(csvsqldb::ValTimestamp{csvsqldb::Timestamp(1983, csvsqldb::Date::May, 5, 8, 35, 55)} ==
          csvsqldb::ValTimestamp{csvsqldb::Timestamp(1983, csvsqldb::Date::May, 5, 8, 35, 55)});

    CHECK_THROWS(csvsqldb::ValTimestamp{csvsqldb::Timestamp(1983, csvsqldb::Date::May, 5, 8, 35, 55)} ==
                 csvsqldb::ValDouble{42.0});
  }

  SECTION("to stream")
  {
    {
      std::ostringstream os;
      csvsqldb::ValTimestamp{csvsqldb::Timestamp(1983, csvsqldb::Date::May, 5, 8, 35, 55)}.toStream(os);
      CHECK("1983-05-05T08:35:55" == os.str());
    }
    {
      std::ostringstream os;
      csvsqldb::ValTimestamp{}.toStream(os);
      CHECK("NULL" == os.str());
    }
  }

  SECTION("to string")
  {
    CHECK("1983-05-05T08:35:55" == csvsqldb::ValTimestamp{csvsqldb::Timestamp(1983, csvsqldb::Date::May, 5, 8, 35, 55)}.toString());
    CHECK("NULL" == csvsqldb::ValTimestamp{}.toString());
  }

  SECTION("size")
  {
    csvsqldb::ValTimestamp d{};
    CHECK(d.getBaseSize() == d.getSize());

    csvsqldb::ValTimestamp d1{csvsqldb::Timestamp(1983, csvsqldb::Date::May, 5, 8, 35, 55)};
    CHECK(d1.getBaseSize() == d1.getSize());

    CHECK(d.getSize() == d1.getSize());
  }

  SECTION("hash")
  {
    CHECK(csvsqldb::ValTimestamp{}.getHash() !=
          csvsqldb::ValTimestamp{csvsqldb::Timestamp(1983, csvsqldb::Date::May, 5, 8, 35, 55)}.getHash());
    CHECK(csvsqldb::ValTimestamp{csvsqldb::Timestamp(1990, csvsqldb::Date::October, 15, 9, 11, 15)}.getHash() !=
          csvsqldb::ValTimestamp{csvsqldb::Timestamp(1983, csvsqldb::Date::May, 5, 8, 35, 55)}.getHash());
    CHECK(csvsqldb::ValTimestamp{csvsqldb::Timestamp(1990, csvsqldb::Date::October, 15, 9, 11, 15)}.getHash() ==
          csvsqldb::ValTimestamp{csvsqldb::Timestamp(1990, csvsqldb::Date::October, 15, 9, 11, 15)}.getHash());
  }
}
