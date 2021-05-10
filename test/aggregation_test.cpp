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


#include "data_test_framework.h"

#include <catch2/catch.hpp>


TEST_CASE("Aggreagation Test", "[engine]")
{
  DatabaseTestWrapper dbWrapper;
  dbWrapper.addTable(TableInitializer("employees", {{"id", csvsqldb::INT},
                                                    {"first_name", csvsqldb::STRING},
                                                    {"last_name", csvsqldb::STRING},
                                                    {"birth_date", csvsqldb::DATE},
                                                    {"hire_date", csvsqldb::DATE}}));

  csvsqldb::ExecutionContext context(dbWrapper.getDatabase());
  csvsqldb::ExecutionEngine<TestOperatorNodeFactory> engine(context);

  SECTION("count")
  {
    TestRowProvider::setRows("employees",
                             {{815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
                               csvsqldb::Date(2003, csvsqldb::Date::April, 15)},
                              {4711, "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23),
                               csvsqldb::Date(2010, csvsqldb::Date::February, 1)},
                              {9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6),
                               csvsqldb::Date(2003, csvsqldb::Date::June, 15)}});

    {
      csvsqldb::ExecutionStatistics statistics;
      std::stringstream ss;
      int64_t rowCount = engine.execute("SELECT count(*) FROM employees", statistics, ss);
      CHECK(1 == rowCount);
      CHECK("#$alias_1\n3\n" == ss.str());
    }
    {
      csvsqldb::ExecutionStatistics statistics;
      std::stringstream ss;
      int64_t rowCount = engine.execute("SELECT count(id) FROM employees", statistics, ss);
      CHECK(1 == rowCount);
      CHECK("#$alias_1\n3\n" == ss.str());
    }
  }

  SECTION("sum")
  {
    TestRowProvider::setRows("employees",
                             {{815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
                               csvsqldb::Date(2003, csvsqldb::Date::April, 15)},
                              {4711, "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23),
                               csvsqldb::Date(2010, csvsqldb::Date::February, 1)},
                              {9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6),
                               csvsqldb::Date(2003, csvsqldb::Date::June, 15)}});

    csvsqldb::ExecutionStatistics statistics;
    std::stringstream ss;
    int64_t rowCount = engine.execute("SELECT sum(id) as \"sum\" FROM employees", statistics, ss);
    CHECK(1 == rowCount);
    CHECK("#SUM\n14753\n" == ss.str());
  }

  SECTION("avg")
  {
    TestRowProvider::setRows("employees",
                             {{815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
                               csvsqldb::Date(2003, csvsqldb::Date::April, 15)},
                              {4711, "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23),
                               csvsqldb::Date(2010, csvsqldb::Date::February, 1)},
                              {9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6),
                               csvsqldb::Date(2003, csvsqldb::Date::June, 15)}});

    csvsqldb::ExecutionStatistics statistics;
    std::stringstream ss;
    int64_t rowCount = engine.execute("SELECT avg(id) as \"avg\" FROM employees", statistics, ss);
    CHECK(1 == rowCount);
    CHECK("#AVG\n4917\n" == ss.str());
  }

  SECTION("max")
  {
    TestRowProvider::setRows("employees",
                             {{815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
                               csvsqldb::Date(2003, csvsqldb::Date::April, 15)},
                              {4711, "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23),
                               csvsqldb::Date(2010, csvsqldb::Date::February, 1)},
                              {9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6),
                               csvsqldb::Date(2003, csvsqldb::Date::June, 15)}});

    {
      csvsqldb::ExecutionStatistics statistics;
      std::stringstream ss;
      int64_t rowCount = engine.execute("SELECT max(id) as \"max\" FROM employees", statistics, ss);
      CHECK(1 == rowCount);
      CHECK("#MAX\n9227\n" == ss.str());
    }

    {
      csvsqldb::ExecutionStatistics statistics;
      std::stringstream ss;
      int64_t rowCount = engine.execute("SELECT max(char_length(last_name)) as \"max name\" FROM employees", statistics, ss);
      CHECK(1 == rowCount);
      // the length is 21 bytes and 20 unicode characters
      CHECK("#MAX NAME\n21\n" == ss.str());
    }
  }

  SECTION("min")
  {
    TestRowProvider::setRows("employees",
                             {{815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
                               csvsqldb::Date(2003, csvsqldb::Date::April, 15)},
                              {4711, "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23),
                               csvsqldb::Date(2010, csvsqldb::Date::February, 1)},
                              {9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6),
                               csvsqldb::Date(2003, csvsqldb::Date::June, 15)}});

    csvsqldb::ExecutionStatistics statistics;
    std::stringstream ss;
    int64_t rowCount = engine.execute("SELECT min(id) as \"min\" FROM employees", statistics, ss);
    CHECK(1 == rowCount);
    CHECK("#MIN\n815\n" == ss.str());
  }

  SECTION("null count")
  {
    TestRowProvider::setRows(
      "employees", {{815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
                     csvsqldb::Date(2003, csvsqldb::Date::April, 15)},
                    {csvsqldb::Variant(csvsqldb::INT), "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23),
                     csvsqldb::Date(2010, csvsqldb::Date::February, 1)},
                    {9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6),
                     csvsqldb::Date(2003, csvsqldb::Date::June, 15)}});

    {
      csvsqldb::ExecutionStatistics statistics;
      std::stringstream ss;
      int64_t rowCount = engine.execute("SELECT count(*) FROM employees", statistics, ss);
      CHECK(1 == rowCount);
      CHECK("#$alias_1\n3\n" == ss.str());
    }
    {
      csvsqldb::ExecutionStatistics statistics;
      std::stringstream ss;
      int64_t rowCount = engine.execute("SELECT count(id) FROM employees", statistics, ss);
      CHECK(1 == rowCount);
      CHECK("#$alias_1\n2\n" == ss.str());
    }
  }

  SECTION("null sum")
  {
    TestRowProvider::setRows(
      "employees", {{815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
                     csvsqldb::Date(2003, csvsqldb::Date::April, 15)},
                    {csvsqldb::Variant(csvsqldb::INT), "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23),
                     csvsqldb::Date(2010, csvsqldb::Date::February, 1)},
                    {9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6),
                     csvsqldb::Date(2003, csvsqldb::Date::June, 15)}});

    csvsqldb::ExecutionStatistics statistics;
    std::stringstream ss;
    int64_t rowCount = engine.execute("SELECT sum(id) as \"sum\" FROM employees", statistics, ss);
    CHECK(1 == rowCount);
    CHECK("#SUM\n10042\n" == ss.str());
  }

  SECTION("all null")
  {
    TestRowProvider::setRows(
      "employees", {{csvsqldb::Variant(csvsqldb::INT), "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
                     csvsqldb::Date(2003, csvsqldb::Date::April, 15)},
                    {csvsqldb::Variant(csvsqldb::INT), "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23),
                     csvsqldb::Date(2010, csvsqldb::Date::February, 1)},
                    {csvsqldb::Variant(csvsqldb::INT), "Angelica", "Tello de Fürstenberg",
                     csvsqldb::Date(1963, csvsqldb::Date::March, 6), csvsqldb::Date(2003, csvsqldb::Date::June, 15)}});

    {
      csvsqldb::ExecutionStatistics statistics;
      std::stringstream ss;
      int64_t rowCount = engine.execute("SELECT sum(id) as \"sum\" FROM employees", statistics, ss);
      CHECK(1 == rowCount);
      CHECK("#SUM\nNULL\n" == ss.str());
    }
    {
      csvsqldb::ExecutionStatistics statistics;
      std::stringstream ss;
      int64_t rowCount = engine.execute("SELECT count(*) FROM employees", statistics, ss);
      CHECK(1 == rowCount);
      CHECK("#$alias_1\n3\n" == ss.str());
    }
    {
      csvsqldb::ExecutionStatistics statistics;
      std::stringstream ss;
      int64_t rowCount = engine.execute("SELECT count(id) FROM employees", statistics, ss);
      CHECK(1 == rowCount);
      CHECK("#$alias_1\nNULL\n" == ss.str());
    }
  }

  SECTION("multi aggregation")
  {
    TestRowProvider::setRows(
      "employees", {{815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
                     csvsqldb::Date(2003, csvsqldb::Date::April, 15)},
                    {csvsqldb::Variant(csvsqldb::INT), "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23),
                     csvsqldb::Date(2010, csvsqldb::Date::February, 1)},
                    {9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6),
                     csvsqldb::Date(2003, csvsqldb::Date::June, 15)}});

    csvsqldb::ExecutionStatistics statistics;
    std::stringstream ss;
    int64_t rowCount = engine.execute(
      "SELECT min(id) as \"min\", max(char_length(first_name)) as \"max name\", count(*) as \"count\", count(id) as \"id "
      "count\" FROM employees",
      statistics, ss);
    CHECK(1 == rowCount);
    CHECK("#MIN,MAX NAME,COUNT,ID COUNT\n815,8,3,2\n" == ss.str());
  }
}
