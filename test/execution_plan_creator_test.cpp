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

#include <csvsqldb/execution_plan_creator.h>
#include <csvsqldb/operatornode_factory.h>
#include <csvsqldb/sql_parser.h>

#include "data_test_framework.h"

#include <catch2/catch.hpp>


TEST_CASE("Execution Plan Creator Test", "[engine]")
{
  DatabaseTestWrapper dbWrapper;
  dbWrapper.addTable(TableInitializer("test", {{"id", csvsqldb::INT}, {"name", csvsqldb::STRING}}));
  csvsqldb::FunctionRegistry functions;
  csvsqldb::SQLParser parser(functions);
  csvsqldb::BlockManager blockManager;
  csvsqldb::OperatorContext context(dbWrapper.getDatabase(), functions, blockManager, {});
  csvsqldb::ExecutionPlan execPlan;
  std::stringstream output;
  csvsqldb::ExecutionPlanVisitor<TestOperatorNodeFactory> execPlanVisitor(context, execPlan, output);

  SECTION("create table")
  {
    csvsqldb::ASTNodePtr node = parser.parse("CREATE TABLE employees(emp_no INTEGER,first_name VARCHAR(25) NOT NULL)");
    REQUIRE(node);
    node->accept(execPlanVisitor);
    execPlan.dump(output);
    std::string expected = R"(CreateTableExecution (EMPLOYEES -> EMP_NO: INTEGER, FIRST_NAME: VARCHAR))";
    CHECK(expected == output.str());
  }
  SECTION("drop table")
  {
    csvsqldb::ASTNodePtr node = parser.parse("DROP TABLE employees");
    REQUIRE(node);
    node->accept(execPlanVisitor);
    execPlan.dump(output);
    std::string expected = R"(DropTableExecution (EMPLOYEES))";
    CHECK(expected == output.str());
  }
  SECTION("create mapping")
  {
    csvsqldb::ASTNodePtr node = parser.parse("CREATE MAPPING airport('airports\\d*.csv',',',true)");
    REQUIRE(node);
    node->accept(execPlanVisitor);
    execPlan.dump(output);
    std::string expected = R"(CreateMappingExecution (airports\d*.csv -> AIRPORT))";
    CHECK(expected == output.str());
  }
  SECTION("drop mapping")
  {
    csvsqldb::ASTNodePtr node = parser.parse("DROP MAPPING airport");
    REQUIRE(node);
    node->accept(execPlanVisitor);
    execPlan.dump(output);
    std::string expected = R"(DropMappingExecution (AIRPORT))";
    CHECK(expected == output.str());
  }
  SECTION("explain ast")
  {
    csvsqldb::ASTNodePtr node = parser.parse("EXPLAIN AST SELECT * FROM employees");
    REQUIRE(node);
    node->accept(execPlanVisitor);
    execPlan.dump(output);
    std::string expected = R"(ExplainExecution (AST) --> ASTQuerySpecification
    ASTQualifiedAsterisk -> *
  ASTTableExpression
    ASTFrom
      ASTTableIdentifier
        EMPLOYEES

)";
    CHECK(expected == output.str());
  }
  SECTION("explain exec")
  {
    csvsqldb::ASTNodePtr node = parser.parse("EXPLAIN EXEC SELECT * FROM employees");
    REQUIRE(node);
    node->accept(execPlanVisitor);
    execPlan.dump(output);
    std::string expected = R"(ExplainExecution (EXEC) --> ASTQuerySpecification
    ASTQualifiedAsterisk -> *
  ASTTableExpression
    ASTFrom
      ASTTableIdentifier
        EMPLOYEES

)";
    CHECK(expected == output.str());
  }
  SECTION("select")
  {
    csvsqldb::ASTNodePtr node = parser.parse("SELECT id,name FROM test WHERE id > 4711 order by id LIMIT 10");
    REQUIRE(node);
    csvsqldb::ASTValidationVisitor validationVisitor(dbWrapper.getDatabase());
    node->accept(validationVisitor);
    node->accept(execPlanVisitor);
    execPlan.dump(output);
    std::string expected = R"(OutputRowOperator (ID,NAME)
-->LimitOperator (0 -> 10)
-->SortOperator (ID ASC)
-->ExtendedProjectionOperator (ID,NAME)
-->SelectOperator
-->TestScanOperatorNode
)";
    CHECK(expected == output.str());
  }
}
