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
#include "libcsvsqldb/base/timestamp.h"


class TimestampTestCase
{
public:
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void constructionTest()
    {
        csvsqldb::Timestamp ts1(1970, csvsqldb::Date::September, 23, 8, 9, 11, 115);
        MPF_TEST_ASSERTEQUAL(23, ts1.day());
        MPF_TEST_ASSERTEQUAL(9, ts1.month());
        MPF_TEST_ASSERTEQUAL(1970, ts1.year());
        MPF_TEST_ASSERTEQUAL(8, ts1.hour());
        MPF_TEST_ASSERTEQUAL(9, ts1.minute());
        MPF_TEST_ASSERTEQUAL(11, ts1.second());
        MPF_TEST_ASSERTEQUAL(115, ts1.millisecond());

        csvsqldb::Timestamp ts2;
        MPF_TEST_ASSERT(ts2.isInfinite());

        csvsqldb::Timestamp ts3(0, csvsqldb::Date::January, 1, 0, 0, 0, 0);
        MPF_TEST_ASSERTEQUAL(1, ts3.day());
        MPF_TEST_ASSERTEQUAL(1, ts3.month());
        MPF_TEST_ASSERTEQUAL(0, ts3.year());
        MPF_TEST_ASSERTEQUAL(0, ts3.hour());
        MPF_TEST_ASSERTEQUAL(0, ts3.minute());
        MPF_TEST_ASSERTEQUAL(0, ts3.second());
        MPF_TEST_ASSERTEQUAL(0, ts3.millisecond());

        csvsqldb::Timestamp ts4(9999, csvsqldb::Date::December, 31, 23, 59, 59, 999);
        MPF_TEST_ASSERTEQUAL(31, ts4.day());
        MPF_TEST_ASSERTEQUAL(12, ts4.month());
        MPF_TEST_ASSERTEQUAL(9999, ts4.year());
        MPF_TEST_ASSERTEQUAL(23, ts4.hour());
        MPF_TEST_ASSERTEQUAL(59, ts4.minute());
        MPF_TEST_ASSERTEQUAL(59, ts4.second());
        MPF_TEST_ASSERTEQUAL(999, ts4.millisecond());
    }

    void copyTest()
    {
        csvsqldb::Timestamp ts1(1970, csvsqldb::Date::September, 23, 8, 9, 11, 115);
        csvsqldb::Timestamp ts2(ts1);

        MPF_TEST_ASSERTEQUAL(23, ts2.day());
        MPF_TEST_ASSERTEQUAL(9, ts2.month());
        MPF_TEST_ASSERTEQUAL(1970, ts2.year());
        MPF_TEST_ASSERTEQUAL(8, ts2.hour());
        MPF_TEST_ASSERTEQUAL(9, ts2.minute());
        MPF_TEST_ASSERTEQUAL(11, ts2.second());
        MPF_TEST_ASSERTEQUAL(115, ts2.millisecond());

        csvsqldb::Timestamp ts3(2014, csvsqldb::Date::December, 30, 9, 5, 8);
        MPF_TEST_ASSERTEQUAL(30, ts3.day());
        MPF_TEST_ASSERTEQUAL(12, ts3.month());
        MPF_TEST_ASSERTEQUAL(2014, ts3.year());
        MPF_TEST_ASSERTEQUAL(9, ts3.hour());
        MPF_TEST_ASSERTEQUAL(5, ts3.minute());
        MPF_TEST_ASSERTEQUAL(8, ts3.second());
        MPF_TEST_ASSERTEQUAL(0, ts3.millisecond());

        ts3 = ts2;
        MPF_TEST_ASSERTEQUAL(23, ts3.day());
        MPF_TEST_ASSERTEQUAL(9, ts3.month());
        MPF_TEST_ASSERTEQUAL(1970, ts3.year());
        MPF_TEST_ASSERTEQUAL(8, ts3.hour());
        MPF_TEST_ASSERTEQUAL(9, ts3.minute());
        MPF_TEST_ASSERTEQUAL(11, ts3.second());
        MPF_TEST_ASSERTEQUAL(115, ts3.millisecond());
    }

