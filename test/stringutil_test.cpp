//
//  csvsqldb test
//
//  BSD 3-Clause License
//  Copyright (c) 2015, Lars-Christian Fürstenberg
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


#include "test.h"

#include "libcsvsqldb/base/string_helper.h"

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


class StringHelperTestCase
{
public:
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void tokenizer()
    {
        std::string s("This is my glorious test   string");
        std::vector<std::string> tokens;
        csvsqldb::split(s, ' ', tokens);

        MPF_TEST_ASSERTEQUAL(8u, tokens.size());

        MPF_TEST_ASSERTEQUAL("This", tokens[0]);
        MPF_TEST_ASSERTEQUAL("is", tokens[1]);
        MPF_TEST_ASSERTEQUAL("my", tokens[2]);
        MPF_TEST_ASSERTEQUAL("glorious", tokens[3]);
        MPF_TEST_ASSERTEQUAL("test", tokens[4]);
        MPF_TEST_ASSERTEQUAL("", tokens[5]);
        MPF_TEST_ASSERTEQUAL("", tokens[6]);
        MPF_TEST_ASSERTEQUAL("string", tokens[7]);

        s = "This,is,my,glorious,test,,,string";
        csvsqldb::split(s, ' ', tokens);

        MPF_TEST_ASSERTEQUAL(1u, tokens.size());

        MPF_TEST_ASSERTEQUAL("This,is,my,glorious,test,,,string", tokens[0]);

        s = "This,is,my,glorious,test,,,string";
        csvsqldb::split(s, ',', tokens);

        MPF_TEST_ASSERTEQUAL(8u, tokens.size());

        MPF_TEST_ASSERTEQUAL("This", tokens[0]);
        MPF_TEST_ASSERTEQUAL("is", tokens[1]);
        MPF_TEST_ASSERTEQUAL("my", tokens[2]);
        MPF_TEST_ASSERTEQUAL("glorious", tokens[3]);
        MPF_TEST_ASSERTEQUAL("test", tokens[4]);
        MPF_TEST_ASSERTEQUAL("", tokens[5]);
        MPF_TEST_ASSERTEQUAL("", tokens[6]);
        MPF_TEST_ASSERTEQUAL("string", tokens[7]);

        s = "This,is,my,glorious,test,,,string";
        csvsqldb::split(s, ',', tokens, false);
        MPF_TEST_ASSERTEQUAL(6u, tokens.size());

        MPF_TEST_ASSERTEQUAL("This", tokens[0]);
        MPF_TEST_ASSERTEQUAL("is", tokens[1]);
        MPF_TEST_ASSERTEQUAL("my", tokens[2]);
        MPF_TEST_ASSERTEQUAL("glorious", tokens[3]);
        MPF_TEST_ASSERTEQUAL("test", tokens[4]);
        MPF_TEST_ASSERTEQUAL("string", tokens[5]);
    }

    void joinTest()
    {
        std::vector<std::string> tokens;
        tokens.push_back("This");
        tokens.push_back("is");
        tokens.push_back("my");
        tokens.push_back("glorious");
        tokens.push_back("test");
        tokens.push_back("string");

        MPF_TEST_ASSERTEQUAL("This,is,my,glorious,test,string", csvsqldb::join(tokens, ","));
    }

    void upperTest()
    {
        std::string s("Not All upper");
        csvsqldb::toupper(s);
        MPF_TEST_ASSERTEQUAL("NOT ALL UPPER", s);

        std::string s1("Not All upper");
        std::string s2 = csvsqldb::toupper_copy(s1);
        MPF_TEST_ASSERTEQUAL("NOT ALL UPPER", s2);
        MPF_TEST_ASSERTEQUAL("Not All upper", s1);
    }

    void lowerTest()
    {
        std::string s("Not All upper");
        csvsqldb::tolower(s);
        MPF_TEST_ASSERTEQUAL("not all upper", s);

        std::string s1("Not All upper");
        std::string s2 = csvsqldb::tolower_copy(s1);
        MPF_TEST_ASSERTEQUAL("not all upper", s2);
        MPF_TEST_ASSERTEQUAL("Not All upper", s1);
    }

