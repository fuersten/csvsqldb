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


#include <csvsqldb/base/time.h>

#include <catch2/catch.hpp>

#ifdef _WIN32
  #define timegm _mkgmtime
#endif


TEST_CASE("Time Test", "[time]")
{
  SECTION("construction")
  {
    csvsqldb::Time t1(13, 45, 24, 115);
    CHECK(13 == t1.hour());
    CHECK(45 == t1.minute());
    CHECK(24 == t1.second());
    CHECK(115 == t1.millisecond());

    csvsqldb::Time t2;
    CHECK(0 == t2.hour());
    CHECK(0 == t2.minute());
    CHECK(0 == t2.second());
    CHECK(0 == t2.millisecond());

    struct tm y2k;
    y2k.tm_hour = 16;
    y2k.tm_min = 55;
    y2k.tm_sec = 22;
    y2k.tm_year = 2000 - 1900;
    y2k.tm_mon = 0;
    y2k.tm_mday = 1;
    y2k.tm_isdst = 0;
    time_t time = timegm(&y2k);

    csvsqldb::Time t3(time);
    struct tm* ts = ::localtime(&time);
    CHECK(ts->tm_hour == t3.hour());
    CHECK(ts->tm_min == t3.minute());
    CHECK(ts->tm_sec == t3.second());

    csvsqldb::Time t4(0, 0, 0, 0);
    CHECK(0 == t4.hour());
    CHECK(0 == t4.minute());
    CHECK(0 == t4.second());
    CHECK(0 == t4.millisecond());

    csvsqldb::Time t5(23, 59, 59, 999);
    CHECK(23 == t5.hour());
    CHECK(59 == t5.minute());
    CHECK(59 == t5.second());
    CHECK(999 == t5.millisecond());
  }

  SECTION("copy")
  {
    csvsqldb::Time t1(13, 45, 24, 115);

    csvsqldb::Time t2(t1);
    CHECK(13 == t2.hour());
    CHECK(45 == t2.minute());
    CHECK(24 == t2.second());
    CHECK(115 == t2.millisecond());

    csvsqldb::Time t3(9, 5, 8);
    CHECK(9 == t3.hour());
    CHECK(5 == t3.minute());
    CHECK(8 == t3.second());
    CHECK(0 == t3.millisecond());

    t3 = t2;
    CHECK(13 == t2.hour());
    CHECK(45 == t2.minute());
    CHECK(24 == t2.second());
    CHECK(115 == t2.millisecond());
  }

  SECTION("validate")
  {
    CHECK_FALSE(csvsqldb::Time::isValid(25, 5, 17, 0));
    CHECK(csvsqldb::Time::isValid(23, 59, 59, 135));
    CHECK_FALSE(csvsqldb::Time::isValid(23, 59, 59, 1350));
    CHECK(csvsqldb::Time::isValid(0, 0, 0, 0));
    CHECK_FALSE(csvsqldb::Time::isValid(23, 60, 17, 0));
    CHECK_FALSE(csvsqldb::Time::isValid(23, 5, 60, 0));
  }

  SECTION("set time part")
  {
    csvsqldb::Time t1(13, 45, 24, 115);

    t1.hour(10);
    t1.minute(33);
    t1.second(41);
    t1.millisecond(93);

    CHECK(10 == t1.hour());
    CHECK(33 == t1.minute());
    CHECK(41 == t1.second());
    CHECK(93 == t1.millisecond());

    t1.hmsm(9, 18, 56, 115);

    CHECK(9 == t1.hour());
    CHECK(18 == t1.minute());
    CHECK(56 == t1.second());
    CHECK(115 == t1.millisecond());
  }

  SECTION("add")
  {
    csvsqldb::Time t(18, 50, 45);
    int32_t days = t.addHours(6);
    CHECK(1 == days);
    CHECK(0 == t.hour());
    CHECK(50 == t.minute());
    CHECK(45 == t.second());

    csvsqldb::Time t1(23, 50, 45);
    days = t1.addMinutes(10);
    CHECK(1 == days);
    CHECK(0 == t1.hour());
    CHECK(0 == t1.minute());
    CHECK(45 == t1.second());

    csvsqldb::Time t2(23, 59, 45);
    days = t2.addSeconds(20);
    CHECK(1 == days);
    CHECK(0 == t2.hour());
    CHECK(0 == t2.minute());
    CHECK(5 == t2.second());
  }

  SECTION("compare")
  {
    csvsqldb::Time t1(13, 45, 24);
    csvsqldb::Time t2(18, 50, 45);

    CHECK(t1 != t2);
    CHECK_FALSE(t1 == t2);
    CHECK(t1 < t2);
    CHECK(t2 > t1);
    CHECK(t1 <= t2);
    CHECK(t2 >= t1);

    csvsqldb::Time t3(t2);
    CHECK(t2 == t3);
    CHECK_FALSE(t2 != t3);
    CHECK(t2 >= t3);
    CHECK(t2 <= t3);
  }

  SECTION("format")
  {
    csvsqldb::Time t1(13, 45, 24);
    csvsqldb::Time t2(22, 50, 45, 834);

    CHECK("13:45:24" == t1.format("%H:%M:%S"));
    CHECK("13:45:24" == t1.format("%X"));
    CHECK("22:50:45.834" == t2.format("%H:%M:%S.%s"));

    CHECK("01:45:24 pm" == t1.format("%I:%M:%S %p"));
    CHECK("10:50:45 pm" == t2.format("%I:%M:%S %p"));
  }

  SECTION("time arithmetic")
  {
    csvsqldb::Time t1(13, 45, 24);
    csvsqldb::Time t2(22, 50, 45, 834);

    CHECK(32721834 == t2 - t1);
    CHECK(-32721834 == t1 - t2);
  }
}
