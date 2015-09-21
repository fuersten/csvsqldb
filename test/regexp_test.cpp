//
//  regexptest.cpp
//  csvsqldb
//
//  Created by Lars-Christian Fürstenberg on 07.04.13.
//  Copyright (c) 2013 Fürstenberg IT Systems. All rights reserved.
//


#include "test.h"

#include "libcsvsqldb/base/regexp.h"


class RegExpTestCase
{
public:
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void empty()
    {
        csvsqldb::RegExp r("");
        MPF_TEST_ASSERT(r.match(""));
    }

    void simple()
    {
        csvsqldb::RegExp r("ab");
        MPF_TEST_ASSERT(r.match("ab"));
        MPF_TEST_ASSERT(!r.match("ba"));

        r = "(a|b)(a|b)";
        MPF_TEST_ASSERT(r.match("ba"));
        MPF_TEST_ASSERT(r.match("ab"));
        MPF_TEST_ASSERT(r.match("aa"));
        MPF_TEST_ASSERT(r.match("bb"));
        MPF_TEST_ASSERT(!r.match("bbb"));

        r = "((ab)cd)";
        MPF_TEST_ASSERT(r.match("abcd"));
        MPF_TEST_ASSERT(!r.match("ab"));

        MPF_TEST_EXPECTS(r = "(ab", std::runtime_error);
        MPF_TEST_EXPECTS(r = "((ab)cd", std::runtime_error);
    }

    void copyRegexp()
    {
        csvsqldb::RegExp r("abcd");
        MPF_TEST_ASSERT(r.match("abcd"));

        csvsqldb::RegExp e(r);
        MPF_TEST_ASSERT(e.match("abcd"));

        e = "abcd";
        MPF_TEST_ASSERT(e.match("abcd"));
    }

