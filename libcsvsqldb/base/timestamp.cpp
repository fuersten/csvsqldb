//
//  timestamp.cpp
//  csvsqldb
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

#include "timestamp.h"

#include "exception.h"
#include "time.h"
#include "types.h"

#include <iomanip>
#include <math.h>
#include <mutex>
#include <thread>
#include <time.h>


namespace csvsqldb
{
#define SECONDSPERDAY 86400L
#define MILLISECONDSPERDAY (SECONDSPERDAY * 1000L)

  Timestamp Timestamp::now()
  {
    time_t t;
    time(&t);
    struct tm today;
    localtime_r(&t, &today);

    return Timestamp(static_cast<uint16_t>(today.tm_year + 1900), static_cast<Date::eMonth>(today.tm_mon + 1),
                     static_cast<uint16_t>(today.tm_mday), static_cast<uint16_t>(today.tm_hour),
                     static_cast<uint16_t>(today.tm_min), static_cast<uint16_t>(today.tm_sec), 0);
  }

  void Timestamp::Now()
  {
    *this = Timestamp::now();
  }

  Timestamp::Timestamp()
  : _time(0)
  {
  }

  Timestamp::Timestamp(uint16_t year, Date::eMonth month, uint16_t day, uint16_t hour, uint16_t minute, uint16_t second,
                       uint16_t millisecond)
  {
    if (!Timestamp::isValid(year, month, day, hour, minute, second, millisecond)) {
      CSVSQLDB_THROW(TimestampException, "Invalid timestamp specified: year "
                                           << year << ", month " << month << ", day " << day << ", hour " << hour << ", minute "
                                           << minute << ", second " << second << ", millisecond " << millisecond);
    }
    _time = calcJulDay(year, month, day, hour, minute, second, millisecond);
  }

  Timestamp::Timestamp(const Date& date)
  {
    uint16_t year = date.year();
    uint16_t month = date.month();
    uint16_t day = date.day();

    if (!Timestamp::isValid(year, static_cast<Date::eMonth>(month), day, 0, 0, 0, 0)) {
      CSVSQLDB_THROW(TimestampException, "Invalid timestamp specified: year " << year << ", month " << month << ", day " << day
                                                                              << ", hour " << 0 << ", minute " << 0 << ", second "
                                                                              << 0 << ", millisecond " << 0);
    }
    _time = calcJulDay(year, month, day, 0, 0, 0, 0);
  }

  Timestamp::Timestamp(const Time& time)
  {
    uint16_t hour = time.hour();
    uint16_t minute = time.minute();
    uint16_t second = time.second();
    uint16_t millisecond = time.millisecond();

    if (!Timestamp::isValid(0, static_cast<Date::eMonth>(1), 1, hour, minute, second, millisecond)) {
      CSVSQLDB_THROW(TimestampException, "Invalid timestamp specified: year "
                                           << 0 << ", month " << 0 << ", day " << 0 << ", hour " << hour << ", minute " << minute
                                           << ", second " << second << ", millisecond " << millisecond);
    }
    _time = calcJulDay(0, 0, 0, hour, minute, second, millisecond);
  }

  Timestamp::Timestamp(int64_t time)
  : _time(time)
  {
  }

  /*
  Timestamp::Timestamp(time_t timeT)
  {
   struct tm ptime;
   if(!localtime_r(&timeT, &ptime)) {
          CSVSQLDB_THROW(TimestampException, "Invalid time_t specified: " << timeT);
      }
  }*/

  Timestamp::Timestamp(const Timestamp& timestamp)
  {
    _time = timestamp._time;
  }

  Timestamp& Timestamp::operator=(const Timestamp& timestamp)
  {
    _time = timestamp._time;
    return *this;
  }

  bool Timestamp::operator==(const Timestamp& timestamp) const
  {
    return _time == timestamp._time;
  }

  bool Timestamp::operator!=(const Timestamp& timestamp) const
  {
    return _time != timestamp._time;
  }

  bool Timestamp::operator>(const Timestamp& timestamp) const
  {
    return _time > timestamp._time;
  }

  bool Timestamp::operator<(const Timestamp& timestamp) const
  {
    return _time < timestamp._time;
  }

  bool Timestamp::operator>=(const Timestamp& timestamp) const
  {
    return _time >= timestamp._time;
  }

  bool Timestamp::operator<=(const Timestamp& timestamp) const
  {
    return _time <= timestamp._time;
  }

