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


#include <csvsqldb/base/csv_parser.h>

#include "test/test_util.h"
#include "test_helper.h"

#include <catch2/catch.hpp>

#include <fstream>
#include <functional>
#include <sstream>

using namespace std::placeholders;


namespace
{
  class MyCSVParserCallback : public csvsqldb::csv::CSVParserCallback
  {
  public:
    void onLong(int64_t num, bool isNull) override
    {
      if (isNull) {
        _results.push_back("<NULL>");
      } else {
        _results.push_back(std::to_string(num));
      }
    }

    void onDouble(double num, bool isNull) override
    {
      if (isNull) {
        _results.push_back("<NULL>");
      } else {
        _results.push_back(std::to_string(num));
      }
    }

    void onString(const char* s, size_t, bool isNull) override
    {
      if (isNull) {
        _results.push_back("<NULL>");
      } else {
        _results.push_back(s);
      }
    }

    void onDate(const csvsqldb::Date& date, bool isNull) override
    {
      if (isNull) {
        _results.push_back("<NULL>");
      } else {
        _results.push_back(date.format("%F"));
      }
    }

    void onTime(const csvsqldb::Time& time, bool isNull) override
    {
      if (isNull) {
        _results.push_back("<NULL>");
      } else {
        _results.push_back(time.format("%H:%M:%S"));
      }
    }

    void onTimestamp(const csvsqldb::Timestamp& timestamp, bool isNull) override
    {
      if (isNull) {
        _results.push_back("<NULL>");
      } else {
        _results.push_back(timestamp.format("%F"));
      }
    }

    void onBoolean(bool boolean, bool isNull) override
    {
      if (isNull) {
        _results.push_back("<NULL>");
      } else {
        _results.push_back(boolean ? "true" : "false");
      }
    }

    csvsqldb::StringVector _results;
  };

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

  void checkLog(const std::string& message)
  {
    std::ifstream log(getTestPath() / "stderr.txt");
    CHECK(true == log.good());
    std::string line;
    CHECK(std::getline(log, line).good());
    CHECK(message == line);
  }

  void parse(csvsqldb::csv::CSVParser& csvparser)
  {
    while (csvparser.parseLine()) {
    }
  }
}


