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


TEST_CASE("ValTime Test", "[values]")
{
  SECTION("construct empty")
  {
    csvsqldb::ValTime d;
    CHECK(csvsqldb::TIME == d.getType());
    CHECK(d.isNull());
    CHECK(csvsqldb::Time{} == d.asTime());
  }

  SECTION("construct from date")
  {
    csvsqldb::ValTime d{csvsqldb::Time(20, 11, 30)};
    CHECK(csvsqldb::TIME == d.getType());
    CHECK_FALSE(d.isNull());
    CHECK(csvsqldb::Time(20, 11, 30) == d.asTime());
  }

  SECTION("copy from ValTime")
  {
    csvsqldb::ValTime d{csvsqldb::Time(19, 17, 1)};

    csvsqldb::ValTime d1{d};
    CHECK_FALSE(d1.isNull());
    CHECK(csvsqldb::Time(19, 17, 1) == d1.asTime());

    csvsqldb::ValTime d2;
    d2 = d1;
    CHECK_FALSE(d2.isNull());
    CHECK(csvsqldb::Time(19, 17, 1) == d2.asTime());
  }

  SECTION("compare less")
  {
    CHECK_FALSE(csvsqldb::ValTime{} < csvsqldb::ValTime{});
    CHECK_FALSE(csvsqldb::ValTime{} < csvsqldb::ValTime{csvsqldb::Time(8, 35, 55)});
    CHECK_FALSE(csvsqldb::ValTime{csvsqldb::Time(8, 35, 55)} < csvsqldb::ValTime{});

    CHECK(csvsqldb::ValTime{csvsqldb::Time(8, 35, 55)} < csvsqldb::ValTime{csvsqldb::Time(9, 11, 15)});
    CHECK_FALSE(csvsqldb::ValTime{csvsqldb::Time(9, 11, 15)} < csvsqldb::ValTime{csvsqldb::Time(8, 35, 55)});

    CHECK_THROWS(csvsqldb::ValTime{csvsqldb::Time(8, 35, 55)} < csvsqldb::ValDouble{43.0});
  }

  SECTION("compare equal")
  {
    CHECK_FALSE(csvsqldb::ValTime{} == csvsqldb::ValTime{});
    CHECK_FALSE(csvsqldb::ValTime{} == csvsqldb::ValTime{csvsqldb::Time(8, 35, 55)});
    CHECK_FALSE(csvsqldb::ValTime{csvsqldb::Time(8, 35, 55)} == csvsqldb::ValTime{});

    CHECK_FALSE(csvsqldb::ValTime{csvsqldb::Time(8, 35, 55)} == csvsqldb::ValTime{csvsqldb::Time(9, 11, 15)});
    CHECK_FALSE(csvsqldb::ValTime{csvsqldb::Time(9, 11, 15)} == csvsqldb::ValTime{csvsqldb::Time(8, 35, 55)});
    CHECK(csvsqldb::ValTime{csvsqldb::Time(8, 35, 55)} == csvsqldb::ValTime{csvsqldb::Time(8, 35, 55)});

    CHECK_THROWS(csvsqldb::ValTime{csvsqldb::Time(8, 35, 55)} == csvsqldb::ValDouble{42.0});
  }

  SECTION("to stream")
  {
    {
      std::ostringstream os;
      csvsqldb::ValTime{csvsqldb::Time(8, 35, 55)}.toStream(os);
      CHECK("08:35:55" == os.str());
    }
    {
      std::ostringstream os;
      csvsqldb::ValTime{}.toStream(os);
      CHECK("NULL" == os.str());
    }
  }

  SECTION("to string")
  {
    CHECK("08:35:55" == csvsqldb::ValTime{csvsqldb::Time(8, 35, 55)}.toString());
    CHECK("NULL" == csvsqldb::ValTime{}.toString());
  }

  SECTION("size")
  {
    csvsqldb::ValTime d{};
    CHECK(d.getBaseSize() == d.getSize());

    csvsqldb::ValTime d1{csvsqldb::Time(8, 35, 55)};
    CHECK(d1.getBaseSize() == d1.getSize());

    CHECK(d.getSize() == d1.getSize());
  }

  SECTION("hash")
  {
    CHECK(csvsqldb::ValTime{}.getHash() != csvsqldb::ValTime{csvsqldb::Time(8, 35, 55)}.getHash());
    CHECK(csvsqldb::ValTime{csvsqldb::Time(9, 11, 15)}.getHash() != csvsqldb::ValTime{csvsqldb::Time(8, 35, 55)}.getHash());
    CHECK(csvsqldb::ValTime{csvsqldb::Time(9, 11, 15)}.getHash() == csvsqldb::ValTime{csvsqldb::Time(9, 11, 15)}.getHash());
  }
}
