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

#include "data_test_framework.h"


class UnionTestCase
{
public:
    UnionTestCase()
    {
    }
    
    void setUp()
    {
    }
    
    void tearDown()
    {
    }

    void simpleUnionTest()
    {
        DatabaseTestWrapper dbWrapper;
        dbWrapper.addTable(TableInitializer("employees",
                                            {
                                                {"id", csvsqldb::INT},
                                                {"first_name", csvsqldb::STRING},
                                                {"last_name", csvsqldb::STRING},
                                                {"birth_date", csvsqldb::DATE},
                                                {"hire_date", csvsqldb::DATE}
                                            }));

        csvsqldb::ExecutionContext context(dbWrapper.getDatabase());
        csvsqldb::ExecutionEngine<TestOperatorNodeFactory> engine(context);
        
        TestRowProvider::setRows("employees", {
            { 815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17), csvsqldb::Date(2003, csvsqldb::Date::April, 15) },
            { 4711, "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23), csvsqldb::Date(2010, csvsqldb::Date::February, 1) },
            { 9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6), csvsqldb::Date(2003, csvsqldb::Date::June, 15) }
        });
        
        csvsqldb::ExecutionStatistics statistics;
        std::stringstream ss;
        int64_t rowCount = engine.execute("SELECT * FROM employees WHERE id < 4700 UNION (SELECT * FROM employees WHERE id >= 4700)", statistics, ss);
        MPF_TEST_ASSERTEQUAL(3, rowCount);
        MPF_TEST_ASSERTEQUAL("#ID,EMPLOYEES.FIRST_NAME,EMPLOYEES.LAST_NAME,EMPLOYEES.BIRTH_DATE,EMPLOYEES.HIRE_DATE\n4711,'Lars','Fürstenberg',1970-09-23,2010-02-01\n9227,'Angelica','Tello de Fürstenberg',1963-03-06,2003-06-15\n815,'Mark','Fürstenberg',1969-05-17,2003-04-15\n", ss.str());
    }
    
    void failedUnionTest()
    {
        DatabaseTestWrapper dbWrapper;
        dbWrapper.addTable(TableInitializer("employees",
                                            {
                                                {"id", csvsqldb::INT},
                                                {"first_name", csvsqldb::STRING},
                                                {"last_name", csvsqldb::STRING},
                                                {"birth_date", csvsqldb::DATE},
                                                {"hire_date", csvsqldb::DATE}
                                            }));
        dbWrapper.addTable(TableInitializer("salaries",
                                            {
                                                {"id", csvsqldb::INT},
                                                {"salary", csvsqldb::REAL},
                                                {"from_date", csvsqldb::DATE},
                                                {"to_date", csvsqldb::DATE}
                                            }));

        csvsqldb::ExecutionContext context(dbWrapper.getDatabase());
        csvsqldb::ExecutionEngine<TestOperatorNodeFactory> engine(context);
        
        TestRowProvider::setRows("employees", {
            { 815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17), csvsqldb::Date(2003, csvsqldb::Date::April, 15) },
            { 4711, "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23), csvsqldb::Date(2010, csvsqldb::Date::February, 1) },
            { 9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6), csvsqldb::Date(2003, csvsqldb::Date::June, 15) }
        });
        
        csvsqldb::ExecutionStatistics statistics;
        std::stringstream ss;
        MPF_TEST_EXPECTS(engine.execute("SELECT * FROM employees emp UNION (SELECT * FROM salaries)", statistics, ss), csvsqldb::SqlParserException);
    }
    
    void complexUnionTest()
    {
        DatabaseTestWrapper dbWrapper;
        dbWrapper.addTable(TableInitializer("employees",
                                            {
                                                {"id", csvsqldb::INT},
                                                {"first_name", csvsqldb::STRING},
                                                {"last_name", csvsqldb::STRING},
                                                {"birth_date", csvsqldb::DATE},
                                                {"hire_date", csvsqldb::DATE}
                                            }));
        dbWrapper.addTable(TableInitializer("salaries",
                                            {
                                                {"id", csvsqldb::INT},
                                                {"salary", csvsqldb::REAL},
                                                {"from_date", csvsqldb::DATE},
                                                {"to_date", csvsqldb::DATE}
                                            }));
        
        csvsqldb::ExecutionContext context(dbWrapper.getDatabase());
        csvsqldb::ExecutionEngine<TestOperatorNodeFactory> engine(context);
        
        TestRowProvider::setRows("employees", {
            { 815, "Mark", "Fürstenberg", csvsqldb::Date(1969, csvsqldb::Date::May, 17), csvsqldb::Date(2003, csvsqldb::Date::April, 15) },
            { 4711, "Lars", "Fürstenberg", csvsqldb::Date(1970, csvsqldb::Date::September, 23), csvsqldb::Date(2010, csvsqldb::Date::February, 1) },
            { 9227, "Angelica", "Tello de Fürstenberg", csvsqldb::Date(1963, csvsqldb::Date::March, 6), csvsqldb::Date(2003, csvsqldb::Date::June, 15) }
        });
        
        TestRowProvider::setRows("salaries", {
            { 815, 5000.00, csvsqldb::Date(2003, csvsqldb::Date::April, 15), csvsqldb::Date(2015, csvsqldb::Date::December, 31) },
            { 4711, 12000.00, csvsqldb::Date(2010, csvsqldb::Date::February, 1), csvsqldb::Date(2015, csvsqldb::Date::December, 31) },
            { 9227, 450.00, csvsqldb::Date(2003, csvsqldb::Date::June, 15), csvsqldb::Date(2015, csvsqldb::Date::December, 31) }
        });
        
        csvsqldb::ExecutionStatistics statistics;
        std::stringstream ss;
        int64_t rowCount = engine.execute("SELECT id FROM employees UNION (SELECT id FROM salaries)", statistics, ss);
        MPF_TEST_ASSERTEQUAL(6, rowCount);
        MPF_TEST_ASSERTEQUAL("#ID\n815\n4711\n9227\n815\n4711\n9227\n", ss.str());
    }
};

MPF_REGISTER_TEST_START("UnionTestSuite", UnionTestCase);
MPF_REGISTER_TEST(UnionTestCase::simpleUnionTest);
MPF_REGISTER_TEST(UnionTestCase::failedUnionTest);
MPF_REGISTER_TEST(UnionTestCase::complexUnionTest);
MPF_REGISTER_TEST_END();
