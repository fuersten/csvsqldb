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


#include <csvsqldb/base/duration.h>

#include <catch2/catch.hpp>


TEST_CASE("Duration Test", "[duration]")
{
  SECTION("construction")
  {
    csvsqldb::Duration d1(1, 2, 5);
    CHECK(1 == d1.years());
    CHECK(2 == d1.months());
    CHECK(5 == d1.days());
    CHECK(0 == d1.hours());
    CHECK(0 == d1.minutes());
    CHECK(0 == d1.seconds());
    CHECK(csvsqldb::Duration::Positive == d1.sign());

    csvsqldb::Duration d2(1, 2, 5, 17, 6, 45);
    CHECK(1 == d2.years());
    CHECK(2 == d2.months());
    CHECK(5 == d2.days());
    CHECK(17 == d2.hours());
    CHECK(6 == d2.minutes());
    CHECK(45 == d2.seconds());
    CHECK(csvsqldb::Duration::Positive == d2.sign());

    csvsqldb::Duration d3;
    CHECK(0 == d3.years());
    CHECK(0 == d3.months());
    CHECK(0 == d3.days());
    CHECK(0 == d3.hours());
    CHECK(0 == d3.minutes());
    CHECK(0 == d3.seconds());
    CHECK(csvsqldb::Duration::Positive == d3.sign());

    csvsqldb::Duration d4(1, 2, 3, csvsqldb::Duration::Negative);
    CHECK(1 == d4.years());
    CHECK(2 == d4.months());
    CHECK(3 == d4.days());
    CHECK(0 == d4.hours());
    CHECK(0 == d4.minutes());
    CHECK(0 == d4.seconds());
    CHECK(csvsqldb::Duration::Negative == d4.sign());

    CHECK(-1 == d4.yearsWithSign());
    CHECK(-2 == d4.monthsWithSign());
    CHECK(-3 == d4.daysWithSign());
    CHECK(0 == d4.hoursWithSign());
    CHECK(0 == d4.minutesWithSign());
    CHECK(0 == d4.secondsWithSign());
  }

  SECTION("copy")
  {
    csvsqldb::Duration d1(3, 1, 90, 17, 6, 45, csvsqldb::Duration::Negative);
    csvsqldb::Duration d2(d1);

    CHECK(3 == d2.years());
    CHECK(1 == d2.months());
    CHECK(90 == d2.days());
    CHECK(17 == d2.hours());
    CHECK(6 == d2.minutes());
    CHECK(45 == d2.seconds());
    CHECK(csvsqldb::Duration::Negative == d2.sign());

    csvsqldb::Duration d3 = d2;

    CHECK(3 == d3.years());
    CHECK(1 == d3.months());
    CHECK(90 == d3.days());
    CHECK(17 == d3.hours());
    CHECK(6 == d3.minutes());
    CHECK(45 == d3.seconds());
    CHECK(csvsqldb::Duration::Negative == d3.sign());
  }

  SECTION("set duration part")
  {
    csvsqldb::Duration d1(3, 1, 90, 17, 6, 45);

    d1.ymdhmss(1, 2, 3, 4, 5, 6, csvsqldb::Duration::Negative);
    CHECK(1 == d1.years());
    CHECK(2 == d1.months());
    CHECK(3 == d1.days());
    CHECK(4 == d1.hours());
    CHECK(5 == d1.minutes());
    CHECK(6 == d1.seconds());
    CHECK(csvsqldb::Duration::Negative == d1.sign());

    d1.years(6);
    d1.months(5);
    d1.days(4);
    d1.hours(3);
    d1.minutes(2);
    d1.seconds(1);
    d1.sign(csvsqldb::Duration::Positive);
    CHECK(6 == d1.years());
    CHECK(5 == d1.months());
    CHECK(4 == d1.days());
    CHECK(3 == d1.hours());
    CHECK(2 == d1.minutes());
    CHECK(1 == d1.seconds());
    CHECK(csvsqldb::Duration::Positive == d1.sign());
  }

  SECTION("add")
  {
    csvsqldb::Duration d1(3, 1, 90, 17, 6, 45);
    d1.add(csvsqldb::Duration(5, 2, 1, 3, 7, 5));
    CHECK(8 == d1.years());
    CHECK(3 == d1.months());
    CHECK(91 == d1.days());
    CHECK(20 == d1.hours());
    CHECK(13 == d1.minutes());
    CHECK(50 == d1.seconds());

    csvsqldb::Duration d2(1, 2, 5);
    d2.add(d1);
    CHECK(9 == d2.years());
    CHECK(5 == d2.months());
    CHECK(96 == d2.days());
    CHECK(20 == d2.hours());
    CHECK(13 == d2.minutes());
    CHECK(50 == d2.seconds());
    CHECK(csvsqldb::Duration::Positive == d2.sign());

    csvsqldb::Duration d3(1, 2, 3, csvsqldb::Duration::Negative);
    CHECK_THROWS_AS(d3.add(d2), csvsqldb::DurationException);

    d3.add(csvsqldb::Duration(1, 2, 3, csvsqldb::Duration::Negative));
    CHECK(2 == d3.years());
    CHECK(4 == d3.months());
    CHECK(6 == d3.days());
    CHECK(0 == d3.hours());
    CHECK(0 == d3.minutes());
    CHECK(0 == d3.seconds());
    CHECK(csvsqldb::Duration::Negative == d3.sign());
  }

  SECTION("compare")
  {
    csvsqldb::Duration d1(3, 1, 90, 17, 6, 45);
    csvsqldb::Duration d2(1, 2, 5);
    csvsqldb::Duration d3(1, 2, 5, 17, 6, 45);

    CHECK(d1 != d2);
    CHECK_FALSE(d1 == d2);
    CHECK(d1 > d2);
    CHECK(d2 < d1);
    CHECK(d3 > d2);
    CHECK(d1 > d3);
    CHECK(d1 == d1);

    CHECK(csvsqldb::Duration(3, 2, 90, 17, 6, 45) > csvsqldb::Duration(3, 2, 90, 17, 6, 45, csvsqldb::Duration::Negative));
    CHECK_FALSE(csvsqldb::Duration(3, 2, 90, 17, 6, 45, csvsqldb::Duration::Negative) > csvsqldb::Duration(3, 2, 90, 17, 6, 45));

    CHECK(csvsqldb::Duration(3, 2, 90, 17, 6, 45) > csvsqldb::Duration(2, 2, 90, 17, 6, 45));
    CHECK_FALSE(csvsqldb::Duration(2, 2, 90, 17, 6, 45) > csvsqldb::Duration(3, 2, 90, 17, 6, 45));
    CHECK(csvsqldb::Duration(3, 2, 90, 17, 6, 45) > csvsqldb::Duration(3, 1, 90, 17, 6, 45));
    CHECK_FALSE(csvsqldb::Duration(3, 1, 90, 17, 6, 45) > csvsqldb::Duration(3, 2, 90, 17, 6, 45));
    CHECK(csvsqldb::Duration(3, 2, 90, 17, 6, 45) > csvsqldb::Duration(3, 2, 89, 17, 6, 45));
    CHECK_FALSE(csvsqldb::Duration(3, 2, 89, 17, 6, 45) > csvsqldb::Duration(3, 2, 90, 17, 6, 45));
    CHECK(csvsqldb::Duration(3, 2, 90, 17, 6, 45) > csvsqldb::Duration(3, 2, 90, 16, 6, 45));
    CHECK_FALSE(csvsqldb::Duration(3, 2, 90, 16, 6, 45) > csvsqldb::Duration(3, 2, 90, 17, 6, 45));
    CHECK(csvsqldb::Duration(3, 2, 90, 17, 6, 45) > csvsqldb::Duration(3, 2, 90, 17, 5, 45));
    CHECK_FALSE(csvsqldb::Duration(3, 2, 90, 17, 5, 45) > csvsqldb::Duration(3, 2, 90, 17, 6, 45));
    CHECK(csvsqldb::Duration(3, 2, 90, 17, 6, 45) > csvsqldb::Duration(3, 2, 90, 17, 6, 44));
    CHECK_FALSE(csvsqldb::Duration(3, 2, 90, 17, 6, 44) > csvsqldb::Duration(3, 2, 90, 17, 6, 45));

    CHECK_FALSE(csvsqldb::Duration(3, 2, 90, 17, 6, 45) < csvsqldb::Duration(3, 2, 90, 17, 6, 45, csvsqldb::Duration::Negative));
    CHECK(csvsqldb::Duration(3, 2, 90, 17, 6, 45, csvsqldb::Duration::Negative) < csvsqldb::Duration(3, 2, 90, 17, 6, 45));

    CHECK_FALSE(csvsqldb::Duration(3, 2, 90, 17, 6, 45) < csvsqldb::Duration(2, 2, 90, 17, 6, 45));
    CHECK(csvsqldb::Duration(2, 2, 90, 17, 6, 45) < csvsqldb::Duration(3, 2, 90, 17, 6, 45));
    CHECK_FALSE(csvsqldb::Duration(3, 2, 90, 17, 6, 45) < csvsqldb::Duration(3, 1, 90, 17, 6, 45));
    CHECK(csvsqldb::Duration(3, 1, 90, 17, 6, 45) < csvsqldb::Duration(3, 2, 90, 17, 6, 45));
    CHECK_FALSE(csvsqldb::Duration(3, 2, 90, 17, 6, 45) < csvsqldb::Duration(3, 2, 89, 17, 6, 45));
    CHECK(csvsqldb::Duration(3, 2, 89, 17, 6, 45) < csvsqldb::Duration(3, 2, 90, 17, 6, 45));
    CHECK_FALSE(csvsqldb::Duration(3, 2, 90, 17, 6, 45) < csvsqldb::Duration(3, 2, 90, 16, 6, 45));
    CHECK(csvsqldb::Duration(3, 2, 90, 16, 6, 45) < csvsqldb::Duration(3, 2, 90, 17, 6, 45));
    CHECK_FALSE(csvsqldb::Duration(3, 2, 90, 17, 6, 45) < csvsqldb::Duration(3, 2, 90, 17, 5, 45));
    CHECK(csvsqldb::Duration(3, 2, 90, 17, 5, 45) < csvsqldb::Duration(3, 2, 90, 17, 6, 45));
    CHECK_FALSE(csvsqldb::Duration(3, 2, 90, 17, 6, 45) < csvsqldb::Duration(3, 2, 90, 17, 6, 44));
    CHECK(csvsqldb::Duration(3, 2, 90, 17, 6, 44) < csvsqldb::Duration(3, 2, 90, 17, 6, 45));
  }

  SECTION("format")
  {
    csvsqldb::Duration d1(3, 1, 90, 17, 6, 45);
    CHECK("P3Y1M90DT17H6M45S" == d1.format());
    csvsqldb::Duration d2(1, 2, 5);
    CHECK("P1Y2M5D" == d2.format());
    csvsqldb::Duration d3(1, 2, 5, 17, 6, 45);
    CHECK("P1Y2M5DT17H6M45S" == d3.format());
  }
}