  int64_t Timestamp::operator-(const Timestamp& timestamp) const
  {
    int64_t julDayRight = timestamp._time / 100000000L;
    int64_t timeRight = (timestamp._time - (julDayRight * 100000000L)) / 1000;
    julDayRight *= 24 * 60 * 60;
    int64_t julDayLeft = _time / 100000000L;
    int64_t timeLeft = (_time - (julDayLeft * 100000000L)) / 1000;
    julDayLeft *= 24 * 60 * 60;

    return (julDayLeft + timeLeft) - (julDayRight + timeRight);
  }

  Timestamp::operator bool() const
  {
    return !isInfinite();
  }

  uint16_t Timestamp::day() const
  {
    uint16_t year, month, day, hour, minute, second, millisecond;
    calcFromJulDay(_time, year, month, day, hour, minute, second, millisecond);
    return day;
  }

  Date::eMonth Timestamp::month() const
  {
    uint16_t year, month, day, hour, minute, second, millisecond;
    calcFromJulDay(_time, year, month, day, hour, minute, second, millisecond);
    return static_cast<Date::eMonth>(month);
  }

  uint16_t Timestamp::year() const
  {
    uint16_t year, month, day, hour, minute, second, millisecond;
    calcFromJulDay(_time, year, month, day, hour, minute, second, millisecond);
    return year;
  }

  uint16_t Timestamp::hour() const
  {
    uint16_t year, month, day, hour, minute, second, millisecond;
    calcFromJulDay(_time, year, month, day, hour, minute, second, millisecond);
    return hour;
  }

  uint16_t Timestamp::minute() const
  {
    uint16_t year, month, day, hour, minute, second, millisecond;
    calcFromJulDay(_time, year, month, day, hour, minute, second, millisecond);
    return minute;
  }

  uint16_t Timestamp::second() const
  {
    uint16_t year, month, day, hour, minute, second, millisecond;
    calcFromJulDay(_time, year, month, day, hour, minute, second, millisecond);
    return second;
  }

  uint16_t Timestamp::millisecond() const
  {
    uint16_t year, month, day, hour, minute, second, millisecond;
    calcFromJulDay(_time, year, month, day, hour, minute, second, millisecond);
    return millisecond;
  }

  void Timestamp::day(uint16_t day)
  {
    uint16_t year, month, tday, hour, minute, second, millisecond;
    calcFromJulDay(_time, year, month, tday, hour, minute, second, millisecond);
    if (!Timestamp::isValid(year, static_cast<Date::eMonth>(month), day, hour, minute, second, millisecond)) {
      CSVSQLDB_THROW(TimestampException, "Invalid timestamp specified: year "
                                           << year << ", month " << month << ", day " << day << ", hour " << hour << ", minute "
                                           << minute << ", second " << second << ", millisecond " << millisecond);
    }
    _time = calcJulDay(year, month, day, hour, minute, second, millisecond);
  }

  void Timestamp::month(Date::eMonth month)
  {
    uint16_t year, tmonth, day, hour, minute, second, millisecond;
    calcFromJulDay(_time, year, tmonth, day, hour, minute, second, millisecond);
    if (!Timestamp::isValid(year, static_cast<Date::eMonth>(month), day, hour, minute, second, millisecond)) {
      CSVSQLDB_THROW(TimestampException, "Invalid timestamp specified: year "
                                           << year << ", month " << month << ", day " << day << ", hour " << hour << ", minute "
                                           << minute << ", second " << second << ", millisecond " << millisecond);
    }
    _time = calcJulDay(year, month, day, hour, minute, second, millisecond);
  }

  void Timestamp::year(uint16_t year)
  {
    uint16_t tyear, month, day, hour, minute, second, millisecond;
    calcFromJulDay(_time, tyear, month, day, hour, minute, second, millisecond);
    if (!Timestamp::isValid(year, static_cast<Date::eMonth>(month), day, hour, minute, second, millisecond)) {
      CSVSQLDB_THROW(TimestampException, "Invalid timestamp specified: year "
                                           << year << ", month " << month << ", day " << day << ", hour " << hour << ", minute "
                                           << minute << ", second " << second << ", millisecond " << millisecond);
    }
    _time = calcJulDay(year, month, day, hour, minute, second, millisecond);
  }

  void Timestamp::hour(uint16_t hour)
  {
    uint16_t year, month, day, thour, minute, second, millisecond;
    calcFromJulDay(_time, year, month, day, thour, minute, second, millisecond);
    if (!Timestamp::isValid(year, static_cast<Date::eMonth>(month), day, hour, minute, second, millisecond)) {
      CSVSQLDB_THROW(TimestampException, "Invalid timestamp specified: year "
                                           << year << ", month " << month << ", day " << day << ", hour " << hour << ", minute "
                                           << minute << ", second " << second << ", millisecond " << millisecond);
    }
    _time = calcJulDay(year, month, day, hour, minute, second, millisecond);
  }