TEST_CASE("CSV Parser Test", "[csv]")
{
  MyCSVParserCallback callback;
  csvsqldb::csv::CSVParserContext context;
  context._skipFirstLine = true;

  SECTION("parse simple")
  {
    csvsqldb::csv::Types types;
    types.push_back(csvsqldb::csv::STRING);
    types.push_back(csvsqldb::csv::STRING);
    types.push_back(csvsqldb::csv::DATE);
    types.push_back(csvsqldb::csv::STRING);
    types.push_back(csvsqldb::csv::LONG);
    types.push_back(csvsqldb::csv::STRING);
    types.push_back(csvsqldb::csv::DOUBLE);
    types.push_back(csvsqldb::csv::BOOLEAN);
    types.push_back(csvsqldb::csv::TIME);
    types.push_back(csvsqldb::csv::TIMESTAMP);
    types.push_back(csvsqldb::csv::LONG);

    context._skipFirstLine = false;
    std::fstream csvfile(getTestPath() / "testdata" / "csv" / "test.csv");
    CHECK(csvfile);
    csvsqldb::csv::CSVParser csvparser(context, csvfile, types, callback);
    parse(csvparser);

    CHECK(2U == csvparser.getLineCount());
    CHECK(22U == callback._results.size());

    const size_t firstRowBase = 0;
    CHECK("Testologe" == callback._results[firstRowBase + 0]);
    CHECK("<NULL>" == callback._results[firstRowBase + 1]);
    CHECK("1970-09-23" == callback._results[firstRowBase + 2]);
    CHECK("Teststr., 315" == callback._results[firstRowBase + 3]);
    CHECK("33509" == callback._results[firstRowBase + 4]);
    CHECK("Ger\"linde" == callback._results[firstRowBase + 5]);
    CHECK("12000.000000" == callback._results[firstRowBase + 6]);
    CHECK("true" == callback._results[firstRowBase + 7]);
    CHECK("08:09:11" == callback._results[firstRowBase + 8]);
    CHECK("2015-07-02T14:20:30" == callback._results[firstRowBase + 9]);
    CHECK("-735" == callback._results[firstRowBase + 10]);

    const size_t secondRowBase = 11;
    CHECK("von Ravensbrück" == callback._results[secondRowBase + 0]);
    CHECK("Maria Angelica" == callback._results[secondRowBase + 1]);
    CHECK("1973-04-06" == callback._results[secondRowBase + 2]);
    CHECK("Teststr., 315" == callback._results[secondRowBase + 3]);
    CHECK("<NULL>" == callback._results[secondRowBase + 4]);
    CHECK("Gerlinde" == callback._results[secondRowBase + 5]);
    CHECK("450.000000" == callback._results[secondRowBase + 6]);
    CHECK("false" == callback._results[secondRowBase + 7]);
    CHECK("<NULL>" == callback._results[secondRowBase + 8]);
    CHECK("<NULL>" == callback._results[secondRowBase + 9]);
    CHECK("525" == callback._results[secondRowBase + 10]);
  }
  SECTION("parse")
  {
    csvsqldb::csv::Types types;
    types.push_back(csvsqldb::csv::LONG);
    types.push_back(csvsqldb::csv::DATE);
    types.push_back(csvsqldb::csv::STRING);
    types.push_back(csvsqldb::csv::STRING);
    types.push_back(csvsqldb::csv::STRING);
    types.push_back(csvsqldb::csv::DATE);

    std::stringstream ss(R"(emp_no,birth_date,first_name,last_name,gender,hire_date
47291,1960-09-09,"Ulf",Flexer,M,2000-01-12
60134,1964-04-21,Seshu,Rathonyi,F,2000-01-02
72329,1953-02-09,Randi,Luit,F,2000-01-02
108201,1955-04-14,Mariangiola,Boreale,M,2000-01-01
205048,1960-09-12,Ennio,Alblas,F,2000-01-06
222965,1959-08-07,Volkmar,Perko,F,2000-01-13
226633,1958-06-10,Xuejun,Benzmuller,F,2000-01-04
227544,1954-11-17,Shahab,Demeyer,M,2000-01-08
422990,1953-04-09,Jaana,Verspoor,F,2000-01-11
424445,1953-04-27,Jeong,Boreale,M,2000-01-03
428377,1957-05-09,Yucai,Gerlach,M,2000-01-23
463807,1964-06-12,Bikash,Covnot,M,2000-01-28
499553,1954-05-06,Hideyuki,Delgrande,F,2000-01-22
)");

    csvsqldb::csv::CSVParser csvparser(context, ss, types, callback);
    parse(csvparser);

    CHECK(14U == csvparser.getLineCount());
    CHECK(78U == callback._results.size());

    CHECK("47291" == callback._results[0]);
    CHECK("1960-09-09" == callback._results[1]);
    CHECK("Ulf" == callback._results[2]);
    CHECK("Flexer" == callback._results[3]);
    CHECK("M" == callback._results[4]);
    CHECK("2000-01-12" == callback._results[5]);

    CHECK("60134" == callback._results[6]);
    CHECK("72329" == callback._results[12]);
    CHECK("108201" == callback._results[18]);
    CHECK("205048" == callback._results[24]);
    CHECK("222965" == callback._results[30]);
    CHECK("226633" == callback._results[36]);

    CHECK("1958-06-10" == callback._results[37]);
    CHECK("Xuejun" == callback._results[38]);
    CHECK("Benzmuller" == callback._results[39]);
    CHECK("F" == callback._results[40]);
    CHECK("2000-01-04" == callback._results[41]);

    CHECK("227544" == callback._results[42]);
    CHECK("422990" == callback._results[48]);
    CHECK("424445" == callback._results[54]);
    CHECK("428377" == callback._results[60]);
    CHECK("463807" == callback._results[66]);
    CHECK("499553" == callback._results[72]);

    CHECK("1954-05-06" == callback._results[73]);
    CHECK("Hideyuki" == callback._results[74]);
    CHECK("Delgrande" == callback._results[75]);
    CHECK("F" == callback._results[76]);
    CHECK("2000-01-22" == callback._results[77]);
  }
  SECTION("parse strings")
  {
    csvsqldb::csv::Types types;
    types.push_back(csvsqldb::csv::LONG);
    types.push_back(csvsqldb::csv::STRING);
    types.push_back(csvsqldb::csv::DATE);

    std::stringstream ss(R"(emp_no,first_name,birth_date
47291,"Ulf",1960-09-09
+60134,Seshu,1964-04-21
72329,'Randi',1953-02-09
00001,"abc'def",1960-09-09
00002,'abc"def',1960-09-09
00003,"abc""def",1960-09-09
00004,'abc''def',1960-09-09
00005,"abc,def",1960-09-09
00006,'abc,def',1960-09-09
)");

    csvsqldb::csv::CSVParser csvparser(context, ss, types, callback);
    parse(csvparser);

    // 10 because the header line is also counted in order to output correct line numbers for errors
    CHECK(10U == csvparser.getLineCount());
    CHECK(27U == callback._results.size());

    size_t nextRowBase = 0;
    CHECK("47291" == callback._results[nextRowBase + 0]);
    CHECK("Ulf" == callback._results[nextRowBase + 1]);
    CHECK("1960-09-09" == callback._results[nextRowBase + 2]);

    nextRowBase = 3;
    CHECK("60134" == callback._results[nextRowBase + 0]);
    CHECK("Seshu" == callback._results[nextRowBase + 1]);
    CHECK("1964-04-21" == callback._results[nextRowBase + 2]);

    nextRowBase = 6;
    CHECK("72329" == callback._results[nextRowBase + 0]);
    CHECK("Randi" == callback._results[nextRowBase + 1]);
    CHECK("1953-02-09" == callback._results[nextRowBase + 2]);

    nextRowBase = 9;
    CHECK("abc'def" == callback._results[nextRowBase + 1]);

    nextRowBase = 12;
    CHECK("abc\"def" == callback._results[nextRowBase + 1]);

    nextRowBase = 15;
    CHECK("abc\"def" == callback._results[nextRowBase + 1]);

    nextRowBase = 18;
    CHECK("abc'def" == callback._results[nextRowBase + 1]);

    nextRowBase = 21;
    CHECK("abc,def" == callback._results[nextRowBase + 1]);

    nextRowBase = 24;
    CHECK("abc,def" == callback._results[nextRowBase + 1]);
  }
}