    void validateTest()
    {
        MPF_TEST_ASSERTEQUAL(true, csvsqldb::Timestamp::isValid(9999, csvsqldb::Date::December, 31, 23, 59, 59, 999));
        MPF_TEST_ASSERTEQUAL(false, csvsqldb::Timestamp::isValid(10000, csvsqldb::Date::January, 1, 0, 0, 0, 0));
        MPF_TEST_ASSERTEQUAL(false, csvsqldb::Timestamp::isValid(2014, csvsqldb::Date::November, 31, 25, 5, 17, 0));
        MPF_TEST_ASSERTEQUAL(false, csvsqldb::Timestamp::isValid(2014, csvsqldb::Date::November, 30, 24, 5, 17, 0));
        MPF_TEST_ASSERTEQUAL(true, csvsqldb::Timestamp::isValid(2012, csvsqldb::Date::February, 29, 23, 59, 59, 135));
        MPF_TEST_ASSERTEQUAL(false, csvsqldb::Timestamp::isValid(2013, csvsqldb::Date::February, 29, 23, 59, 59, 1350));
        MPF_TEST_ASSERTEQUAL(false, csvsqldb::Timestamp::isValid(2013, static_cast<csvsqldb::Date::eMonth>(0), 25, 23, 5, 60, 0));
        MPF_TEST_ASSERTEQUAL(false, csvsqldb::Timestamp::isValid(2012, csvsqldb::Date::February, 29, 23, 59, 59, 1350));
        MPF_TEST_ASSERTEQUAL(false, csvsqldb::Timestamp::isValid(2012, csvsqldb::Date::February, 29, 23, 59, 60, 135));

        MPF_TEST_ASSERT(!csvsqldb::Timestamp(2014, csvsqldb::Date::November, 30, 23, 5, 17, 0).isLeapYear());
        MPF_TEST_ASSERT(!csvsqldb::Timestamp::isLeapYear(2009));
        MPF_TEST_ASSERT(!csvsqldb::Timestamp::isLeapYear(2013));
        MPF_TEST_ASSERT(!csvsqldb::Timestamp::isLeapYear(2014));
        MPF_TEST_ASSERT(!csvsqldb::Timestamp::isLeapYear(2021));

        MPF_TEST_ASSERT(csvsqldb::Timestamp::isLeapYear(2008));
        MPF_TEST_ASSERT(csvsqldb::Timestamp::isLeapYear(2012));
        MPF_TEST_ASSERT(csvsqldb::Timestamp::isLeapYear(2016));
        MPF_TEST_ASSERT(csvsqldb::Timestamp::isLeapYear(2020));
        MPF_TEST_ASSERT(csvsqldb::Timestamp::isLeapYear(2024));
        MPF_TEST_ASSERT(csvsqldb::Timestamp::isLeapYear(2028));
        MPF_TEST_ASSERT(csvsqldb::Timestamp::isLeapYear(2032));
    }

    void setDatePartTest()
    {
        csvsqldb::Timestamp ts1(1970, csvsqldb::Date::September, 23, 8, 9, 11, 115);

        ts1.day(17);
        ts1.month(csvsqldb::Date::August);
        ts1.year(2015);
        ts1.hour(16);
        ts1.minute(33);
        ts1.second(47);
        ts1.millisecond(899);

        MPF_TEST_ASSERTEQUAL(17, ts1.day());
        MPF_TEST_ASSERTEQUAL(8, ts1.month());
        MPF_TEST_ASSERTEQUAL(2015, ts1.year());
        MPF_TEST_ASSERTEQUAL(16, ts1.hour());
        MPF_TEST_ASSERTEQUAL(33, ts1.minute());
        MPF_TEST_ASSERTEQUAL(47, ts1.second());
        MPF_TEST_ASSERTEQUAL(899, ts1.millisecond());

        ts1.ymdhmsm(1963, csvsqldb::Date::March, 6, 10, 5, 59, 1);

        MPF_TEST_ASSERTEQUAL(6, ts1.day());
        MPF_TEST_ASSERTEQUAL(3, ts1.month());
        MPF_TEST_ASSERTEQUAL(1963, ts1.year());
        MPF_TEST_ASSERTEQUAL(10, ts1.hour());
        MPF_TEST_ASSERTEQUAL(5, ts1.minute());
        MPF_TEST_ASSERTEQUAL(59, ts1.second());
        MPF_TEST_ASSERTEQUAL(1, ts1.millisecond());
    }

    void specialGetterTest()
    {
        csvsqldb::Timestamp ts1(1970, csvsqldb::Date::September, 23, 8, 9, 11, 115);
        MPF_TEST_ASSERTEQUAL(csvsqldb::Date::Wednesday, ts1.dayOfWeek());
        MPF_TEST_ASSERTEQUAL(266, ts1.dayOfYear());
        MPF_TEST_ASSERTEQUAL(39, ts1.weekOfYear());

        csvsqldb::Timestamp ts2(1963, csvsqldb::Date::March, 6, 10, 5, 59, 1);
        MPF_TEST_ASSERTEQUAL(csvsqldb::Date::Wednesday, ts2.dayOfWeek());
        MPF_TEST_ASSERTEQUAL(65, ts2.dayOfYear());
        MPF_TEST_ASSERTEQUAL(10, ts2.weekOfYear());
    }