  void Timestamp::minute(uint16_t minute)
  {
    uint16_t year, month, day, hour, tminute, second, millisecond;
    calcFromJulDay(_time, year, month, day, hour, tminute, second, millisecond);
    if (!Timestamp::isValid(year, static_cast<Date::eMonth>(month), day, hour, minute, second, millisecond)) {
      CSVSQLDB_THROW(TimestampException, "Invalid timestamp specified: year "
                                           << year << ", month " << month << ", day " << day << ", hour " << hour << ", minute "
                                           << minute << ", second " << second << ", millisecond " << millisecond);
    }
    _time = calcJulDay(year, month, day, hour, minute, second, millisecond);
  }

  void Timestamp::second(uint16_t second)
  {
    uint16_t year, month, day, hour, minute, tsecond, millisecond;
    calcFromJulDay(_time, year, month, day, hour, minute, tsecond, millisecond);
    if (!Timestamp::isValid(year, static_cast<Date::eMonth>(month), day, hour, minute, second, millisecond)) {
      CSVSQLDB_THROW(TimestampException, "Invalid timestamp specified: year "
                                           << year << ", month " << month << ", day " << day << ", hour " << hour << ", minute "
                                           << minute << ", second " << second << ", millisecond " << millisecond);
    }
    _time = calcJulDay(year, month, day, hour, minute, second, millisecond);
  }

  void Timestamp::millisecond(uint16_t millisecond)
  {
    uint16_t year, month, day, hour, minute, second, tmillisecond;
    calcFromJulDay(_time, year, month, day, hour, minute, second, tmillisecond);
    if (!Timestamp::isValid(year, static_cast<Date::eMonth>(month), day, hour, minute, second, millisecond)) {
      CSVSQLDB_THROW(TimestampException, "Invalid timestamp specified: year "
                                           << year << ", month " << month << ", day " << day << ", hour " << hour << ", minute "
                                           << minute << ", second " << second << ", millisecond " << millisecond);
    }
    _time = calcJulDay(year, month, day, hour, minute, second, millisecond);
  }

  void Timestamp::ymdhmsm(uint16_t year, Date::eMonth month, uint16_t day, uint16_t hour, uint16_t minute, uint16_t second,
                          uint16_t millisecond)
  {
    if (!Timestamp::isValid(year, static_cast<Date::eMonth>(month), day, hour, minute, second, millisecond)) {
      CSVSQLDB_THROW(TimestampException, "Invalid timestamp specified: year "
                                           << year << ", month " << month << ", day " << day << ", hour " << hour << ", minute "
                                           << minute << ", second " << second << ", millisecond " << millisecond);
    }
    _time = calcJulDay(year, month, day, hour, minute, second, millisecond);
  }

  bool Timestamp::isLeapYear() const
  {
    uint16_t year, month, day, hour, minute, second, millisecond;
    calcFromJulDay(_time, year, month, day, hour, minute, second, millisecond);
    return isLeapYear(year);
  }

  bool Timestamp::isInfinite() const
  {
    return _time == 0;
  }

  Date::eDay Timestamp::dayOfWeek() const
  {
    uint16_t year, month, day, hour, minute, second, millisecond;
    calcFromJulDay(_time, year, month, day, hour, minute, second, millisecond);
    return Date(year, static_cast<Date::eMonth>(month), day).dayOfWeek();
  }

  uint16_t Timestamp::dayOfYear() const
  {
    uint16_t year, month, day, hour, minute, second, millisecond;
    calcFromJulDay(_time, year, month, day, hour, minute, second, millisecond);
    return Date(year, static_cast<Date::eMonth>(month), day).dayOfYear();
  }

  uint16_t Timestamp::weekOfYear() const
  {
    uint16_t year, month, day, hour, minute, second, millisecond;
    calcFromJulDay(_time, year, month, day, hour, minute, second, millisecond);
    return Date(year, static_cast<Date::eMonth>(month), day).weekOfYear();
  }

  void Timestamp::addDuration(const Duration& duration)
  {
    uint16_t year, month, day, hour, minute, second, millisecond;
    calcFromJulDay(_time, year, month, day, hour, minute, second, millisecond);
    Date::addDuration(year, month, day, hour, minute, second, duration);
    _time = calcJulDay(year, month, day, hour, minute, second, millisecond);
  }

  bool Timestamp::isValid(uint16_t year, Date::eMonth month, uint16_t day, uint16_t hour, uint16_t minute, uint16_t second,
                          uint16_t millisecond)
  {
    return Date::isValid(year, month, day) && Time::isValid(hour, minute, second, millisecond);
  }

