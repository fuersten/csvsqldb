/***************************************************************************
 *                                                                         *
 *   Copyright (c) 2004-2007 by Athelas GmbH. All Rights Reserved.         *
 *                                                                         *
 *   The contents of this file are subject to the Mozilla Public License   *
 *   Version 1.1 (the "License"); you may not use this file except in      *
 *   compliance with the License. You may obtain a copy of the License at  *
 *   http://www.mozilla.org/MPL/                                           *
 *                                                                         *
 *   Software distributed under the License is distributed on an "AS IS"   *
 *   basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See   *
 *   the License for the specificlanguage governing rights and             *
 *   limitations under the License.                                        *
 *                                                                         *
 *   The Original Code is mpf.                                             *
 *                                                                         *
 *   The Initial Developer of the Original Code is Lars-Christian          *
 *   Fuerstenberg, with contributions from Steffen Schuemann. Portions     *
 *   created by Lars-Christian Fuerstenberg are Copyright (c) 2002-2003    *
 *   All Rights Reserved.                                                  *
 *                                                                         *
 ***************************************************************************/

#ifndef MPF_DATETIME_H
#define MPF_DATETIME_H

#include <mpfcore/date.h>
#include <mpfcore/duration.h>
#include <mpfcore/inc.h>
#include <mpfcore/time.h>
#include <mpfcore/timezone.h>


namespace mpf
{
  /** This class represents a specific datetime. The date part is represented by
   *  the year, month and day. The date is based on the gregorian calendar.
   *  The time part is represented by the hour, minute and second. The datetime
   *  is represented as a localtime, which dependends on the given timezone. If no
   *  timezone is given, the default timezone will be set.
   *  Beware that most of the methods will throw either a DateException or a
   *  TimeException,if illegal or infinite dates/times are used.
   */
  class MPFCORE_EXPORTS DateTime
  {
  public:
    enum TimeScope { LOCAL, UTC };

    DateTime();

    /** Constructs a datetime with the given date and time parts. If the TimeScope
     *  is LOCAL, the date and time will be set as given. If the TimeScope is
     *  UTC, the date and time parts will be adjusted to the default time zone.
     *  @param date The date part to set
     *  @param time The time part to set
     *  @param ts The time scope to use, either LOCAL or UTC
     */
    DateTime(const Date& date, const Time& time, TimeScope ts = LOCAL);

    /** Constructs a datetime with the given date and time parts and timezone.
     *  @param date The date part to set
     *  @param time The time part to set
     *  @param timeZone The timezone to use
     */
    DateTime(const Date& date, const Time& time, const TimeZone& timeZone);

    /** Constructs a datetime with the given parts. If the TimeScope
     *  is LOCAL, the date and time will be set as given. If the TimeScope is
     *  UTC, the date and time parts will be adjusted to the default time zone.
     *  If the parameters give an invalid date, a DateException will be thrown.
     *  @param year The year of the date (1582-9999).
     *  @param month The month of the date (1-12).
     *  @param day The day of the date (1-31, depending on month).
     *  @param hour The hour of the time (0-23).
     *  @param minute The minute of the time (0-59).
     *  @param second The optional second of the time (0-59).
     *  @param ts The time scope to use, either LOCAL or UTC
     */
    DateTime(UInt16 year, UInt16 month, UInt16 day, UInt16 hour, UInt16 minute, UInt16 second = 0, TimeScope ts = LOCAL);

    /** Constructs a datetime with the given parts and timezone.
     *  If the parameters give an invalid date, a DateException will be thrown.
     *  @param year The year of the date (1582-9999).
     *  @param month The month of the date (1-12).
     *  @param day The day of the date (1-31, depending on month).
     *  @param hour The hour of the time (0-23).
     *  @param minute The minute of the time (0-59).
     *  @param second The optional second of the time (0-59).
     *  @param timeZone The timezone to use
     */
    DateTime(UInt16 year, UInt16 month, UInt16 day, UInt16 hour, UInt16 minute, UInt16 second, const TimeZone& timeZone);

    DateTime(const DateTime& datetime);

    virtual ~DateTime();

    /** Returns the current system date and time using the default timezone.
     *  @return The current system date date and time.
     */
    static DateTime now();

