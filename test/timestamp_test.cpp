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


#include "csvsqldb/base/timestamp.h"

#include <catch2/catch.hpp>


TEST_CASE("Timestamp Test", "[timestamp]")
{
  SECTION("construction")
  {
    csvsqldb::Timestamp ts1(1970, csvsqldb::Date::September, 23, 8, 9, 11, 115);
    CHECK(23 == ts1.day());
    CHECK(9 == ts1.month());
    CHECK(1970 == ts1.year());
    CHECK(8 == ts1.hour());
    CHECK(9 == ts1.minute());
    CHECK(11 == ts1.second());
    CHECK(115 == ts1.millisecond());

    csvsqldb::Timestamp ts2;
    CHECK(ts2.isInfinite());

    csvsqldb::Timestamp ts3(0, csvsqldb::Date::January, 1, 0, 0, 0, 0);
    CHECK(1 == ts3.day());
    CHECK(1 == ts3.month());
    CHECK(0 == ts3.year());
    CHECK(0 == ts3.hour());
    CHECK(0 == ts3.minute());
    CHECK(0 == ts3.second());
    CHECK(0 == ts3.millisecond());

    csvsqldb::Timestamp ts4(9999, csvsqldb::Date::December, 31, 23, 59, 59, 999);
    CHECK(31 == ts4.day());
    CHECK(12 == ts4.month());
    CHECK(9999 == ts4.year());
    CHECK(23 == ts4.hour());
    CHECK(59 == ts4.minute());
    CHECK(59 == ts4.second());
    CHECK(999 == ts4.millisecond());
  }

  SECTION("copy")
  {
    csvsqldb::Timestamp ts1(1970, csvsqldb::Date::September, 23, 8, 9, 11, 115);
    csvsqldb::Timestamp ts2(ts1);

    CHECK(23 == ts2.day());
    CHECK(9 == ts2.month());
    CHECK(1970 == ts2.year());
    CHECK(8 == ts2.hour());
    CHECK(9 == ts2.minute());
    CHECK(11 == ts2.second());
    CHECK(115 == ts2.millisecond());

    csvsqldb::Timestamp ts3(2014, csvsqldb::Date::December, 30, 9, 5, 8);
    CHECK(30 == ts3.day());
    CHECK(12 == ts3.month());
    CHECK(2014 == ts3.year());
    CHECK(9 == ts3.hour());
    CHECK(5 == ts3.minute());
    CHECK(8 == ts3.second());
    CHECK(0 == ts3.millisecond());

    ts3 = ts2;
    CHECK(23 == ts3.day());
    CHECK(9 == ts3.month());
    CHECK(1970 == ts3.year());
    CHECK(8 == ts3.hour());
    CHECK(9 == ts3.minute());
    CHECK(11 == ts3.second());
    CHECK(115 == ts3.millisecond());
  }

  SECTION("validate")
  {
    CHECK(csvsqldb::Timestamp::isValid(9999, csvsqldb::Date::December, 31, 23, 59, 59, 999));
    CHECK_FALSE(csvsqldb::Timestamp::isValid(10000, csvsqldb::Date::January, 1, 0, 0, 0, 0));
    CHECK_FALSE(csvsqldb::Timestamp::isValid(2014, csvsqldb::Date::November, 31, 25, 5, 17, 0));
    CHECK_FALSE(csvsqldb::Timestamp::isValid(2014, csvsqldb::Date::November, 30, 24, 5, 17, 0));
    CHECK(csvsqldb::Timestamp::isValid(2012, csvsqldb::Date::February, 29, 23, 59, 59, 135));
    CHECK_FALSE(csvsqldb::Timestamp::isValid(2013, csvsqldb::Date::February, 29, 23, 59, 59, 1350));
    CHECK_FALSE(csvsqldb::Timestamp::isValid(2013, static_cast<csvsqldb::Date::eMonth>(0), 25, 23, 5, 60, 0));
    CHECK_FALSE(csvsqldb::Timestamp::isValid(2012, csvsqldb::Date::February, 29, 23, 59, 59, 1350));
    CHECK_FALSE(csvsqldb::Timestamp::isValid(2012, csvsqldb::Date::February, 29, 23, 59, 60, 135));

    CHECK_FALSE(csvsqldb::Timestamp(2014, csvsqldb::Date::November, 30, 23, 5, 17, 0).isLeapYear());
    CHECK_FALSE(csvsqldb::Timestamp::isLeapYear(2009));
    CHECK_FALSE(csvsqldb::Timestamp::isLeapYear(2013));
    CHECK_FALSE(csvsqldb::Timestamp::isLeapYear(2014));
    CHECK_FALSE(csvsqldb::Timestamp::isLeapYear(2021));

    CHECK(csvsqldb::Timestamp::isLeapYear(2008));
    CHECK(csvsqldb::Timestamp::isLeapYear(2012));
    CHECK(csvsqldb::Timestamp::isLeapYear(2016));
    CHECK(csvsqldb::Timestamp::isLeapYear(2020));
    CHECK(csvsqldb::Timestamp::isLeapYear(2024));
    CHECK(csvsqldb::Timestamp::isLeapYear(2028));
    CHECK(csvsqldb::Timestamp::isLeapYear(2032));
  }

  SECTION("set date part")
  {
    csvsqldb::Timestamp ts1(1970, csvsqldb::Date::September, 23, 8, 9, 11, 115);

    ts1.day(17);
    ts1.month(csvsqldb::Date::August);
    ts1.year(2015);
    ts1.hour(16);
    ts1.minute(33);
    ts1.second(47);
    ts1.millisecond(899);

    CHECK(17 == ts1.day());
    CHECK(8 == ts1.month());
    CHECK(2015 == ts1.year());
    CHECK(16 == ts1.hour());
    CHECK(33 == ts1.minute());
    CHECK(47 == ts1.second());
    CHECK(899 == ts1.millisecond());

    ts1.ymdhmsm(1963, csvsqldb::Date::March, 6, 10, 5, 59, 1);

    CHECK(6 == ts1.day());
    CHECK(3 == ts1.month());
    CHECK(1963 == ts1.year());
    CHECK(10 == ts1.hour());
    CHECK(5 == ts1.minute());
    CHECK(59 == ts1.second());
    CHECK(1 == ts1.millisecond());
  }

  SECTION("special getter")
  {
    csvsqldb::Timestamp ts1(1970, csvsqldb::Date::September, 23, 8, 9, 11, 115);
    CHECK(csvsqldb::Date::Wednesday == ts1.dayOfWeek());
    CHECK(266 == ts1.dayOfYear());
    CHECK(39 == ts1.weekOfYear());

    csvsqldb::Timestamp ts2(1963, csvsqldb::Date::March, 6, 10, 5, 59, 1);
    CHECK(csvsqldb::Date::Wednesday == ts2.dayOfWeek());
    CHECK(65 == ts2.dayOfYear());
    CHECK(10 == ts2.weekOfYear());
  }

  SECTION("compare")
  {
    csvsqldb::Timestamp ts1(1970, csvsqldb::Date::September, 23, 8, 9, 11, 115);
    csvsqldb::Timestamp ts2(1963, csvsqldb::Date::March, 6, 10, 5, 59, 1);

    CHECK(ts1 != ts2);
    CHECK_FALSE(ts1 == ts2);
    CHECK(ts2 < ts1);
    CHECK(ts1 > ts2);
    CHECK(ts2 <= ts1);
    CHECK(ts1 >= ts2);

    csvsqldb::Timestamp ts3(ts2);
    CHECK(ts2 == ts3);
    CHECK_FALSE(ts2 != ts3);
    CHECK(ts2 >= ts3);
    CHECK(ts2 <= ts3);
  }

  SECTION("format")
  {
    csvsqldb::Timestamp ts1(1970, csvsqldb::Date::September, 23, 8, 9, 11, 115);

    CHECK("1970-09-23T08:09:11" == ts1.format("%F"));
    CHECK("1970-09-23T08:09:11.115" == ts1.format("%Y-%m-%dT%H:%M:%S.%s"));

    csvsqldb::Timestamp ts2(2015, csvsqldb::Date::July, 2, 14, 20, 30, 0);

    CHECK("2015-07-02T14:20:30" == ts2.format("%F"));

    CHECK("23.09.1970 266 39 3" == ts1.format("%d.%m.%Y %j %U %w"));
    CHECK("02.07.2015 183 27 4" == ts2.format("%d.%m.%Y %j %U %w"));

    csvsqldb::Timestamp ts3(0, csvsqldb::Date::January, 1, 0, 0, 0, 0);
    CHECK("0000-01-01T00:00:00" == ts3.format("%F"));
  }

  SECTION("operator")
  {
    csvsqldb::Timestamp ts1(1970, csvsqldb::Date::September, 23, 8, 9, 11, 115);
    csvsqldb::Timestamp ts2(2015, csvsqldb::Date::January, 1, 0, 0, 0);

    CHECK(1397145049 == ts2 - ts1);

    csvsqldb::Timestamp ts3(2035, csvsqldb::Date::September, 23, 8, 9, 11, 115);
    CHECK(65 == (ts3 - ts1) / (365 * 24 * 60 * 60));
  }

  SECTION("add duration")
  {
    csvsqldb::Timestamp ts1(1970, csvsqldb::Date::September, 23, 8, 9, 11, 115);
    csvsqldb::Duration d(45, 2, 7, 3, 57, 999);
    ts1.addDuration(d);

    CHECK(2015 == ts1.year());
    CHECK(csvsqldb::Date::November == ts1.month());
    CHECK(30 == ts1.day());
    CHECK(12 == ts1.hour());
    CHECK(22 == ts1.minute());
    CHECK(50 == ts1.second());
  }
}
