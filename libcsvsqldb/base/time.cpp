//
//  time.cpp
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

#include "time.h"

#include "types.h"

#include <iomanip>


namespace csvsqldb
{
#define MODULO(x,y) ((abs(x) % (y)) * (x<0?-1:1))
#define SECONDSPERDAY 86400L
#define MILLISECONDSPERDAY (SECONDSPERDAY * 1000L)
    
    
    Time Time::now()
    {
        time_t t;
        time(&t);
        struct tm today;
        localtime_r(&t, &today);
        
        return Time(static_cast<uint16_t>(today.tm_hour), static_cast<uint16_t>(today.tm_min), static_cast<uint16_t>(today.tm_sec));
    }
    
    void Time::Now()
    {
        time_t t;
        time(&t);
        struct tm today;
        localtime_r(&t, &today);
        _time = calcNumberFromTime(static_cast<uint16_t>(today.tm_hour), static_cast<uint16_t>(today.tm_min), static_cast<uint16_t>(today.tm_sec), 0);
    }
    
    bool Time::isValid(uint16_t hour, uint16_t minute, uint16_t second, uint16_t milliseconds)
    {
        return (hour <= 23 && minute <= 59 && second <= 59 && milliseconds <= 999);
    }
    
    Time::Time()
    : _time(0)
    {
    }
    
    Time::Time(int32_t time)
    : _time(time)
    {
        uint16_t hour, minute, second, millisecond;
        calcTimeFromNumber(_time, hour, minute, second, millisecond);
        if(!Time::isValid(hour, minute, second, millisecond)) {
            CSVSQLDB_THROW(TimeException, "Invalid time specified: hour " << hour << ", minute " << minute << ", second " << second << ", millisecond " << millisecond);
        }
    }
    
    Time::Time(uint16_t hour, uint16_t minute, uint16_t second, uint16_t millisecond)
    {
        if(!Time::isValid(hour, minute, second, millisecond)) {
            CSVSQLDB_THROW(TimeException, "Invalid time specified: hour " << hour << ", minute " << minute << ", second " << second << ", millisecond " << millisecond);
        }
        
        _time = calcNumberFromTime(hour, minute, second, millisecond);
    }
    
    Time::Time(time_t timeT)
    {
        struct tm ptime;
        if(!localtime_r(&timeT, &ptime)) {
            CSVSQLDB_THROW(TimeException, "Invalid time_t specified: " << timeT);
        }
        
        _time = calcNumberFromTime(static_cast<uint16_t>(ptime.tm_hour), static_cast<uint16_t>(ptime.tm_min), static_cast<uint16_t>(ptime.tm_sec), 0);
    }
    
    Time::Time(const Time& time)
    {
        _time = time._time;
    }
    
    Time::~Time()
    {
    }
    
    Time& Time::operator=(const Time& time)
    {
        _time = time._time;
        return *this;
    }
    
    bool Time::operator==(const Time& time) const
    {
        return _time == time._time;
    }
    
    bool Time::operator!=(const Time& time) const
    {
        return _time != time._time;
    }
    
    bool Time::operator>(const Time& time) const
    {
        return _time > time._time;
    }
    
    bool Time::operator<(const Time& time) const
    {
        return _time < time._time;
    }
    
    bool Time::operator>=(const Time& time) const
    {
        return _time >= time._time;
    }
    
    bool Time::operator<=(const Time& time) const
    {
        return _time <= time._time;
    }
    
    int32_t Time::operator-(const Time& time) const
    {
        return _time - time._time;
    }
    
    Time::operator bool() const
    {
        uint16_t hour, minute, second, millisecond;
        calcTimeFromNumber(_time, hour, minute, second, millisecond);
        return hour + minute + second + millisecond;
    }
    
    void Time::hour(uint16_t hour)
    {
        uint16_t thour, tminute, tsecond, tmillisecond;
        calcTimeFromNumber(_time, thour, tminute, tsecond, tmillisecond);
        
        if(!Time::isValid(hour, tminute, tsecond, tmillisecond)) {
            CSVSQLDB_THROW(TimeException, "Invalid time specified: hour " << hour << ", minute " << tminute << ", second " << tsecond << ", millisecond " << tmillisecond);
        }
        
        _time = calcNumberFromTime(hour, tminute, tsecond, tmillisecond);
    }
    
    void Time::minute(uint16_t minute)
    {
        uint16_t thour, tminute, tsecond, tmillisecond;
        calcTimeFromNumber(_time, thour, tminute, tsecond, tmillisecond);
        
        if(!Time::isValid(thour, minute, tsecond, tmillisecond)) {
            CSVSQLDB_THROW(TimeException, "Invalid time specified: hour " << thour << ", minute " << minute << ", second " << tsecond << ", millisecond " << tmillisecond);
        }
        
        _time = calcNumberFromTime(thour, minute, tsecond, tmillisecond);
    }
    
    void Time::second(uint16_t second)
    {
        uint16_t thour, tminute, tsecond, tmillisecond;
        calcTimeFromNumber(_time, thour, tminute, tsecond, tmillisecond);
        
        if(!Time::isValid(thour, tminute, second, tmillisecond)) {
            CSVSQLDB_THROW(TimeException, "Invalid time specified: hour " << thour << ", minute " << tminute << ", second " << second << ", millisecond " << tmillisecond);
        }
        
        _time = calcNumberFromTime(thour, tminute, second, tmillisecond);
    }
    