    /** Sets this datetime to the current systems date and time.
     *
     */
    void Now();

    /** Returns the date part.
     *  @return The date part.
     */
    Date date() const;

    /** Returns the time part.
     *  @return The time part.
     */
    Time time() const;

    /** Extracts the day part of a date.
     *  @return The day of the date (1-31, depending on month).
     */
    UInt16 day() const;
    /** Extracts the month part of a date.
     *  @return The month of the date (1-12).
     */
    UInt16 month() const;
    /** Extracts the year part of a date.
     *  @return The year of the date (1582-9999).
     */
    UInt16 year() const;

    /** Sets the day part of a date.
     *  If the given day value does not give a valid date, a DateException
     *  will be thrown.
     *  @param day The day of the date (1-31, depending on month).
     */
    void day(UInt16 day);
    /** Sets the month part of a date.
     *  If the given month value does not give a valid date, a DateException
     *  will be thrown.
     *  @param month The month of the date (1-12).
     */
    void month(UInt16 month);
    /** Sets the year part of a date.
     *  If the given year value does not give a valid date, a DateException
     *  will be thrown.
     *  @param year The year of the date (1582-9999).
     */
    void year(UInt16 year);
    /** Sets all three parts of a date.
     *  If the given values do not give a valid date, a DateException will be
     *  thrown.
     *  @param year The year of the date (1582-9999).
     *  @param month The month of the date (1-12).
     *  @param day The day of the date (1-31, depending on month).
     */
    void ymd(UInt16 year, UInt16 month, UInt16 day);

    /** Sets the hour of a time.
     *  If the given hour value does not give a valid time, a TimeException
     *  will be thrown.
     *  @param hour The hour of the time (0-23).
     */
    void hour(UInt16 hour);
    /** Sets the minute of a time.
     *  If the given minute value does not give a valid time, a TimeException
     *  will be thrown.
     *  @param minute The minute of the time (0-59).
     */
    void minute(UInt16 minute);
    /** Sets the second of a time.
     *  If the given second value does not give a valid time, a TimeException
     *  will be thrown.
     *  @param second The second of the time (0-59).
     */
    void second(UInt16 second);
    /** Sets all three parts of a time.
     *  If the given values do not give a valid time, a TimeException will be
     *  thrown.
     *  @param hour The hour of the time (0-23).
     *  @param minute The minute of the time (0-59).
     *  @param second The second of the time (0-59).
     */
    void hms(UInt16 hour, UInt16 minute, UInt16 second);

    /** Extracts the hour of a time.
     *  @return The hour of the time (0-23).
     */
    UInt16 hour() const;
    /** Extracts the minute of a time.
     *  @return The minute of the time (0-59).
     */
    UInt16 minute() const;
    /** Extracts the second of a time.
     *  @return The second of the time (0-59).
     */
    UInt16 second() const;

    /** Returns the datetime as utc representation. The date and
     *  time parts of the returned datetime will be modified
     *  according to the timezone.
     *  @return The datetime as utc representation.
     */
    DateTime utc() const;

    /** Returns true if a daylight saving time rule is active.
     *  @return true if a daylight saving time rule is active,
     *          otherwise false
     */
    bool isDST() const;

    /** Adds the given hours to the datetime.
     *  The hours to add can be either positive or negative. The return value
     *  reflects the days passed by the addition.<br>
     *  Example:<br>
     <pre>
     mpf::DateTime dt(23,9,1970,18,50,45); // 1970-09-23 18:50:45
     dt.addHours(6);
     assert(dt.hour() == 0);
     assert(dt.day() == 24);
     </pre>
     *  @param hours The hours to add.
     */
    void addHours(Int32 hours);

    /** Adds the given minutes to the datetime.
     *  The minutes to add can be either positive or negative. The return value
     *  reflects the days passed by the addition.<br>
     *  Example:<br>
     <pre>
     mpf::DateTime dt(23,9,1970,23,50,45); // 1970-09-23 23:50:45
     dt.addMinutes(10);
     assert(dt.hour() == 0);
     assert(dt.minute() == 0);
     assert(dt.day() == 24);
     </pre>
     *  @return The elapsed days.
     */
    void addMinutes(Int32 minutes);

