//
//  string_helper.h
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
#include <csvsqldb/base/types.h>
#include <csvsqldb/compat/put_time.h>
#include <csvsqldb/compat/time_api.h>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <iterator>


namespace csvsqldb
{
  /**
   * Splits the given string into its elements using the delimiter.
   * @param s The string to split
   * @param delim The delimiter to use
   * @param elems The collection to put the splitted elemts into. The collection will be cleared before inserting into it.
   * @param addEmpty If empty elements shall be added. Default is true.
   * @return The number of elements in elems.
   */
  template<typename T>
  size_t split(const std::string& s, char delim, T& elems, bool addEmpty = true)
  {
    elems.clear();
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
      if (addEmpty || item.length()) {
        elems.push_back(item);
      }
    }
    return elems.size();
  }

  /**
   * Joins the elements of container elems using the given separator. The separator will not be added to the last element.
   * The code was found here: http://stackoverflow.com/questions/5288396/c-ostream-out-manipulation/5289170#5289170
   * @param elems Container to elements to join
   * @param separator String to separate elements with
   * @return A string with all elements separated by the separator
   */
  template<typename T>
  std::string join(const T& elems, const char* const separator)
  {
    switch (elems.size()) {
      case 0:
        return "";
      case 1:
        return elems[0];
      default:
        std::ostringstream os;
        std::copy(elems.begin(), elems.end() - 1, std::ostream_iterator<std::string>(os, separator));
        os << *elems.rbegin();
        return os.str();
    }
  }

  /**
   * Stream overload for the output of std::chrono time points. Time points are outputted as iso time strings.
   * @param stream The stream to output to
   * @param time_point The time point to output
   * @return The stream operator after the streaming of the time point.
   */
  template<typename Clock, typename Duration>
  std::ostream& operator<<(std::ostream& stream, const std::chrono::time_point<Clock, Duration>& time_point)
  {
    const time_t time = Clock::to_time_t(time_point);
    struct tm ptime;
    localtime_r(&time, &ptime);
    return (stream << put_time(&ptime, "%Y-%m-%dT%H:%M:%S"));
  }

  /**
   * Returns a string representation of the time point formatted as rfc1123 date.
   * @param time_point The time point to format
   * @return The formatted date representation.
   */
  template<typename Clock, typename Duration>
  std::string formatDateRfc1123(const std::chrono::time_point<Clock, Duration>& time_point)
  {
    const time_t time = Clock::to_time_t(time_point);
    std::stringstream ss;
    struct tm ptime;
    gmtime_r(&time, &ptime);
    ss << put_time(&ptime, "%a, %d %b %Y %H:%M:%S GMT");
    return ss.str();
  }

  /**
   * Converts the string inplace into upper characters only.
   * @param s The string to convert
   * @return Returns A reference to the converted string.
   */
  CSVSQLDB_EXPORT std::string& toupper(std::string& s);

  /**
   * Converts the string into upper characters only.
   * @param s The string to convert
   * @return Returns The converted string.
   */
  CSVSQLDB_EXPORT std::string toupper_copy(const std::string& s);

  /**
   * Converts the string inplace into lower characters only.
   * @param s The string to convert
   * @return Returns A reference to the converted string.
   */
  CSVSQLDB_EXPORT std::string& tolower(std::string& s);

  /**
   * Converts the string into lower characters only.
   * @param s The string to convert
   * @return Returns The converted string.
   */
  CSVSQLDB_EXPORT std::string tolower_copy(const std::string& s);

  /**
   * Platform independent case insensitive string compare function.
   * @param str1 First string to compare
   * @param str2 Second string to compare
   * @return A value less than zero, if str1 is less than str2, a value of zero, if str1 matches str2 and a value greater than
   * zero, if
   * str1 is greater than str2.
   */
  CSVSQLDB_EXPORT int stricmp(const char* str1, const char* str2);

  /**
   * Platform independent case insensitive string compare function.
   * @param str1 First string to compare
   * @param str2 Second string to compare
   * @param count Number of characters to compare
   * @return A value less than zero, if str1 is less than str2, a value of zero, if str1 matches str2 and a value greater than
   * zero, if
   * str1 is greater than str2. This is true up to the first count bytes thereof.
   */
  CSVSQLDB_EXPORT int strnicmp(const char* str1, const char* str2, size_t count);

  /**
   * Trims right whitespaces from string.
   * @param s String to trim whitespaces from
   * @return Returns the trimmed string.
   */
  CSVSQLDB_EXPORT std::string trim_right(const std::string& s);

  /**
   * Trims left whitespaces from string.
   * @param s String to trim whitespaces from
   * @return Returns the trimmed string.
   */
  CSVSQLDB_EXPORT std::string trim_left(const std::string& s);

  /**
   * Decode url encoded string.
   * @param in The string to url decode
   * @param out The url decoded string
   * @return true, if decoding was successful, otherwise false.
   */
  CSVSQLDB_EXPORT bool decode(const std::string& in, std::string& out);
}
