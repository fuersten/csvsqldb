//
//  time_helper.cpp
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

#include "time_helper.h"

#include "compat/get_time.h"
#include "compat/put_time.h"

#include <iomanip>
#include <sstream>


namespace csvsqldb
{
    
    Timepoint now()
    {
        return std::chrono::system_clock::now();
    }
    
    std::string timestampToGMTString(const Timepoint& timestamp)
    {
        const time_t time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        struct tm ptime;
        gmtime_r(&time, &ptime);
        ss << put_time(&ptime, "%Y-%m-%dT%H:%M:%S");
        return ss.str();
    }
    
    std::string timestampToLocalString(const Timepoint& timestamp)
    {
        const time_t time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        struct tm ptime;
        localtime_r(&time, &ptime);
        ss << put_time(&ptime, "%Y-%m-%dT%H:%M:%S");
        return ss.str();
    }
    
    Timepoint stringToTimestamp(const std::string& time)
    {
        std::tm t;
        std::istringstream ss(time);
        ss >> get_time(&t, "%Y-%m-%dT%H:%M:%S");
#ifndef _MSC_VER // TODO LCF: find a valid workaround for Windows (maybe boost something?)
        time_t tt = timegm(&t);
        localtime_r(&tt, &t);
#endif
        return std::chrono::system_clock::from_time_t(std::mktime(&t));
    }
    
    Date dateFromString(const std::string& date)
    {
        uint16_t year = static_cast<uint16_t>(std::stoi(date.substr(0,4)));
        Date::eMonth month = static_cast<Date::eMonth>(std::stoi(date.substr(5,2)));
        uint16_t day = static_cast<uint16_t>(std::stoi(date.substr(8,2)));
        return Date(year, month, day);
    }

    Time timeFromString(const std::string& time)
    {
        uint16_t hour = static_cast<uint16_t>(std::stoi(time.substr(0,2)));
        uint16_t minute = static_cast<uint16_t>(std::stoi(time.substr(3,2)));
        uint16_t second = static_cast<uint16_t>(std::stoi(time.substr(6,2)));
        return Time(hour, minute, second, 0);
    }

    Timestamp timestampFromString(const std::string& timestamp)
    {
        uint16_t year = static_cast<uint16_t>(std::stoi(timestamp.substr(0,4)));
        Date::eMonth month = static_cast<Date::eMonth>(std::stoi(timestamp.substr(5,2)));
        uint16_t day = static_cast<uint16_t>(std::stoi(timestamp.substr(8,2)));
        uint16_t hour = static_cast<uint16_t>(std::stoi(timestamp.substr(11,2)));
        uint16_t minute = static_cast<uint16_t>(std::stoi(timestamp.substr(14,2)));
        uint16_t second = static_cast<uint16_t>(std::stoi(timestamp.substr(17,2)));
        return Timestamp(year, month, day, hour, minute, second, 0);
    }
}
