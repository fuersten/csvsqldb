//
//  timestamp.h
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

#ifndef csvsqldb_timestamp_h
#define csvsqldb_timestamp_h

#include "libcsvsqldb/inc.h"

#include "date.h"
#include "time.h"

#include <cstdint>
#include <ctime>
#include <string>


namespace csvsqldb
{
  /** This class represents a specific timestamp. The timestamp is represented by
   *  the year, month, day, hour, minute, second and milliseconds. The timestamps date is based on the gregorian calendar.
   *  Beware that most of the methods will throw a TimestampException, if illegal or infinite
   *  dates are used.
   */
  class CSVSQLDB_EXPORT Timestamp
  {
  public:
    /** Gets the current system timestamp.
     *  @return Current system timestamp.
     */
    static Timestamp now();

    /** Sets this date to the current systems timestamp.
     */
    void Now();

    /** Validates a given year, month and day.
     *  @return If the triple is a valid date, true is given back,
     *          otherwise false.
     *  @param year The year to be validated (0000-9999).
     *  @param month The month to be validated (1-12).
     *  @param day The day to be validated (1-31, depending on month).
     *  @param hour The hour of the time (0-23).
     *  @param minute The minute of the time (0-59).
     *  @param second The second of the time (0-59).
     *  @param millisecond The millisecond of the time (0-999).
     */
    static bool isValid(uint16_t year, Date::eMonth month, uint16_t day, uint16_t hour, uint16_t minute, uint16_t second,
                        uint16_t millisecond);

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
    static uint16_t maximumDayInMonthFor(uint16_t year, Date::eMonth month);

    /** Constructs a timestamp.
     *  All parts (year, month and day) are set to 0. The resulting date is infinite.
     *  @see Timestamp::isInfinite
     */
    Timestamp() = default;

    /** Constructs a timestamp with the given parameters.
     *  If the parameters give an invalid date, a DateException will be thrown.
     *  @param year The year of the date (0000-9999).
     *  @param month The month of the date (1-12).
     *  @param day The day of the date (1-31, depending on month).
     *  @param hour The hour of the time (0-23).
     *  @param minute The minute of the time (0-59).
     *  @param second The second of the time (0-59).
     *  @param millisecond The millisecond of the time (0-999).
     */
    Timestamp(uint16_t year, Date::eMonth month, uint16_t day, uint16_t hour, uint16_t minute, uint16_t second,
              uint16_t millisecond = 0);

    /** Constructs a timestamp from an integer.
     *  If the given values do not give a valid time, a TimestampException will be
     *  thrown.
     *  @param time The integer value to set the timestamp to
     */
    explicit Timestamp(int64_t time);

    /** Constructs a timestamp from a Date.
     *  If the given values do not give a valid time, a TimestampException will be
     *  thrown.
     *  @param date The Date to set the timestamp to
     */
    explicit Timestamp(const Date& date);

    /** Constructs a timestamp from a Time.
     *  If the given values do not give a valid time, a TimestampException will be
     *  thrown.
     *  @param time The Time to set the timestamp to
     */
    explicit Timestamp(const Time& time);

    Timestamp(const Timestamp&) = default;
    Timestamp& operator=(const Timestamp&) = default;
    Timestamp(Timestamp&&) = default;
    Timestamp& operator=(Timestamp&&) = default;

    bool operator==(const Timestamp& timestamp) const;
    bool operator!=(const Timestamp& timestamp) const;
    bool operator>(const Timestamp& timestamp) const;
    bool operator<(const Timestamp& timestamp) const;
    bool operator>=(const Timestamp& timestamp) const;
    bool operator<=(const Timestamp& timestamp) const;

    /** Returns the seconds of the time difference between the two timestamps.
     * @param timestamp The timestamp to subtract
     * @return The difference between the this timestamp and the given one in seconds
     */
    int64_t operator-(const Timestamp& timestamp) const;

    /** Returns true if the timestamp is not infinite
     *  @return true if the timestamp is not infinite, otherwise false
     */
    explicit operator bool() const;

    /** Extracts the day part of a timestamp.
     *  @return The day of the timestamp (1-31, depending on month).
     */
    uint16_t day() const;

    /** Extracts the month part of a timestamp.
     *  @return The month of the timestamp (1-12).
     */
    Date::eMonth month() const;

    /** Extracts the year part of a timestamp.
     *  @return The year of the timestamp (0000-9999).
     */
    uint16_t year() const;

    /** Extracts the hour of a time.
     *  @return The hour of the time (0-23).
     */
    uint16_t hour() const;

