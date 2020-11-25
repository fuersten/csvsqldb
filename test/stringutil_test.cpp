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


#include <csvsqldb/base/string_helper.h>

#include <catch2/catch.hpp>

#include <vector>


namespace csvsqldb
{
  namespace testspace
  {
    class Test
    {
    };
  }

  static std::string callTimeStream(const std::chrono::system_clock::time_point& tp)
  {
    std::ostringstream os;
    os << tp;
    return os.str();
  }
}


TEST_CASE("String Util Test", "[utils]")
{
  SECTION("tokenizer")
  {
    std::string s("This is my glorious test   string");
    std::vector<std::string> tokens;
    csvsqldb::split(s, ' ', tokens);

    CHECK(8u == tokens.size());

    CHECK("This" == tokens[0]);
    CHECK("is" == tokens[1]);
    CHECK("my" == tokens[2]);
    CHECK("glorious" == tokens[3]);
    CHECK("test" == tokens[4]);
    CHECK("" == tokens[5]);
    CHECK("" == tokens[6]);
    CHECK("string" == tokens[7]);

    s = "This,is,my,glorious,test,,,string";
    csvsqldb::split(s, ' ', tokens);

    CHECK(1u == tokens.size());

    CHECK("This,is,my,glorious,test,,,string" == tokens[0]);

    s = "This,is,my,glorious,test,,,string";
    csvsqldb::split(s, ',', tokens);

    CHECK(8u == tokens.size());

    CHECK("This" == tokens[0]);
    CHECK("is" == tokens[1]);
    CHECK("my" == tokens[2]);
    CHECK("glorious" == tokens[3]);
    CHECK("test" == tokens[4]);
    CHECK("" == tokens[5]);
    CHECK("" == tokens[6]);
    CHECK("string" == tokens[7]);

    s = "This,is,my,glorious,test,,,string";
    csvsqldb::split(s, ',', tokens, false);
    CHECK(6u == tokens.size());

    CHECK("This" == tokens[0]);
    CHECK("is" == tokens[1]);
    CHECK("my" == tokens[2]);
    CHECK("glorious" == tokens[3]);
    CHECK("test" == tokens[4]);
    CHECK("string" == tokens[5]);
  }

  SECTION("join")
  {
    std::vector<std::string> tokens;
    tokens.push_back("This");
    tokens.push_back("is");
    tokens.push_back("my");
    tokens.push_back("glorious");
    tokens.push_back("test");
    tokens.push_back("string");

    CHECK("This,is,my,glorious,test,string" == csvsqldb::join(tokens, ","));
  }

  SECTION("upper")
  {
    std::string s("Not All upper");
    csvsqldb::toupper(s);
    CHECK("NOT ALL UPPER" == s);

    std::string s1("Not All upper");
    std::string s2 = csvsqldb::toupper_copy(s1);
    CHECK("NOT ALL UPPER" == s2);
    CHECK("Not All upper" == s1);
  }

  SECTION("lower")
  {
    std::string s("Not All upper");
    csvsqldb::tolower(s);
    CHECK("not all upper" == s);

    std::string s1("Not All upper");
    std::string s2 = csvsqldb::tolower_copy(s1);
    CHECK("not all upper" == s2);
    CHECK("Not All upper" == s1);
  }

  SECTION("strip type name")
  {
    CHECK("csvsqldb::testspace::Test" == csvsqldb::stripTypeName(typeid(csvsqldb::testspace::Test).name()));
  }

  SECTION("string compare")
  {
    CHECK(csvsqldb::stricmp("Test it", "TEST IT") == 0);
    CHECK(csvsqldb::stricmp("Txst it", "TEST IT") > 0);
    CHECK(csvsqldb::strnicmp("Test it", "TEST IT", 5) == 0);
    CHECK(csvsqldb::strnicmp("Txst it", "TEST IT", 5) > 0);
  }

  SECTION("time format")
  {
    std::chrono::duration<int, std::mega> megaSecs(22);
    std::chrono::duration<int, std::kilo> kiloSecs(921);
    std::chrono::duration<int, std::deca> decaSecs(20);
    std::chrono::system_clock::time_point tp;
    tp += megaSecs;
    tp += kiloSecs;
    tp += decaSecs;

    struct tm ts;
    ts.tm_hour = 7;
    ts.tm_min = 0;
    ts.tm_sec = 0;
    ts.tm_year = 1970 - 1900;
    ts.tm_mon = 8;
    ts.tm_mday = 23;
    ts.tm_isdst = 0;
    char utc[] = "UTC";
    ts.tm_zone = &utc[0];

    char buffer[20];
    time_t tt = timegm(&ts);
    struct tm* lt = ::localtime(&tt);
    ::strftime(buffer, 20, "%FT%T", lt);

    CHECK(buffer == csvsqldb::callTimeStream(tp));
    CHECK("Wed, 23 Sep 1970 07:00:00 GMT" == csvsqldb::formatDateRfc1123(tp));
  }

  SECTION("trim")
  {
    std::string s("     left whitespace");
    CHECK("left whitespace" == csvsqldb::trim_left(s));
    s = "no left whitespace";
    CHECK("no left whitespace" == csvsqldb::trim_left(s));
    s = "right whitespace    ";
    CHECK("right whitespace" == csvsqldb::trim_right(s));
    s = "no right whitespace";
    CHECK("no right whitespace" == csvsqldb::trim_right(s));
  }
}