    /** Adds the given seconds to the datetime.
     *  The seconds to add can be either positive or negative. The return value
     *  reflects the days passed by the addition.<br>
     *  Example:<br>
     <pre>
     mpf::DateTime dt(23,9,1970,23,59,45); // 1970-09-23 23:59:45
     dt.addSeconds(10);
     assert(dt.hour() == 0);
     assert(dt.minute() == 0);
     assert(dt.day() == 24);
     </pre>
     *  @return The elapsed days.
     */
    void addSeconds(Int32 seconds);

    /** Adds the given duration to the datetime.
     *  @param duration The duration to be added.
     *  @return The modified datetime.
     *  @see Duration
     */
    DateTime& addDuration(const Duration& duration);

    /** Determines if the year of the datetime is a leap year (february has 29 days).
     *  @return If the year is a leap year true, otherwise false.
     */
    bool isLeapYear() const;

    /** Determines if the datetime is infinite.
     *  @return If the is infinite true, otherwise false.
     */
    bool isInfinite() const;

    /** Determines the day of the week of the datetime.
     *  @return The day of the week (Saturday-Sunday).
     *  @see Date::enDay
     */
    Date::enDay dayOfWeek() const;

    /** Determines the day of the year.
     *  @return The day of the year (1-366).
     */
    UInt16 dayOfYear() const;

    /** Determines the week of the year.
     *  @return The week of the year (1-53).
     */
    UInt16 weekOfYear() const;

    /** Adds the given days to the datetime. Negative days are subtracted.
     *  @param days The days to be added/subtracted.
     *  @return The modified date.
     */
    void addDays(Int32 days);

    DateTime& operator=(const DateTime& o);
    bool operator==(const DateTime& o) const;
    bool operator!=(const DateTime& o) const;
    bool operator>(const DateTime& o) const;
    bool operator<(const DateTime& o) const;
    bool operator>=(const DateTime& o) const;
    bool operator<=(const DateTime& o) const;

    /** Converts a datetime into a string representation using the given
     *  format instruction.
     *  The format instruction is like the well known printf format. Using the
     *  following characters, an individual representation can be accomplished:<br>
     *  <table>
     *  <tr><td><b>%%a</b></td><td>Abbreviated weekday name</td></tr>
     *  <tr><td><b>%%A</b></td><td>Full weekday name</td></tr>
     *  <tr><td><b>%%b</b></td><td>Abbreviated month name</td></tr>
     *  <tr><td><b>%%B</b></td><td>Full month name</td></tr>
     *  <tr><td><b>%%d</b></td><td>Day of month as decimal number (01-31)</td></tr>
     *  <tr><td><b>%%j</b></td><td>Day of year as decimal number (001-366)</td></tr>
     *  <tr><td><b>%%m</b></td><td>Month as decimal number (01-12)</td></tr>
     *  <tr><td><b>%%U</b></td><td>Week of year as decimal number, with Sunday as first day of week (01-53)</td></tr>
     *  <tr><td><b>%%w</b></td><td>Weekday as decimal number (0-6; Sunday is 0)</td></tr>
     *  <tr><td><b>%%y</b></td><td>Year without century, as decimal number (00-99)</td></tr>
     *  <tr><td><b>%%Y</b></td><td>Year with century, as decimal number (1582-9999)</td></tr>
     *  <tr><td><b>%%H</b></td><td>hour (00-23)</td></tr>
     *  <tr><td><b>%%I</b></td><td>hour (01-12)</td></tr>
     *  <tr><td><b>%%p</b></td><td>am/pm</td></tr>
     *  <tr><td><b>%%M</b></td><td>minute (00-59)</td></tr>
     *  <tr><td><b>%%S</b></td><td>second (00-59)</td></tr>
     *  <tr><td><b>%%</b></td><td>%</td></tr>
     *  </table>
     *  @param format Format string.
     *  @return Formated string.
     */
    std::string format(const std::string& format) const;

  protected:
    Date _date;
    Time _time;
    TimeZone _timeZone;
  };

}

#endif  // MPF_DateTime_H
