//
//  date.cpp
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

#include "duration.h"


namespace csvsqldb
{

    Duration::Duration()
    : _years(0)
    , _months(0)
    , _days(0)
    , _hours(0)
    , _minutes(0)
    , _seconds(0)
    , _sign(Duration::Positive)
    {
    }

    Duration::Duration(uint16_t years, uint16_t months, uint16_t days, Duration::enSign sign)
    : _years(years)
    , _months(months)
    , _days(days)
    , _hours(0)
    , _minutes(0)
    , _seconds(0)
    , _sign(sign)
    {
    }

    Duration::Duration(uint16_t years, uint16_t months, uint16_t days, uint16_t hours, uint16_t minutes, uint16_t seconds, Duration::enSign sign)
    : _years(years)
    , _months(months)
    , _days(days)
    , _hours(hours)
    , _minutes(minutes)
    , _seconds(seconds)
    , _sign(sign)
    {
    }

    Duration::Duration(const Duration& duration)
    : _years(duration._years)
    , _months(duration._months)
    , _days(duration._days)
    , _hours(duration._hours)
    , _minutes(duration._minutes)
    , _seconds(duration._seconds)
    , _sign(duration._sign)
    {
    }

    Duration& Duration::operator=(const Duration& duration)
    {
        _years = duration._years;
        _months = duration._months;
        _days = duration._days;
        _hours = duration._hours;
        _minutes = duration._minutes;
        _seconds = duration._seconds;
        _sign = duration._sign;

        return *this;
    }

    bool Duration::operator==(const Duration& duration) const
    {
        return (_years == duration._years && _months == duration._months && _days == duration._days && _hours == duration._hours
                && _minutes == duration._minutes
                && _seconds == duration._seconds
                && _sign == duration._sign);
    }

    bool Duration::operator!=(const Duration& duration) const
    {
        return (_years != duration._years || _months != duration._months || _days != duration._days || _hours != duration._hours
                || _minutes != duration._minutes
                || _seconds != duration._seconds
                || _sign != duration._sign);
    }

    bool Duration::operator>(const Duration& duration) const
    {
        if(_sign < duration._sign)
            return true;
        else if(_years > duration._years)
            return true;
        else if(_years < duration._years)
            return false;
        if(_months > duration._months)
            return true;
        else if(_months < duration._months)
            return false;
        if(_days > duration._days)
            return true;
        else if(_days < duration._days)
            return false;
        if(_hours > duration._hours)
            return true;
        else if(_hours < duration._hours)
            return false;
        if(_minutes > duration._minutes)
            return true;
        else if(_minutes < duration._minutes)
            return false;
        if(_seconds > duration._seconds)
            return true;
        else if(_seconds < duration._seconds)
            return false;

        return false;
    }

    bool Duration::operator<(const Duration& duration) const
    {
        if(_sign > duration._sign)
            return true;
        else if(_years < duration._years)
            return true;
        else if(_years > duration._years)
            return false;
        if(_months < duration._months)
            return true;
        else if(_months > duration._months)
            return false;
        if(_days < duration._days)
            return true;
        else if(_days > duration._days)
            return false;
        if(_hours < duration._hours)
            return true;
        else if(_hours > duration._hours)
            return false;
        if(_minutes < duration._minutes)
            return true;
        else if(_minutes > duration._minutes)
            return false;
        if(_seconds < duration._seconds)
            return true;
        else if(_seconds > duration._seconds)
            return false;

        return false;
    }

    void Duration::ymdhmss(uint16_t years, uint16_t months, uint16_t days, uint16_t hours, uint16_t minutes, uint16_t seconds, Duration::enSign sign)
    {
        _years = years;
        _months = months;
        _days = days;
        _hours = hours;
        _minutes = minutes;
        _seconds = seconds;
        _sign = sign;
    }

    void Duration::years(uint16_t years)
    {
        _years = years;
    }

    void Duration::months(uint16_t months)
    {
        _months = months;
    }

    void Duration::days(uint16_t days)
    {
        _days = days;
    }

    void Duration::hours(uint16_t hours)
    {
        _hours = hours;
    }

    void Duration::minutes(uint16_t minutes)
    {
        _minutes = minutes;
    }

    void Duration::seconds(uint16_t seconds)
    {
        _seconds = seconds;
    }

    void Duration::sign(Duration::enSign sign)
    {
        _sign = sign;
    }

    uint16_t Duration::years() const
    {
        return _years;
    }

    uint16_t Duration::months() const
    {
        return _months;
    }

    uint16_t Duration::days() const
    {
        return _days;
    }

    uint16_t Duration::hours() const
    {
        return _hours;
    }

    uint16_t Duration::minutes() const
    {
        return _minutes;
    }

    uint16_t Duration::seconds() const
    {
        return _seconds;
    }

    Duration::enSign Duration::sign() const
    {
        return _sign;
    }

    int16_t Duration::yearsWithSign() const
    {
        return static_cast<int16_t>(_sign == Duration::Positive ? _years : -_years);
    }

    int16_t Duration::monthsWithSign() const
    {
        return static_cast<int16_t>(_sign == Duration::Positive ? _months : -_months);
    }

    int16_t Duration::daysWithSign() const
    {
        return static_cast<int16_t>(_sign == Duration::Positive ? _days : -_days);
    }

    int16_t Duration::hoursWithSign() const
    {
        return static_cast<int16_t>(_sign == Duration::Positive ? _hours : -_hours);
    }

    int16_t Duration::minutesWithSign() const
    {
        return static_cast<int16_t>(_sign == Duration::Positive ? _minutes : -_minutes);
    }

    int16_t Duration::secondsWithSign() const
    {
        return static_cast<int16_t>(_sign == Duration::Positive ? _seconds : -_seconds);
    }

    Duration& Duration::add(const Duration& duration)
    {
        if(_sign != duration._sign) {
            CSVSQLDB_THROW(DurationException, "The addition of durations with different signs is not allowed");
        }

        _years += duration._years;
        _months += duration._months;
        _days += duration._days;
        _hours += duration._hours;
        _minutes += duration._minutes;
        _seconds += duration._seconds;

        return *this;
    }

    std::string Duration::format() const
    {
        std::stringstream ss;
        ss << "P";
        if(_years) {
            ss << _years << "Y";
        }
        if(_months) {
            ss << _months << "M";
        }
        if(_days) {
            ss << _days << "D";
        }
        if(_hours + _minutes + _seconds) {
            ss << "T";
        }
        if(_hours) {
            ss << _hours << "H";
        }
        if(_minutes) {
            ss << _minutes << "M";
        }
        if(_seconds) {
            ss << _seconds << "S";
        }

        return ss.str();
    }
}