    void compareTest()
    {
        csvsqldb::Timestamp ts1(1970, csvsqldb::Date::September, 23, 8, 9, 11, 115);
        csvsqldb::Timestamp ts2(1963, csvsqldb::Date::March, 6, 10, 5, 59, 1);

        MPF_TEST_ASSERT(ts1 != ts2);
        MPF_TEST_ASSERT(!(ts1 == ts2));
        MPF_TEST_ASSERT(ts2 < ts1);
        MPF_TEST_ASSERT(ts1 > ts2);
        MPF_TEST_ASSERT(ts2 <= ts1);
        MPF_TEST_ASSERT(ts1 >= ts2);

        csvsqldb::Timestamp ts3(ts2);
        MPF_TEST_ASSERT(ts2 == ts3);
        MPF_TEST_ASSERT(!(ts2 != ts3));
        MPF_TEST_ASSERT(ts2 >= ts3);
        MPF_TEST_ASSERT(ts2 <= ts3);
    }

    void formatTest()
    {
        csvsqldb::Timestamp ts1(1970, csvsqldb::Date::September, 23, 8, 9, 11, 115);

        MPF_TEST_ASSERTEQUAL("1970-09-23T08:09:11", ts1.format("%F"));
        MPF_TEST_ASSERTEQUAL("1970-09-23T08:09:11.115", ts1.format("%Y-%m-%dT%H:%M:%S.%s"));

        csvsqldb::Timestamp ts2(2015, csvsqldb::Date::July, 2, 14, 20, 30, 0);

        MPF_TEST_ASSERTEQUAL("2015-07-02T14:20:30", ts2.format("%F"));

        MPF_TEST_ASSERTEQUAL("23.09.1970 266 39 3", ts1.format("%d.%m.%Y %j %U %w"));
        MPF_TEST_ASSERTEQUAL("02.07.2015 183 27 4", ts2.format("%d.%m.%Y %j %U %w"));

        csvsqldb::Timestamp ts3(0, csvsqldb::Date::January, 1, 0, 0, 0, 0);
        MPF_TEST_ASSERTEQUAL("0000-01-01T00:00:00", ts3.format("%F"));
    }

    void operatorTest()
    {
        csvsqldb::Timestamp ts1(1970, csvsqldb::Date::September, 23, 8, 9, 11, 115);
        csvsqldb::Timestamp ts2(2015, csvsqldb::Date::January, 1, 0, 0, 0);

        MPF_TEST_ASSERTEQUAL(1397145049, ts2 - ts1);

        csvsqldb::Timestamp ts3(2035, csvsqldb::Date::September, 23, 8, 9, 11, 115);
        MPF_TEST_ASSERTEQUAL(65, (ts3 - ts1) / (365 * 24 * 60 * 60));
    }

    void addDurationTest()
    {
        csvsqldb::Timestamp ts1(1970, csvsqldb::Date::September, 23, 8, 9, 11, 115);
        csvsqldb::Duration d(45, 2, 7, 3, 57, 999);
        ts1.addDuration(d);

        MPF_TEST_ASSERTEQUAL(2015, ts1.year());
        MPF_TEST_ASSERTEQUAL(csvsqldb::Date::November, ts1.month());
        MPF_TEST_ASSERTEQUAL(30, ts1.day());
        MPF_TEST_ASSERTEQUAL(12, ts1.hour());
        MPF_TEST_ASSERTEQUAL(22, ts1.minute());
        MPF_TEST_ASSERTEQUAL(50, ts1.second());
    }
};

MPF_REGISTER_TEST_START("BaseValuesTestSuite", TimestampTestCase);
MPF_REGISTER_TEST(TimestampTestCase::constructionTest);
MPF_REGISTER_TEST(TimestampTestCase::copyTest);
MPF_REGISTER_TEST(TimestampTestCase::validateTest);
MPF_REGISTER_TEST(TimestampTestCase::setDatePartTest);
MPF_REGISTER_TEST(TimestampTestCase::specialGetterTest);
MPF_REGISTER_TEST(TimestampTestCase::compareTest);
MPF_REGISTER_TEST(TimestampTestCase::formatTest);
MPF_REGISTER_TEST(TimestampTestCase::operatorTest);
MPF_REGISTER_TEST(TimestampTestCase::addDurationTest);
MPF_REGISTER_TEST_END();
