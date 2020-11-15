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


#include "libcsvsqldb/block.h"

#include "data_test_framework.h"
#include "test.h"
#include <unordered_set>


class GroupByTestCase
{
public:
  GroupByTestCase()
  {
  }

  void setUp()
  {
  }

  void tearDown()
  {
  }

  void groupingElementTest()
  {
    csvsqldb::Variants first;
    first.push_back(csvsqldb::Variant(4711));
    first.push_back(csvsqldb::Variant("Fürstenberg"));

    csvsqldb::Variants second;
    second.push_back(csvsqldb::Variant(815));
    second.push_back(csvsqldb::Variant("Fürstenberg"));

    csvsqldb::GroupingElement elementLeft(first);
    csvsqldb::GroupingElement elementRight(second);

    MPF_TEST_ASSERT(elementLeft.getHash() != elementRight.getHash());
    MPF_TEST_ASSERT(!(elementLeft == elementRight));
    MPF_TEST_ASSERT(elementLeft == elementLeft);


    typedef std::unordered_set<csvsqldb::GroupingElement> HashSet;
    HashSet hashSet;
    hashSet.insert(elementLeft);
    hashSet.insert(elementRight);

    MPF_TEST_ASSERTEQUAL(2UL, hashSet.size());

    HashSet::const_iterator found = hashSet.find(elementRight);
    MPF_TEST_ASSERT(found != hashSet.end());

    csvsqldb::Variants third;
    third.push_back(csvsqldb::Variant(815));
    third.push_back(csvsqldb::Variant("Fürstenberg"));
    csvsqldb::GroupingElement compareElement(third);

    found = hashSet.find(compareElement);
    MPF_TEST_ASSERT(found != hashSet.end());
  }

  void simpleGroupByTest()
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
      "employees", {{815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
                     csvsqldb::Date(2003, csvsqldb::Date::April, 15)},
                    {csvsqldb::Variant(csvsqldb::INT), "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23),
                     csvsqldb::Date(2010, csvsqldb::Date::February, 1)},
                    {9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6),
                     csvsqldb::Date(2003, csvsqldb::Date::June, 15)}});

    csvsqldb::ExecutionStatistics statistics;
    std::stringstream ss;
    int64_t rowCount =
      engine.execute("SELECT count(*) as \"count\",last_name,max(birth_date) as \"max birthdate\",min(hire_date) as \"min hire\" \
                                          FROM employees group by employees.last_name",
                     statistics, ss);
    MPF_TEST_ASSERTEQUAL(2, rowCount);

    std::string expected = R"(#COUNT,LAST_NAME,MAX BIRTHDATE,MIN HIRE
1,'Tello de Fürstenberg',1963-03-06,2003-06-15
2,'Fürstenberg',1970-09-23,2003-04-15
)";
    MPF_TEST_ASSERTEQUAL(expected, ss.str());
  }

  void simpleGroupByCountWithNullTest()
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
      "employees", {{815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
                     csvsqldb::Date(2003, csvsqldb::Date::April, 15)},
                    {csvsqldb::Variant(csvsqldb::INT), "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23),
                     csvsqldb::Date(2010, csvsqldb::Date::February, 1)},
                    {9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6),
                     csvsqldb::Date(2003, csvsqldb::Date::June, 15)}});

    csvsqldb::ExecutionStatistics statistics;
    std::stringstream ss;
    int64_t rowCount = engine.execute(
      "SELECT count(id) as \"count\",last_name,max(birth_date) as \"max birthdate\",min(hire_date) as \"min hire\" \
                                          FROM employees group by last_name",
      statistics, ss);
    MPF_TEST_ASSERTEQUAL(2, rowCount);

    std::string expected = R"(#COUNT,LAST_NAME,MAX BIRTHDATE,MIN HIRE
1,'Tello de Fürstenberg',1963-03-06,2003-06-15
1,'Fürstenberg',1970-09-23,2003-04-15
)";
    MPF_TEST_ASSERTEQUAL(expected, ss.str());
  }

  void groupByWithSupressedGroupBy()
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
      "employees", {{815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17),
                     csvsqldb::Date(2003, csvsqldb::Date::April, 15)},
                    {csvsqldb::Variant(csvsqldb::INT), "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23),
                     csvsqldb::Date(2010, csvsqldb::Date::February, 1)},
                    {9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6),
                     csvsqldb::Date(2003, csvsqldb::Date::June, 15)}});

    csvsqldb::ExecutionStatistics statistics;
    std::stringstream ss;
    int64_t rowCount =
      engine.execute("SELECT count(*) as \"count\",max(birth_date) as \"max birthdate\",min(hire_date) as \"min hire\" \
                                          FROM employees group by last_name",
                     statistics, ss);
    MPF_TEST_ASSERTEQUAL(2, rowCount);

    std::string expected = R"(#COUNT,MAX BIRTHDATE,MIN HIRE
1,1963-03-06,2003-06-15
2,1970-09-23,2003-04-15
)";
    MPF_TEST_ASSERTEQUAL(expected, ss.str());
  }
};

MPF_REGISTER_TEST_START("GroupByTestSuite", GroupByTestCase);
MPF_REGISTER_TEST(GroupByTestCase::groupingElementTest);
MPF_REGISTER_TEST(GroupByTestCase::simpleGroupByTest);
MPF_REGISTER_TEST(GroupByTestCase::simpleGroupByCountWithNullTest);
MPF_REGISTER_TEST(GroupByTestCase::groupByWithSupressedGroupBy);
MPF_REGISTER_TEST_END();