    /** Extracts the minute of a time.
     *  @return The minute of the time (0-59).
     */
    uint16_t minute() const;

    /** Extracts the second of a time.
     *  @return The second of the time (0-59).
     */
    uint16_t second() const;

    /** Extracts the millisecond of a time.
     *  @return The millisecond of the time (0-999).
     */
    uint16_t millisecond() const;

    /** Sets the day part of a timestamp.
     *  If the given day value does not give a valid date, a TimestampException
     *  will be thrown.
     *  @param day The day of the timestamp (1-31, depending on month).
     */
    void day(uint16_t day);

    /** Sets the month part of a timestamp.
     *  If the given month value does not give a valid date, a TimestampException
     *  will be thrown.
     *  @param month The month of the timestamp (1-12).
     */
    void month(Date::eMonth month);

    /** Sets the year part of a timestamp.
     *  If the given year value does not give a valid date, a TimestampException
     *  will be thrown.
     *  @param year The year of the timestamp (0000-9999).
     */
    void year(uint16_t year);

    /** Sets the hour of a timestamp.
     *  If the given hour value does not give a valid time, a TimestampException
     *  will be thrown.
     *  @param hour The hour of the timestamp (0-23).
     */
    void hour(uint16_t hour);

    /** Sets the minute of a timestamp.
     *  If the given minute value does not give a valid time, a TimestampException
     *  will be thrown.
     *  @param minute The minute of the timestamp (0-59).
     */
    void minute(uint16_t minute);

    /** Sets the second of a timestamp.
     *  If the given second value does not give a valid time, a TimestampException
     *  will be thrown.
     *  @param second The second of the timestamp (0-59).
     */
    void second(uint16_t second);

    /** Sets the millisecond of a timestamp.
     *  If the given millisecond value does not give a valid time, a TimestampException
     *  will be thrown.
     *  @param millisecond The millisecond of the timestamp (0-999).
     */
    void millisecond(uint16_t millisecond);

    /** Sets all seven parts of a timestamp.
     *  If the given values do not give a valid date, a TimestampException will be
     *  thrown.
     *  @param year The year of the timestamp (0000-9999).
     *  @param month The month of the timestamp (1-12).
     *  @param day The day of the timestamp (1-31, depending on month).
     *  @param hour The hour of the time (0-23).
     *  @param minute The minute of the time (0-59).
     *  @param second The second of the time (0-59).
     *  @param millisecond The millisecond of the time (0-999).
     */
    void ymdhmsm(uint16_t year, Date::eMonth month, uint16_t day, uint16_t hour, uint16_t minute, uint16_t second,
                 uint16_t millisecond);

    /** Determines if the year of the timestamp is a leap year (february has 29 days).
     *  @return If the year is a leap year true, otherwise false.
     */
    bool isLeapYear() const;

    /** Determines if the timestamp is infinite.
     *  @return If the is infinite true, otherwise false.
     */
    bool isInfinite() const;

    /** Determines the day of the week of the timestamp.
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

    /** Adds a Duration to the timestamp.
     *  @param duration Duration to add
     */
    void addDuration(const Duration& duration);

    /** Returns a string representation of the timestamp according to the given formatting rules.
     *  Currently the following rules are supported:
     *  - Y The year as four digit number
     *  - M The month as two digit number
     *  - d The day as two digit number
     *  - F ISO representation as YYYY-mm-dd
     *  - j day of year as three digit number
     *  - U week of year as two digit number
     *  - w day of the week as number
     *  - H hour (00-23)
     *  - I hour (01-12)
     *  - p am/pm
     *  - M minute (00-59)
     *  - S second (00-59)
     *  - s millisecond (000-999)
     *  - % %
     *  @param format The formatting rule
     *  @return A string representation according to the given formatting rule
     */
    std::string format(const std::string& format) const;

    /** Retrieves the internal integer representation of this timestamp.
     *  @return The internal integer representation
     */
    int64_t asInteger() const
    {
      return _time;
    }

  private:
    static int64_t calcJulDay(uint16_t year, uint16_t month, uint16_t day, uint16_t hour, uint16_t minute, uint16_t second,
                              uint16_t millisecond);
    static void calcFromJulDay(int64_t time, uint16_t& year, uint16_t& month, uint16_t& day, uint16_t& hour, uint16_t& minute,
                               uint16_t& second, uint16_t& millisecond);
    int64_t _time{0};
  } __attribute__((__packed__));
}


#endif