    void Time::millisecond(uint16_t millisecond)
    {
        uint16_t thour, tminute, tsecond, tmillisecond;
        calcTimeFromNumber(_time, thour, tminute, tsecond, tmillisecond);
        
        if(!Time::isValid(thour, tminute, tsecond, millisecond)) {
            CSVSQLDB_THROW(TimeException, "Invalid time specified: hour " << thour << ", minute " << tminute << ", second " << tsecond << ", millisecond " << millisecond);
        }
        
        _time = calcNumberFromTime(thour, tminute, tsecond, millisecond);
    }
    
    void Time::hmsm(uint16_t hour, uint16_t minute, uint16_t second, uint16_t millisecond)
    {
        if(!Time::isValid(hour, minute, second, millisecond)) {
            CSVSQLDB_THROW(TimeException, "Invalid time specified: hour " << hour << ", minute " << minute << ", second " << second << ", millisecond " << millisecond);
        }
        
        _time = calcNumberFromTime(hour, minute, second, millisecond);
    }
    
    uint16_t Time::hour() const
    {
        uint16_t hour, minute, second, millisecond;
        calcTimeFromNumber(_time, hour, minute, second, millisecond);
        return hour;
    }
    
    uint16_t Time::minute() const
    {
        uint16_t hour, minute, second, millisecond;
        calcTimeFromNumber(_time, hour, minute, second, millisecond);
        return minute;
    }
    
    uint16_t Time::second() const
    {
        uint16_t hour, minute, second, millisecond;
        calcTimeFromNumber(_time, hour, minute, second, millisecond);
        return second;
    }

    uint16_t Time::millisecond() const
    {
        uint16_t hour, minute, second, millisecond;
        calcTimeFromNumber(_time, hour, minute, second, millisecond);
        return millisecond;
    }

    int32_t Time::addHours(int32_t hours)
    {
        return addSeconds(hours*3600);
    }
    
    int32_t Time::addMinutes(int32_t minutes)
    {
        return addSeconds(minutes*60);
    }
    
    int32_t Time::addSeconds(int32_t seconds)
    {
        int32_t days = 0;
        
        if(seconds != 0)
        {
            _time += seconds*1000;
            if(labs(_time) >= MILLISECONDSPERDAY)
            {
                days += _time/MILLISECONDSPERDAY;
                _time = MODULO(static_cast<int>(_time),MILLISECONDSPERDAY);
            }
            if(_time < 0)
            {
                --days;
                _time = MILLISECONDSPERDAY + _time;
            }
        }
        
        return days;
    }
    
    int32_t Time::addDuration(const Duration& duration)
    {
        return addSeconds(duration.seconds()+duration.minutes()*60+duration.hours()*3600);
    }
    
    std::string Time::format(const std::string& sformat) const
    {
        std::stringstream ss;
        std::string::const_iterator iter = sformat.begin();
        
        char buf[13];
        uint16_t hour, minute, second, millisecond;
        calcTimeFromNumber(_time, hour, minute, second, millisecond);
        
        while(iter != sformat.end())
        {
            const std::string::value_type& c = *iter;
            if(c == '%')
            {
                ++iter;
                if(iter == sformat.end()) {
                    CSVSQLDB_THROW(TimeException, "Bad format specified: " << sformat);
                }
                switch(*iter)
                {
                    case 'H':
                        ss << std::setw(2) << std::setfill('0') << itoa(hour, buf);
                        break;
                    case 'I':
                        ss << std::setw(2) << std::setfill('0') << itoa(hour > 12 ? hour-12 : hour, buf);
                        break;
                    case 'p':
                        ss << (hour > 12 ? "pm" : "am");
                        break;
                    case 'S':
                        ss << std::setw(2) << std::setfill('0') << itoa(second, buf);
                        break;
                    case 's':
                        ss << std::setw(3) << std::setfill('0') << itoa(millisecond, buf);
                        break;
                    case 'M':
                        ss << std::setw(2) << std::setfill('0') << itoa(minute, buf);
                        break;
                    case 'X':
                        ss << std::setw(2) << std::setfill('0') << itoa(hour, buf);
                        ss << ":";
                        ss << std::setw(2) << std::setfill('0') << itoa(minute, buf);
                        ss << ":";
                        ss << std::setw(2) << std::setfill('0') << itoa(second, buf);
                        break;
                    case '%':
                        ss << '%';
                        break;
                    default:
                        CSVSQLDB_THROW(TimeException, "Bad format specified: " << sformat);
                }
            }
            else
            {
                ss << c;
            }
            
            ++iter;
        }
        
        return ss.str();
    }
    
    void Time::calcTimeFromNumber(int32_t time, uint16_t& hour, uint16_t& minute, uint16_t& second, uint16_t& millisecond)
    {
        if(time > MILLISECONDSPERDAY) {
            CSVSQLDB_THROW(TimeException, "Invalid timevalue: " << time);
        }
        
        int32_t secondTime = time / 1000;
        millisecond = static_cast<uint16_t>(time - secondTime*1000);
        hour = static_cast<uint8_t>(secondTime/3600);
        minute = static_cast<uint8_t>(MODULO(static_cast<int32_t>(secondTime),3600)/60);
        second = static_cast<uint8_t>(MODULO(static_cast<int32_t>(secondTime),60));
    }
    
    int32_t Time::calcNumberFromTime(uint16_t hour, uint16_t minute, uint16_t second, uint16_t millisecond)
    {
        return (hour * 3600 + minute * 60 + second) * 1000 + millisecond;
    }
    
}
