//
//  get_time.h
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

#ifndef csvsqldb_get_time_h
#define csvsqldb_get_time_h

// TODO(BBR) forward to available std fct

#include <ctime>
#include <ios>
#include <string>
#include <vector>

namespace csvsqldb {
namespace detail {
struct get_time_helper {
  std::tm *tmb;
  const std::string fmt;
};
} // namespace detail

template <typename CharT>
detail::get_time_helper get_time(std::tm *tmb, CharT *fmt) {
  return {tmb, fmt};
}

template <typename CharT, typename Traits>
std::basic_istream<CharT, Traits> &
operator>>(std::basic_istream<CharT, Traits> &is, detail::get_time_helper h) {
  std::string s;
  is >> s;
  if (not::strptime(s.c_str(), h.fmt.c_str(), h.tmb)) {
    throw std::ios_base::failure("failure to parse data on stream ('" + s +
                                 "') with format string ('" + h.fmt + "')");
  }
  return is;
}
} // namespace csvsqldb

#endif // csvsqldb_get_time_h

