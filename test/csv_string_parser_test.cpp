//
//  csvsqldb test
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


#include <csvsqldb/base/csv_string_parser.h>

#include "test/test_util.h"
#include "test_helper.h"

#include <catch2/catch.hpp>

#include <fstream>
#include <functional>
#include <sstream>

using namespace std::placeholders;


namespace
{
  class StringReader
  {
  public:
    void setString(const std::string& s)
    {
      _s = s;
      _pos = _s.begin();
    }

    char readNextChar(bool ignoreDelimiter)
    {
      if (_pos == _s.end()) {
        return '\0';
      }
      if (!ignoreDelimiter && *_pos == ',') {
        return '\0';
      }
      return *_pos++;
    }

  private:
    std::string _s;
    std::string::const_iterator _pos{_s.end()};
  };
}


TEST_CASE("CSV String Parser Test", "[csv]")
{
  SECTION("string parser")
  {
    csvsqldb::csv::CSVStringParser::BufferType buffer;
    buffer.resize(2);
    StringReader sr;
    csvsqldb::csv::CSVStringParser sp(buffer, 1024, std::bind(&StringReader::readNextChar, std::ref(sr), _1));

    sr.setString("test");
    CHECK(4U == sp.parseToBuffer());
    CHECK("test" == std::string(&buffer[0]));

    sr.setString("'test'");
    CHECK(4U == sp.parseToBuffer());
    CHECK("test" == std::string(&buffer[0]));

    sr.setString("\"test\"");
    CHECK(4U == sp.parseToBuffer());
    CHECK("test" == std::string(&buffer[0]));

    sr.setString("");
    CHECK(0U == sp.parseToBuffer());
    CHECK("" == std::string(&buffer[0]));

    sr.setString("''");
    CHECK(0U == sp.parseToBuffer());
    CHECK("" == std::string(&buffer[0]));

    sr.setString("\"\"");
    CHECK(0U == sp.parseToBuffer());
    CHECK("" == std::string(&buffer[0]));

    sr.setString("te'st");
    CHECK(5U == sp.parseToBuffer());
    CHECK("te'st" == std::string(&buffer[0]));

    sr.setString("te\"st");
    CHECK(5U == sp.parseToBuffer());
    CHECK("te\"st" == std::string(&buffer[0]));

    sr.setString("'te\"st'");
    CHECK(5U == sp.parseToBuffer());
    CHECK("te\"st" == std::string(&buffer[0]));

    sr.setString("\"te'st\"");
    CHECK(5U == sp.parseToBuffer());
    CHECK("te'st" == std::string(&buffer[0]));

    sr.setString("'te''st'");
    CHECK(5U == sp.parseToBuffer());
    CHECK("te'st" == std::string(&buffer[0]));

    sr.setString("\"te\"\"st\"");
    CHECK(5U == sp.parseToBuffer());
    CHECK("te\"st" == std::string(&buffer[0]));

    sr.setString("'te,st'");
    CHECK(5U == sp.parseToBuffer());
    CHECK("te,st" == std::string(&buffer[0]));

    sr.setString("te,st");
    CHECK(2U == sp.parseToBuffer());
    CHECK("te" == std::string(&buffer[0]));

    sr.setString("M");
    CHECK(1U == sp.parseToBuffer());
    CHECK("M" == std::string(&buffer[0]));
  }
}
