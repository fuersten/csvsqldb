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

#include <csvsqldb/buildin_functions.h>
#include <csvsqldb/sql_astdump.h>
#include <csvsqldb/sql_parser.h>

#include <catch2/catch.hpp>

#include <sstream>


namespace
{
  void dump(csvsqldb::ASTNodePtr node, std::stringstream& ss)
  {
    csvsqldb::ASTNodeDumpVisitor visitor{ss};
    node->accept(visitor);
  }
}

TEST_CASE("AST Node Dump Test", "[engine]")
{
  csvsqldb::FunctionRegistry functions;
  csvsqldb::initBuildInFunctions(functions);
  csvsqldb::SQLParser parser(functions);
  std::stringstream ss;

  SECTION("dump create table")
  {
    csvsqldb::ASTNodePtr node = parser.parse(
      "CREATE TABLE employees(emp_no INTEGER,birth_date DATE NOT NULL,first_name VARCHAR(25) NOT NULL,last_name VARCHAR(50) "
      "NOT NULL,gender CHAR,hire_date DATE,PRIMARY KEY(emp_no))");
    dump(node, ss);

    auto expected = R"(ASTCreateTable
  EMPLOYEES ->
    EMP_NO -> INTEGER
    BIRTH_DATE -> DATE Not Null
    FIRST_NAME -> VARCHAR length 25 Not Null
    LAST_NAME -> VARCHAR length 50 Not Null
    GENDER -> VARCHAR
    HIRE_DATE -> DATE
    primary keys [ EMP_NO  ]

)";
    CHECK(expected == ss.str());
  }
  SECTION("dump alter table add")
  {
    csvsqldb::ASTNodePtr node = parser.parse("ALTER TABLE Test ADD COLUMN test INT DEFAULT 42 NOT NULL");
    dump(node, ss);

    auto expected = R"(ASTAlterTableAdd
  TEST -> INTEGER Not Null Default 42
)";
    CHECK(expected == ss.str());
  }
  SECTION("dump alter table drop")
  {
    csvsqldb::ASTNodePtr node = parser.parse("ALTER TABLE Test DROP COLUMN test");
    dump(node, ss);

    auto expected = R"(ASTAlterTableDrop
  drop column TEST
)";
    CHECK(expected == ss.str());
  }
  SECTION("dump drop table")
  {
    csvsqldb::ASTNodePtr node = parser.parse("DROP TABLE Test");
    dump(node, ss);

    auto expected = R"(ASTDropTable
  drop table TEST
)";
    CHECK(expected == ss.str());
  }
  SECTION("dump drop mapping")
  {
    csvsqldb::ASTNodePtr node = parser.parse("DROP MAPPING Test");
    dump(node, ss);

    auto expected = R"(ASTDropMappingNode
  TEST
)";
    CHECK(expected == ss.str());
  }
  SECTION("dump create mapping")
  {
    csvsqldb::ASTNodePtr node = parser.parse("CREATE MAPPING EMPLOYEES(\"employees\\d*.csv\")");
    dump(node, ss);

    auto expected = R"(ASTMappingNode
  EMPLOYEES ->
    employees\d*.csv
)";
    CHECK(expected == ss.str());
  }
  SECTION("dump explain")
  {
    csvsqldb::ASTNodePtr node = parser.parse("explain ast select * from Test as t where birthday = DATE'1970-09-23'");
    dump(node, ss);

    auto expected = R"(ASTExplainNode
  Type AST
  Query: SELECT * FROM TEST AS T WHERE (BIRTHDAY = DATE'1970-09-23')
)";
    CHECK(expected == ss.str());
  }
  SECTION("dump union select")
  {
    csvsqldb::ASTNodePtr node = parser.parse(
      "SELECT A,B,C FROM TEST WHERE (B = DATE'1970-09-23') UNION (SELECT A,B,C FROM TEST WHERE (((A > B) AND (C < A)) OR (B "
      "IS NOT TRUE)) GROUP BY A,B,C) UNION (SELECT A,B,C FROM TEST WHERE ((A > B) AND C in (2,3,4,5,6)))");
    dump(node, ss);

    auto expected = R"(ASTUnion
  ASTUnion
    ASTQuerySpecification
        ASTIdentifier -> A
        ASTIdentifier -> B
        ASTIdentifier -> C
      ASTTableExpression
        ASTFrom
          ASTTableIdentifier
            TEST
        ASTWhere
          ASTBinary
            =
              ASTIdentifier -> B
              ASTValue -> DATE DATE'1970-09-23'
    ASTQuerySpecification
        ASTIdentifier -> A
        ASTIdentifier -> B
        ASTIdentifier -> C
      ASTTableExpression
        ASTFrom
          ASTTableIdentifier
            TEST
        ASTWhere
          ASTBinary
            OR
              ASTBinary
                AND
                  ASTBinary
                    >
                      ASTIdentifier -> A
                      ASTIdentifier -> B
                  ASTBinary
                    <
                      ASTIdentifier -> C
                      ASTIdentifier -> A
              ASTBinary
                IS NOT
                  ASTIdentifier -> B
                  ASTValue -> BOOLEAN TRUE
        ASTGroupBy
          ASTIdentifier -> A
          ASTIdentifier -> B
          ASTIdentifier -> C
  ASTQuerySpecification
      ASTIdentifier -> A
      ASTIdentifier -> B
      ASTIdentifier -> C
    ASTTableExpression
      ASTFrom
        ASTTableIdentifier
          TEST
      ASTWhere
        ASTBinary
          AND
            ASTBinary
              >
                ASTIdentifier -> A
                ASTIdentifier -> B
            ASTIn
              ASTIdentifier -> C
              in
                ASTValue -> INTEGER 2
                ASTValue -> INTEGER 3
                ASTValue -> INTEGER 4
                ASTValue -> INTEGER 5
                ASTValue -> INTEGER 6
)";
    CHECK(expected == ss.str());
  }
  SECTION("dump select group order")
  {
    csvsqldb::ASTNodePtr node = parser.parse(
      "select count(*) as \"COUNT\", iso_region from airport where iso_region like 'DE%' group by iso_region order by \"COUNT\"");
    dump(node, ss);

    auto expected = R"(ASTQuerySpecification
    ASTAggregateFunction
      COUNT(
        *
      )
    ASTIdentifier -> ISO_REGION
  ASTTableExpression
    ASTFrom
      ASTTableIdentifier
        AIRPORT
    ASTWhere
      ASTLike
        ASTIdentifier -> ISO_REGION
        'DE%'
    ASTGroupBy
      ASTIdentifier -> ISO_REGION
    ASTOrderBy
      ASC
      ASTIdentifier -> COUNT alias: COUNT
)";
    CHECK(expected == ss.str());
  }
  SECTION("dump select function")
  {
    csvsqldb::ASTNodePtr node = parser.parse("select EXTRACT(MONTH FROM CURRENT_DATE) FROM SYSTEM_DUAL");
    dump(node, ss);

    auto expected = R"(ASTQuerySpecification
    ASTFunction
      EXTRACT(
        ASTValue -> INTEGER 5
        ASTFunction
          CURRENT_DATE(
          )
      )
  ASTTableExpression
    ASTFrom
      ASTTableIdentifier
        SYSTEM_DUAL
)";
    CHECK(expected == ss.str());
  }
  SECTION("dump select between")
  {
    csvsqldb::ASTNodePtr node = parser.parse("select a from test where a between 3 and 15");
    dump(node, ss);

    auto expected = R"(ASTQuerySpecification
    ASTIdentifier -> A
  ASTTableExpression
    ASTFrom
      ASTTableIdentifier
        TEST
    ASTWhere
      ASTBetween
        ASTIdentifier -> A
        between
          ASTValue -> INTEGER 3
        and
          ASTValue -> INTEGER 15
)";
    CHECK(expected == ss.str());
  }
  SECTION("dump select cast")
  {
    csvsqldb::ASTNodePtr node = parser.parse("SELECT a,CAST(b AS VARCHAR(255)) AS bs FROM Test");
    dump(node, ss);

    auto expected = R"(ASTQuerySpecification
    ASTIdentifier -> A
    ASTUnary
      CAST
        ASTIdentifier -> B
        AS VARCHAR
  ASTTableExpression
    ASTFrom
      ASTTableIdentifier
        TEST
)";
    CHECK(expected == ss.str());
  }
  SECTION("dump select cross join")
  {
    csvsqldb::ASTNodePtr node = parser.parse("SELECT * FROM employee as emp CROSS JOIN department as dept");
    dump(node, ss);

    auto expected = R"(ASTQuerySpecification
    ASTQualifiedAsterisk -> *
  ASTTableExpression
    ASTFrom
      ASTCrossJoin
        ASTTableIdentifier
          EMPLOYEE alias: EMP
        JOIN
        ASTTableIdentifier
          DEPARTMENT alias: DEPT

)";
    CHECK(expected == ss.str());
  }
  SECTION("dump select inner join")
  {
    csvsqldb::ASTNodePtr node =
      parser.parse("SELECT * FROM employee INNER JOIN department ON employee.DepartmentID = department.DepartmentID");
    dump(node, ss);

    auto expected = R"(ASTQuerySpecification
    ASTQualifiedAsterisk -> *
  ASTTableExpression
    ASTFrom
      ASTInnerJoin
        ASTTableIdentifier
          EMPLOYEE
        JOIN
        ASTTableIdentifier
          DEPARTMENT
        ON
          ASTBinary
            =
              ASTIdentifier -> EMPLOYEE.DEPARTMENTID
              ASTIdentifier -> DEPARTMENT.DEPARTMENTID

)";
    CHECK(expected == ss.str());
  }
  SECTION("dump select natural join")
  {
    csvsqldb::ASTNodePtr node = parser.parse("SELECT * FROM employee NATURAL JOIN department");
    dump(node, ss);

    auto expected = R"(ASTQuerySpecification
    ASTQualifiedAsterisk -> *
  ASTTableExpression
    ASTFrom
      ASTNaturalJoin
        ASTTableIdentifier
          EMPLOYEE
        JOIN
        ASTTableIdentifier
          DEPARTMENT

)";
    CHECK(expected == ss.str());
  }
  SECTION("dump select left outer join")
  {
    csvsqldb::ASTNodePtr node =
      parser.parse("SELECT * FROM employee LEFT OUTER JOIN department ON employee.DepartmentID = department.DepartmentID");
    dump(node, ss);

    auto expected = R"(ASTQuerySpecification
    ASTQualifiedAsterisk -> *
  ASTTableExpression
    ASTFrom
      ASTLeftJoin
        ASTTableIdentifier
          EMPLOYEE
        JOIN
        ASTTableIdentifier
          DEPARTMENT
        ON
          ASTBinary
            =
              ASTIdentifier -> EMPLOYEE.DEPARTMENTID
              ASTIdentifier -> DEPARTMENT.DEPARTMENTID

)";
    CHECK(expected == ss.str());
  }
  SECTION("dump select right outer join")
  {
    csvsqldb::ASTNodePtr node =
      parser.parse("SELECT * FROM employee RIGHT JOIN department ON employee.DepartmentID = department.DepartmentID");
    dump(node, ss);

    auto expected = R"(ASTQuerySpecification
    ASTQualifiedAsterisk -> *
  ASTTableExpression
    ASTFrom
      ASTRightJoin
        ASTTableIdentifier
          EMPLOYEE
        JOIN
        ASTTableIdentifier
          DEPARTMENT
        ON
          ASTBinary
            =
              ASTIdentifier -> EMPLOYEE.DEPARTMENTID
              ASTIdentifier -> DEPARTMENT.DEPARTMENTID

)";
    CHECK(expected == ss.str());
  }
  SECTION("dump select full outer join")
  {
    csvsqldb::ASTNodePtr node =
      parser.parse("SELECT * FROM employee FULL OUTER JOIN department ON employee.DepartmentID = department.DepartmentID");
    dump(node, ss);

    auto expected = R"(ASTQuerySpecification
    ASTQualifiedAsterisk -> *
  ASTTableExpression
    ASTFrom
      ASTFullJoin
        ASTTableIdentifier
          EMPLOYEE
        JOIN
        ASTTableIdentifier
          DEPARTMENT
        ON
          ASTBinary
            =
              ASTIdentifier -> EMPLOYEE.DEPARTMENTID
              ASTIdentifier -> DEPARTMENT.DEPARTMENTID

)";
    CHECK(expected == ss.str());
  }
  SECTION("dump select limit")
  {
    csvsqldb::ASTNodePtr node = parser.parse("select * from Test limit 10");
    dump(node, ss);

    auto expected = R"(ASTQuerySpecification
    ASTQualifiedAsterisk -> *
  ASTTableExpression
    ASTFrom
      ASTTableIdentifier
        TEST
    ASTLimit
      ASTValue -> INTEGER 10
)";
    CHECK(expected == ss.str());
  }
}
