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


TEST_CASE("Subquery Test", "[engine]")
{
  DatabaseTestWrapper dbWrapper;
  dbWrapper.addTable(TableInitializer("employees", {{"id", csvsqldb::INT},
                                                    {"first_name", csvsqldb::STRING},
                                                    {"last_name", csvsqldb::STRING},
                                                    {"birth_date", csvsqldb::DATE},
                                                    {"hire_date", csvsqldb::DATE}}));

  csvsqldb::ExecutionContext context(dbWrapper.getDatabase());
  csvsqldb::ExecutionEngine<TestOperatorNodeFactory> engine(context);
  csvsqldb::ExecutionStatistics statistics;

  SECTION("simple subquery")
  {
    TestRowProvider::setRows("employees",
                             {{815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
                               csvsqldb::Date(2003, csvsqldb::Date::April, 15)},
                              {4711, "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23),
                               csvsqldb::Date(2010, csvsqldb::Date::February, 1)},
                              {9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6),
                               csvsqldb::Date(2003, csvsqldb::Date::June, 15)}});

    {
      std::stringstream ss;
      int64_t rowCount = engine.execute("SELECT count(*) FROM (SELECT id,first_name,last_name FROM employees)", statistics, ss);
      CHECK(1 == rowCount);
      CHECK("#$alias_1\n3\n" == ss.str());
    }

    {
      std::stringstream ss;
      int64_t rowCount = engine.execute(
        "select count(*) as \"COUNT\",last_name from (select * from employees) group by last_name order by last_name limit 1",
        statistics, ss);
      CHECK(1 == rowCount);
      std::string expected = R"(#COUNT,EMPLOYEES.LAST_NAME
2,'Fürstenberg'
)";
      CHECK(expected == ss.str());
    }
  }

  SECTION("simple subquery with alias")
  {
    TestRowProvider::setRows("employees",
                             {{815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
                               csvsqldb::Date(2003, csvsqldb::Date::April, 15)},
                              {4711, "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23),
                               csvsqldb::Date(2010, csvsqldb::Date::February, 1)},
                              {9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6),
                               csvsqldb::Date(2003, csvsqldb::Date::June, 15)}});

    {
      std::stringstream ss;
      int64_t rowCount = engine.execute(
        "select emp.first_name,emp.last_name from (select emp.first_name,emp.last_name from employees as emp)", statistics, ss);
      CHECK(3 == rowCount);
      std::string expected = R"(#EMP.FIRST_NAME,EMP.LAST_NAME
'Mark','Fürstenberg'
'Lars','Fürstenberg'
'Angelica','Tello de Fürstenberg'
)";
      CHECK(expected == ss.str());
    }

    {
      std::stringstream ss;
      int64_t rowCount =
        engine.execute("select first_name,last_name from (select first_name,last_name from employees as emp)", statistics, ss);
      CHECK(3 == rowCount);
      std::string expected = R"(#FIRST_NAME,LAST_NAME
'Mark','Fürstenberg'
'Lars','Fürstenberg'
'Angelica','Tello de Fürstenberg'
)";
      CHECK(expected == ss.str());
    }

    {
      std::stringstream ss;
      int64_t rowCount = engine.execute(
        "select emp.first_name,emp.last_name from (select * from employees) as emp order by emp.first_name", statistics, ss);
      CHECK(3 == rowCount);
      std::string expected = R"(#EMP.FIRST_NAME,EMP.LAST_NAME
'Angelica','Tello de Fürstenberg'
'Lars','Fürstenberg'
'Mark','Fürstenberg'
)";
      CHECK(expected == ss.str());
    }
  }

  SECTION("subquery with join")
  {
    dbWrapper.addTable(TableInitializer(
      "salaries",
      {{"id", csvsqldb::INT}, {"salary", csvsqldb::REAL}, {"from_date", csvsqldb::DATE}, {"to_date", csvsqldb::DATE}}));

    TestRowProvider::setRows(
      "employees",
      {{9384, "John", "Doe", csvsqldb::Date(1965, csvsqldb::Date::August, 8), csvsqldb::Date(9999, csvsqldb::Date::December, 31)},
       {815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
        csvsqldb::Date(2003, csvsqldb::Date::April, 15)},
       {4711, "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23),
        csvsqldb::Date(2010, csvsqldb::Date::February, 1)},
       {9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6),
        csvsqldb::Date(2003, csvsqldb::Date::June, 15)}});

    TestRowProvider::setRows(
      "salaries",
      {{815, 5000.00, csvsqldb::Date(2003, csvsqldb::Date::April, 15), csvsqldb::Date(2015, csvsqldb::Date::December, 31)},
       {4711, 12000.00, csvsqldb::Date(2010, csvsqldb::Date::February, 1), csvsqldb::Date(2015, csvsqldb::Date::December, 31)},
       {9227, 450.00, csvsqldb::Date(2003, csvsqldb::Date::June, 15), csvsqldb::Date(2015, csvsqldb::Date::December, 31)}});

    {
      std::stringstream ss;
      int64_t rowCount = engine.execute(
        "select emp.id,emp.first_name,emp.last_name,sal.salary from (SELECT * FROM employees emp INNER JOIN salaries sal ON "
        "emp.id = sal.id) order by emp.id",
        statistics, ss);
      CHECK(3 == rowCount);

      std::string expected = R"(#EMP.ID,EMP.FIRST_NAME,EMP.LAST_NAME,SAL.SALARY
815,'Mark','Fürstenberg',5000.000000
4711,'Lars','Fürstenberg',12000.000000
9227,'Angelica','Tello de Fürstenberg',450.000000
)";
      CHECK(expected == ss.str());
    }
  }
}
