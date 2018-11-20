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

#include "libcsvsqldb/buildin_functions.h"
#include "libcsvsqldb/sql_astexpressionvisitor.h"
#include "libcsvsqldb/sql_astdump.h"
#include "libcsvsqldb/sql_parser.h"
#include "libcsvsqldb/validation_visitor.h"


class SQLParserTestCase
{
public:
    SQLParserTestCase()
    {
    }

    void setUp()
    {
    }

    void tearDown()
    {
    }

    void parseSelect()
    {
        csvsqldb::ASTNodeSQLPrintVisitor visitor;
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        csvsqldb::ASTNodePtr node = parser.parse("select * from Test as t where birthday = DATE'1970-09-23'");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("SELECT * FROM TEST AS T WHERE (BIRTHDAY = DATE'1970-09-23')", visitor.toString());
        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("select t.* from Test as t where birthday = DATE'1970-09-23'");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("SELECT T.* FROM TEST AS T WHERE (BIRTHDAY = DATE'1970-09-23')", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SeLeCT t.* FRoM Test as t;");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("SELECT T.* FROM TEST AS T", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("(SELECT count(*),a,b as d,c FROM Test where a > b and c < a group by a,b,c)");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("SELECT COUNT(*),A,B AS D,C FROM TEST WHERE ((A > B) AND (C <= A)) GROUP BY A,B,C", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT a,CAST(b AS VARCHAR(255)) AS bs,c FROM Test where a > b and c < a group by a,bs,c");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("SELECT A,CAST(B AS VARCHAR) AS BS,C FROM TEST WHERE ((A > B) AND (C <= A)) GROUP BY A,BS,C", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT count(*),a,b,c FROM Test where a > b and c < a or b is not true group by a,b,c");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //        ASTNodeDumpVisitor visitor;
        //        std::cout << std::endl;
        //        node->accept(visitor);

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("SELECT COUNT(*),A,B,C FROM TEST WHERE (((A > B) AND (C <= A)) OR (B IS NOT TRUE)) GROUP BY A,B,C",
                             visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT ALL count(*),a ff,b,c FROM Test where a > b and c < a group by a,b,c");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("SELECT COUNT(*),A AS FF,B,C FROM TEST WHERE ((A > B) AND (C <= A)) GROUP BY A,B,C", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT ALL sum(DISTINCT b),a ff,b,c FROM Test where a > b and c < a group by a,b,c");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("SELECT SUM(DISTINCT B),A AS FF,B,C FROM TEST WHERE ((A > B) AND (C <= A)) GROUP BY A,B,C", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT ALL count(DISTINCT b),a ff,b,c FROM Test where a > b and c < a group by a,b,c");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("SELECT COUNT(DISTINCT B),A AS FF,B,C FROM TEST WHERE ((A > B) AND (C <= A)) GROUP BY A,B,C", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse(
        "SELECT *,Test.*,\"a is cool\" ff,Test.b,c FROM Test where \"a is cool\" > b and c < \"a is cool\" group by \"a is "
        "cool\",b,c");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL(
        "SELECT *,TEST.*,\"A IS COOL\" AS FF,TEST.B,C FROM TEST WHERE ((\"A IS COOL\" > B) AND (C <= \"A IS COOL\")) GROUP BY "
        "\"A IS COOL\",B,C",
        visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT a,b,c FROM Test where a > b and c in (2,3,4,5,6)");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("SELECT A,B,C FROM TEST WHERE ((A > B) AND C in (2,3,4,5,6))", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT a,b,c FROM Test where a > b and c is not null");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));
    }

    void parseSelectExpression()
    {
        csvsqldb::ASTNodeSQLPrintVisitor visitor;
        csvsqldb::FunctionRegistry functions;
        csvsqldb::initBuildInFunctions(functions);
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        csvsqldb::ASTNodePtr node = parser.parse("SELECT CURRENT_TIMESTAMP FROM SYSTEM_DUAL");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT CURRENT_TIMESTAMP() FROM SYSTEM_DUAL");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT 3+6 FROM SYSTEM_DUAL");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));
    }

    void parseComment()
    {
        csvsqldb::ASTNodeSQLPrintVisitor visitor;
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        csvsqldb::ASTNodePtr node =
        parser.parse("--select * from Test as t where birthday = DATE'1970-09-23'\nSeLeCT t.* FRoM Test as t");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //        ASTNodeDumpVisitor dumpvisitor;
        //        std::cout << std::endl;
        //        node->accept(dumpvisitor);

        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("SELECT T.* FROM TEST AS T", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));
    }

    void parseUnion()
    {
        csvsqldb::ASTNodeSQLPrintVisitor visitor;
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        csvsqldb::ASTNodePtr node = parser.parse(
        "SELECT a,b,c FROM Test WHERE b = DATE'1970-09-23' UNION ALL (SELECT a,b,c FROM Test WHERE a > b AND c < a OR b IS NOT "
        "TRUE GROUP BY a,b,c) UNION (SELECT a,b,c FROM Test WHERE a > b AND c IN (2,3,4,5,6))");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL(
        "SELECT A,B,C FROM TEST WHERE (B = DATE'1970-09-23') UNION (SELECT A,B,C FROM TEST WHERE (((A > B) AND (C <= A)) OR (B "
        "IS NOT TRUE)) GROUP BY A,B,C) UNION (SELECT A,B,C FROM TEST WHERE ((A > B) AND C in (2,3,4,5,6)))",
        visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));
    }

    void parseExplain()
    {
        csvsqldb::ASTNodeSQLPrintVisitor visitor;
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        csvsqldb::ASTNodePtr node = parser.parse("explain ast select * from Test as t where birthday = DATE'1970-09-23'");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTExplainNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("EXPLAIN AST SELECT * FROM TEST AS T WHERE (BIRTHDAY = DATE'1970-09-23')", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTExplainNode>(node));
    }

    void parseSelectFail()
    {
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MPF_TEST_EXPECTS(parser.parse("SELECT a,*,b,c FROM Test where a > b and c in (2,3,4,5,6)"), csvsqldb::SqlParserException);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MPF_TEST_EXPECTS(parser.parse("SELECT emp_no AS id FROM employees UINON (SELECT emp_no AS id FROM salaries)"), csvsqldb::SqlParserException);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MPF_TEST_EXPECTS(parser.parse("SELECT a,sum(*),b,c FROM Test where a > b and c in (2,3,4,5,6)"), csvsqldb::SqlParserException);
    }

    void parseComplexSelect()
    {
        csvsqldb::ASTNodeSQLPrintVisitor visitor;
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        csvsqldb::ASTNodePtr node =
        parser.parse("SELECT emp_no,first_name || ' ' || last_name as name FROM employees WHERE last_name='Tsukuda'");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL(
        "SELECT EMP_NO,((FIRST_NAME || ' ') || LAST_NAME) AS NAME FROM EMPLOYEES WHERE (LAST_NAME = 'Tsukuda')", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT count(*) as counter FROM employees");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("SELECT COUNT(*) AS COUNTER FROM EMPLOYEES", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));
    }

    void parseMutliStatement()
    {
        csvsqldb::ASTNodeSQLPrintVisitor visitor;
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        csvsqldb::ASTNodePtr node = parser.parse(
        "select * from Test as t where birthday = DATE'1970-09-23'; SELECT *,Test.*,a ff,Test.b,c FROM Test where a > b and c < "
        "a group by a,b,c");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        node->accept(visitor);
        csvsqldb::SQLParser parser2(functions);
        csvsqldb::ASTNodePtr node2 = parser2.parse(visitor.toString());
        MPF_TEST_ASSERT(node2);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node2));

        node = parser.parse();
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));
    }

    void parseJoin()
    {
        csvsqldb::ASTNodeSQLPrintVisitor visitor;
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        csvsqldb::ASTNodePtr node = parser.parse("SELECT * FROM employee as emp, department as dept;");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("SELECT * FROM EMPLOYEE AS EMP,DEPARTMENT AS DEPT", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT * FROM employee as emp CROSS JOIN department as dept;");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("SELECT * FROM EMPLOYEE AS EMP CROSS JOIN DEPARTMENT AS DEPT", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT * FROM employee INNER JOIN department ON employee.DepartmentID = department.DepartmentID;");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("SELECT * FROM EMPLOYEE INNER JOIN DEPARTMENT ON (EMPLOYEE.DEPARTMENTID = DEPARTMENT.DEPARTMENTID)",
                             visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT * FROM employee NATURAL JOIN department;");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("SELECT * FROM EMPLOYEE NATURAL JOIN DEPARTMENT", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node =
        parser.parse("SELECT * FROM employee LEFT OUTER JOIN department ON employee.DepartmentID = department.DepartmentID;");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL(
        "SELECT * FROM EMPLOYEE LEFT OUTER JOIN DEPARTMENT ON (EMPLOYEE.DEPARTMENTID = DEPARTMENT.DEPARTMENTID)", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT * FROM employee RIGHT JOIN department ON employee.DepartmentID = department.DepartmentID;");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL(
        "SELECT * FROM EMPLOYEE RIGHT OUTER JOIN DEPARTMENT ON (EMPLOYEE.DEPARTMENTID = DEPARTMENT.DEPARTMENTID)", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node =
        parser.parse("SELECT * FROM employee FULL OUTER JOIN department ON employee.DepartmentID = department.DepartmentID;");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL(
        "SELECT * FROM EMPLOYEE FULL OUTER JOIN DEPARTMENT ON (EMPLOYEE.DEPARTMENTID = DEPARTMENT.DEPARTMENTID)", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT * FROM employee NATURAL FULL OUTER JOIN department;");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("SELECT * FROM EMPLOYEE NATURAL FULL OUTER JOIN DEPARTMENT", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse(R"(SELECT emp.first_name,emp.last_name,dept_emp.dept_no,departments.dept_name
        FROM employees as emp
        JOIN dept_emp
        ON emp.emp_no = dept_emp.emp_no
        JOIN departments
        ON dept_emp.dept_no = departments.dept_no)");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL(
        "SELECT EMP.FIRST_NAME,EMP.LAST_NAME,DEPT_EMP.DEPT_NO,DEPARTMENTS.DEPT_NAME FROM EMPLOYEES AS EMP INNER JOIN DEPT_EMP ON "
        "(EMP.EMP_NO = DEPT_EMP.EMP_NO) INNER JOIN DEPARTMENTS ON (DEPT_EMP.DEPT_NO = DEPARTMENTS.DEPT_NO)",
        visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //        ASTNodeDumpVisitor visitor;
        //        std::cout << std::endl;
        //        node->accept(visitor);
    }

    void parseWithOrder()
    {
        csvsqldb::ASTNodeSQLPrintVisitor visitor;
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        csvsqldb::ASTNodePtr node = parser.parse(
        "SELECT emp_no,first_name || ' ' || last_name as name FROM employees WHERE last_name='Tsukuda' order by name DESC, "
        "emp_no");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL(
        "SELECT EMP_NO,((FIRST_NAME || ' ') || LAST_NAME) AS NAME FROM EMPLOYEES WHERE (LAST_NAME = 'Tsukuda') ORDER BY NAME "
        "DESC,EMP_NO ASC",
        visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));
    }

    void parseWithLimit()
    {
        csvsqldb::ASTNodeSQLPrintVisitor visitor;
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        csvsqldb::ASTNodePtr node = parser.parse("SELECT * FROM employees WHERE emp_no > 490000 LIMIT 10 OFFSET 100");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        MPF_TEST_ASSERTEQUAL("SELECT * FROM EMPLOYEES WHERE (EMP_NO > 490000) LIMIT 10 OFFSET 100", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));
    }

    void parseSubquery()
    {
        csvsqldb::ASTNodeSQLPrintVisitor visitor;
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        csvsqldb::ASTNodePtr node = parser.parse("(SELECT * FROM (SELECT a,b FROM test) as t);");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        // TODO LCF: the alias is missing here
        MPF_TEST_ASSERTEQUAL("SELECT * FROM (SELECT A,B FROM TEST)", visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse(
        "SELECT * FROM (SELECT a,b FROM test) as t JOIN (SELECT * FROM department WHERE name = 'Marketing') ON "
        "employee.DepartmentID = department.DepartmentID;");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));

        visitor.reset();
        node->accept(visitor);
        // TODO LCF: the alias is missing here
        MPF_TEST_ASSERTEQUAL(
        "SELECT * FROM (SELECT A,B FROM TEST) INNER JOIN (SELECT * FROM DEPARTMENT WHERE (NAME = 'Marketing')) ON "
        "(EMPLOYEE.DEPARTMENTID = DEPARTMENT.DEPARTMENTID)",
        visitor.toString());

        node = parser.parse(visitor.toString());
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTQueryNode>(node));
    }

    void parseCreateTable()
    {
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        csvsqldb::ASTNodePtr node = parser.parse(
        "CREATE TABLE Test(id INTEGER PRIMARY KEY, name CHAR VARYING(255) DEFAULT 'Lars', adult BOOLEAN, loan FLOAT NOT NULL "
        "CHECK(loan > 100.0), sex CHAR)");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse(
        "create table if not exists Test(id integer primary key, name varchar(255) default 'Lars', age integer check (age >= "
        "18), constraint pk primary key (id,name),check (id > 4711 AND name = 'Lars'))");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node));
    }

    void parseCreateTableFail()
    {
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MPF_TEST_EXPECTS(parser.parse("CREATE TABLE Test(id INTEGER PRIMARY KEY, CHAR)"), csvsqldb::SqlParserException);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MPF_TEST_EXPECTS(parser.parse(
                         "CREATE TABLE Test(id INTEGER PRIMARY KEY, adult, loan FLOAT NOT NULL CHECK(loan > 100.0))"),
                         csvsqldb::SqlParserException);
    }

    void parseAlterTable()
    {
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        csvsqldb::ASTNodePtr node = parser.parse("ALTER TABLE Test ADD COLUMN test INT NOT NULL");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTAlterTableAddNode>(node));

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("ALTER TABLE Test DROP COLUMN test");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTAlterTableDropNode>(node));
    }

    void parseAlterFailTable()
    {
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MPF_TEST_EXPECTS(parser.parse("ALTER TABLE Test ADD COLUMN"), csvsqldb::SqlParserException);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MPF_TEST_EXPECTS(parser.parse("ALTER TABLE Test DROP COLUMN test INT NOT NULL"), csvsqldb::SqlParserException);
    }

    void parseDropTable()
    {
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        csvsqldb::ASTNodePtr node = parser.parse("DROP TABLE Test");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTDropTableNode>(node));
    }

    void parseDropFailTable()
    {
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MPF_TEST_EXPECTS(parser.parse("DROP TABLE"), csvsqldb::SqlParserException);
    }

    void parseMapping()
    {
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        csvsqldb::ASTNodePtr node = parser.parse("CREATE MAPPING EMPLOYEES(\"employees\\d*.csv\")");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTMappingNode>(node));
    }

    void parseMappingFail()
    {
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MPF_TEST_EXPECTS(parser.parse("CREATE MAPPING EMPLOYEES(\"employees\\d*.csv\", \"employees_test.csv\")"), csvsqldb::SqlParserException);
    }

    void parseExpression()
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
        MPF_TEST_ASSERTEQUAL(sql, visitor.toString());

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        exp = parser.parseExpression("a between 3 and 15");
        visitor.reset();
        exp->accept(visitor);
        sql = visitor.toString();
        re_exp = parser.parseExpression(sql);
        visitor.reset();
        exp->accept(visitor);
        MPF_TEST_ASSERTEQUAL(sql, visitor.toString());
    }

    void validateParsedSelect()
    {
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);

        csvsqldb::Database database("/tmp", csvsqldb::FileMapping());

        csvsqldb::ASTNodePtr node = parser.parse(
        "CREATE TABLE employees(emp_no INTEGER,birth_date DATE NOT NULL,first_name VARCHAR(25) NOT NULL,last_name VARCHAR(50) "
        "NOT NULL,gender CHAR,hire_date DATE,PRIMARY KEY(emp_no))");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node));
        csvsqldb::ASTCreateTableNodePtr createNode = std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node);
        csvsqldb::TableData tabledata = csvsqldb::TableData::fromCreateAST(createNode);

        database.addTable(tabledata);

        node = parser.parse(
        "CREATE TABLE salaries(emp_no INTEGER PRIMARY KEY,salary FLOAT CHECK(salary > 0.0),from_date DATE,to_date DATE)");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node));
        createNode = std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node);
        tabledata = csvsqldb::TableData::fromCreateAST(createNode);

        database.addTable(tabledata);

        csvsqldb::ASTValidationVisitor validationVisitor(database);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT * FROM employees WHERE emp_no > 490000");
        MPF_TEST_ASSERT(node);
        node->accept(validationVisitor);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT * FROM employees");
        MPF_TEST_ASSERT(node);
        node->accept(validationVisitor);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT * FROM employees WHERE emp_no + 1");
        MPF_TEST_ASSERT(node);
        MPF_TEST_EXPECTS(node->accept(validationVisitor), csvsqldb::SqlParserException);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        node = parser.parse("SELECT * FROM employees e, salaries s WHERE e.emp_no = s.emp_no and s.salary > 20000");
        MPF_TEST_ASSERT(node);
        MPF_TEST_EXPECTS(node->accept(validationVisitor), csvsqldb::SqlParserException);
    }

    void createMappingTest()
    {
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);
        csvsqldb::ASTNodePtr node = parser.parse("create mapping test('test.csv', ';', true)");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTMappingNode>(node));
        csvsqldb::ASTMappingNodePtr mapping = std::dynamic_pointer_cast<csvsqldb::ASTMappingNode>(node);
        MPF_TEST_ASSERTEQUAL("test.csv", mapping->_mappings[0]._mapping);
        MPF_TEST_ASSERTEQUAL(';', mapping->_mappings[0]._delimiter);
        MPF_TEST_ASSERTEQUAL(true, mapping->_mappings[0]._skipFirstLine);

        node = parser.parse("create mapping test('test.csv')");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTMappingNode>(node));
        mapping = std::dynamic_pointer_cast<csvsqldb::ASTMappingNode>(node);
        MPF_TEST_ASSERTEQUAL("test.csv", mapping->_mappings[0]._mapping);
        MPF_TEST_ASSERTEQUAL(',', mapping->_mappings[0]._delimiter);
        MPF_TEST_ASSERTEQUAL(false, mapping->_mappings[0]._skipFirstLine);

        node = parser.parse("create mapping test('test.csv',';')");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTMappingNode>(node));
        mapping = std::dynamic_pointer_cast<csvsqldb::ASTMappingNode>(node);
        MPF_TEST_ASSERTEQUAL("test.csv", mapping->_mappings[0]._mapping);
        MPF_TEST_ASSERTEQUAL(';', mapping->_mappings[0]._delimiter);
        MPF_TEST_ASSERTEQUAL(false, mapping->_mappings[0]._skipFirstLine);
    }

    void createBadMappingTest()
    {
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);
        MPF_TEST_EXPECTS(parser.parse("create mapping test('test.csv', true)"), csvsqldb::SqlParserException);
    }

    void dropMappingTest()
    {
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);
        csvsqldb::ASTNodePtr node = parser.parse("drop mapping test");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTDropMappingNode>(node));
    }
    
    void loadTest()
    {
        csvsqldb::FunctionRegistry functions;
        csvsqldb::SQLParser parser(functions);
        csvsqldb::ASTNodePtr node = parser.parse("load '~/Downloads/csvsqldb_example/airports.csv' airport");
        MPF_TEST_ASSERT(node);
        MPF_TEST_ASSERT(std::dynamic_pointer_cast<csvsqldb::ASTLoadNode>(node));
    }
};

