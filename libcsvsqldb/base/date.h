//
//  date.h
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
/* Infos concerning date calculations:
 www.tondering.dk/claus/cal
 */

#pragma once

#include "libcsvsqldb/inc.h"

#include "duration.h"

#include <cstdint>
#include <ctime>
#include <string>


namespace csvsqldb
{
  /** This class represents a specific date. The date is represented by
   *  the year, month and day. The date is based on the gregorian calendar.
   *  Beware that most of the methods will throw a DateException, if illegal or infinite
   *  dates are used.
   */
  class CSVSQLDB_EXPORT Date
  {
  public:
    enum eDay { Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday };

    enum eMonth { January = 1, February, March, April, May, June, July, August, September, October, November, December };

    /** Gets the current system date.
     *  @return Current system date.
     */
    static Date now();

    /** Sets this date to the current systems date.
     */
    void Now();

    /** Validates a given year, month and day.
     *  @return If the triple is a valid date, true is given back,
     *          otherwise false.
     *  @param year The year to be validated (0000-9999).
     *  @param month The month to be validated (1-12).
     *  @param day The day to be validated (1-31, depending on month).
     */
    static bool isValid(uint16_t year, eMonth month, uint16_t day);

    /** Determines if a given year is a leap year (february has 29 days).
     *  @return If the given year is a leap year true, otherwise false.
     *  @param year The year to be tested (0000-9999).
     */
    static bool isLeapYear(uint16_t year);

    /** Determines the maximum days in a month for a given year and month.
     *  @return The maximum days for the given month for the given year.
     *  @param year The year to be used as base for the calculation.
     *  @param month The month for which the maximum days shall be returned.
     */
    static uint16_t maximumDayInMonthFor(uint16_t year, eMonth month);

    /** Constructs a date.
     *  All parts (year, month and day) are set to 0. The resulting date is infinite.
     *  @see Date::isInfinite
     */
    Date() = default;

    /** Constructs a date with the given parameters.
     *  If the parameters give an invalid date, a DateException will be thrown.
     *  @param year The year of the date (0000-9999).
     *  @param month The month of the date (1-12).
     *  @param day The day of the date (1-31, depending on month).
     */
    Date(uint16_t year, eMonth month, uint16_t day);

    /** Constructs a date with the given time_t structure.
     *  If the time_t structure does not give a valid date, a DateException
     *  will be thrown.
     *  @param timeT The time_t value of a C-library function like time().
     */
    explicit Date(time_t timeT);

    /** Constructs a date from the given julian day.
     *  @param julianDay The julian day of the date.
     */
    Date(uint32_t julianDay);

    Date(const Date&) = default;
    Date& operator=(const Date&) = default;
    Date(Date&&) = default;
    Date& operator=(Date&&) = default;

    bool operator==(const Date& date) const;
    bool operator!=(const Date& date) const;
    bool operator>(const Date& date) const;
    bool operator<(const Date& date) const;
    bool operator>=(const Date& date) const;
    bool operator<=(const Date& date) const;
    long operator-(const Date& date) const;

    /** Returns true if the date is not infinite
     *  @return true if the date is not infinite, otherwise false
     */
    explicit operator bool() const;

    /** Extracts the day part of a date.
     *  @return The day of the date (1-31, depending on month).
     */
    uint16_t day() const;

    /** Extracts the month part of a date.
     *  @return The month of the date (1-12).
     */
    eMonth month() const;

    /** Extracts the year part of a date.
     *  @return The year of the date (0000-9999).
     */
    uint16_t year() const;

    /** Sets the day part of a date.
     *  If the given day value does not give a valid date, a DateException
     *  will be thrown.
     *  @param day The day of the date (1-31, depending on month).
     */
    void day(uint16_t day);

    /** Sets the month part of a date.
     *  If the given month value does not give a valid date, a DateException
     *  will be thrown.
     *  @param month The month of the date (1-12).
     */
    void month(eMonth month);

    /** Sets the year part of a date.
     *  If the given year value does not give a valid date, a DateException
     *  will be thrown.
     *  @param year The year of the date (0000-9999).
     */
    void year(uint16_t year);

    /** Sets all three parts of a date.
     *  If the given values do not give a valid date, a DateException will be
     *  thrown.
     *  @param year The year of the date (0000-9999).
     *  @param month The month of the date (1-12).
     *  @param day The day of the date (1-31, depending on month).
     */
    void ymd(uint16_t year, eMonth month, uint16_t day);

    /** Determines if the year of the date is a leap year (february has 29 days).
     *  @return If the year is a leap year true, otherwise false.
     */
    bool isLeapYear() const;

    /** Determines if the date is infinite.
     *  @return If the is infinite true, otherwise false.
     */
    bool isInfinite() const;

    /** Determines the day of the week of the date.
     *  @return The day of the week (Saturday-Sunday).
     *  @see Date::eDay
     */
    Date::eDay dayOfWeek() const;

    /** Determines the day of the year.
     *  @return The day of the year (1-366).
     */
    uint16_t dayOfYear() const;

    /** Determines the week of the year.
     *  @return The week of the year (1-53).
     */
    uint16_t weekOfYear() const;

    /** Adds the given days to the date. Negative days are subtracted.
     *  @param days The days to be added/subtracted.
     *  @return The modified date.
     */
    Date& addDays(int16_t days);

    /** Adds a Duration to the given date and time.
     *  A DateException is thrown, if an invalid date is used.
     *  A TimeException is thrown, if an invalid time is used.
     *  @param year The year of the date.
     *  @param month The month of the date.
     *  @param day The day of the date.
     *  @param hour The hour of the date.
     *  @param minute The minute of the date.
     *  @param second The second of the date.
     *  @param duration The duration to be added to the date.
     */
    static void addDuration(uint16_t& year, uint16_t& month, uint16_t& day, uint16_t& hour, uint16_t& minute, uint16_t& second,
                            const Duration& duration);

    /** Adds a Duration to the date.
     *  @param duration Duration to add
     */
    void addDuration(const Duration& duration);

    /** Returns a string representation of the date according to the given formatting rules.
     *  Currently the following rules are supported:
     *  - Y The year as four digit number
     *  - M The month as two digit number
     *  - d The day as two digit number
     *  - F ISO representation as YYYY-mm-dd
     *  - j day of year as three digit number
     *  - U week of year as two digit number
     *  - w day of the week as number
     *  - % %
     *  @param format The formatting rule
     *  @return A string representation according to the given formatting rule
     */
    std::string format(const std::string& format) const;

    /** Returns the julian day representation of the date.
     *  @return The julian day representation
     */
    uint32_t asJulianDay() const
    {
      return _julianDay;
    }

    static void calcFromJulDay(uint32_t julDay, uint16_t& year, uint16_t& month, uint16_t& day);
    static uint32_t calcJulDay(uint16_t year, uint16_t month, uint16_t day);

  private:
    uint32_t _julianDay{0};
  } __attribute__((__packed__));
}
