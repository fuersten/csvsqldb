//
//  buildin_functions.cpp
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

#include "buildin_functions.h"

#include "csvsqldb/version.h"

#include "base/string_helper.h"

#include <cmath>


namespace csvsqldb
{
  void initBuildInFunctions(FunctionRegistry& registry)
  {
    registry.registerFunction(std::make_shared<CurrentDateFunction>());
    registry.registerFunction(std::make_shared<CurrentTimeFunction>());
    registry.registerFunction(std::make_shared<CurrentTimestampFunction>());
    registry.registerFunction(std::make_shared<ExtractFunction>());
    registry.registerFunction(std::make_shared<DateFormatFunction>());
    registry.registerFunction(std::make_shared<TimeFormatFunction>());
    registry.registerFunction(std::make_shared<TimestampFormatFunction>());
    registry.registerFunction(std::make_shared<PowerFunction>());
    registry.registerFunction(std::make_shared<UpperFunction>());
    registry.registerFunction(std::make_shared<LowerFunction>());
    registry.registerFunction(std::make_shared<CharLengthFunction>("CHARACTER_LENGTH"));
    registry.registerFunction(std::make_shared<CharLengthFunction>("CHAR_LENGTH"));
    registry.registerFunction(std::make_shared<VersionFunction>());
  }


  CurrentDateFunction::CurrentDateFunction()
  : Function("CURRENT_DATE", DATE, Types())
  {
  }

  const Variant CurrentDateFunction::doCall(const Variants& parameter) const
  {
    return Variant(csvsqldb::Date::now());
  }


  CurrentTimeFunction::CurrentTimeFunction()
  : Function("CURRENT_TIME", TIME, Types())
  {
  }

  const Variant CurrentTimeFunction::doCall(const Variants& parameter) const
  {
    return Variant(csvsqldb::Time::now());
  }


  CurrentTimestampFunction::CurrentTimestampFunction()
  : Function("CURRENT_TIMESTAMP", TIMESTAMP, Types())
  {
  }

  const Variant CurrentTimestampFunction::doCall(const Variants& parameter) const
  {
    return Variant(csvsqldb::Timestamp::now());
  }


  DateFormatFunction::DateFormatFunction()
  : Function("DATE_FORMAT", STRING, Types({DATE, STRING}))
  {
  }

  const Variant DateFormatFunction::doCall(const Variants& parameter) const
  {
    return Variant(parameter[0].asDate().format(parameter[1].asString()));
  }


  TimeFormatFunction::TimeFormatFunction()
  : Function("TIME_FORMAT", STRING, Types({TIME, STRING}))
  {
  }

  const Variant TimeFormatFunction::doCall(const Variants& parameter) const
  {
    return Variant(parameter[0].asTime().format(parameter[1].asString()));
  }

  TimestampFormatFunction::TimestampFormatFunction()
  : Function("TIMESTAMP_FORMAT", STRING, Types({TIMESTAMP, STRING}))
  {
  }

  const Variant TimestampFormatFunction::doCall(const Variants& parameter) const
  {
    return Variant(parameter[0].asTimestamp().format(parameter[1].asString()));
  }


  static constexpr int64_t sSecond = 1;
  static constexpr int64_t sMinute = 2;
  static constexpr int64_t sHour = 3;
  static constexpr int64_t sDay = 4;
  static constexpr int64_t sMonth = 5;
  static constexpr int64_t sYear = 6;

  ExtractFunction::ExtractFunction()
  : Function("EXTRACT", INT, Types({INT, TIMESTAMP}))
  {
  }

  const Variant ExtractFunction::doCall(const Variants& parameter) const
  {
    int64_t result = 0;

    switch (parameter[0].asInt()) {
      case sSecond:
        result = parameter[1].asTimestamp().second();
        break;
      case sMinute:
        result = parameter[1].asTimestamp().minute();
        break;
      case sHour:
        result = parameter[1].asTimestamp().hour();
        break;
      case sDay:
        result = parameter[1].asTimestamp().day();
        break;
      case sMonth:
        result = parameter[1].asTimestamp().month();
        break;
      case sYear:
        result = parameter[1].asTimestamp().year();
        break;
      default:
        CSVSQLDB_THROW(csvsqldb::Exception, "unknown extract part");
    }

    return Variant(result);
  }


  PowerFunction::PowerFunction()
  : Function("POW", REAL, Types({REAL, REAL}))
  {
  }

  const Variant PowerFunction::doCall(const Variants& parameter) const
  {
    const Variant base = parameter[0];
    const Variant exponent = parameter[1];

    double result = static_cast<double>(std::pow(base.asDouble(), exponent.asDouble()));

    return Variant(result);
  }


  UpperFunction::UpperFunction()
  : Function("UPPER", STRING, Types({STRING}))
  {
  }

  const Variant UpperFunction::doCall(const Variants& parameter) const
  {
    const Variant s = parameter[0];
    return Variant(csvsqldb::toupper_copy(s.asString()));
  }


  LowerFunction::LowerFunction()
  : Function("LOWER", STRING, Types({STRING}))
  {
  }

  const Variant LowerFunction::doCall(const Variants& parameter) const
  {
    const Variant s = parameter[0];
    return Variant(csvsqldb::tolower_copy(s.asString()));
  }


  CharLengthFunction::CharLengthFunction(const std::string& name)
  : Function(name, INT, Types({STRING}))
  {
  }

  const Variant CharLengthFunction::doCall(const Variants& parameter) const
  {
    const Variant s = parameter[0];
    return Variant(::strlen(s.asString()));
  }


  VersionFunction::VersionFunction()
  : Function("VERSION", STRING, Types())
  {
  }

  const Variant VersionFunction::doCall(const Variants& parameter) const
  {
    return Variant(CSVSQLDB_VERSION_STRING);
  }
}
