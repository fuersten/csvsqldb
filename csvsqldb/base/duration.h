//
//  duration.h
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

#pragma once

#include "csvsqldb/inc.h"

#include "exception.h"


namespace csvsqldb
{
  /** This class represents a duration for time calculations following the ISO8601 standard.
   */
  class CSVSQLDB_EXPORT Duration
  {
  public:
    enum enSign { Positive = 1, Negative = 2 };

    Duration();
    Duration(uint16_t years, uint16_t months, uint16_t days, Duration::enSign sign = Positive);
    Duration(uint16_t years, uint16_t months, uint16_t days, uint16_t hours, uint16_t minutes, uint16_t seconds,
             Duration::enSign sign = Positive);

    Duration(const Duration&) = default;
    Duration& operator=(const Duration&) = default;
    Duration(Duration&&) = default;
    Duration& operator=(Duration&&) = default;

    bool operator==(const Duration& duration) const;
    bool operator!=(const Duration& duration) const;
    bool operator>(const Duration& duration) const;
    bool operator<(const Duration& duration) const;

    void ymdhmss(uint16_t years, uint16_t months, uint16_t days, uint16_t hours, uint16_t minutes, uint16_t seconds,
                 Duration::enSign sign);
    void years(uint16_t years);
    void months(uint16_t months);
    void days(uint16_t days);
    void hours(uint16_t hours);
    void minutes(uint16_t minutes);
    void seconds(uint16_t seconds);
    void sign(Duration::enSign sign);

    uint16_t years() const;
    uint16_t months() const;
    uint16_t days() const;
    uint16_t hours() const;
    uint16_t minutes() const;
    uint16_t seconds() const;
    Duration::enSign sign() const;

    int16_t yearsWithSign() const;
    int16_t monthsWithSign() const;
    int16_t daysWithSign() const;
    int16_t hoursWithSign() const;
    int16_t minutesWithSign() const;
    int16_t secondsWithSign() const;

    /** Adds two Duration objects.
     *  This method throws a DurationException if two Duration objects with different
     *  signs shall be added.
     *  @attention The addition of durations with different signs is not allowed.
     *  @param duration The duration to add to this duration
     *  @return The modified duration object
     */
    Duration& add(const Duration& duration);

    /** Formats the duration as specified by ISO8601.
     *  @return The string representation of the duration
     */
    std::string format() const;

  protected:
    uint16_t _years;
    uint16_t _months;
    uint16_t _days;
    uint16_t _hours;
    uint16_t _minutes;
    uint16_t _seconds;
    Duration::enSign _sign;
  };
}
