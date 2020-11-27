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
#include <csvsqldb/base/string_helper.h>
#include <csvsqldb/base/time_helper.h>

#include <catch2/catch.hpp>


TEST_CASE("Timehelper Test", "[utils]")
{
  SECTION("time conversion")
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

    CHECK("1970-09-23T07:00:00" == csvsqldb::timestampToGMTString(tp));
    CHECK(buffer == csvsqldb::timestampToLocalString(tp));

    csvsqldb::Timepoint ltp = csvsqldb::stringToTimestamp("1970-09-23T07:00:00");
    CHECK(tp == ltp);
  }

  SECTION("time from string")
  {
    csvsqldb::Time t = csvsqldb::timeFromString("11:46:53");
    CHECK(11 == t.hour());
    CHECK(46 == t.minute());
    CHECK(53 == t.second());
  }

  SECTION("time from string error")
  {
    CHECK_THROWS_WITH(csvsqldb::timeFromString("114653"), "not a time");
  }

  SECTION("timestamp from string")
  {
    csvsqldb::Timestamp t = csvsqldb::timestampFromString("1970-09-23T07:08:05");
    CHECK(1970 == t.year());
    CHECK(csvsqldb::Date::September == t.month());
    CHECK(23 == t.day());
    CHECK(7 == t.hour());
    CHECK(8 == t.minute());
    CHECK(5 == t.second());
  }

  SECTION("timestamp from string error")
  {
    CHECK_THROWS_WITH(csvsqldb::timestampFromString("114653"), "not a timestamp");
  }

  SECTION("date from string")
  {
    csvsqldb::Date d = csvsqldb::dateFromString("2014-12-30");
    CHECK(30 == d.day());
    CHECK(12 == d.month());
    CHECK(2014 == d.year());
  }

  SECTION("date from string error")
  {
    CHECK_THROWS_WITH(csvsqldb::dateFromString("201420-12-30"), "not a date");
 }
}