MPF_REGISTER_TEST_START("SQLParserTestSuite", SQLParserTestCase);
MPF_REGISTER_TEST(SQLParserTestCase::parseSelect);
MPF_REGISTER_TEST(SQLParserTestCase::parseSelectExpression);
MPF_REGISTER_TEST(SQLParserTestCase::parseComment);
MPF_REGISTER_TEST(SQLParserTestCase::parseUnion);
MPF_REGISTER_TEST(SQLParserTestCase::parseExplain);
MPF_REGISTER_TEST(SQLParserTestCase::parseSelectFail);
MPF_REGISTER_TEST(SQLParserTestCase::parseComplexSelect);
MPF_REGISTER_TEST(SQLParserTestCase::parseWithOrder);
MPF_REGISTER_TEST(SQLParserTestCase::parseMutliStatement);
MPF_REGISTER_TEST(SQLParserTestCase::parseJoin);
MPF_REGISTER_TEST(SQLParserTestCase::parseWithLimit);
MPF_REGISTER_TEST(SQLParserTestCase::parseSubquery);
MPF_REGISTER_TEST(SQLParserTestCase::parseCreateTable);
MPF_REGISTER_TEST(SQLParserTestCase::parseCreateTableFail);
MPF_REGISTER_TEST(SQLParserTestCase::parseAlterTable);
MPF_REGISTER_TEST(SQLParserTestCase::parseAlterFailTable);
MPF_REGISTER_TEST(SQLParserTestCase::parseDropTable);
MPF_REGISTER_TEST(SQLParserTestCase::parseMapping);
MPF_REGISTER_TEST(SQLParserTestCase::parseMappingFail);
MPF_REGISTER_TEST(SQLParserTestCase::parseExpression);
MPF_REGISTER_TEST(SQLParserTestCase::validateParsedSelect);
MPF_REGISTER_TEST(SQLParserTestCase::createMappingTest);
MPF_REGISTER_TEST(SQLParserTestCase::createBadMappingTest);
MPF_REGISTER_TEST(SQLParserTestCase::dropMappingTest);
MPF_REGISTER_TEST(SQLParserTestCase::loadTest);
MPF_REGISTER_TEST_END();