    void starPlusOr()
    {
        csvsqldb::RegExp r("a+");
        MPF_TEST_ASSERT(!r.match(""));
        MPF_TEST_ASSERT(r.match("a"));
        MPF_TEST_ASSERT(!r.match("b"));
        MPF_TEST_ASSERT(r.match("aa"));
        MPF_TEST_ASSERT(!r.match("ba"));
        MPF_TEST_ASSERT(!r.match("aab"));
        MPF_TEST_ASSERT(!r.match("abb"));
        MPF_TEST_ASSERT(r.match("aaaaaaaa"));
        MPF_TEST_ASSERT(!r.match("aaaaaaaabb"));
        MPF_TEST_ASSERT(!r.match("aba"));
        MPF_TEST_ASSERT(!r.match("abba"));
        MPF_TEST_ASSERT(!r.match("abbba"));
        MPF_TEST_ASSERT(!r.match("abbbba"));

        r = "(a|b)*";
        MPF_TEST_ASSERT(r.match(""));
        MPF_TEST_ASSERT(r.match("a"));
        MPF_TEST_ASSERT(r.match("b"));
        MPF_TEST_ASSERT(r.match("aa"));
        MPF_TEST_ASSERT(r.match("ba"));
        MPF_TEST_ASSERT(r.match("aab"));
        MPF_TEST_ASSERT(r.match("abb"));
        MPF_TEST_ASSERT(r.match("aaaaaaaa"));
        MPF_TEST_ASSERT(r.match("aaaaaaaabb"));
        MPF_TEST_ASSERT(r.match("aba"));
        MPF_TEST_ASSERT(r.match("abba"));
        MPF_TEST_ASSERT(r.match("abbba"));
        MPF_TEST_ASSERT(r.match("abbbba"));

        r = "a*";
        MPF_TEST_ASSERT(r.match(""));
        MPF_TEST_ASSERT(r.match("a"));
        MPF_TEST_ASSERT(!r.match("b"));
        MPF_TEST_ASSERT(r.match("aa"));
        MPF_TEST_ASSERT(!r.match("ba"));
        MPF_TEST_ASSERT(!r.match("aab"));
        MPF_TEST_ASSERT(!r.match("abb"));
        MPF_TEST_ASSERT(r.match("aaaaaaaa"));
        MPF_TEST_ASSERT(!r.match("aaaaaaaabb"));
        MPF_TEST_ASSERT(!r.match("aba"));
        MPF_TEST_ASSERT(!r.match("abba"));
        MPF_TEST_ASSERT(!r.match("abbba"));
        MPF_TEST_ASSERT(!r.match("abbbba"));

        r = "a(a|b)b";
        MPF_TEST_ASSERT(!r.match(""));
        MPF_TEST_ASSERT(!r.match("a"));
        MPF_TEST_ASSERT(!r.match("b"));
        MPF_TEST_ASSERT(!r.match("aa"));
        MPF_TEST_ASSERT(!r.match("ba"));
        MPF_TEST_ASSERT(r.match("aab"));
        MPF_TEST_ASSERT(r.match("abb"));
        MPF_TEST_ASSERT(!r.match("aaaaaaaa"));
        MPF_TEST_ASSERT(!r.match("aaaaaaaabb"));
        MPF_TEST_ASSERT(!r.match("aba"));
        MPF_TEST_ASSERT(!r.match("abba"));
        MPF_TEST_ASSERT(!r.match("abbba"));
        MPF_TEST_ASSERT(!r.match("abbbba"));

        r = "(a|b)*abb";
        MPF_TEST_ASSERT(!r.match(""));
        MPF_TEST_ASSERT(!r.match("a"));
        MPF_TEST_ASSERT(!r.match("b"));
        MPF_TEST_ASSERT(!r.match("aa"));
        MPF_TEST_ASSERT(!r.match("ba"));
        MPF_TEST_ASSERT(!r.match("aab"));
        MPF_TEST_ASSERT(r.match("abb"));
        MPF_TEST_ASSERT(!r.match("aaaaaaaa"));
        MPF_TEST_ASSERT(r.match("aaaaaaaabb"));
        MPF_TEST_ASSERT(!r.match("aba"));
        MPF_TEST_ASSERT(!r.match("abba"));
        MPF_TEST_ASSERT(!r.match("abbba"));
        MPF_TEST_ASSERT(!r.match("abbbba"));

        r = "(a|b)*a";
        MPF_TEST_ASSERT(!r.match(""));
        MPF_TEST_ASSERT(r.match("a"));
        MPF_TEST_ASSERT(!r.match("b"));
        MPF_TEST_ASSERT(r.match("aa"));
        MPF_TEST_ASSERT(r.match("ba"));
        MPF_TEST_ASSERT(!r.match("aab"));
        MPF_TEST_ASSERT(!r.match("abb"));
        MPF_TEST_ASSERT(r.match("aaaaaaaa"));
        MPF_TEST_ASSERT(!r.match("aaaaaaaabb"));
        MPF_TEST_ASSERT(r.match("aba"));
        MPF_TEST_ASSERT(r.match("abba"));
        MPF_TEST_ASSERT(r.match("abbba"));
        MPF_TEST_ASSERT(r.match("abbbba"));

        r = "a(bb)+a";
        MPF_TEST_ASSERT(!r.match(""));
        MPF_TEST_ASSERT(!r.match("a"));
        MPF_TEST_ASSERT(!r.match("b"));
        MPF_TEST_ASSERT(!r.match("aa"));
        MPF_TEST_ASSERT(!r.match("ba"));
        MPF_TEST_ASSERT(!r.match("aab"));
        MPF_TEST_ASSERT(!r.match("abb"));
        MPF_TEST_ASSERT(!r.match("aaaaaaaa"));
        MPF_TEST_ASSERT(!r.match("aaaaaaaabb"));
        MPF_TEST_ASSERT(!r.match("aba"));
        MPF_TEST_ASSERT(r.match("abba"));
        MPF_TEST_ASSERT(!r.match("abbba"));
        MPF_TEST_ASSERT(r.match("abbbba"));

        r = "(a|b)c*e+f";
        MPF_TEST_ASSERT(r.match("beef"));
        MPF_TEST_ASSERT(r.match("acef"));
        MPF_TEST_ASSERT(r.match("bceeeeeeef"));
        MPF_TEST_ASSERT(!r.match("bceeeeeeeff"));
    }

    void quest()
    {
        csvsqldb::RegExp r("abc?d");
        MPF_TEST_ASSERT(r.match("abcd"));
        MPF_TEST_ASSERT(r.match("abd"));

        r = "a(a|b)?b";
        MPF_TEST_ASSERT(r.match("abb"));
        MPF_TEST_ASSERT(r.match("ab"));
        MPF_TEST_ASSERT(r.match("aab"));
        MPF_TEST_ASSERT(!r.match("aaab"));

        r = "abcd?";
        MPF_TEST_ASSERT(r.match("abcd"));
        MPF_TEST_ASSERT(r.match("abc"));
    }

