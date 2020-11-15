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


#include "libcsvsqldb/base/date.h"

#include "test.h"


class DateTestCase
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
    csvsqldb::Date d1(1970, csvsqldb::Date::September, 23);
    MPF_TEST_ASSERTEQUAL(23, d1.day());
    MPF_TEST_ASSERTEQUAL(9, d1.month());
    MPF_TEST_ASSERTEQUAL(1970, d1.year());

    csvsqldb::Date d2;
    MPF_TEST_ASSERT(d2.isInfinite());

    struct tm y2k;
    y2k.tm_hour = 0;
    y2k.tm_min = 0;
    y2k.tm_sec = 0;
    y2k.tm_year = 2000 - 1900;
    y2k.tm_mon = 0;
    y2k.tm_mday = 1;
    y2k.tm_isdst = 0;
    char utc[] = "UTC";
    y2k.tm_zone = &utc[0];
    time_t time = timegm(&y2k);

    csvsqldb::Date d3(time);
    MPF_TEST_ASSERTEQUAL(1, d3.day());
    MPF_TEST_ASSERTEQUAL(1, d3.month());
    MPF_TEST_ASSERTEQUAL(2000, d3.year());

    csvsqldb::Date d4(0, csvsqldb::Date::January, 1);
    MPF_TEST_ASSERTEQUAL(1, d4.day());
    MPF_TEST_ASSERTEQUAL(1, d4.month());
    MPF_TEST_ASSERTEQUAL(0, d4.year());

    csvsqldb::Date d5(9999, csvsqldb::Date::December, 31);
    MPF_TEST_ASSERTEQUAL(31, d5.day());
    MPF_TEST_ASSERTEQUAL(12, d5.month());
    MPF_TEST_ASSERTEQUAL(9999, d5.year());
  }

  void copyTest()
  {
    csvsqldb::Date d1(1970, csvsqldb::Date::September, 23);

    csvsqldb::Date d2(d1);
    MPF_TEST_ASSERTEQUAL(23, d2.day());
    MPF_TEST_ASSERTEQUAL(9, d2.month());
    MPF_TEST_ASSERTEQUAL(1970, d2.year());

    csvsqldb::Date d3(2014, csvsqldb::Date::December, 30);
    MPF_TEST_ASSERTEQUAL(30, d3.day());
    MPF_TEST_ASSERTEQUAL(12, d3.month());
    MPF_TEST_ASSERTEQUAL(2014, d3.year());

    d3 = d2;
    MPF_TEST_ASSERTEQUAL(23, d3.day());
    MPF_TEST_ASSERTEQUAL(9, d3.month());
    MPF_TEST_ASSERTEQUAL(1970, d3.year());
  }

  void validateTest()
  {
    MPF_TEST_ASSERTEQUAL(true, csvsqldb::Date::isValid(9999, csvsqldb::Date::December, 31));
    MPF_TEST_ASSERTEQUAL(false, csvsqldb::Date::isValid(10000, csvsqldb::Date::January, 1));
    MPF_TEST_ASSERTEQUAL(false, csvsqldb::Date::isValid(2014, csvsqldb::Date::November, 31));
    MPF_TEST_ASSERTEQUAL(true, csvsqldb::Date::isValid(2012, csvsqldb::Date::February, 29));
    MPF_TEST_ASSERTEQUAL(false, csvsqldb::Date::isValid(2013, csvsqldb::Date::February, 29));
    MPF_TEST_ASSERTEQUAL(false, csvsqldb::Date::isValid(2013, static_cast<csvsqldb::Date::eMonth>(0), 25));

    MPF_TEST_ASSERT(!csvsqldb::Date(2014, csvsqldb::Date::November, 30).isLeapYear());
    MPF_TEST_ASSERT(!csvsqldb::Date::isLeapYear(2009));
    MPF_TEST_ASSERT(!csvsqldb::Date::isLeapYear(2013));
    MPF_TEST_ASSERT(!csvsqldb::Date::isLeapYear(2014));
    MPF_TEST_ASSERT(!csvsqldb::Date::isLeapYear(2021));

    MPF_TEST_ASSERT(csvsqldb::Date::isLeapYear(2008));
    MPF_TEST_ASSERT(csvsqldb::Date::isLeapYear(2012));
    MPF_TEST_ASSERT(csvsqldb::Date::isLeapYear(2016));
    MPF_TEST_ASSERT(csvsqldb::Date::isLeapYear(2020));
    MPF_TEST_ASSERT(csvsqldb::Date::isLeapYear(2024));
    MPF_TEST_ASSERT(csvsqldb::Date::isLeapYear(2028));
    MPF_TEST_ASSERT(csvsqldb::Date::isLeapYear(2032));
  }

  void setDatePartTest()
  {
    csvsqldb::Date d1(1970, csvsqldb::Date::September, 23);

    d1.day(17);
    d1.month(csvsqldb::Date::May);
    d1.year(1969);

    MPF_TEST_ASSERTEQUAL(17, d1.day());
    MPF_TEST_ASSERTEQUAL(5, d1.month());
    MPF_TEST_ASSERTEQUAL(1969, d1.year());

    d1.ymd(1946, csvsqldb::Date::May, 4);

    MPF_TEST_ASSERTEQUAL(4, d1.day());
    MPF_TEST_ASSERTEQUAL(5, d1.month());
    MPF_TEST_ASSERTEQUAL(1946, d1.year());
  }

  void specialGetterTest()
  {
    csvsqldb::Date d1(1970, csvsqldb::Date::September, 23);
    MPF_TEST_ASSERTEQUAL(csvsqldb::Date::Wednesday, d1.dayOfWeek());
    MPF_TEST_ASSERTEQUAL(266, d1.dayOfYear());
    MPF_TEST_ASSERTEQUAL(39, d1.weekOfYear());

    csvsqldb::Date d2(2014, csvsqldb::Date::December, 30);
    MPF_TEST_ASSERTEQUAL(csvsqldb::Date::Tuesday, d2.dayOfWeek());
    MPF_TEST_ASSERTEQUAL(364, d2.dayOfYear());
    MPF_TEST_ASSERTEQUAL(1, d2.weekOfYear());
  }

  void addDaysTest()
  {
    csvsqldb::Date d1(1970, csvsqldb::Date::September, 23);

    d1.addDays(7);
    MPF_TEST_ASSERTEQUAL(30, d1.day());
    MPF_TEST_ASSERTEQUAL(9, d1.month());
    MPF_TEST_ASSERTEQUAL(1970, d1.year());

    d1.addDays(-7);
    MPF_TEST_ASSERTEQUAL(23, d1.day());
    MPF_TEST_ASSERTEQUAL(9, d1.month());
    MPF_TEST_ASSERTEQUAL(1970, d1.year());

    csvsqldb::Date d2(2014, csvsqldb::Date::December, 30);

    d2.addDays(10);
    MPF_TEST_ASSERTEQUAL(9, d2.day());
    MPF_TEST_ASSERTEQUAL(1, d2.month());
    MPF_TEST_ASSERTEQUAL(2015, d2.year());
  }

  void compareTest()
  {
    csvsqldb::Date d1(1970, csvsqldb::Date::September, 23);
    csvsqldb::Date d2(2014, csvsqldb::Date::December, 30);

    MPF_TEST_ASSERT(d1 != d2);
    MPF_TEST_ASSERT(!(d1 == d2));
    MPF_TEST_ASSERT(d1 < d2);
    MPF_TEST_ASSERT(d2 > d1);
    MPF_TEST_ASSERT(d1 <= d2);
    MPF_TEST_ASSERT(d2 >= d1);

    csvsqldb::Date d3(d2);
    MPF_TEST_ASSERT(d2 == d3);
    MPF_TEST_ASSERT(!(d2 != d3));
    MPF_TEST_ASSERT(d2 >= d3);
    MPF_TEST_ASSERT(d2 <= d3);
  }

  void formatTest()
  {
    csvsqldb::Date d1(1970, csvsqldb::Date::September, 23);
    csvsqldb::Date d2(2015, csvsqldb::Date::January, 01);

    MPF_TEST_ASSERTEQUAL("1970-09-23", d1.format("%F"));
    MPF_TEST_ASSERTEQUAL("2015-01-01", d2.format("%F"));

    MPF_TEST_ASSERTEQUAL("23.09.1970 266 39 3", d1.format("%d.%m.%Y %j %U %w"));
    MPF_TEST_ASSERTEQUAL("01.01.2015 001 01 4", d2.format("%d.%m.%Y %j %U %w"));

    csvsqldb::Date d3(1, csvsqldb::Date::January, 01);
    MPF_TEST_ASSERTEQUAL("0001-01-01", d3.format("%F"));
  }

  void operatorTest()
  {
    csvsqldb::Date d1(1970, csvsqldb::Date::September, 23);
    csvsqldb::Date d2(2015, csvsqldb::Date::January, 01);

    MPF_TEST_ASSERTEQUAL(16171, d2 - d1);

    csvsqldb::Date d3(0, csvsqldb::Date::January, 01);
    csvsqldb::Date d4(2015, csvsqldb::Date::January, 01);

    MPF_TEST_ASSERTEQUAL(2016, (d4 - d3) / 365);
  }

  void addDurationTest()
  {
    csvsqldb::Date dt(1970, csvsqldb::Date::September, 23);
    csvsqldb::Duration d(45, 2, 7);
    dt.addDuration(d);

    MPF_TEST_ASSERTEQUAL(2015, dt.year());
    MPF_TEST_ASSERTEQUAL(csvsqldb::Date::November, dt.month());
    MPF_TEST_ASSERTEQUAL(30, dt.day());

    csvsqldb::Date dt1(1970, csvsqldb::Date::September, 23);
    csvsqldb::Duration d1(1, 5, 7);
    dt1.addDuration(d1);

    MPF_TEST_ASSERTEQUAL(1972, dt1.year());
    MPF_TEST_ASSERTEQUAL(csvsqldb::Date::March, dt1.month());
    MPF_TEST_ASSERTEQUAL(1, dt1.day());
  }
};

MPF_REGISTER_TEST_START("BaseValuesTestSuite", DateTestCase);
MPF_REGISTER_TEST(DateTestCase::constructionTest);
MPF_REGISTER_TEST(DateTestCase::copyTest);
MPF_REGISTER_TEST(DateTestCase::validateTest);
MPF_REGISTER_TEST(DateTestCase::setDatePartTest);
MPF_REGISTER_TEST(DateTestCase::specialGetterTest);
MPF_REGISTER_TEST(DateTestCase::addDaysTest);
MPF_REGISTER_TEST(DateTestCase::compareTest);
MPF_REGISTER_TEST(DateTestCase::formatTest);
MPF_REGISTER_TEST(DateTestCase::operatorTest);
MPF_REGISTER_TEST(DateTestCase::addDurationTest);
MPF_REGISTER_TEST_END();
