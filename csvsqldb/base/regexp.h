//
//  regexp.h
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

#include <memory>
#include <string>


namespace csvsqldb
{
  /**
   *  Regular expression matcher class. Supports most of the commen regular exression constructs. Does only matching and no
   * capturing.
   *  Supported constructs:
   *  - ab
   *  - a | b
   *  - a*
   *  - a+
   *  - a?
   *  - (a | b)
   *  - [a-zA-Z]
   *  - [^a-z]
   *  - .
   *  - \\w
   *  - \\d
   *  - \\s
   */
  class CSVSQLDB_EXPORT RegExp
  {
  public:
    /**
     *  Constructs an empty regular expression which will match nothing.
     */
    RegExp();

    /**
     * Constructs a regular expression from the given string.
     * @param s The regular expression to construct. Will throw a RegExpException upon errors.
     */
    explicit RegExp(std::string s);

    RegExp(RegExp&& r) noexcept;
    RegExp& operator=(RegExp&& r) noexcept;
    RegExp(const RegExp&) = delete;
    RegExp& operator=(const RegExp&) = delete;

    ~RegExp();

    RegExp& operator=(const std::string& s);
    RegExp& operator=(std::string&& s);

    /**
     * Matches the given string against the regular expression.
     * @param s String to match
     * @return true if the string matches, otherwise false
     */
    bool match(const std::string& s) const;

    /**
     * Matches the given string against the regular expression.
     * @param s String to match
     * @return true if the string matches, otherwise false
     */
    bool match(const char* s) const;

  private:
    struct Private;
    std::unique_ptr<Private> _m;
  };
}
