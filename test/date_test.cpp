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


#include <csvsqldb/base/date.h>

#include <catch2/catch.hpp>

#ifdef _WIN32
  #define timegm _mkgmtime
#endif


TEST_CASE("Date Test", "[date]")
{
  SECTION("construction")
  {
    csvsqldb::Date d1(1970, csvsqldb::Date::September, 23);
    CHECK(23 == d1.day());
    CHECK(9 == d1.month());
    CHECK(1970 == d1.year());

    csvsqldb::Date d2;
    CHECK(d2.isInfinite());

    struct tm y2k;
    y2k.tm_hour = 0;
    y2k.tm_min = 0;
    y2k.tm_sec = 0;
    y2k.tm_year = 2000 - 1900;
    y2k.tm_mon = 0;
    y2k.tm_mday = 1;
    y2k.tm_isdst = 0;
    time_t time = timegm(&y2k);

    csvsqldb::Date d3(time);
    CHECK(1 == d3.day());
    CHECK(1 == d3.month());
    CHECK(2000 == d3.year());

    csvsqldb::Date d4(0, csvsqldb::Date::January, 1);
    CHECK(1 == d4.day());
    CHECK(1 == d4.month());
    CHECK(0 == d4.year());

    csvsqldb::Date d5(9999, csvsqldb::Date::December, 31);
    CHECK(31 == d5.day());
    CHECK(12 == d5.month());
    CHECK(9999 == d5.year());
  }

  SECTION("copy")
  {
    csvsqldb::Date d1(1970, csvsqldb::Date::September, 23);

    csvsqldb::Date d2(d1);
    CHECK(23 == d2.day());
    CHECK(9 == d2.month());
    CHECK(1970 == d2.year());

    csvsqldb::Date d3(2014, csvsqldb::Date::December, 30);
    CHECK(30 == d3.day());
    CHECK(12 == d3.month());
    CHECK(2014 == d3.year());

    d3 = d2;
    CHECK(23 == d3.day());
    CHECK(9 == d3.month());
    CHECK(1970 == d3.year());
  }

  SECTION("validate")
  {
    CHECK(csvsqldb::Date::isValid(9999, csvsqldb::Date::December, 31));
    CHECK_FALSE(csvsqldb::Date::isValid(10000, csvsqldb::Date::January, 1));
    CHECK_FALSE(csvsqldb::Date::isValid(2014, csvsqldb::Date::November, 31));
    CHECK(csvsqldb::Date::isValid(2012, csvsqldb::Date::February, 29));
    CHECK_FALSE(csvsqldb::Date::isValid(2013, csvsqldb::Date::February, 29));
    CHECK_FALSE(csvsqldb::Date::isValid(2013, static_cast<csvsqldb::Date::eMonth>(0), 25));

    CHECK_FALSE(csvsqldb::Date(2014, csvsqldb::Date::November, 30).isLeapYear());
    CHECK_FALSE(csvsqldb::Date::isLeapYear(2009));
    CHECK_FALSE(csvsqldb::Date::isLeapYear(2013));
    CHECK_FALSE(csvsqldb::Date::isLeapYear(2014));
    CHECK_FALSE(csvsqldb::Date::isLeapYear(2021));

    CHECK(csvsqldb::Date::isLeapYear(2008));
    CHECK(csvsqldb::Date::isLeapYear(2012));
    CHECK(csvsqldb::Date::isLeapYear(2016));
    CHECK(csvsqldb::Date::isLeapYear(2020));
    CHECK(csvsqldb::Date::isLeapYear(2024));
    CHECK(csvsqldb::Date::isLeapYear(2028));
    CHECK(csvsqldb::Date::isLeapYear(2032));
  }

  SECTION("set date part")
  {
    csvsqldb::Date d1(1970, csvsqldb::Date::September, 23);

    d1.day(17);
    d1.month(csvsqldb::Date::May);
    d1.year(1969);

    CHECK(17 == d1.day());
    CHECK(5 == d1.month());
    CHECK(1969 == d1.year());

    d1.ymd(1946, csvsqldb::Date::May, 4);

    CHECK(4 == d1.day());
    CHECK(5 == d1.month());
    CHECK(1946 == d1.year());
  }

  SECTION("special getter")
  {
    csvsqldb::Date d1(1970, csvsqldb::Date::September, 23);
    CHECK(csvsqldb::Date::Wednesday == d1.dayOfWeek());
    CHECK(266 == d1.dayOfYear());
    CHECK(39 == d1.weekOfYear());

    csvsqldb::Date d2(2014, csvsqldb::Date::December, 30);
    CHECK(csvsqldb::Date::Tuesday == d2.dayOfWeek());
    CHECK(364 == d2.dayOfYear());
    CHECK(1 == d2.weekOfYear());
  }

  SECTION("add days")
  {
    csvsqldb::Date d1(1970, csvsqldb::Date::September, 23);

    d1.addDays(7);
    CHECK(30 == d1.day());
    CHECK(9 == d1.month());
    CHECK(1970 == d1.year());

    d1.addDays(-7);
    CHECK(23 == d1.day());
    CHECK(9 == d1.month());
    CHECK(1970 == d1.year());

    csvsqldb::Date d2(2014, csvsqldb::Date::December, 30);

    d2.addDays(10);
    CHECK(9 == d2.day());
    CHECK(1 == d2.month());
    CHECK(2015 == d2.year());
  }

  SECTION("compare")
  {
    csvsqldb::Date d1(1970, csvsqldb::Date::September, 23);
    csvsqldb::Date d2(2014, csvsqldb::Date::December, 30);

    CHECK(d1 != d2);
    CHECK_FALSE(d1 == d2);
    CHECK(d1 < d2);
    CHECK(d2 > d1);
    CHECK(d1 <= d2);
    CHECK(d2 >= d1);

    csvsqldb::Date d3(d2);
    CHECK(d2 == d3);
    CHECK_FALSE(d2 != d3);
    CHECK(d2 >= d3);
    CHECK(d2 <= d3);
  }

  SECTION("format")
  {
    csvsqldb::Date d1(1970, csvsqldb::Date::September, 23);
    csvsqldb::Date d2(2015, csvsqldb::Date::January, 01);

    CHECK("1970-09-23" == d1.format("%F"));
    CHECK("2015-01-01" == d2.format("%F"));

    CHECK("23.09.1970 266 39 3" == d1.format("%d.%m.%Y %j %U %w"));
    CHECK("01.01.2015 001 01 4" == d2.format("%d.%m.%Y %j %U %w"));

    csvsqldb::Date d3(1, csvsqldb::Date::January, 01);
    CHECK("0001-01-01" == d3.format("%F"));
  }

  SECTION("operator")
  {
    csvsqldb::Date d1(1970, csvsqldb::Date::September, 23);
    csvsqldb::Date d2(2015, csvsqldb::Date::January, 01);

    CHECK(16171 == d2 - d1);

    csvsqldb::Date d3(0, csvsqldb::Date::January, 01);
    csvsqldb::Date d4(2015, csvsqldb::Date::January, 01);

    CHECK(2016 == (d4 - d3) / 365);
  }

  SECTION("add duration")
  {
    csvsqldb::Date dt(1970, csvsqldb::Date::September, 23);
    csvsqldb::Duration d(45, 2, 7);
    dt.addDuration(d);

    CHECK(2015 == dt.year());
    CHECK(csvsqldb::Date::November == dt.month());
    CHECK(30 == dt.day());

    csvsqldb::Date dt1(1970, csvsqldb::Date::September, 23);
    csvsqldb::Duration d1(1, 5, 7);
    dt1.addDuration(d1);

    CHECK(1972 == dt1.year());
    CHECK(csvsqldb::Date::March == dt1.month());
    CHECK(1 == dt1.day());
  }
}
