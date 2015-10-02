//
//  csvsqldb test
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


#include "test.h"
#include "test_helper.h"
#include "test/test_util.h"

#include "libcsvsqldb/base/csv_parser.h"

#include <fstream>
#include <sstream>


class DummyCSVParserCallback : public csvsqldb::csv::CSVParserCallback
{
public:
    virtual void onLong(int64_t num, bool isNull)
    {
        if(isNull) {
            _results.push_back("<NULL>");
        } else {
            _results.push_back(std::to_string(num));
        }
    }

    virtual void onDouble(double num, bool isNull)
    {
        if(isNull) {
            _results.push_back("<NULL>");
        } else {
            _results.push_back(std::to_string(num));
        }
    }

    virtual void onString(const char* s, size_t len, bool isNull)
    {
        if(isNull) {
            _results.push_back("<NULL>");
        } else {
            _results.push_back(s);
        }
    }

    virtual void onDate(const csvsqldb::Date& date, bool isNull)
    {
        if(isNull) {
            _results.push_back("<NULL>");
        } else {
            _results.push_back(date.format("%F"));
        }
    }

    virtual void onTime(const csvsqldb::Time& time, bool isNull)
    {
        if(isNull) {
            _results.push_back("<NULL>");
        } else {
            _results.push_back(time.format("%H:%M:%S"));
        }
    }

    virtual void onTimestamp(const csvsqldb::Timestamp& timestamp, bool isNull)
    {
        if(isNull) {
            _results.push_back("<NULL>");
        } else {
            _results.push_back(timestamp.format("%F"));
        }
    }

    virtual void onBoolean(bool boolean, bool isNull)
    {
        if(isNull) {
            _results.push_back("<NULL>");
        } else {
            _results.push_back(boolean ? "true" : "false");
        }
    }

    csvsqldb::StringVector _results;
};


class CSVParserTestCase
{
public:
    CSVParserTestCase()
    {
    }

    void setUp()
    {
    }

    void tearDown()
    {
    }

    void parseSimpleTest()
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

        DummyCSVParserCallback callback;
        csvsqldb::csv::CSVParserContext context;
        context._skipFirstLine = false;
        std::fstream csvfile(CSVSQLDB_TEST_PATH + std::string("/testdata/csv/test.csv"));
        MPF_TEST_ASSERT(csvfile);
        csvsqldb::csv::CSVParser csvparser(context, csvfile, types, callback);
        while(csvparser.parseLine()) {
        }

        MPF_TEST_ASSERTEQUAL(2U, csvparser.getLineCount());
        MPF_TEST_ASSERTEQUAL(22U, callback._results.size());

        const size_t firstRowBase = 0;
        MPF_TEST_ASSERTEQUAL("Testologe", callback._results[firstRowBase + 0]);
        MPF_TEST_ASSERTEQUAL("<NULL>", callback._results[firstRowBase + 1]);
        MPF_TEST_ASSERTEQUAL("1970-09-23", callback._results[firstRowBase + 2]);
        MPF_TEST_ASSERTEQUAL("Teststr., 315", callback._results[firstRowBase + 3]);
        MPF_TEST_ASSERTEQUAL("33509", callback._results[firstRowBase + 4]);
        MPF_TEST_ASSERTEQUAL("Gerlinde", callback._results[firstRowBase + 5]);
        MPF_TEST_ASSERTEQUAL("12000.000000", callback._results[firstRowBase + 6]);
        MPF_TEST_ASSERTEQUAL("true", callback._results[firstRowBase + 7]);
        MPF_TEST_ASSERTEQUAL("08:09:11", callback._results[firstRowBase + 8]);
        MPF_TEST_ASSERTEQUAL("2015-07-02T14:20:30", callback._results[firstRowBase + 9]);
        MPF_TEST_ASSERTEQUAL("-735", callback._results[firstRowBase + 10]);

