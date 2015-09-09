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

#include <mpfcore/datetime.h>

using namespace mpf;


DateTime::DateTime()
{
}

DateTime::DateTime(const Date& date, const Time& time, const TimeZone& timeZone)
: _timeZone(timeZone)
{
    Time mod;
    Date d(date);

    d.addDays(_timeZone.calcModifiedTime(date, time, mod, false));

    _date = d;
    _time = mod;
}

DateTime::DateTime(const Date& date, const Time& time, TimeScope ts)
: _timeZone(TimeZone::defaultTimeZone())
{
    if(ts == LOCAL)
    {
        Time mod;
        Date d(date);

        d.addDays(_timeZone.calcModifiedTime(date, time, mod, false));

        _date = d;
        _time = mod;
    }
    else
    {
        _date = date;
        _time = time;
    }
}

DateTime::DateTime(UInt16 year, UInt16 month, UInt16 day, 
            UInt16 hour, UInt16 minute, UInt16 second, TimeScope ts)
: _timeZone(TimeZone::defaultTimeZone())
{
    if(ts == LOCAL)
    {
        Time mod;
        Date d(year, month, day);

        d.addDays(_timeZone.calcModifiedTime(Date(year, month, day), Time(hour, minute, second), mod, false));

        _date = d;
        _time = mod;
    }
    else
    {
        _date = Date(year, month, day);
        _time = Time(hour, minute, second);
    }
}

DateTime::DateTime(UInt16 year, UInt16 month, UInt16 day, 
         UInt16 hour, UInt16 minute, UInt16 second, const TimeZone& timeZone)
: _timeZone(timeZone)
{
    Time mod;
    Date d(year, month, day);

    d.addDays(_timeZone.calcModifiedTime(Date(year, month, day), Time(hour, minute, second), mod, false));

    _date = d;
    _time = mod;
}

DateTime::DateTime(const DateTime& datetime)
: _date(datetime._date)
, _time(datetime._time)
, _timeZone(datetime._timeZone)
{
}

DateTime::~DateTime()
{
}

DateTime DateTime::now()
{
    return DateTime(Date::now(), Time::now());
}

void DateTime::Now()
{
    Time mod;
    Date d(Date::now());

    d.addDays(_timeZone.calcModifiedTime(d, Time::now(), mod, false));

    _date = d;
    _time = mod;
}

Date DateTime::date() const
{
    Time mod;
    Date date(_date);
    
    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));
    return date;
}

Time DateTime::time() const
{
    Time mod;
    _timeZone.calcModifiedTime(_date, _time, mod, true);
    return mod;
}

void DateTime::day(UInt16 day)
{
    Time mod;
    Date date(_date);
    
    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));
    date.day(day);
    date.addDays(_timeZone.calcModifiedTime(date, mod, _time, false));

    _date = date;
}

void DateTime::month(UInt16 month)
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));
    date.month(month);
    date.addDays(_timeZone.calcModifiedTime(date, mod, _time, false));

    _date = date;
}

void DateTime::year(UInt16 year)
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));
    date.year(year);
    date.addDays(_timeZone.calcModifiedTime(date, mod, _time, false));

    _date = date;
}

UInt16 DateTime::day() const
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));

    return date.day();
}

UInt16 DateTime::month() const
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));

    return date.month();
}

UInt16 DateTime::year() const
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));

    return date.year();
}

void DateTime::ymd(UInt16 year, UInt16 month, UInt16 day)
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));
    date.ymd(year, month, day);
    date.addDays(_timeZone.calcModifiedTime(date, mod, _time, false));

    _date = date;
}

void DateTime::hour(UInt16 hour)
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));
    mod.hour(hour);
    _timeZone.calcModifiedTime(date, mod, _time, false);
}

void DateTime::minute(UInt16 minute)
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));
    mod.minute(minute);
    _timeZone.calcModifiedTime(date, mod, _time, false);
}

void DateTime::second(UInt16 second)
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));
    mod.second(second);
    _timeZone.calcModifiedTime(date, mod, _time, false);
}

void DateTime::hms(UInt16 hour, UInt16 minute, UInt16 second)
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));
    mod.hms(hour, minute, second);
    _timeZone.calcModifiedTime(date, mod, _time, false);
}

