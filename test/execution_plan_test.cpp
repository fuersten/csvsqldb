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


#include "csvsqldb/execution_plan_creator.h"
#include "csvsqldb/operatornode_factory.h"
#include "csvsqldb/sql_parser.h"
#include "csvsqldb/validation_visitor.h"

#include "data_test_framework.h"

#include <catch2/catch.hpp>

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;


namespace
{
  csvsqldb::ASTNodePtr createExecutionPlan(csvsqldb::ExecutionPlan& execPlan, const std::string& sql,
                                           csvsqldb::OperatorContext& context, std::ostream& output)
  {
    csvsqldb::ASTValidationVisitor validationVisitor(context._database);
    csvsqldb::SQLParser parser(context._functions);
    csvsqldb::ASTNodePtr node = parser.parse(sql);
    REQUIRE(node);
    node->accept(validationVisitor);
    csvsqldb::ExecutionPlanVisitor<TestOperatorNodeFactory> execVisitor(context, execPlan, output);
    node->accept(execVisitor);
    return node;
  }
}

TEST_CASE("Execution Plan Test", "[engine]")
{
  DatabaseTestWrapper dbWrapper;
  dbWrapper.addTable(TableInitializer("test", {{"id", csvsqldb::INT}, {"name", csvsqldb::STRING}}));

  TestRowProvider::setRows("test", {{815, "Mark"}, {4711, "Lars"}});

  csvsqldb::FunctionRegistry functions;
  csvsqldb::BlockManager blockManager;
  std::stringstream output;
  csvsqldb::OperatorContext context(dbWrapper.getDatabase(), functions, blockManager, {});
  csvsqldb::ExecutionPlan execPlan;

  SECTION("plan execute")
  {
    auto node = createExecutionPlan(execPlan, "SELECT * FROM test", context, output);
    CHECK(2 == execPlan.execute());

    std::string expected =
      R"(#TEST.ID,TEST.NAME
815,'Mark'
4711,'Lars'
)";
    CHECK(expected == output.str());
  }
  SECTION("plan dump")
  {
    auto node = createExecutionPlan(execPlan, "SELECT * FROM test", context, output);
    execPlan.dump(output);

    std::string expected =
      R"(OutputRowOperator (TEST.ID,TEST.NAME)
-->ExtendedProjectionOperator (TEST.ID,TEST.NAME)
-->TestScanOperatorNode
)";
    CHECK(expected == output.str());
  }
  SECTION("plan explain ast")
  {
    auto node = createExecutionPlan(execPlan, "EXPLAIN AST SELECT * FROM test", context, output);
    CHECK(0 == execPlan.execute());

    std::string expected =
      R"(ASTQuerySpecification
    ASTQualifiedAsterisk -> *
  ASTTableExpression
    ASTFrom
      ASTTableIdentifier
        TEST

)";
    CHECK(expected == output.str());
  }
  SECTION("plan explain exec")
  {
    auto node = createExecutionPlan(execPlan, "EXPLAIN EXEC SELECT * FROM test", context, output);
    CHECK(0 == execPlan.execute());

    std::string expected =
      R"(OutputRowOperator (TEST.ID,TEST.NAME)
-->ExtendedProjectionOperator (TEST.ID,TEST.NAME)
-->TableScanOperator (TEST)
)";
    CHECK(expected == output.str());
  }
}
