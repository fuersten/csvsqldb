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


#include "data_test_framework.h"
#include "test.h"


class DataFrameworkTestCase
{
public:
  DataFrameworkTestCase()
  {
  }

  void setUp()
  {
  }

  void tearDown()
  {
  }

  void frameworkTest()
  {
    DatabaseTestWrapper dbWrapper;

    dbWrapper.addTable(TableInitializer("employees", {{"id", csvsqldb::INT},
                                                      {"first_name", csvsqldb::STRING},
                                                      {"last_name", csvsqldb::STRING},
                                                      {"birth_date", csvsqldb::DATE},
                                                      {"hire_date", csvsqldb::DATE}}));
    dbWrapper.addTable(TableInitializer(
      "salaries",
      {{"id", csvsqldb::INT}, {"salary", csvsqldb::REAL}, {"from_date", csvsqldb::DATE}, {"to_date", csvsqldb::DATE}}));

    MPF_TEST_ASSERTEQUAL("EMPLOYEES", dbWrapper.getDatabase().getTable("EMPLOYEES").name());
    MPF_TEST_ASSERTEQUAL(5u, dbWrapper.getDatabase().getTable("EMPLOYEES").columnCount());
    MPF_TEST_ASSERTEQUAL("SALARIES", dbWrapper.getDatabase().getTable("SALARIES").name());
    MPF_TEST_ASSERTEQUAL(4u, dbWrapper.getDatabase().getTable("SALARIES").columnCount());
  }

  void operationTest()
  {
    DatabaseTestWrapper dbWrapper;
    dbWrapper.addTable(TableInitializer("employees", {{"id", csvsqldb::INT},
                                                      {"first_name", csvsqldb::STRING},
                                                      {"last_name", csvsqldb::STRING},
                                                      {"birth_date", csvsqldb::DATE},
                                                      {"hire_date", csvsqldb::DATE}}));

    csvsqldb::ExecutionContext context(dbWrapper.getDatabase());
    csvsqldb::ExecutionEngine<TestOperatorNodeFactory> engine(context);

    TestRowProvider::setRows("employees",
                             {{4711, "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23),
                               csvsqldb::Date(2010, csvsqldb::Date::February, 1)},
                              {815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
                               csvsqldb::Date(2003, csvsqldb::Date::April, 15)},
                              {9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6),
                               csvsqldb::Date(2003, csvsqldb::Date::June, 15)}});

    csvsqldb::ExecutionStatistics statistics;
    std::stringstream ss;
    int64_t rowCount = engine.execute(
      "SELECT id,(first_name || ' ' || last_name) as name,birth_date birthday, 7 * 5 / 4 as calc FROM employees emp WHERE "
      "\"id\" BETWEEN 100 AND 9999 AND emp.birth_date > DATE'1960-01-01'",
      statistics, ss);
    MPF_TEST_ASSERTEQUAL(3, rowCount);
    MPF_TEST_ASSERTEQUAL(
      "#ID,NAME,BIRTHDAY,CALC\n4711,'Lars Fürstenberg',1970-09-23,8\n815,'Mark Fürstenberg',1969-05-17,8\n9227,'Angelica Tello "
      "de Fürstenberg',1963-03-06,8\n",
      ss.str());

    ss.str("");
    ss.clear();
    rowCount = engine.execute("select * from employees where birth_date > date'1965-01-01'", statistics, ss);
    MPF_TEST_ASSERTEQUAL(2, rowCount);
    MPF_TEST_ASSERTEQUAL(
      "#EMPLOYEES.ID,EMPLOYEES.FIRST_NAME,EMPLOYEES.LAST_NAME,BIRTH_DATE,EMPLOYEES.HIRE_DATE\n4711,'Lars','Fürstenberg',1970-"
      "09-23,2010-02-01\n815,'Mark','Fürstenberg',1969-05-17,2003-04-15\n",
      ss.str());
  }
};

MPF_REGISTER_TEST_START("TestFrameworkTestSuite", DataFrameworkTestCase);
MPF_REGISTER_TEST(DataFrameworkTestCase::frameworkTest);
MPF_REGISTER_TEST(DataFrameworkTestCase::operationTest);
MPF_REGISTER_TEST_END();
