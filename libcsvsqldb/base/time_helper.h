//
//  time_helper.h
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

#ifndef csvsqldb_time_helper_h
#define csvsqldb_time_helper_h

#include "libcsvsqldb/inc.h"

#include "date.h"
#include "time.h"
#include "timestamp.h"
#include "types.h"


#if defined _MSC_VER
  // make available a timegm function for Windows builds
  #define timegm _mkgmtime
#endif

namespace csvsqldb
{
  /**
   * Returns the current timestamp.
   * @return The current timestamp
   */
  CSVSQLDB_EXPORT Timepoint now();

  /**
   * Converts the given timestamp to a GMT string representation.
   * @param timestamp The timestamp to convert
   * @return The converted timestamp as GMT string
   */
  CSVSQLDB_EXPORT std::string timestampToGMTString(const Timepoint& timestamp);

  /**
   * Converts the given timestamp to a local string representation.
   * @param timestamp The timestamp to convert
   * @return The converted timestamp as local string
   */
  CSVSQLDB_EXPORT std::string timestampToLocalString(const Timepoint& timestamp);

  /**
   * Converts the given GMT string representation into a timestamp
   * @param time The GMT time string to convert
   * @return The converted timestamp
   */
  CSVSQLDB_EXPORT Timepoint stringToTimestamp(const std::string& time);

  /**
   * Converts a string into a Date representation. The date string has to be in an iso format: YYYY-mm-dd.
   * @param isodate The string in iso foramt YYYY-mm-dd to convert
   * @return The converted date
   */
  CSVSQLDB_EXPORT Date dateFromString(const std::string& isodate);

  /**
   * Converts a string into a Time representation. The time string has to be in an iso format: HH:MM:SS.
   * @param isotime The string in iso foramt HH:MM:SS to convert
   * @return The converted time
   */
  CSVSQLDB_EXPORT Time timeFromString(const std::string& isotime);

  /**
   * Converts a string into a Timestamp representation. The timestamp string has to be in an iso format: YYYY-mm-ddTHH:MM:SS.
   * @param isotimestamp The string in iso foramt YYYY-mm-ddTHH:MM:SS to convert
   * @return The converted timestamp
   */
  CSVSQLDB_EXPORT Timestamp timestampFromString(const std::string& isotimestamp);
}

#endif
