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


#include "libcsvsqldb/base/regexp.h"

#include <catch2/catch.hpp>


TEST_CASE("Regexp Test", "[utils]")
{
  SECTION("empty")
  {
    csvsqldb::RegExp r("");
    CHECK(r.match(""));
  }

  SECTION("simple")
  {
    csvsqldb::RegExp r("ab");
    CHECK(r.match("ab"));
    CHECK_FALSE(r.match("ba"));

    r = "(a|b)(a|b)";
    CHECK(r.match("ba"));
    CHECK(r.match("ab"));
    CHECK(r.match("aa"));
    CHECK(r.match("bb"));
    CHECK_FALSE(r.match("bbb"));

    r = "((ab)cd)";
    CHECK(r.match("abcd"));
    CHECK_FALSE(r.match("ab"));

    CHECK_THROWS_AS(r = "(ab", std::runtime_error);
    CHECK_THROWS_AS(r = "((ab)cd", std::runtime_error);
  }

  SECTION("move & copy")
  {
    csvsqldb::RegExp r("abcd");
    CHECK(r.match("abcd"));

    csvsqldb::RegExp e(std::move(r));
    CHECK(e.match("abcd"));

    e = "abcd";
    CHECK(e.match("abcd"));

    csvsqldb::RegExp e1 = std::move(e);
    CHECK(e1.match("abcd"));
  }

  SECTION("star plus or")
  {
    csvsqldb::RegExp r("a+");
    CHECK_FALSE(r.match(""));
    CHECK(r.match("a"));
    CHECK_FALSE(r.match("b"));
    CHECK(r.match("aa"));
    CHECK_FALSE(r.match("ba"));
    CHECK_FALSE(r.match("aab"));
    CHECK_FALSE(r.match("abb"));
    CHECK(r.match("aaaaaaaa"));
    CHECK_FALSE(r.match("aaaaaaaabb"));
    CHECK_FALSE(r.match("aba"));
    CHECK_FALSE(r.match("abba"));
    CHECK_FALSE(r.match("abbba"));
    CHECK_FALSE(r.match("abbbba"));

    r = "(a|b)*";
    CHECK(r.match(""));
    CHECK(r.match("a"));
    CHECK(r.match("b"));
    CHECK(r.match("aa"));
    CHECK(r.match("ba"));
    CHECK(r.match("aab"));
    CHECK(r.match("abb"));
    CHECK(r.match("aaaaaaaa"));
    CHECK(r.match("aaaaaaaabb"));
    CHECK(r.match("aba"));
    CHECK(r.match("abba"));
    CHECK(r.match("abbba"));
    CHECK(r.match("abbbba"));

    r = "a*";
    CHECK(r.match(""));
    CHECK(r.match("a"));
    CHECK_FALSE(r.match("b"));
    CHECK(r.match("aa"));
    CHECK_FALSE(r.match("ba"));
    CHECK_FALSE(r.match("aab"));
    CHECK_FALSE(r.match("abb"));
    CHECK(r.match("aaaaaaaa"));
    CHECK_FALSE(r.match("aaaaaaaabb"));
    CHECK_FALSE(r.match("aba"));
    CHECK_FALSE(r.match("abba"));
    CHECK_FALSE(r.match("abbba"));
    CHECK_FALSE(r.match("abbbba"));

    r = "a(a|b)b";
    CHECK_FALSE(r.match(""));
    CHECK_FALSE(r.match("a"));
    CHECK_FALSE(r.match("b"));
    CHECK_FALSE(r.match("aa"));
    CHECK_FALSE(r.match("ba"));
    CHECK(r.match("aab"));
    CHECK(r.match("abb"));
    CHECK_FALSE(r.match("aaaaaaaa"));
    CHECK_FALSE(r.match("aaaaaaaabb"));
    CHECK_FALSE(r.match("aba"));
    CHECK_FALSE(r.match("abba"));
    CHECK_FALSE(r.match("abbba"));
    CHECK_FALSE(r.match("abbbba"));

    r = "(a|b)*abb";
    CHECK_FALSE(r.match(""));
    CHECK_FALSE(r.match("a"));
    CHECK_FALSE(r.match("b"));
    CHECK_FALSE(r.match("aa"));
    CHECK_FALSE(r.match("ba"));
    CHECK_FALSE(r.match("aab"));
    CHECK(r.match("abb"));
    CHECK_FALSE(r.match("aaaaaaaa"));
    CHECK(r.match("aaaaaaaabb"));
    CHECK_FALSE(r.match("aba"));
    CHECK_FALSE(r.match("abba"));
    CHECK_FALSE(r.match("abbba"));
    CHECK_FALSE(r.match("abbbba"));

    r = "(a|b)*a";
    CHECK_FALSE(r.match(""));
    CHECK(r.match("a"));
    CHECK_FALSE(r.match("b"));
    CHECK(r.match("aa"));
    CHECK(r.match("ba"));
    CHECK_FALSE(r.match("aab"));
    CHECK_FALSE(r.match("abb"));
    CHECK(r.match("aaaaaaaa"));
    CHECK_FALSE(r.match("aaaaaaaabb"));
    CHECK(r.match("aba"));
    CHECK(r.match("abba"));
    CHECK(r.match("abbba"));
    CHECK(r.match("abbbba"));

    r = "a(bb)+a";
    CHECK_FALSE(r.match(""));
    CHECK_FALSE(r.match("a"));
    CHECK_FALSE(r.match("b"));
    CHECK_FALSE(r.match("aa"));
    CHECK_FALSE(r.match("ba"));
    CHECK_FALSE(r.match("aab"));
    CHECK_FALSE(r.match("abb"));
    CHECK_FALSE(r.match("aaaaaaaa"));
    CHECK_FALSE(r.match("aaaaaaaabb"));
    CHECK_FALSE(r.match("aba"));
    CHECK(r.match("abba"));
    CHECK_FALSE(r.match("abbba"));
    CHECK(r.match("abbbba"));

    r = "(a|b)c*e+f";
    CHECK(r.match("beef"));
    CHECK(r.match("acef"));
    CHECK(r.match("bceeeeeeef"));
    CHECK_FALSE(r.match("bceeeeeeeff"));
  }

  SECTION("quest")
  {
    csvsqldb::RegExp r("abc?d");
    CHECK(r.match("abcd"));
    CHECK(r.match("abd"));

    r = "a(a|b)?b";
    CHECK(r.match("abb"));
    CHECK(r.match("ab"));
    CHECK(r.match("aab"));
    CHECK_FALSE(r.match("aaab"));

    r = "abcd?";
    CHECK(r.match("abcd"));
    CHECK(r.match("abc"));
  }

  SECTION("complex")
  {
    csvsqldb::RegExp r("(0|(1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*)\\.(0|1|2|3|4|5|6|7|8|9)+");
    CHECK(r.match("1.1"));
    CHECK(r.match("0.52635"));
    CHECK(r.match("6253700.52635"));

    CHECK_FALSE(r.match("00.52635"));
    CHECK_FALSE(r.match("0."));
  }

  SECTION("character classes")
  {
    csvsqldb::RegExp r(".*");
    CHECK(r.match(""));
    CHECK(r.match("ashfd8w9hnf0"));

    r = "abc.*";
    CHECK(r.match("abcsdkuwdg"));
    CHECK_FALSE(r.match("absdkuwdg"));

    r = "abc.*a";
    CHECK_FALSE(r.match("abcsdkuwdg"));
    CHECK(r.match("abcsdkuwdga"));

    r = "\\w(\\w|\\.)+@\\w+\\.\\w\\w\\w?";
    CHECK(r.match("lcf@miztli.de"));
    CHECK(r.match("lars.fuerstenberg@parstream.com"));

    r = "(0|(1|2|3|4|5|6|7|8|9)\\d*)\\.\\d+";
    CHECK(r.match("1.1"));
    CHECK(r.match("0.52635"));
    CHECK(r.match("6253700.52635"));

    CHECK_FALSE(r.match("00.52635"));
    CHECK_FALSE(r.match("0."));

    r = ".*(test).*";
    CHECK(r.match("test"));
    CHECK(r.match("This is a test of a regular expression"));
  }

  SECTION("character sets")
  {
    csvsqldb::RegExp r("[A-Za-z_]+");
    CHECK(r.match("aszgdwzZFTFfsf_T"));

    r = "[^B]+";
    CHECK(r.match("aszgdwzZFTFfsf_T"));
    CHECK_FALSE(r.match("aszgdwzZFTFfsfBT"));

    r = "(0|[1-9]\\d*)\\.\\d+";
    CHECK(r.match("1.1"));
    CHECK(r.match("0.52635"));
    CHECK(r.match("6253700.52635"));

    CHECK_FALSE(r.match("00.52635"));
    CHECK_FALSE(r.match("0."));

    r = "[-0-9]+";
    CHECK(r.match("23874-31-31938-138310-313872"));

    r = "[0-9-]+";
    CHECK(r.match("23874-31-31938-138310-313872"));

    r = "[0-9\\]]+";
    CHECK(r.match("23874]31]31938]138310]313872"));

    CHECK_THROWS_AS(r = "[0-9-+", std::runtime_error);
    CHECK_THROWS_AS(r = "[0-9\\", std::runtime_error);
  }
}
