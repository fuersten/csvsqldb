//
//  time.h
//  csvsqldb
//
//  BSD 3-Clause License
//  Copyright (c) 2015, Lars-Christian Fürstenberg
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

#ifndef csvsqldb_time_h
#define csvsqldb_time_h

#include "libcsvsqldb/inc.h"

#include "duration.h"
#include "exception.h"


namespace csvsqldb
{

/** This class represents a specific daytime. The time is represented by
 *  the hour, minute and second. The hour is represented on a 24 hour per day
 *  base.
 */
class CSVSQLDB_EXPORT Time
{
public:
    /** Gets the current systemtime.
     *  @return Current systemtime.
     */
    static Time now();
    
    /** Sets the time to the current system time.
     */
    void Now();
    
    /** Validates a given hour, minute and second.
     *  @return If the triple is a valid time, true is given back, 
     *          otherwise false.
     *  @param hour The hour to be validated (0-23).
     *  @param minute The minute to be validated (0-59).
     *  @param second The second to be validated (0-59).
     *  @param millisecond The millisecond to be validated (0-999).
     */
    static bool isValid(uint16_t hour, uint16_t minute, uint16_t second, uint16_t millisecond);

    /** Constructs a time.
     *  All parts (hour, minute, second, millisecond) are set to 0.
     */
    Time();
    
    /** Constructs a time from an integer.
     *  If the given values do not give a valid time, a TimeException will be
     *  thrown.
     *  @param time The integer value to set the time to
     */
    explicit Time(int32_t time);
    
    /** Constructs a time with the given parameters.
     *  If the parameters give an invalid time, a TimeException will be thrown.
     *  @param hour The hour of the time (0-23).
     *  @param minute The minute of the time (0-59).
     *  @param second The optional second of the time (0-59).
     *  @param millisecond The optional millisecond of the time (0-999).
     */
    Time(uint16_t hour, uint16_t minute, uint16_t second=0, uint16_t millisecond=0);
    
    /** Constructs a time with the given time_t structure.
     *  If the time_t structure does not give a valid time, a TimeException 
     *  will be thrown.
     *  @param timeT The time_t value of a C-library function like time().
     */
    explicit Time(time_t timeT);
    Time(const Time& time);
    virtual ~Time();

    Time& operator=(const Time& time);

    bool operator==(const Time& time) const;
    bool operator!=(const Time& time) const;
    bool operator>(const Time& time) const;
    bool operator<(const Time& time) const;
    bool operator>=(const Time& time) const;
    bool operator<=(const Time& time) const;
    
    /** Returns the milliseconds of the time difference between the two times.
     * @param time The Time to subtract
     * @return The difference between the this time and the given one in milliseconds
     */
    int32_t operator-(const Time& time) const;

    explicit operator bool() const;
    
    /** Sets the hour of a time. 
     *  If the given hour value does not give a valid time, a TimeException 
     *  will be thrown.
     *  @param hour The hour of the time (0-23).
     */
    void hour(uint16_t hour);
    
    /** Sets the minute of a time. 
     *  If the given minute value does not give a valid time, a TimeException 
     *  will be thrown.
     *  @param minute The minute of the time (0-59).
     */
    void minute(uint16_t minute);
    
    /** Sets the second of a time.
     *  If the given second value does not give a valid time, a TimeException 
     *  will be thrown.
     *  @param second The second of the time (0-59).
     */
    void second(uint16_t second);
    
    /** Sets the millisecond of a time.
     *  If the given millisecond value does not give a valid time, a TimeException
     *  will be thrown.
     *  @param millisecond The millisecond of the time (0-999).
     */
    void millisecond(uint16_t millisecond);
    
    /** Sets all three parts of a time.
     *  If the given values do not give a valid time, a TimeException will be 
     *  thrown.
     *  @param hour The hour of the time (0-23).
     *  @param minute The minute of the time (0-59).
     *  @param second The second of the time (0-59).
     *  @param millisecond The millisecond of the time (0-999).
     */
    void hmsm(uint16_t hour, uint16_t minute, uint16_t second, uint16_t millisecond=0);

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

    /** Adds the given hours to the time.
     *  The hours to add can be either positive or negative. The return value 
     *  reflects the days passed by the addition.<br>
     *  Example:<br>
        <pre>
        csvsqldb::Time t(18,50,45); // 18:50:45
        int32_t days = t.addHours(6);
        assert(days == 1);
        </pre>
     *  @return The elapsed days.
     */
    int32_t addHours(int32_t hours);
    
    /** Adds the given minutes to the time.
     *  The minutes to add can be either positive or negative. The return value
     *  reflects the days passed by the addition.<br>
     *  Example:<br>
        <pre>
        csvsqldb::Time t(23,50,45); // 23:50:45
        int32_t days = t.addMinutes(10);
        assert(days == 1);
        </pre>
     *  @return The elapsed days.
     */
    int32_t addMinutes(int32_t minutes);
    
    /** Adds the given seconds to the time.
     *  The seconds to add can be either positive or negative. The return value
     *  reflects the days passed by the addition.<br>
     *  Example:<br>
        <pre>
        csvsqldb::Time t(23,59,45); // 23:59:45
        int32_t days = t.addSeconds(20);
        assert(days == 1);
        </pre>
     *  @return The elapsed days.
     */
    int32_t addSeconds(int32_t seconds);
    
    /** Adds the given duration to the time.
     *  Only the hour, minute and second parts of the duration are used. All
     *  other parts are ignored.
     *  @param duration The duration to be added.
     *  @return The elapsed days.
     *  @see Duration
     */
    int32_t addDuration(const Duration& duration);

    /** Converts a time into a string representation using the given 
     *  format instruction.
     *  The format instruction is like the well known printf format. Using the
     *  following characters, an individual representation can be accomplished:<br>
     *  - H hour (00-23)
     *  - I hour (01-12)
     *  - p am/pm
     *  - M minute (00-59)
     *  - S second (00-59)
     *  - s millisecond (000-999)
     *  - X ISO representation as HH:MM:SS
     *  - % %
     *  @param format Format string.
     *  @return Formated string.
     */
    std::string format(const std::string& format) const;

    /** Retrieves the internal integer representation of this time.
     *  @return The internal integer representation
     */
    int32_t asInteger() const { return _time; }
    
    static void calcTimeFromNumber(int32_t time, uint16_t& hour, uint16_t& minute, uint16_t& second, uint16_t& millisecond);
    static int32_t calcNumberFromTime(uint16_t hour, uint16_t minute, uint16_t second, uint16_t millisecond);

private:
    int32_t _time;
} __attribute__((__packed__));

}

#endif