UInt16 DateTime::hour() const
{
    Time mod;
    _timeZone.calcModifiedTime(_date, _time, mod, true);
    return mod.hour();
}

UInt16 DateTime::minute() const
{
    Time mod;
    _timeZone.calcModifiedTime(_date, _time, mod, true);
    return mod.minute();
}

UInt16 DateTime::second() const
{
    Time mod;
    _timeZone.calcModifiedTime(_date, _time, mod, true);
    return mod.second();
}

DateTime DateTime::utc() const
{
    return DateTime(_date, _time, TimeZone(0, "UTC"));
}

bool DateTime::isDST() const
{
    DSTRuleElement::Ptr rule = _timeZone.dstRule()->activeRule(_date);
    return _timeZone.dstRule()->isDST(rule, _date, _time);
}

void DateTime::addHours(Int32 hours)
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));
    date.addDays(mod.addHours(hours));
    date.addDays(_timeZone.calcModifiedTime(date, mod, _time, false));

    _date = date;
}

void DateTime::addMinutes(Int32 minutes)
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));
    date.addDays(mod.addMinutes(minutes));
    date.addDays(_timeZone.calcModifiedTime(date, mod, _time, false));

    _date = date;
}

void DateTime::addSeconds(Int32 seconds)
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));
    date.addDays(mod.addSeconds(seconds));
    date.addDays(_timeZone.calcModifiedTime(date, mod, _time, false));

    _date = date;
}

DateTime& DateTime::addDuration(const Duration& duration)
{
    MPF_THROW_IF(isInfinite(), DateException, ("Infinite date"));
    Time mod;
    Date date(_date);
    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));
    UInt16 year = date.year();
    UInt16 month = date.month();
    UInt16 day = date.day();
    UInt16 hour = mod.hour();
    UInt16 minute = mod.minute();
    UInt16 second = mod.second();

    Date::addDuration(year, month, day, hour, minute, second, duration);
    
    ymd(year, month, day);
    hms(hour, minute, second);

    return *this;
}

bool DateTime::isLeapYear() const
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));

    return date.isLeapYear();
}

bool DateTime::isInfinite() const
{
    return _date.isInfinite();
}

Date::enDay DateTime::dayOfWeek() const
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));

    return date.dayOfWeek();
}

UInt16 DateTime::dayOfYear() const
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));

    return date.dayOfYear();
}

UInt16 DateTime::weekOfYear() const
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));

    return date.weekOfYear();
}

void DateTime::addDays(Int32 days)
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));
    date.addDays(days);
    date.addDays(_timeZone.calcModifiedTime(date, mod, _time, false));

    _date = date;
}

DateTime& DateTime::operator=(const DateTime& o)
{
    _date = o._date;
    _time = o._time;
    _timeZone = o._timeZone;

    return *this;
}

bool DateTime::operator==(const DateTime& o) const
{
    return (_date == o._date && _time == o._time);
}

bool DateTime::operator!=(const DateTime& o) const
{
    return (_date != o._date || _time != o._time);
}

bool DateTime::operator>(const DateTime& o) const
{
    return (_date > o._date || (_date == o._date && _time > o._time));
}

bool DateTime::operator<(const DateTime& o) const
{
    return (_date < o._date || (_date == o._date && _time < o._time));
}

bool DateTime::operator>=(const DateTime& o) const
{
    return (_date > o._date || (_date == o._date && _time >= o._time));
}

bool DateTime::operator<=(const DateTime& o) const
{
    return (_date < o._date || (_date == o._date && _time <= o._time));
}

std::string DateTime::format(const std::string& sformat) const
{
    Time mod;
    Date date(_date);

    date.addDays(_timeZone.calcModifiedTime(date, _time, mod, true));

    std::string result;
    std::string::const_iterator iter = sformat.begin();

    while(iter != sformat.end())
    {
        std::string::value_type c = *iter;
        if(c == '%')
        {
            ++iter;
            MPF_THROW_IF(iter == sformat.end(), DateException, ("Bad format specified: %s") % sformat);

            c = *iter;
            MPF_THROW_IF(!date.format(c, result) && !mod.format(c, result), DateException, ("Bad format specified: %s") % sformat);
        }
        else
        {
            result += c;
        }

        ++iter;
    }

    return result;
}