        const size_t secondRowBase = 11;
        MPF_TEST_ASSERTEQUAL("von Ravensbrück", callback._results[secondRowBase + 0]);
        MPF_TEST_ASSERTEQUAL("Maria Angelica", callback._results[secondRowBase + 1]);
        MPF_TEST_ASSERTEQUAL("1973-04-06", callback._results[secondRowBase + 2]);
        MPF_TEST_ASSERTEQUAL("Teststr., 315", callback._results[secondRowBase + 3]);
        MPF_TEST_ASSERTEQUAL("<NULL>", callback._results[secondRowBase + 4]);
        MPF_TEST_ASSERTEQUAL("Gerlinde", callback._results[secondRowBase + 5]);
        MPF_TEST_ASSERTEQUAL("450.000000", callback._results[secondRowBase + 6]);
        MPF_TEST_ASSERTEQUAL("false", callback._results[secondRowBase + 7]);
        MPF_TEST_ASSERTEQUAL("<NULL>", callback._results[secondRowBase + 8]);
        MPF_TEST_ASSERTEQUAL("<NULL>", callback._results[secondRowBase + 9]);
        MPF_TEST_ASSERTEQUAL("525", callback._results[secondRowBase + 10]);
    }

    void parseTest()
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

        DummyCSVParserCallback callback;
        csvsqldb::csv::CSVParserContext context;
        context._skipFirstLine = true;
        csvsqldb::csv::CSVParser csvparser(context, ss, types, callback);
        while(csvparser.parseLine()) {
        }

        MPF_TEST_ASSERTEQUAL(14U, csvparser.getLineCount());
        MPF_TEST_ASSERTEQUAL(78U, callback._results.size());

        MPF_TEST_ASSERTEQUAL("47291", callback._results[0]);
        MPF_TEST_ASSERTEQUAL("1960-09-09", callback._results[1]);
        MPF_TEST_ASSERTEQUAL("Ulf", callback._results[2]);
        MPF_TEST_ASSERTEQUAL("Flexer", callback._results[3]);
        MPF_TEST_ASSERTEQUAL("M", callback._results[4]);
        MPF_TEST_ASSERTEQUAL("2000-01-12", callback._results[5]);

        MPF_TEST_ASSERTEQUAL("60134", callback._results[6]);
        MPF_TEST_ASSERTEQUAL("72329", callback._results[12]);
        MPF_TEST_ASSERTEQUAL("108201", callback._results[18]);
        MPF_TEST_ASSERTEQUAL("205048", callback._results[24]);
        MPF_TEST_ASSERTEQUAL("222965", callback._results[30]);
        MPF_TEST_ASSERTEQUAL("226633", callback._results[36]);

        MPF_TEST_ASSERTEQUAL("1958-06-10", callback._results[37]);
        MPF_TEST_ASSERTEQUAL("Xuejun", callback._results[38]);
        MPF_TEST_ASSERTEQUAL("Benzmuller", callback._results[39]);
        MPF_TEST_ASSERTEQUAL("F", callback._results[40]);
        MPF_TEST_ASSERTEQUAL("2000-01-04", callback._results[41]);

        MPF_TEST_ASSERTEQUAL("227544", callback._results[42]);
        MPF_TEST_ASSERTEQUAL("422990", callback._results[48]);
        MPF_TEST_ASSERTEQUAL("424445", callback._results[54]);
        MPF_TEST_ASSERTEQUAL("428377", callback._results[60]);
        MPF_TEST_ASSERTEQUAL("463807", callback._results[66]);
        MPF_TEST_ASSERTEQUAL("499553", callback._results[72]);

        MPF_TEST_ASSERTEQUAL("1954-05-06", callback._results[73]);
        MPF_TEST_ASSERTEQUAL("Hideyuki", callback._results[74]);
        MPF_TEST_ASSERTEQUAL("Delgrande", callback._results[75]);
        MPF_TEST_ASSERTEQUAL("F", callback._results[76]);
        MPF_TEST_ASSERTEQUAL("2000-01-22", callback._results[77]);
    }

    void parseErroneousCSV()
    {
        csvsqldb::csv::Types types;
        types.push_back(csvsqldb::csv::LONG);
        types.push_back(csvsqldb::csv::DATE);
        types.push_back(csvsqldb::csv::STRING);

        std::stringstream ss(R"(emp_no,birth_date,first_name
47291,1960-09-09,"Ulf"
60134,1964-04,Seshu
72329,1953-02-09,Randi
)");

        DummyCSVParserCallback callback;
        csvsqldb::csv::CSVParserContext context;
        context._skipFirstLine = true;
        csvsqldb::csv::CSVParser csvparser(context, ss, types, callback);

        RedirectStdErr red;
        while(csvparser.parseLine()) {
        }
        std::ifstream log((CSVSQLDB_TEST_PATH + std::string("/stderr.txt")));
        MPF_TEST_ASSERTEQUAL(true, log.good());
        std::string line;
        MPF_TEST_ASSERT(std::getline(log, line).good());
        MPF_TEST_ASSERTEQUAL("ERROR: skipping line 3: expected a date field (YYYY-mm-dd) in line 3", line);
    }
};

MPF_REGISTER_TEST_START("CSVSuite", CSVParserTestCase);
MPF_REGISTER_TEST(CSVParserTestCase::parseSimpleTest);
MPF_REGISTER_TEST(CSVParserTestCase::parseTest);
MPF_REGISTER_TEST(CSVParserTestCase::parseErroneousCSV);
MPF_REGISTER_TEST_END();