  bool Timestamp::isLeapYear(uint16_t year)
  {
    return Date::isLeapYear(year);
  }

  struct FormatContext {
    FormatContext(const std::string& format)
    : _format(format)
    , _n(0)
    , _length(_format.length())
    {
    }

    char nextFormatSpecifier()
    {
      if (_n >= _length) {
        CSVSQLDB_THROW(TimestampException, "invalid format specifier '" << _format << "'");
      }
      return _format[_n++];
    }

    bool ready() const
    {
      return _n >= _length;
    }

    const std::string& _format;
    std::string::size_type _n;
    std::string::size_type _length;
  };

  std::string Timestamp::format(const std::string& format) const
  {
    char buf[20];
    uint16_t year, month, day, hour, minute, second, millisecond;
    calcFromJulDay(_time, year, month, day, hour, minute, second, millisecond);

    FormatContext context(format);
    std::stringstream ss;
    while (!context.ready()) {
      char c = context.nextFormatSpecifier();
      switch (c) {
        case '%': {
          char n = context.nextFormatSpecifier();
          switch (n) {
            case 'Y':
              ss << std::setw(4) << std::setfill('0') << itoa(year, buf);
              break;
            case 'm':
              ss << std::setw(2) << std::setfill('0') << itoa(month, buf);
              break;
            case 'd':
              ss << std::setw(2) << std::setfill('0') << itoa(day, buf);
              break;
            case 'F': {
              if (year > 1000) {
                itoa(year, buf);
              } else if (year > 100) {
                buf[0] = '0';
                itoa(year, &buf[1]);
              } else if (year > 10) {
                buf[0] = '0';
                buf[1] = '0';
                itoa(year, &buf[2]);
              } else {
                buf[0] = '0';
                buf[1] = '0';
                buf[2] = '0';
                itoa(year, &buf[3]);
              }
              buf[4] = '-';
              if (month > 9) {
                itoa(month, &buf[5]);
              } else {
                buf[5] = '0';
                itoa(month, &buf[6]);
              }
              buf[7] = '-';
              if (day > 9) {
                itoa(day, &buf[8]);
              } else {
                buf[8] = '0';
                itoa(day, &buf[9]);
              }
              buf[10] = 'T';
              if (hour > 9) {
                itoa(hour, &buf[11]);
              } else {
                buf[11] = '0';
                itoa(hour, &buf[12]);
              }
              buf[13] = ':';
              if (minute > 9) {
                itoa(minute, &buf[14]);
              } else {
                buf[14] = '0';
                itoa(minute, &buf[15]);
              }
              buf[16] = ':';
              if (second > 9) {
                itoa(second, &buf[17]);
              } else {
                buf[17] = '0';
                itoa(second, &buf[18]);
              }
              ss << buf;
              break;
            }
            case 'j':
              ss << std::setw(3) << std::setfill('0') << itoa(dayOfYear(), buf);
              break;
            case 'U':
              ss << std::setw(2) << std::setfill('0') << itoa(weekOfYear(), buf);
              break;
            case 'w':
              ss << itoa(dayOfWeek(), buf);
              break;
            case 'H':
              ss << std::setw(2) << std::setfill('0') << itoa(hour, buf);
              break;
            case 'M':
              ss << std::setw(2) << std::setfill('0') << itoa(minute, buf);
              break;
            case 'S':
              ss << std::setw(2) << std::setfill('0') << itoa(second, buf);
              break;
            case 's':
              ss << std::setw(3) << std::setfill('0') << itoa(millisecond, buf);
              break;
          }
          break;
        }
        default:
          ss << c;
      }
    }

    return ss.str();
  }

  int64_t Timestamp::calcJulDay(uint16_t year, uint16_t month, uint16_t day, uint16_t hour, uint16_t minute, uint16_t second,
                                uint16_t millisecond)
  {
    return (Date::calcJulDay(year, month, day) * 100000000L) + Time::calcNumberFromTime(hour, minute, second, millisecond);
  }

  void Timestamp::calcFromJulDay(int64_t time, uint16_t& year, uint16_t& month, uint16_t& day, uint16_t& hour, uint16_t& minute,
                                 uint16_t& second, uint16_t& millisecond)
  {
    uint32_t julDay = static_cast<uint32_t>(time / 100000000L);
    Date::calcFromJulDay(julDay, year, month, day);
    Time::calcTimeFromNumber(static_cast<int32_t>(time - (julDay * 100000000L)), hour, minute, second, millisecond);
  }
}