    void stripTypeNameTest()
    {
        MPF_TEST_ASSERTEQUAL("csvsqldb::testspace::Test", csvsqldb::stripTypeName(typeid(csvsqldb::testspace::Test).name()));
    }

    void stringCompareTest()
    {
        MPF_TEST_ASSERTEQUAL(0, csvsqldb::stricmp("Test it", "TEST IT"));
        MPF_TEST_ASSERTEQUAL(1, csvsqldb::stricmp("Txst it", "TEST IT"));
        MPF_TEST_ASSERTEQUAL(0, csvsqldb::strnicmp("Test it", "TEST IT", 5));
        MPF_TEST_ASSERTEQUAL(1, csvsqldb::strnicmp("Txst it", "TEST IT", 5));
    }

    void timeFormatTest()
    {
        std::chrono::duration<int, std::mega> megaSecs(22);
        std::chrono::duration<int, std::kilo> kiloSecs(921);
        std::chrono::duration<int, std::deca> decaSecs(20);
        std::chrono::system_clock::time_point tp;
        tp += megaSecs;
        tp += kiloSecs;
        tp += decaSecs;

        MPF_TEST_ASSERTEQUAL("1970-09-23T08:00:00", csvsqldb::callTimeStream(tp));
        MPF_TEST_ASSERTEQUAL("Wed, 23 Sep 1970 07:00:00 GMT", csvsqldb::formatDateRfc1123(tp));
    }

    void trimTest()
    {
        std::string s("     left whitespace");
        MPF_TEST_ASSERTEQUAL("left whitespace", csvsqldb::trim_left(s));
        s = "no left whitespace";
        MPF_TEST_ASSERTEQUAL("no left whitespace", csvsqldb::trim_left(s));
        s = "right whitespace    ";
        MPF_TEST_ASSERTEQUAL("right whitespace", csvsqldb::trim_right(s));
        s = "no right whitespace";
        MPF_TEST_ASSERTEQUAL("no right whitespace", csvsqldb::trim_right(s));
    }

    void decodeTest()
    {
        std::string encoded(
        "wikiPageName=Testpage&wikiPageMarkdown=Markdown%0D%0A%3D%3D%3D%3D%3D%3D%3D%3D%0D%0A%0D%0A-+some+tests%0D%0A-+and+more%"
        "0D%0A%0D%0A");
        std::string decoded;
        MPF_TEST_ASSERT(csvsqldb::decode(encoded, decoded));
        MPF_TEST_ASSERTEQUAL(
        "wikiPageName=Testpage&wikiPageMarkdown=Markdown\r\n========\r\n\r\n- some tests\r\n- and more\r\n\r\n", decoded);

        encoded =
        "wikiPageName=Testpage&wikiPageMarkdown=Markdown% "
        "%0A%3D%3D%3D%3D%3D%3D%3D%3D%0D%0A%0D%0A-+some+tests%0D%0A-+and+more%0D%0A%0D%0A";
        MPF_TEST_ASSERT(!csvsqldb::decode(encoded, decoded));
    }
};

MPF_REGISTER_TEST_START("ApplicationTestSuite", StringHelperTestCase);
MPF_REGISTER_TEST(StringHelperTestCase::tokenizer);
MPF_REGISTER_TEST(StringHelperTestCase::joinTest);
MPF_REGISTER_TEST(StringHelperTestCase::upperTest);
MPF_REGISTER_TEST(StringHelperTestCase::lowerTest);
MPF_REGISTER_TEST(StringHelperTestCase::stripTypeNameTest);
MPF_REGISTER_TEST(StringHelperTestCase::timeFormatTest);
MPF_REGISTER_TEST(StringHelperTestCase::trimTest);
MPF_REGISTER_TEST(StringHelperTestCase::decodeTest);
MPF_REGISTER_TEST_END();
