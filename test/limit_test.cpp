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


#include <csvsqldb/block.h>

#include "data_test_framework.h"

#include <catch2/catch.hpp>


TEST_CASE("Limit Test", "[engine]")
{
  SECTION("simple limit")
  {
    DatabaseTestWrapper dbWrapper;
    dbWrapper.addTable(TableInitializer("employees", {{"id", csvsqldb::INT},
                                                      {"first_name", csvsqldb::STRING},
                                                      {"last_name", csvsqldb::STRING},
                                                      {"birth_date", csvsqldb::DATE},
                                                      {"hire_date", csvsqldb::DATE}}));

    csvsqldb::ExecutionContext context(dbWrapper.getDatabase());
    csvsqldb::ExecutionEngine<TestOperatorNodeFactory> engine(context);

    TestRowProvider::setRows(
      "employees",
      {{815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
        csvsqldb::Date(2003, csvsqldb::Date::April, 15)},
       {4711, "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23),
        csvsqldb::Date(2010, csvsqldb::Date::February, 1)},
       {3467, "Ingo", "Fürstenberg", csvsqldb::Date(1946, csvsqldb::Date::May, 4), csvsqldb::Date(1969, csvsqldb::Date::June, 1)},
       {1423, "Tilo", "Bürstenbinder", csvsqldb::Date(1973, csvsqldb::Date::January, 8),
        csvsqldb::Date(2001, csvsqldb::Date::October, 1)},
       {192, "Anette", "Zipiriski", csvsqldb::Date(1956, csvsqldb::Date::August, 5),
        csvsqldb::Date(1978, csvsqldb::Date::February, 25)},
       {9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6),
        csvsqldb::Date(2003, csvsqldb::Date::June, 15)}});

    csvsqldb::ExecutionStatistics statistics;
    std::stringstream ss;
    int64_t rowCount =
      engine.execute("SELECT id,first_name,last_name,birth_date,hire_date FROM employees order by id limit 3", statistics, ss);
    CHECK(3 == rowCount);

    std::string expected = R"(#ID,FIRST_NAME,LAST_NAME,BIRTH_DATE,HIRE_DATE
192,'Anette','Zipiriski',1956-08-05,1978-02-25
815,'Mark','Fürstenberg',1969-05-17,2003-04-15
1423,'Tilo','Bürstenbinder',1973-01-08,2001-10-01
)";
    CHECK(expected == ss.str());
  }

  SECTION("simple limit offset")
  {
    DatabaseTestWrapper dbWrapper;
    dbWrapper.addTable(TableInitializer("employees", {{"id", csvsqldb::INT},
                                                      {"first_name", csvsqldb::STRING},
                                                      {"last_name", csvsqldb::STRING},
                                                      {"birth_date", csvsqldb::DATE},
                                                      {"hire_date", csvsqldb::DATE}}));

    csvsqldb::ExecutionContext context(dbWrapper.getDatabase());
    csvsqldb::ExecutionEngine<TestOperatorNodeFactory> engine(context);

    TestRowProvider::setRows(
      "employees",
      {{815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
        csvsqldb::Date(2003, csvsqldb::Date::April, 15)},
       {4711, "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23),
        csvsqldb::Date(2010, csvsqldb::Date::February, 1)},
       {3467, "Ingo", "Fürstenberg", csvsqldb::Date(1946, csvsqldb::Date::May, 4), csvsqldb::Date(1969, csvsqldb::Date::June, 1)},
       {1423, "Tilo", "Bürstenbinder", csvsqldb::Date(1973, csvsqldb::Date::January, 8),
        csvsqldb::Date(2001, csvsqldb::Date::October, 1)},
       {192, "Anette", "Zipiriski", csvsqldb::Date(1956, csvsqldb::Date::August, 5),
        csvsqldb::Date(1978, csvsqldb::Date::February, 25)},
       {9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6),
        csvsqldb::Date(2003, csvsqldb::Date::June, 15)}});

    csvsqldb::ExecutionStatistics statistics;
    std::stringstream ss;
    int64_t rowCount = engine.execute(
      "SELECT id,first_name,last_name,birth_date,hire_date FROM employees order by id limit 3 offset 3", statistics, ss);
    CHECK(3 == rowCount);

    std::string expected = R"(#ID,FIRST_NAME,LAST_NAME,BIRTH_DATE,HIRE_DATE
3467,'Ingo','Fürstenberg',1946-05-04,1969-06-01
4711,'Lars','Fürstenberg',1970-09-23,2010-02-01
9227,'Angelica','Tello de Fürstenberg',1963-03-06,2003-06-15
)";
    CHECK(expected == ss.str());
  }
}
