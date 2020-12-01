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

#include <cstring>


TEST_CASE("ValString Test", "[values]")
{
  SECTION("construct empty")
  {
    csvsqldb::ValString s;
    CHECK(csvsqldb::STRING == s.getType());
    CHECK(s.isNull());
    CHECK_FALSE(s.asString());
  }

  SECTION("construct from char pointer")
  {
    csvsqldb::ValString s{"Test"};
    CHECK(csvsqldb::STRING == s.getType());
    CHECK_FALSE(s.isNull());
    CHECK(std::strcmp("Test", s.asString()) == 0);
  }

  SECTION("construct from empty char pointer")
  {
    csvsqldb::ValString s{nullptr};
    CHECK(csvsqldb::STRING == s.getType());
    CHECK(s.isNull());
    CHECK_FALSE(s.asString());
  }

  SECTION("copy from ValString")
  {
    csvsqldb::ValString s{"Hello world!"};

    csvsqldb::ValString s1{s};
    CHECK_FALSE(s1.isNull());
    CHECK(std::strcmp("Hello world!", s1.asString()) == 0);

    csvsqldb::ValString s2;
    s2 = s1;
    CHECK_FALSE(s2.isNull());
    CHECK(std::strcmp("Hello world!", s2.asString()) == 0);
  }

  SECTION("compare less")
  {
    CHECK_FALSE(csvsqldb::ValString{} < csvsqldb::ValString{});
    CHECK_FALSE(csvsqldb::ValString{} < csvsqldb::ValString{"Hello world!"});
    CHECK_FALSE(csvsqldb::ValString{"Hello world!"} < csvsqldb::ValString{});

    CHECK(csvsqldb::ValString{"ABC"} < csvsqldb::ValString{"Hello world!"});
    CHECK_FALSE(csvsqldb::ValString{"Hello world!"} < csvsqldb::ValString{"ABC"});

    CHECK_THROWS(csvsqldb::ValString{"ABC"} < csvsqldb::ValDouble{43.0});
  }

  SECTION("compare equal")
  {
    CHECK_FALSE(csvsqldb::ValString{} == csvsqldb::ValString{});
    CHECK_FALSE(csvsqldb::ValString{} == csvsqldb::ValString{"Hello world!"});
    CHECK_FALSE(csvsqldb::ValString{"Hello world!"} == csvsqldb::ValString{});

    CHECK_FALSE(csvsqldb::ValString{"Hello world!"} == csvsqldb::ValString{"ABC"});
    CHECK_FALSE(csvsqldb::ValString{"ABC"} == csvsqldb::ValString{"Hello world!"});
    CHECK(csvsqldb::ValString{"Hello world!"} == csvsqldb::ValString{"Hello world!"});

    CHECK_THROWS(csvsqldb::ValString{"ABC"} == csvsqldb::ValDouble{42.0});
  }

  SECTION("to stream")
  {
    {
      std::ostringstream os;
      csvsqldb::ValString{"Hello world!"}.toStream(os);
      CHECK("Hello world!" == os.str());
    }
    {
      std::ostringstream os;
      csvsqldb::ValString{}.toStream(os);
      CHECK("NULL" == os.str());
    }
  }

  SECTION("to string")
  {
    CHECK("Hello world!" == csvsqldb::ValString{"Hello world!"}.toString());
    CHECK("NULL" == csvsqldb::ValString{}.toString());
  }

  SECTION("size")
  {
    csvsqldb::ValString s{};
    CHECK(s.getBaseSize() != s.getSize());
    CHECK(s.getBaseSize() + s.getLength() + 1 == s.getSize());

    csvsqldb::ValString s1{"Hello world!"};
    CHECK(s1.getBaseSize() != s1.getSize());
    CHECK(s1.getBaseSize() + s1.getLength() + 1 == s1.getSize());

    CHECK(s.getSize() != s1.getSize());
  }

  SECTION("hash")
  {
    CHECK(csvsqldb::ValString{}.getHash() != csvsqldb::ValString{"Hello world!"}.getHash());
    CHECK(csvsqldb::ValString{"Check this out"}.getHash() != csvsqldb::ValString{"Hello world!"}.getHash());
    CHECK(csvsqldb::ValString{"Check this out"}.getHash() == csvsqldb::ValString{"Check this out"}.getHash());

    CHECK(csvsqldb::ValString{"Check this out"}.getHash() == std::hash<csvsqldb::Value>()(csvsqldb::ValString{"Check this out"}));
  }
}
