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
#include <csvsqldb/sql_astexpressionvisitor.h>
#include <csvsqldb/sql_parser.h>
#include <csvsqldb/validation_visitor.h>

#include "temporary_directory.h"

#include <catch2/catch.hpp>


TEST_CASE("SQL Parser Test", "[sql parser]")
{
  SECTION("parse select")
  {
    csvsqldb::ASTNodeSQLPrintVisitor visitor;
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    csvsqldb::ASTNodePtr node = parser.parse("select * from Test as t where birthday = DATE'1970-09-23'");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    node->accept(visitor);
    CHECK("SELECT * FROM TEST AS T WHERE (BIRTHDAY = DATE'1970-09-23')" == visitor.toString());
    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("select t.* from Test as t where birthday = DATE'1970-09-23'");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT T.* FROM TEST AS T WHERE (BIRTHDAY = DATE'1970-09-23')" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SeLeCT t.* FRoM Test as t;");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT T.* FROM TEST AS T" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("(SELECT count(*),a,b as d,c FROM Test where a > b and c < a group by a,b,c)");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT COUNT(*),A,B AS D,C FROM TEST WHERE ((A > B) AND (C < A)) GROUP BY A,B,C" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT a,CAST(b AS VARCHAR(255)) AS bs,c FROM Test where a > b and c < a group by a,bs,c");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT A,CAST(B AS VARCHAR) AS BS,C FROM TEST WHERE ((A > B) AND (C < A)) GROUP BY A,BS,C" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT count(*),a,b,c FROM Test where a > b and c < a or b is not true group by a,b,c");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //        ASTNodeDumpVisitor visitor;
    //        std::cout << std::endl;
    //        node->accept(visitor);

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT COUNT(*),A,B,C FROM TEST WHERE (((A > B) AND (C < A)) OR (B IS NOT TRUE)) GROUP BY A,B,C" ==
          visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT ALL count(*),a ff,b,c FROM Test where a > b and c <= a group by a,b,c");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT COUNT(*),A AS FF,B,C FROM TEST WHERE ((A > B) AND (C <= A)) GROUP BY A,B,C" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT ALL sum(DISTINCT b),a ff,b,c FROM Test where a > b and c <= a group by a,b,c");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT SUM(DISTINCT B),A AS FF,B,C FROM TEST WHERE ((A > B) AND (C <= A)) GROUP BY A,B,C" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT ALL count(DISTINCT b),a ff,b,c FROM Test where a >= b and c < a group by a,b,c");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT COUNT(DISTINCT B),A AS FF,B,C FROM TEST WHERE ((A >= B) AND (C < A)) GROUP BY A,B,C" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse(
      "SELECT *,Test.*,\"a is cool\" ff,Test.b,c FROM Test where \"a is cool\" > b and c < \"a is cool\" group by \"a is "
      "cool\",b,c");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT *,TEST.*,\"A IS COOL\" AS FF,TEST.B,C FROM TEST WHERE ((\"A IS COOL\" > B) AND (C < \"A IS COOL\")) GROUP BY "
          "\"A IS COOL\",B,C" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT a,b,c FROM Test where a > b and c in (2,3,4,5,6)");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT A,B,C FROM TEST WHERE ((A > B) AND C in (2,3,4,5,6))" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT a,b,c FROM Test where a > b and c is not null");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));
  }

  SECTION("parse select expression")
  {
    csvsqldb::ASTNodeSQLPrintVisitor visitor;
    csvsqldb::FunctionRegistry functions;
    csvsqldb::initBuildInFunctions(functions);
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    csvsqldb::ASTNodePtr node = parser.parse("SELECT CURRENT_TIMESTAMP FROM SYSTEM_DUAL");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT CURRENT_TIMESTAMP() FROM SYSTEM_DUAL");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT 3+6 FROM SYSTEM_DUAL");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));
  }

  SECTION("parse comment")
  {
    csvsqldb::ASTNodeSQLPrintVisitor visitor;
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    csvsqldb::ASTNodePtr node =
      parser.parse("--select * from Test as t where birthday = DATE'1970-09-23'\nSeLeCT t.* FRoM Test as t");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //        ASTNodeDumpVisitor dumpvisitor;
    //        std::cout << std::endl;
    //        node->accept(dumpvisitor);

    node->accept(visitor);
    CHECK("SELECT T.* FROM TEST AS T" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));
  }

  SECTION("parse union")
  {
    csvsqldb::ASTNodeSQLPrintVisitor visitor;
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    csvsqldb::ASTNodePtr node = parser.parse(
      "SELECT a,b,c FROM Test WHERE b = DATE'1970-09-23' UNION ALL (SELECT a,b,c FROM Test WHERE a > b AND c < a OR b IS NOT "
      "TRUE GROUP BY a,b,c) UNION (SELECT a,b,c FROM Test WHERE a > b AND c IN (2,3,4,5,6))");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT A,B,C FROM TEST WHERE (B = DATE'1970-09-23') UNION (SELECT A,B,C FROM TEST WHERE (((A > B) AND (C < A)) OR (B "
          "IS NOT TRUE)) GROUP BY A,B,C) UNION (SELECT A,B,C FROM TEST WHERE ((A > B) AND C in (2,3,4,5,6)))" ==
          visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));
  }

  SECTION("parse explain")
  {
    csvsqldb::ASTNodeSQLPrintVisitor visitor;
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    csvsqldb::ASTNodePtr node = parser.parse("explain ast select * from Test as t where birthday = DATE'1970-09-23'");
    REQUIRE(node);
    CHECK(std::dynamic_pointer_cast<csvsqldb::ASTExplainNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("EXPLAIN AST SELECT * FROM TEST AS T WHERE (BIRTHDAY = DATE'1970-09-23')" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    CHECK(std::dynamic_pointer_cast<csvsqldb::ASTExplainNode>(node));
  }

  SECTION("parse select fail")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CHECK_THROWS_AS(parser.parse("SELECT a,*,b,c FROM Test where a > b and c in (2,3,4,5,6)"), csvsqldb::SqlParserException);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CHECK_THROWS_AS(parser.parse("SELECT emp_no AS id FROM employees UINON (SELECT emp_no AS id FROM salaries)"),
                    csvsqldb::SqlParserException);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CHECK_THROWS_AS(parser.parse("SELECT a,sum(*),b,c FROM Test where a > b and c in (2,3,4,5,6)"), csvsqldb::SqlParserException);
  }

  SECTION("parse complex select")
  {
    csvsqldb::ASTNodeSQLPrintVisitor visitor;
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    csvsqldb::ASTNodePtr node =
      parser.parse("SELECT emp_no,first_name || ' ' || last_name as name FROM employees WHERE last_name='Tsukuda'");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    node->accept(visitor);
    CHECK("SELECT EMP_NO,((FIRST_NAME || ' ') || LAST_NAME) AS NAME FROM EMPLOYEES WHERE (LAST_NAME = 'Tsukuda')" ==
          visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT count(*) as counter FROM employees");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT COUNT(*) AS COUNTER FROM EMPLOYEES" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));
  }

  SECTION("parse multi statement")
  {
    csvsqldb::ASTNodeSQLPrintVisitor visitor;
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    csvsqldb::ASTNodePtr node = parser.parse(
      "select * from Test as t where birthday = DATE'1970-09-23'; SELECT *,Test.*,a ff,Test.b,c FROM Test where a > b and c < "
      "a group by a,b,c");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    node->accept(visitor);
    csvsqldb::SQLParser parser2(functions);
    csvsqldb::ASTNodePtr node2 = parser2.parse(visitor.toString());
    CHECK(node2);
    CHECK(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node2));

    node = parser.parse();
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));
  }

  SECTION("parse join")
  {
    csvsqldb::ASTNodeSQLPrintVisitor visitor;
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    csvsqldb::ASTNodePtr node = parser.parse("SELECT * FROM employee as emp, department as dept;");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT * FROM EMPLOYEE AS EMP,DEPARTMENT AS DEPT" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT * FROM employee as emp CROSS JOIN department as dept;");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT * FROM EMPLOYEE AS EMP CROSS JOIN DEPARTMENT AS DEPT" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT * FROM employee INNER JOIN department ON employee.DepartmentID = department.DepartmentID;");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT * FROM EMPLOYEE INNER JOIN DEPARTMENT ON (EMPLOYEE.DEPARTMENTID = DEPARTMENT.DEPARTMENTID)" ==
          visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT * FROM employee NATURAL JOIN department;");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT * FROM EMPLOYEE NATURAL JOIN DEPARTMENT" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT * FROM employee LEFT OUTER JOIN department ON employee.DepartmentID = department.DepartmentID;");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT * FROM EMPLOYEE LEFT OUTER JOIN DEPARTMENT ON (EMPLOYEE.DEPARTMENTID = DEPARTMENT.DEPARTMENTID)" ==
          visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT * FROM employee RIGHT JOIN department ON employee.DepartmentID = department.DepartmentID;");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT * FROM EMPLOYEE RIGHT OUTER JOIN DEPARTMENT ON (EMPLOYEE.DEPARTMENTID = DEPARTMENT.DEPARTMENTID)" ==
          visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT * FROM employee FULL OUTER JOIN department ON employee.DepartmentID = department.DepartmentID;");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT * FROM EMPLOYEE FULL OUTER JOIN DEPARTMENT ON (EMPLOYEE.DEPARTMENTID = DEPARTMENT.DEPARTMENTID)" ==
          visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT * FROM employee NATURAL FULL OUTER JOIN department;");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT * FROM EMPLOYEE NATURAL FULL OUTER JOIN DEPARTMENT" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse(R"(SELECT emp.first_name,emp.last_name,dept_emp.dept_no,departments.dept_name
        FROM employees as emp
        JOIN dept_emp
        ON emp.emp_no = dept_emp.emp_no
        JOIN departments
        ON dept_emp.dept_no = departments.dept_no)");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK(
      "SELECT EMP.FIRST_NAME,EMP.LAST_NAME,DEPT_EMP.DEPT_NO,DEPARTMENTS.DEPT_NAME FROM EMPLOYEES AS EMP INNER JOIN DEPT_EMP ON "
      "(EMP.EMP_NO = DEPT_EMP.EMP_NO) INNER JOIN DEPARTMENTS ON (DEPT_EMP.DEPT_NO = DEPARTMENTS.DEPT_NO)" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //        ASTNodeDumpVisitor visitor;
    //        std::cout << std::endl;
    //        node->accept(visitor);
  }

  SECTION("parse with order")
  {
    csvsqldb::ASTNodeSQLPrintVisitor visitor;
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    csvsqldb::ASTNodePtr node = parser.parse(
      "SELECT emp_no,first_name || ' ' || last_name as name FROM employees WHERE last_name='Tsukuda' order by name DESC, "
      "emp_no");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT EMP_NO,((FIRST_NAME || ' ') || LAST_NAME) AS NAME FROM EMPLOYEES WHERE (LAST_NAME = 'Tsukuda') ORDER BY NAME "
          "DESC,EMP_NO ASC" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));
  }

  SECTION("parse with limit")
  {
    csvsqldb::ASTNodeSQLPrintVisitor visitor;
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    csvsqldb::ASTNodePtr node = parser.parse("SELECT * FROM employees WHERE emp_no > 490000 LIMIT 10 OFFSET 100");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    CHECK("SELECT * FROM EMPLOYEES WHERE (EMP_NO > 490000) LIMIT 10 OFFSET 100" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));
  }

  SECTION("parse subquery")
  {
    csvsqldb::ASTNodeSQLPrintVisitor visitor;
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    csvsqldb::ASTNodePtr node = parser.parse("(SELECT * FROM (SELECT a,b FROM test) as t);");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    // TODO LCF: the alias is missing here
    CHECK("SELECT * FROM (SELECT A,B FROM TEST)" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT * FROM (SELECT a,b FROM test) as t JOIN (SELECT * FROM department WHERE name = 'Marketing') ON "
                        "employee.DepartmentID = department.DepartmentID;");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

    visitor.reset();
    node->accept(visitor);
    // TODO LCF: the alias is missing here
    CHECK("SELECT * FROM (SELECT A,B FROM TEST) INNER JOIN (SELECT * FROM DEPARTMENT WHERE (NAME = 'Marketing')) ON "
          "(EMPLOYEE.DEPARTMENTID = DEPARTMENT.DEPARTMENTID)" == visitor.toString());

    node = parser.parse(visitor.toString());
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));
  }

  SECTION("parse create table")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    csvsqldb::ASTNodePtr node = parser.parse(
      "CREATE TABLE Test(id INTEGER PRIMARY KEY, name CHAR VARYING(255) DEFAULT 'Lars', adult BOOLEAN, loan FLOAT NOT NULL "
      "CHECK(loan > 100.0), sex CHAR)");
    REQUIRE(node);
    CHECK(std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse(
      "create table if not exists Test(id integer primary key, name varchar(255) default 'Lars', age integer check (age >= "
      "18), constraint pk primary key (id,name),check (id > 4711 AND name = 'Lars'))");
    REQUIRE(node);
    CHECK(std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node));
  }

  SECTION("parse create table fail")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CHECK_THROWS_AS(parser.parse("CREATE TABLE Test(id INTEGER PRIMARY KEY, CHAR)"), csvsqldb::SqlParserException);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CHECK_THROWS_AS(parser.parse("CREATE TABLE Test(id INTEGER PRIMARY KEY, adult, loan FLOAT NOT NULL CHECK(loan > 100.0))"),
                    csvsqldb::SqlParserException);
  }

  SECTION("parse alter table")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    csvsqldb::ASTNodePtr node = parser.parse("ALTER TABLE Test ADD COLUMN test INT NOT NULL");
    REQUIRE(node);
    CHECK(std::dynamic_pointer_cast<csvsqldb::ASTAlterTableAddColumnNode>(node));

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("ALTER TABLE Test DROP COLUMN test");
    REQUIRE(node);
    CHECK(std::dynamic_pointer_cast<csvsqldb::ASTAlterTableDropColumnNode>(node));
  }

  SECTION("parse alter table fail")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CHECK_THROWS_AS(parser.parse("ALTER TABLE Test ADD COLUMN"), csvsqldb::SqlParserException);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CHECK_THROWS_AS(parser.parse("ALTER TABLE Test DROP COLUMN test INT NOT NULL"), csvsqldb::SqlParserException);
  }

  SECTION("parse drop table")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    csvsqldb::ASTNodePtr node = parser.parse("DROP TABLE Test");
    REQUIRE(node);
    CHECK(std::dynamic_pointer_cast<csvsqldb::ASTDropTableNode>(node));
  }

  SECTION("parse drop table fail")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CHECK_THROWS_AS(parser.parse("DROP TABLE"), csvsqldb::SqlParserException);
  }

  SECTION("parse mapping")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    csvsqldb::ASTNodePtr node = parser.parse("CREATE MAPPING EMPLOYEES(\"employees\\d*.csv\")");
    REQUIRE(node);
    CHECK(std::dynamic_pointer_cast<csvsqldb::ASTMappingNode>(node));
  }

  SECTION("parse mapping fail")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CHECK_THROWS_AS(parser.parse("CREATE MAPPING EMPLOYEES(\"employees\\d*.csv\", \"employees_test.csv\")"),
                    csvsqldb::SqlParserException);
  }

  SECTION("parse expression")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    csvsqldb::ASTExprNodePtr exp = parser.parseExpression("a + (37*5) / 3");
    csvsqldb::ASTExpressionVisitor visitor;
    exp->accept(visitor);
    std::string sql = visitor.toString();
    csvsqldb::ASTExprNodePtr re_exp = parser.parseExpression(sql);
    visitor.reset();
    exp->accept(visitor);
    CHECK(sql == visitor.toString());

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    exp = parser.parseExpression("a between 3 and 15");
    visitor.reset();
    exp->accept(visitor);
    sql = visitor.toString();
    re_exp = parser.parseExpression(sql);
    visitor.reset();
    exp->accept(visitor);
    CHECK(sql == visitor.toString());
  }

  SECTION("parse validate select")
  {
    TemporaryDirectoryGuard tmpDir;
    auto path = tmpDir.temporaryDirectoryPath();

    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    csvsqldb::Database database(path, csvsqldb::FileMapping());

    csvsqldb::ASTNodePtr node = parser.parse(
      "CREATE TABLE employees(emp_no INTEGER,birth_date DATE NOT NULL,first_name VARCHAR(25) NOT NULL,last_name VARCHAR(50) "
      "NOT NULL,gender CHAR,hire_date DATE,PRIMARY KEY(emp_no))");
    REQUIRE(node);
    CHECK(std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node));
    csvsqldb::ASTCreateTableNodePtr createNode = std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node);
    csvsqldb::TableData tabledata = csvsqldb::TableData::fromCreateAST(createNode);

    database.addTable(std::move(tabledata), false);

    node = parser.parse(
      "CREATE TABLE salaries(emp_no INTEGER PRIMARY KEY,salary FLOAT CHECK(salary > 0.0),from_date DATE,to_date DATE)");
    REQUIRE(node);
    CHECK(std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node));
    createNode = std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node);
    tabledata = csvsqldb::TableData::fromCreateAST(createNode);

    database.addTable(std::move(tabledata), false);

    csvsqldb::ASTValidationVisitor validationVisitor(database);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT * FROM employees WHERE emp_no > 490000");
    REQUIRE(node);
    node->accept(validationVisitor);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT * FROM employees");
    REQUIRE(node);
    node->accept(validationVisitor);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT * FROM employees WHERE emp_no + 1");
    REQUIRE(node);
    CHECK_THROWS_AS(node->accept(validationVisitor), csvsqldb::SqlParserException);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    node = parser.parse("SELECT * FROM employees e, salaries s WHERE e.emp_no = s.emp_no and s.salary > 20000");
    REQUIRE(node);
    CHECK_THROWS_AS(node->accept(validationVisitor), csvsqldb::SqlParserException);
  }

  SECTION("create mapping")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);
    csvsqldb::ASTNodePtr node = parser.parse("create mapping test('test.csv', ';', true)");
    REQUIRE(node);
    CHECK(std::dynamic_pointer_cast<csvsqldb::ASTMappingNode>(node));
    csvsqldb::ASTMappingNodePtr mapping = std::dynamic_pointer_cast<csvsqldb::ASTMappingNode>(node);
    CHECK("test.csv" == mapping->_mappings[0]._mapping);
    CHECK(';' == mapping->_mappings[0]._delimiter);
    CHECK(mapping->_mappings[0]._skipFirstLine);

    node = parser.parse("create mapping test('test.csv')");
    REQUIRE(node);
    CHECK(std::dynamic_pointer_cast<csvsqldb::ASTMappingNode>(node));
    mapping = std::dynamic_pointer_cast<csvsqldb::ASTMappingNode>(node);
    CHECK("test.csv" == mapping->_mappings[0]._mapping);
    CHECK(',' == mapping->_mappings[0]._delimiter);
    CHECK_FALSE(mapping->_mappings[0]._skipFirstLine);

    node = parser.parse("create mapping test('test.csv',';')");
    REQUIRE(node);
    CHECK(std::dynamic_pointer_cast<csvsqldb::ASTMappingNode>(node));
    mapping = std::dynamic_pointer_cast<csvsqldb::ASTMappingNode>(node);
    CHECK("test.csv" == mapping->_mappings[0]._mapping);
    CHECK(';' == mapping->_mappings[0]._delimiter);
    CHECK_FALSE(mapping->_mappings[0]._skipFirstLine);
  }

  SECTION("create bad mapping")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);
    CHECK_THROWS_AS(parser.parse("create mapping test('test.csv', true)"), csvsqldb::SqlParserException);
  }

  SECTION("drop mapping")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);
    csvsqldb::ASTNodePtr node = parser.parse("drop mapping test");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTDropMappingNode>(node));
  }
}

TEST_CASE("SQL AST Test", "[sql parser]")
{
  SECTION("identifiers")
  {
    csvsqldb::IdentifierSet identifiers;

    csvsqldb::SymbolTablePtr symbolTable = csvsqldb::SymbolTable::createSymbolTable();
    csvsqldb::SymbolInfoPtr info = std::make_shared<csvsqldb::SymbolInfo>();
    info->_name = "BIRTHDATE";

    csvsqldb::ASTIdentifier bd(symbolTable, info, "", "BIRTHDATE", false);

    identifiers.insert(bd);
    CHECK(identifiers.size() == 1);

    csvsqldb::ASTIdentifier id(symbolTable, info, "", "ID", false);
    identifiers.insert(id);
    CHECK(identifiers.size() == 2);
    identifiers.insert(id);
    CHECK(identifiers.size() == 2);

    csvsqldb::ASTIdentifier prefix_bd(symbolTable, info, "EMPLOYEES", "BIRTHDATE", false);
    identifiers.insert(prefix_bd);
    CHECK(identifiers.size() == 3);
  }
}
