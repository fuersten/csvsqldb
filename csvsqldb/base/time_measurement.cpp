//
//  time_measurement.cpp
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

#include "time_measurement.h"

#ifndef _MSC_VER
  #include <sys/resource.h>
  #include <sys/times.h>
  #include <unistd.h>
#endif


namespace std
{
  std::ostream& operator<<(std::ostream& out, const csvsqldb::chrono::ProcessTimeDuration& duration)
  {
    out << "[user " << duration._user << "ms system " << duration._system << "ms real " << duration._real << "ms]";
    return out;
  }
}

namespace csvsqldb
{
  namespace chrono
  {
    ProcessTimeDuration operator-(const ProcessTimePoint& lhs, const ProcessTimePoint& rhs)
    {
      ProcessTimeDuration duration;
      duration._real = std::chrono::duration_cast<std::chrono::milliseconds>(lhs._real - rhs._real).count();
      duration._user = lhs._user - rhs._user;
      duration._system = lhs._system - rhs._system;
      return duration;
    }

    ProcessTimePoint ProcessTimeClock::now()
    {
      ProcessTimePoint timePoint;
      timePoint._real = std::chrono::steady_clock::now();

#ifndef _MSC_VER
      struct rusage usage;
      ::getrusage(RUSAGE_SELF, &usage);

      timePoint._user = ((usage.ru_utime.tv_sec * 1000000) + usage.ru_utime.tv_usec) / 1000;
      timePoint._system = ((usage.ru_stime.tv_sec * 1000000) + usage.ru_stime.tv_usec) / 1000;
#else
      timePoint._system = 0;
      timePoint._user = 0;
#endif
      return timePoint;
    }
  }
}