TEST_CASE("CSV Parser Error Test", "[csv]")
{
  MyCSVParserCallback callback;
  csvsqldb::csv::CSVParserContext context;
  context._skipFirstLine = false;
  RedirectStdErr red;

  SECTION("parse erroneous csv")
  {
    csvsqldb::csv::Types types;
    types.push_back(csvsqldb::csv::LONG);
    types.push_back(csvsqldb::csv::DATE);
    types.push_back(csvsqldb::csv::STRING);

    std::stringstream ss(R"(47291,1960-09-09,"Ulf"
60134,1964-04,Seshu
72329,1953-02-09,Randi
)");

    csvsqldb::csv::CSVParser csvparser(context, ss, types, callback);
    parse(csvparser);

    checkLog("ERROR: skipping line 2: expected a date field (YYYY-mm-dd) in line 2");
  }
  SECTION("too many fields")
  {
    csvsqldb::csv::Types types;
    types.push_back(csvsqldb::csv::LONG);
    types.push_back(csvsqldb::csv::DATE);

    std::stringstream ss(R"(47291,1960-09-09,"Ulf"
60134,1964-04-21,Seshu
72329,1953-02-09,Randi
)");

    csvsqldb::csv::CSVParser csvparser(context, ss, types, callback);
    parse(csvparser);

    checkLog("ERROR: skipping line 1: too many fields found in line 1");
  }
  SECTION("too few fields")
  {
    csvsqldb::csv::Types types;
    types.push_back(csvsqldb::csv::LONG);
    types.push_back(csvsqldb::csv::DATE);
    types.push_back(csvsqldb::csv::STRING);
    types.push_back(csvsqldb::csv::LONG);

    std::stringstream ss(R"(47291,1960-09-09,"Ulf"
60134,1964-04-21,Seshu
72329,1953-02-09,Randi
)");

    csvsqldb::csv::CSVParser csvparser(context, ss, types, callback);
    parse(csvparser);

    checkLog("ERROR: skipping line 1: too few fields found in line 1");
  }
  SECTION("no more input")
  {
    csvsqldb::csv::Types types;
    types.push_back(csvsqldb::csv::LONG);
    types.push_back(csvsqldb::csv::DATE);
    types.push_back(csvsqldb::csv::STRING);

    std::stringstream ss(R"(47291,1960-09-09,"Ulf"
60134,1964-04-21,Seshu
72329,1953-02-09,)");

    csvsqldb::csv::CSVParser csvparser(context, ss, types, callback);
    parse(csvparser);

    checkLog("ERROR: skipping line 3: too few fields found in line 3");
  }
  SECTION("no long type")
  {
    csvsqldb::csv::Types types;
    types.push_back(csvsqldb::csv::LONG);

    std::stringstream ss(R"(472gdb)");

    csvsqldb::csv::CSVParser csvparser(context, ss, types, callback);
    parse(csvparser);

    checkLog("ERROR: skipping line 1: field is not a long in line 1");
  }
  SECTION("no bool type")
  {
    csvsqldb::csv::Types types;
    types.push_back(csvsqldb::csv::BOOLEAN);

    std::stringstream ss(R"(check)");

    csvsqldb::csv::CSVParser csvparser(context, ss, types, callback);
    parse(csvparser);

    checkLog("ERROR: skipping line 1: field is not a bool in line 1");
  }
  SECTION("no time type")
  {
    csvsqldb::csv::Types types;
    types.push_back(csvsqldb::csv::TIME);

    std::stringstream ss(R"(12.06.34)");

    csvsqldb::csv::CSVParser csvparser(context, ss, types, callback);
    parse(csvparser);

    checkLog("ERROR: skipping line 1: expected a time field (HH:MM:SS) in line 1");
  }
}