    void complex()
    {
        csvsqldb::RegExp r("(0|(1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*)\\.(0|1|2|3|4|5|6|7|8|9)+");
        MPF_TEST_ASSERT(r.match("1.1"));
        MPF_TEST_ASSERT(r.match("0.52635"));
        MPF_TEST_ASSERT(r.match("6253700.52635"));

        MPF_TEST_ASSERT(!r.match("00.52635"));
        MPF_TEST_ASSERT(!r.match("0."));
    }

    void characterClasses()
    {
        csvsqldb::RegExp r(".*");
        MPF_TEST_ASSERT(r.match(""));
        MPF_TEST_ASSERT(r.match("ashfd8w9hnf0"));

        r = "abc.*";
        MPF_TEST_ASSERT(r.match("abcsdkuwdg"));
        MPF_TEST_ASSERT(!r.match("absdkuwdg"));

        r = "abc.*a";
        MPF_TEST_ASSERT(!r.match("abcsdkuwdg"));
        MPF_TEST_ASSERT(r.match("abcsdkuwdga"));

        r = "\\w(\\w|\\.)+@\\w+\\.\\w\\w\\w?";
        MPF_TEST_ASSERT(r.match("lcf@miztli.de"));
        MPF_TEST_ASSERT(r.match("lars.fuerstenberg@parstream.com"));

        r = "(0|(1|2|3|4|5|6|7|8|9)\\d*)\\.\\d+";
        MPF_TEST_ASSERT(r.match("1.1"));
        MPF_TEST_ASSERT(r.match("0.52635"));
        MPF_TEST_ASSERT(r.match("6253700.52635"));

        MPF_TEST_ASSERT(!r.match("00.52635"));
        MPF_TEST_ASSERT(!r.match("0."));

        r = ".*(test).*";
        MPF_TEST_ASSERT(r.match("test"));
        MPF_TEST_ASSERT(r.match("This is a test of a regular expression"));
    }

    void characterSets()
    {
        csvsqldb::RegExp r("[A-Za-z_]+");
        MPF_TEST_ASSERT(r.match("aszgdwzZFTFfsf_T"));

        r = "[^B]+";
        MPF_TEST_ASSERT(r.match("aszgdwzZFTFfsf_T"));
        MPF_TEST_ASSERT(!r.match("aszgdwzZFTFfsfBT"));

        r = "(0|[1-9]\\d*)\\.\\d+";
        MPF_TEST_ASSERT(r.match("1.1"));
        MPF_TEST_ASSERT(r.match("0.52635"));
        MPF_TEST_ASSERT(r.match("6253700.52635"));

        MPF_TEST_ASSERT(!r.match("00.52635"));
        MPF_TEST_ASSERT(!r.match("0."));

        r = "[-0-9]+";
        MPF_TEST_ASSERT(r.match("23874-31-31938-138310-313872"));

        r = "[0-9-]+";
        MPF_TEST_ASSERT(r.match("23874-31-31938-138310-313872"));

        r = "[0-9\\]]+";
        MPF_TEST_ASSERT(r.match("23874]31]31938]138310]313872"));

        MPF_TEST_EXPECTS(r = "[0-9-+", std::runtime_error);
        MPF_TEST_EXPECTS(r = "[0-9\\", std::runtime_error);
    }
};

MPF_REGISTER_TEST_START("RegExpTestSuite", RegExpTestCase);
MPF_REGISTER_TEST(RegExpTestCase::empty);
MPF_REGISTER_TEST(RegExpTestCase::simple);
MPF_REGISTER_TEST(RegExpTestCase::copyRegexp);
MPF_REGISTER_TEST(RegExpTestCase::starPlusOr);
MPF_REGISTER_TEST(RegExpTestCase::quest);
MPF_REGISTER_TEST(RegExpTestCase::complex);
MPF_REGISTER_TEST(RegExpTestCase::characterClasses);
MPF_REGISTER_TEST(RegExpTestCase::characterSets);
MPF_REGISTER_TEST_END();
