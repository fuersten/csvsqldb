//
//  date.cpp
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

#include "date.h"

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
  static int quotient(int a, int b)
  {
    return static_cast<int>(floor(static_cast<double>(a) / static_cast<double>(b)));
  }

  static int quotient(int a, int low, int high)
  {
    return quotient(a - low, high - low);
  }

  static int modulo(int a, int b)
  {
    return (a - quotient(a, b) * b);
  }

  static int modulo(int a, int low, int high)
  {
    return modulo(a - low, high - low) + low;
  }

  uint8_t g_daysOfMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


  Date Date::now()
  {
    time_t t;
    time(&t);
    struct tm today;
    localtime_r(&t, &today);

    return Date(static_cast<uint16_t>(today.tm_year + 1900), static_cast<eMonth>(today.tm_mon + 1),
                static_cast<uint16_t>(today.tm_mday));
  }

  void Date::Now()
  {
    *this = Date::now();
  }

  uint16_t Date::maximumDayInMonthFor(uint16_t year, eMonth month)
  {
    if (!Date::isValid(year, month, 1)) {
      CSVSQLDB_THROW(DateException, "Invalid date specified: year " << year << ", month " << month);
    }
    return month == 2 ? (Date::isLeapYear(year) ? 29 : 28) : g_daysOfMonth[month - 1];
  }


  Date::Date()
  : _julianDay(0)
  {
  }

  Date::Date(uint16_t year, eMonth month, uint16_t day)
  {
    if (!Date::isValid(year, month, day)) {
      CSVSQLDB_THROW(DateException, "Invalid date specified: year " << year << ", month " << month << ", day " << day);
    }

    _julianDay = calcJulDay(year, month, day);
  }

  Date::Date(time_t timeT)
  {
    struct tm ptime;
    if (!localtime_r(&timeT, &ptime)) {
      CSVSQLDB_THROW(DateException, "Invalid time_t specified: " << timeT);
    }

    _julianDay = calcJulDay(static_cast<uint16_t>(ptime.tm_year + 1900), static_cast<uint16_t>(ptime.tm_mon + 1),
                            static_cast<uint16_t>(ptime.tm_mday));
  }

  Date::Date(uint32_t julianDay)
  : _julianDay(julianDay)
  {
  }

  Date::Date(const Date& date)
  {
    _julianDay = date._julianDay;
  }

  Date& Date::operator=(const Date& date)
  {
    _julianDay = date._julianDay;

    return *this;
  }

  bool Date::operator==(const Date& date) const
  {
    return _julianDay == date._julianDay;
  }

  bool Date::operator!=(const Date& date) const
  {
    return _julianDay != date._julianDay;
  }

  bool Date::operator>(const Date& date) const
  {
    return _julianDay > date._julianDay;
  }

  bool Date::operator<(const Date& date) const
  {
    return _julianDay < date._julianDay;
  }

  bool Date::operator>=(const Date& date) const
  {
    return _julianDay >= date._julianDay;
  }

  bool Date::operator<=(const Date& date) const
  {
    return _julianDay <= date._julianDay;
  }

  long Date::operator-(const Date& date) const
  {
    return static_cast<int64_t>(_julianDay) - static_cast<int64_t>(date._julianDay);
  }

  Date::operator bool() const
  {
    return !isInfinite();
  }

  uint16_t Date::day() const
  {
    uint16_t year, month, day;
    calcFromJulDay(_julianDay, year, month, day);
    return day;
  }

  Date::eMonth Date::month() const
  {
    uint16_t year, month, day;
    calcFromJulDay(_julianDay, year, month, day);
    return static_cast<eMonth>(month);
  }

  uint16_t Date::year() const
  {
    uint16_t year, month, day;
    calcFromJulDay(_julianDay, year, month, day);
    return year;
  }

  void Date::day(uint16_t day)
  {
    uint16_t year, month, mday;
    calcFromJulDay(_julianDay, year, month, mday);

    if (!Date::isValid(year, static_cast<eMonth>(month), day)) {
      CSVSQLDB_THROW(DateException, "Invalid date specified: year " << year << ", month " << month << ", day " << day);
    }

    _julianDay = calcJulDay(year, month, day);
  }

  void Date::month(eMonth month)
  {
    uint16_t year, mmonth, day;
    calcFromJulDay(_julianDay, year, mmonth, day);
    if (!Date::isValid(year, month, day)) {
      CSVSQLDB_THROW(DateException, "Invalid date specified: year " << year << ", month " << month << ", day " << day);
    }

    _julianDay = calcJulDay(year, month, day);
  }

  void Date::year(uint16_t year)
  {
    uint16_t myear, month, day;
    calcFromJulDay(_julianDay, myear, month, day);
    if (!Date::isValid(year, static_cast<eMonth>(month), day)) {
      CSVSQLDB_THROW(DateException, "Invalid date specified: year " << year << ", month " << month << ", day " << day);
    }

    _julianDay = calcJulDay(year, month, day);
  }

  void Date::ymd(uint16_t year, eMonth month, uint16_t day)
  {
    if (!Date::isValid(year, month, day)) {
      CSVSQLDB_THROW(DateException, "Invalid date specified: year " << year << ", month " << month << ", day " << day);
    }

    _julianDay = calcJulDay(year, month, day);
  }

  bool Date::isLeapYear() const
  {
    uint16_t year, month, day;
    calcFromJulDay(_julianDay, year, month, day);
    return Date::isLeapYear(year);
  }

  bool Date::isInfinite() const
  {
    return _julianDay == 0;
  }

  Date::eDay Date::dayOfWeek() const
  {
    return static_cast<Date::eDay>(static_cast<int32_t>(_julianDay + 1.5) % 7);
  }

  uint16_t Date::dayOfYear() const
  {
    uint16_t year, month, day;
    calcFromJulDay(_julianDay, year, month, day);

    uint16_t dayOfYear = 0;

    for (uint8_t i = 0; i < month - 1U; i++) {
      dayOfYear += g_daysOfMonth[i];
    }

    dayOfYear += day;

    if (isLeapYear() && month > 2) {
      ++dayOfYear;
    }

    return dayOfYear;
  }

  uint16_t Date::weekOfYear() const
  {
    long d4 = ((((_julianDay + 31741) - (_julianDay % 7)) % 146097) % 36524) % 1461;
    long L = d4 / 1460;
    long d1 = ((d4 - L) % 365) + L;
    return static_cast<uint16_t>((d1 / 7) + 1);
  }

  void Date::calcFromJulDay(uint32_t julDay, uint16_t& year, uint16_t& month, uint16_t& day)
  {
    uint32_t a;
    uint32_t m;
    a = julDay + 32044;
    uint32_t b = (4 * a + 3) / 146097;
    uint32_t c = a - (146097 * b) / 4;
    uint32_t d = (4 * c + 3) / 1461;
    uint32_t e = c - ((1461 * d) / 4);
    m = (5 * e + 2) / 153;

    day = static_cast<uint16_t>((e - ((153 * m + 2) / 5)) + 1);
    month = static_cast<uint16_t>(m + (3 - (12 * (m / 10))));
    year = static_cast<uint16_t>((100 * b) + (d - 4800) + (m / 10));
  }

  uint32_t Date::calcJulDay(uint16_t year, uint16_t month, uint16_t day)
  {
    int32_t a = (14 - month) / 12;
    int32_t y = year + 4800 - a;
    int32_t m = month + 12 * a - 3;
    return static_cast<uint32_t>(day + ((153 * m + 2) / 5) + 365 * y + (y / 4) - (y / 100) + (y / 400) - 32045);
  }

  Date& Date::addDays(int16_t days)
  {
    if (days > 0) {
      _julianDay += static_cast<uint16_t>(days);
    } else if (days < 0) {
      _julianDay -= static_cast<uint16_t>(-days);
    }
    return *this;
  }

  void Date::addDuration(uint16_t& year, uint16_t& month, uint16_t& day, uint16_t& hour, uint16_t& minute, uint16_t& second,
                         const Duration& duration)
  {
    int32_t orgDay = day;
    int32_t temp = month + duration.monthsWithSign();
    month = static_cast<uint16_t>(modulo(temp, 1, 13));
    int carry = quotient(temp, 1, 13);
    year = static_cast<uint16_t>(year + duration.yearsWithSign() + carry);

    temp = second + duration.secondsWithSign();
    second = static_cast<uint16_t>(modulo(temp, 60));
    carry = quotient(temp, 60);
    temp = minute + duration.minutesWithSign() + carry;
    minute = static_cast<uint16_t>(modulo(temp, 60));
    carry = quotient(temp, 60);
    temp = hour + duration.hoursWithSign() + carry;
    hour = static_cast<uint16_t>(modulo(temp, 24));
    carry = quotient(temp, 24);

    int32_t tempDays;

    if (day > maximumDayInMonthFor(year, static_cast<eMonth>(month))) {
      tempDays = maximumDayInMonthFor(year, static_cast<eMonth>(month));
    } else if (day < 1) {
      tempDays = 1;
    } else {
      tempDays = day;
    }

    orgDay = tempDays + duration.daysWithSign() + carry;

    while (1) {
      if (orgDay < 1) {
        orgDay += maximumDayInMonthFor(year, static_cast<eMonth>(month - 1));
        carry = -1;
      } else if (orgDay > maximumDayInMonthFor(year, static_cast<eMonth>(month))) {
        orgDay -= maximumDayInMonthFor(year, static_cast<eMonth>(month));
        carry = 1;
      } else {
        break;
      }
      int tempMonth = month + carry;
      month = static_cast<uint16_t>(modulo(tempMonth, 1, 13));
      year += quotient(tempMonth, 1, 13);
    }
    day = static_cast<uint16_t>(orgDay);
    if (!Date::isValid(year, static_cast<eMonth>(month), day)) {
      CSVSQLDB_THROW(DateException, "Invalid date specified: year " << year << ", month " << month << ", day " << day);
    }
    if (!Time::isValid(hour, minute, second, 0)) {
      CSVSQLDB_THROW(TimeException, "Invalid time specified: hour " << hour << ", minute " << minute << ", second " << second);
    }
  }

  void Date::addDuration(const Duration& duration)
  {
    uint16_t year, month, day, hour, minute, second;
    hour = minute = second = 0;
    calcFromJulDay(_julianDay, year, month, day);
    addDuration(year, month, day, hour, minute, second, duration);
    _julianDay = calcJulDay(year, month, day);
  }

  bool Date::isValid(uint16_t year, eMonth month, uint16_t day)
  {
    return (month >= 1 && month <= 12 && year >= 0 && year <= 9999 && day >= 1 &&
            day <= (g_daysOfMonth[month - 1] + ((Date::isLeapYear(year) && month == 2) ? 1 : 0)));
  }

  bool Date::isLeapYear(uint16_t year)
  {
    return (((year & 3 ? false : true) && !(year % 100 ? false : true)) || (year % 400 ? false : true));
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
        CSVSQLDB_THROW(DateException, "invalid format specifier '" << _format << "'");
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

  std::string Date::format(const std::string& format) const
  {
    char buf[11];
    uint16_t year, month, day;
    calcFromJulDay(_julianDay, year, month, day);

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
          }
          break;
        }
        default:
          ss << c;
      }
    }

    return ss.str();
  }
}
