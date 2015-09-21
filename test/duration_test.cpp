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
#include "libcsvsqldb/base/duration.h"


class DurationTestCase
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
        csvsqldb::Duration d1(1, 2, 5);
        MPF_TEST_ASSERTEQUAL(1, d1.years());
        MPF_TEST_ASSERTEQUAL(2, d1.months());
        MPF_TEST_ASSERTEQUAL(5, d1.days());
        MPF_TEST_ASSERTEQUAL(0, d1.hours());
        MPF_TEST_ASSERTEQUAL(0, d1.minutes());
        MPF_TEST_ASSERTEQUAL(0, d1.seconds());
        MPF_TEST_ASSERTEQUAL(csvsqldb::Duration::Positive, d1.sign());

        csvsqldb::Duration d2(1, 2, 5, 17, 6, 45);
        MPF_TEST_ASSERTEQUAL(1, d2.years());
        MPF_TEST_ASSERTEQUAL(2, d2.months());
        MPF_TEST_ASSERTEQUAL(5, d2.days());
        MPF_TEST_ASSERTEQUAL(17, d2.hours());
        MPF_TEST_ASSERTEQUAL(6, d2.minutes());
        MPF_TEST_ASSERTEQUAL(45, d2.seconds());
        MPF_TEST_ASSERTEQUAL(csvsqldb::Duration::Positive, d2.sign());

        csvsqldb::Duration d3;
        MPF_TEST_ASSERTEQUAL(0, d3.years());
        MPF_TEST_ASSERTEQUAL(0, d3.months());
        MPF_TEST_ASSERTEQUAL(0, d3.days());
        MPF_TEST_ASSERTEQUAL(0, d3.hours());
        MPF_TEST_ASSERTEQUAL(0, d3.minutes());
        MPF_TEST_ASSERTEQUAL(0, d3.seconds());
        MPF_TEST_ASSERTEQUAL(csvsqldb::Duration::Positive, d3.sign());

        csvsqldb::Duration d4(1, 2, 3, csvsqldb::Duration::Negative);
        MPF_TEST_ASSERTEQUAL(1, d4.years());
        MPF_TEST_ASSERTEQUAL(2, d4.months());
        MPF_TEST_ASSERTEQUAL(3, d4.days());
        MPF_TEST_ASSERTEQUAL(0, d4.hours());
        MPF_TEST_ASSERTEQUAL(0, d4.minutes());
        MPF_TEST_ASSERTEQUAL(0, d4.seconds());
        MPF_TEST_ASSERTEQUAL(csvsqldb::Duration::Negative, d4.sign());

        MPF_TEST_ASSERTEQUAL(-1, d4.yearsWithSign());
        MPF_TEST_ASSERTEQUAL(-2, d4.monthsWithSign());
        MPF_TEST_ASSERTEQUAL(-3, d4.daysWithSign());
        MPF_TEST_ASSERTEQUAL(0, d4.hoursWithSign());
        MPF_TEST_ASSERTEQUAL(0, d4.minutesWithSign());
        MPF_TEST_ASSERTEQUAL(0, d4.secondsWithSign());
    }

    void copyTest()
    {
        csvsqldb::Duration d1(3, 1, 90, 17, 6, 45, csvsqldb::Duration::Negative);
        csvsqldb::Duration d2(d1);

        MPF_TEST_ASSERTEQUAL(3, d2.years());
        MPF_TEST_ASSERTEQUAL(1, d2.months());
        MPF_TEST_ASSERTEQUAL(90, d2.days());
        MPF_TEST_ASSERTEQUAL(17, d2.hours());
        MPF_TEST_ASSERTEQUAL(6, d2.minutes());
        MPF_TEST_ASSERTEQUAL(45, d2.seconds());
        MPF_TEST_ASSERTEQUAL(csvsqldb::Duration::Negative, d2.sign());

        csvsqldb::Duration d3 = d2;

        MPF_TEST_ASSERTEQUAL(3, d3.years());
        MPF_TEST_ASSERTEQUAL(1, d3.months());
        MPF_TEST_ASSERTEQUAL(90, d3.days());
        MPF_TEST_ASSERTEQUAL(17, d3.hours());
        MPF_TEST_ASSERTEQUAL(6, d3.minutes());
        MPF_TEST_ASSERTEQUAL(45, d3.seconds());
        MPF_TEST_ASSERTEQUAL(csvsqldb::Duration::Negative, d3.sign());
    }

    void setDurationPartTest()
    {
        csvsqldb::Duration d1(3, 1, 90, 17, 6, 45);

        d1.ymdhmss(1, 2, 3, 4, 5, 6, csvsqldb::Duration::Negative);
        MPF_TEST_ASSERTEQUAL(1, d1.years());
        MPF_TEST_ASSERTEQUAL(2, d1.months());
        MPF_TEST_ASSERTEQUAL(3, d1.days());
        MPF_TEST_ASSERTEQUAL(4, d1.hours());
        MPF_TEST_ASSERTEQUAL(5, d1.minutes());
        MPF_TEST_ASSERTEQUAL(6, d1.seconds());
        MPF_TEST_ASSERTEQUAL(csvsqldb::Duration::Negative, d1.sign());

        d1.years(6);
        d1.months(5);
        d1.days(4);
        d1.hours(3);
        d1.minutes(2);
        d1.seconds(1);
        d1.sign(csvsqldb::Duration::Positive);
        MPF_TEST_ASSERTEQUAL(6, d1.years());
        MPF_TEST_ASSERTEQUAL(5, d1.months());
        MPF_TEST_ASSERTEQUAL(4, d1.days());
        MPF_TEST_ASSERTEQUAL(3, d1.hours());
        MPF_TEST_ASSERTEQUAL(2, d1.minutes());
        MPF_TEST_ASSERTEQUAL(1, d1.seconds());
        MPF_TEST_ASSERTEQUAL(csvsqldb::Duration::Positive, d1.sign());
    }

    void addTest()
    {
        csvsqldb::Duration d1(3, 1, 90, 17, 6, 45);
        d1.add(csvsqldb::Duration(5, 2, 1, 3, 7, 5));
        MPF_TEST_ASSERTEQUAL(8, d1.years());
        MPF_TEST_ASSERTEQUAL(3, d1.months());
        MPF_TEST_ASSERTEQUAL(91, d1.days());
        MPF_TEST_ASSERTEQUAL(20, d1.hours());
        MPF_TEST_ASSERTEQUAL(13, d1.minutes());
        MPF_TEST_ASSERTEQUAL(50, d1.seconds());

        csvsqldb::Duration d2(1, 2, 5);
        d2.add(d1);
        MPF_TEST_ASSERTEQUAL(9, d2.years());
        MPF_TEST_ASSERTEQUAL(5, d2.months());
        MPF_TEST_ASSERTEQUAL(96, d2.days());
        MPF_TEST_ASSERTEQUAL(20, d2.hours());
        MPF_TEST_ASSERTEQUAL(13, d2.minutes());
        MPF_TEST_ASSERTEQUAL(50, d2.seconds());
        MPF_TEST_ASSERTEQUAL(csvsqldb::Duration::Positive, d2.sign());

        csvsqldb::Duration d3(1, 2, 3, csvsqldb::Duration::Negative);
        MPF_TEST_EXPECTS(d3.add(d2), csvsqldb::DurationException);

        d3.add(csvsqldb::Duration(1, 2, 3, csvsqldb::Duration::Negative));
        MPF_TEST_ASSERTEQUAL(2, d3.years());
        MPF_TEST_ASSERTEQUAL(4, d3.months());
        MPF_TEST_ASSERTEQUAL(6, d3.days());
        MPF_TEST_ASSERTEQUAL(0, d3.hours());
        MPF_TEST_ASSERTEQUAL(0, d3.minutes());
        MPF_TEST_ASSERTEQUAL(0, d3.seconds());
        MPF_TEST_ASSERTEQUAL(csvsqldb::Duration::Negative, d3.sign());
    }

    void compareTest()
    {
        csvsqldb::Duration d1(3, 1, 90, 17, 6, 45);
        csvsqldb::Duration d2(1, 2, 5);
        csvsqldb::Duration d3(1, 2, 5, 17, 6, 45);

        MPF_TEST_ASSERT(d1 != d2);
        MPF_TEST_ASSERT(!(d1 == d2));
        MPF_TEST_ASSERT(d1 > d2);
        MPF_TEST_ASSERT(d2 < d1);
        MPF_TEST_ASSERT(d3 > d2);
        MPF_TEST_ASSERT(d1 > d3);
        MPF_TEST_ASSERT(d1 == d1);
    }

    void formatTest()
    {
        csvsqldb::Duration d1(3, 1, 90, 17, 6, 45);
        MPF_TEST_ASSERTEQUAL("P3Y1M90DT17H6M45S", d1.format());
        csvsqldb::Duration d2(1, 2, 5);
        MPF_TEST_ASSERTEQUAL("P1Y2M5D", d2.format());
        csvsqldb::Duration d3(1, 2, 5, 17, 6, 45);
        MPF_TEST_ASSERTEQUAL("P1Y2M5DT17H6M45S", d3.format());
    }
};

MPF_REGISTER_TEST_START("BaseValuesTestSuite", DurationTestCase);
MPF_REGISTER_TEST(DurationTestCase::constructionTest);
MPF_REGISTER_TEST(DurationTestCase::copyTest);
MPF_REGISTER_TEST(DurationTestCase::setDurationPartTest);
MPF_REGISTER_TEST(DurationTestCase::addTest);
MPF_REGISTER_TEST(DurationTestCase::compareTest);
MPF_REGISTER_TEST(DurationTestCase::formatTest);
MPF_REGISTER_TEST_END();
