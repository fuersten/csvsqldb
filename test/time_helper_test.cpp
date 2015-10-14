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

#include "libcsvsqldb/base/date.h"
#include "libcsvsqldb/base/time_helper.h"
#include "libcsvsqldb/base/string_helper.h"


namespace csvsqldb
{

    static std::string callTimeStream(const std::chrono::system_clock::time_point& tp)
    {
        std::ostringstream os;
        os << tp;
        return os.str();
    }
}


class TimeHelperTestCase
{
public:
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void timeConversionTest()
    {
        std::chrono::duration<int, std::mega> megaSecs(22);
        std::chrono::duration<int, std::kilo> kiloSecs(921);
        std::chrono::duration<int, std::deca> decaSecs(20);
        csvsqldb::Timepoint tp;
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

        MPF_TEST_ASSERTEQUAL(buffer, csvsqldb::callTimeStream(tp));
        MPF_TEST_ASSERTEQUAL("1970-09-23T07:00:00", csvsqldb::timestampToGMTString(tp));
        MPF_TEST_ASSERTEQUAL(buffer, csvsqldb::timestampToLocalString(tp));

        csvsqldb::Timepoint ltp = csvsqldb::stringToTimestamp("1970-09-23T07:00:00");
        MPF_TEST_ASSERTEQUAL(tp, ltp);
    }

    void dateConversion()
    {
        csvsqldb::Date d1 = csvsqldb::dateFromString("2014-12-30");
        MPF_TEST_ASSERTEQUAL(30, d1.day());
        MPF_TEST_ASSERTEQUAL(12, d1.month());
        MPF_TEST_ASSERTEQUAL(2014, d1.year());
    }
};

MPF_REGISTER_TEST_START("ApplicationTestSuite", TimeHelperTestCase);
MPF_REGISTER_TEST(TimeHelperTestCase::timeConversionTest);
MPF_REGISTER_TEST(TimeHelperTestCase::dateConversion);
MPF_REGISTER_TEST_END();
