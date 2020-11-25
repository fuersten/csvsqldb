//
//  time_measurement.h
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

#include <csvsqldb/inc.h>

#include <csvsqldb/base/exception.h>

#include <chrono>
#include <ostream>


namespace csvsqldb
{
  /**
   * Time measurement handling stuff
   */
  namespace chrono
  {
    struct CSVSQLDB_EXPORT ProcessTimeDuration {
      int64_t _real{0};
      int64_t _user{0};
      int64_t _system{0};
    };

    struct CSVSQLDB_EXPORT ProcessTimePoint {
      std::chrono::steady_clock::time_point _real;
      int64_t _user{0};
      int64_t _system{0};

      friend CSVSQLDB_EXPORT ProcessTimeDuration operator-(const ProcessTimePoint& lhs, const ProcessTimePoint& rhs);
    };

    CSVSQLDB_EXPORT ProcessTimeDuration operator-(const ProcessTimePoint& lhs, const ProcessTimePoint& rhs);

    /**
     * A clock for the retrieval of real, user and system time. Can be used to measure runtimes.
     */
    class CSVSQLDB_EXPORT ProcessTimeClock
    {
    public:
      ProcessTimeClock(const ProcessTimeClock&) = delete;
      ProcessTimeClock& operator=(const ProcessTimeClock&) = delete;
      ProcessTimeClock(ProcessTimeClock&&) = delete;
      ProcessTimeClock& operator=(ProcessTimeClock&&) = delete;

      static ProcessTimePoint now();
    };
  }
}

namespace std
{
  CSVSQLDB_EXPORT std::ostream& operator<<(std::ostream& out, const csvsqldb::chrono::ProcessTimeDuration& duration);
}
