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


TEST_CASE("Join Test", "[engine]")
{
  DatabaseTestWrapper dbWrapper;
  dbWrapper.addTable(TableInitializer("employees", {{"id", csvsqldb::INT},
                                                    {"first_name", csvsqldb::STRING},
                                                    {"last_name", csvsqldb::STRING},
                                                    {"birth_date", csvsqldb::DATE},
                                                    {"hire_date", csvsqldb::DATE}}));
  dbWrapper.addTable(TableInitializer(
    "salaries", {{"id", csvsqldb::INT}, {"salary", csvsqldb::REAL}, {"from_date", csvsqldb::DATE}, {"to_date", csvsqldb::DATE}}));

  csvsqldb::ExecutionContext context(dbWrapper.getDatabase());
  csvsqldb::ExecutionEngine<TestOperatorNodeFactory> engine(context);
  csvsqldb::ExecutionStatistics statistics;

  SECTION("simple cross join")
  {
    TestRowProvider::setRows("employees",
                             {{815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
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

    std::stringstream ss;
    int64_t rowCount = engine.execute("SELECT * FROM employees CROSS JOIN salaries", statistics, ss);
    CHECK(9 == rowCount);
    std::string expected =
      R"(#EMPLOYEES.ID,EMPLOYEES.FIRST_NAME,EMPLOYEES.LAST_NAME,EMPLOYEES.BIRTH_DATE,EMPLOYEES.HIRE_DATE,SALARIES.ID,SALARIES.SALARY,SALARIES.FROM_DATE,SALARIES.TO_DATE
815,'Mark','Fürstenberg',1969-05-17,2003-04-15,815,5000.000000,2003-04-15,2015-12-31
815,'Mark','Fürstenberg',1969-05-17,2003-04-15,4711,12000.000000,2010-02-01,2015-12-31
815,'Mark','Fürstenberg',1969-05-17,2003-04-15,9227,450.000000,2003-06-15,2015-12-31
4711,'Lars','Fürstenberg',1970-09-23,2010-02-01,815,5000.000000,2003-04-15,2015-12-31
4711,'Lars','Fürstenberg',1970-09-23,2010-02-01,4711,12000.000000,2010-02-01,2015-12-31
4711,'Lars','Fürstenberg',1970-09-23,2010-02-01,9227,450.000000,2003-06-15,2015-12-31
9227,'Angelica','Tello de Fürstenberg',1963-03-06,2003-06-15,815,5000.000000,2003-04-15,2015-12-31
9227,'Angelica','Tello de Fürstenberg',1963-03-06,2003-06-15,4711,12000.000000,2010-02-01,2015-12-31
9227,'Angelica','Tello de Fürstenberg',1963-03-06,2003-06-15,9227,450.000000,2003-06-15,2015-12-31
)";
    CHECK(expected == ss.str());
  }

  SECTION("simple inner join")
  {
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

    std::stringstream ss;
    int64_t rowCount = engine.execute("SELECT * FROM employees emp INNER JOIN salaries sal ON emp.id = sal.id", statistics, ss);
    CHECK(3 == rowCount);
    std::string expected =
      R"(#EMP.ID,EMP.FIRST_NAME,EMP.LAST_NAME,EMP.BIRTH_DATE,EMP.HIRE_DATE,SAL.ID,SAL.SALARY,SAL.FROM_DATE,SAL.TO_DATE
815,'Mark','Fürstenberg',1969-05-17,2003-04-15,815,5000.000000,2003-04-15,2015-12-31
4711,'Lars','Fürstenberg',1970-09-23,2010-02-01,4711,12000.000000,2010-02-01,2015-12-31
9227,'Angelica','Tello de Fürstenberg',1963-03-06,2003-06-15,9227,450.000000,2003-06-15,2015-12-31
)";
    CHECK(expected == ss.str());
  }

  SECTION("complex inner join")
  {
    dbWrapper.addTable(TableInitializer("dept_emp", {
                                                      {"id", csvsqldb::INT},
                                                      {"dept_no", csvsqldb::STRING},
                                                      {"from_date", csvsqldb::DATE},
                                                      {"to_date", csvsqldb::DATE},
                                                    }));
    dbWrapper.addTable(TableInitializer("departments", {
                                                         {"dept_no", csvsqldb::STRING},
                                                         {"dept_name", csvsqldb::STRING},
                                                       }));

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
      "dept_emp",
      {{4711, "d005", csvsqldb::Date(2010, csvsqldb::Date::February, 1), csvsqldb::Date(9999, csvsqldb::Date::December, 31)},
       {815, "d007", csvsqldb::Date(2003, csvsqldb::Date::April, 15), csvsqldb::Date(9999, csvsqldb::Date::December, 31)},
       {9227, "d003", csvsqldb::Date(2003, csvsqldb::Date::June, 15), csvsqldb::Date(9999, csvsqldb::Date::December, 31)}});

    TestRowProvider::setRows("departments", {{"d009", "Customer Service"},
                                             {"d005", "Development"},
                                             {"d002", "Finance"},
                                             {"d003", "Human Resources"},
                                             {"d001", "Marketing"},
                                             {"d004", "Production"},
                                             {"d006", "Quality Management"},
                                             {"d008", "Research"},
                                             {"d007", "Sales"}});

    std::stringstream ss;
    int64_t rowCount = engine.execute(
      "SELECT emp.first_name,emp.last_name,dept_emp.dept_no,departments.dept_name FROM employees as emp JOIN dept_emp ON "
      "emp.id = dept_emp.id JOIN departments ON dept_emp.dept_no = departments.dept_no",
      statistics, ss);
    CHECK(3 == rowCount);
    std::string expected = R"(#EMP.FIRST_NAME,EMP.LAST_NAME,DEPT_EMP.DEPT_NO,DEPARTMENTS.DEPT_NAME
'Mark','Fürstenberg','d007','Sales'
'Lars','Fürstenberg','d005','Development'
'Angelica','Tello de Fürstenberg','d003','Human Resources'
)";
    CHECK(expected == ss.str());
  }

  SECTION("self join")
  {
    dbWrapper.addTable(TableInitializer("creditcards", {{"nr", csvsqldb::INT},
                                                        {"company", csvsqldb::STRING},
                                                        {"expiration_date", csvsqldb::DATE},
                                                        {"customer_nr", csvsqldb::INT}}));

    TestRowProvider::setRows("creditcards",
                             {
                               {4024007180543240UL, "Visa", csvsqldb::Date(2019, csvsqldb::Date::May, 01), 123457},
                               {5397979799985804UL, "Master Card", csvsqldb::Date(2020, csvsqldb::Date::January, 01), 123459},
                               {374580812235447UL, "American Express", csvsqldb::Date(2019, csvsqldb::Date::May, 01), 123459},
                               {36010567235582UL, "Diners Club", csvsqldb::Date(2022, csvsqldb::Date::February, 01), 123458},
                               {4916510282619314UL, "Visa", csvsqldb::Date(2017, csvsqldb::Date::March, 01), 123458},
                             });

    {
      std::stringstream ss;
      int64_t rowCount = engine.execute(
        "SELECT cc1.customer_nr,cc1.company,cc2.customer_nr,cc2.company FROM creditcards cc1 INNER JOIN creditcards cc2 ON "
        "cc1.customer_nr = cc2.customer_nr order by cc1.company,cc2.company,cc1.customer_nr",
        statistics, ss);
      CHECK(9 == rowCount);
      std::string expected = R"(#CC1.CUSTOMER_NR,CC1.COMPANY,CC2.CUSTOMER_NR,CC2.COMPANY
123459,'American Express',123459,'American Express'
123459,'American Express',123459,'Master Card'
123458,'Diners Club',123458,'Diners Club'
123458,'Diners Club',123458,'Visa'
123459,'Master Card',123459,'American Express'
123459,'Master Card',123459,'Master Card'
123458,'Visa',123458,'Diners Club'
123457,'Visa',123457,'Visa'
123458,'Visa',123458,'Visa'
)";
      CHECK(expected == ss.str());
    }

    {
      std::stringstream ss;
      int64_t rowCount = engine.execute(
        "SELECT cc1.customer_nr,cc1.company,cc2.customer_nr,cc2.company FROM creditcards cc1 INNER JOIN creditcards cc2 ON "
        "cc1.customer_nr = cc2.customer_nr WHERE cc1.company <> cc2.company order by cc1.company",
        statistics, ss);
      CHECK(4 == rowCount);
      std::string expected = R"(#CC1.CUSTOMER_NR,CC1.COMPANY,CC2.CUSTOMER_NR,CC2.COMPANY
123459,'American Express',123459,'Master Card'
123458,'Diners Club',123458,'Visa'
123459,'Master Card',123459,'American Express'
123458,'Visa',123458,'Diners Club'
)";
      CHECK(expected == ss.str());
    }
  }
}
