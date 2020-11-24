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


#include <csvsqldb/operatornode_factory.h>
#include <test/data_test_framework.h>
#include <test/test_util.h>

#include <catch2/catch.hpp>


TEST_CASE("Operatornode Factory Test", "[engine]")
{
  DatabaseTestWrapper wrapper;
  wrapper.addTable(TableInitializer("test", {{"id", csvsqldb::INT}, {"name", csvsqldb::STRING}}));
  csvsqldb::FunctionRegistry functions;
  csvsqldb::BlockManager blockManager;
  csvsqldb::OperatorContext context{wrapper.getDatabase(), functions, blockManager, {}};
  auto symbolTable = csvsqldb::SymbolTable::createSymbolTable();

  SECTION("output row operator")
  {
    std::ostringstream os;
    auto op = csvsqldb::OperatorNodeFactory::createOutputRowOperatorNode(context, symbolTable, os);
    CHECK(std::dynamic_pointer_cast<csvsqldb::OutputRowOperatorNode>(op));
  }
  SECTION("limit operator")
  {
    auto limit = std::make_shared<csvsqldb::ASTValueNode>(symbolTable, csvsqldb::INT, "5");
    auto offset = std::make_shared<csvsqldb::ASTValueNode>(symbolTable, csvsqldb::INT, "1");
    auto op = csvsqldb::OperatorNodeFactory::createLimitOperatorNode(context, symbolTable, limit, offset);
    CHECK(std::dynamic_pointer_cast<csvsqldb::LimitOperatorNode>(op));
  }
  SECTION("sort operator")
  {
    csvsqldb::OrderExpressions orders;
    auto op = csvsqldb::OperatorNodeFactory::createSortOperatorNode(context, symbolTable, orders);
    CHECK(std::dynamic_pointer_cast<csvsqldb::SortOperatorNode>(op));
  }
  SECTION("grouping operator")
  {
    csvsqldb::Expressions expressions;
    csvsqldb::Identifiers identifiers;
    auto op = csvsqldb::OperatorNodeFactory::createGroupingOperatorNode(context, symbolTable, expressions, identifiers);
    CHECK(std::dynamic_pointer_cast<csvsqldb::GroupingOperatorNode>(op));
  }
  SECTION("aggregation operator")
  {
    csvsqldb::Expressions expressions;
    auto op = csvsqldb::OperatorNodeFactory::createAggregationOperatorNode(context, symbolTable, expressions);
    CHECK(std::dynamic_pointer_cast<csvsqldb::AggregationOperatorNode>(op));
  }
  SECTION("projection operator")
  {
    csvsqldb::Expressions expressions;
    auto op = csvsqldb::OperatorNodeFactory::createExtendedProjectionOperatorNode(context, symbolTable, expressions);
    CHECK(std::dynamic_pointer_cast<csvsqldb::ExtendedProjectionOperatorNode>(op));
  }
  SECTION("cross join operator")
  {
    auto op = csvsqldb::OperatorNodeFactory::createCrossJoinOperatorNode(context, symbolTable);
    CHECK(std::dynamic_pointer_cast<csvsqldb::CrossJoinOperatorNode>(op));
  }
  SECTION("inner join operator")
  {
    csvsqldb::ASTExprNodePtr expression;
    auto op = csvsqldb::OperatorNodeFactory::createInnerJoinOperatorNode(context, symbolTable, expression);
    CHECK(std::dynamic_pointer_cast<csvsqldb::InnerJoinOperatorNode>(op));
  }
  SECTION("inner hash join operator")
  {
    csvsqldb::ASTExprNodePtr expression;
    auto op = csvsqldb::OperatorNodeFactory::createInnerHashJoinOperatorNode(context, symbolTable, expression);
    CHECK(std::dynamic_pointer_cast<csvsqldb::InnerHashJoinOperatorNode>(op));
  }
  SECTION("union operator")
  {
    auto op = csvsqldb::OperatorNodeFactory::createUnionOperatorNode(context, symbolTable);
    CHECK(std::dynamic_pointer_cast<csvsqldb::UnionOperatorNode>(op));
  }
  SECTION("select operator")
  {
    auto expression =
      std::make_shared<csvsqldb::ASTIdentifier>(symbolTable, std::make_shared<csvsqldb::SymbolInfo>(), "", "ID", false);
    auto op = csvsqldb::OperatorNodeFactory::createSelectOperatorNode(context, symbolTable, expression);
    CHECK(std::dynamic_pointer_cast<csvsqldb::SelectOperatorNode>(op));
  }
  SECTION("scan operator")
  {
    csvsqldb::SymbolInfo info;
    info._identifier = "TEST";
    auto op = csvsqldb::OperatorNodeFactory::createScanOperatorNode(context, symbolTable, info);
    CHECK(std::dynamic_pointer_cast<csvsqldb::ScanOperatorNode>(op));
  }
}
