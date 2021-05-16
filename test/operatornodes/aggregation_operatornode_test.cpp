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

#include <csvsqldb/operatornodes/aggregation_operatornode.h>

#include "test/operatornodes/operatornode_test_fixture.h"

#include <catch2/catch.hpp>


TEST_CASE("Aggregation Operator Node Test", "[operatornodes]")
{
  OperatorNodeTestFixture fixture;

  SECTION("Simple Expression")
  {
    csvsqldb::SymbolInfo info = fixture.addTable("TEST", {{"A", csvsqldb::INT}, {"B", csvsqldb::INT}});
    auto scanOperatorNode = std::make_shared<ScanOperatorNodeMock>(fixture.context, fixture.symbolTable, info);

    csvsqldb::Expressions expressions;
    fixture.addExpression(expressions, "sum(a)");

    csvsqldb::AggregationOperatorNode operatorNode(fixture.context, fixture.symbolTable, expressions);
    operatorNode.connect(scanOperatorNode);

    scanOperatorNode->setProducer([](csvsqldb::BlockProducer& producer) {
      for (auto n = 0; n < 10; ++n) {
        producer.addInt(n, false);
        producer.addInt(n * 10, false);
        producer.nextRow();
      }
    });

    const auto* values = operatorNode.getNextRow();
    REQUIRE(values);
    CHECK(values->size() == 1);
    CHECK(csvsqldb::valueToVariant(*(values->at(0))) == csvsqldb::Variant{45});
  }

  SECTION("Complex Expression")
  {
    csvsqldb::SymbolInfo info = fixture.addTable("TEST", {{"A", csvsqldb::REAL}, {"B", csvsqldb::BOOLEAN}, {"C", csvsqldb::INT}});
    auto scanOperatorNode = std::make_shared<ScanOperatorNodeMock>(fixture.context, fixture.symbolTable, info);

    csvsqldb::Expressions expressions;
    fixture.addExpression(expressions, "sum(c)");
    fixture.addExpression(expressions, "avg(a)");
    fixture.addExpression(expressions, "max(cast(b as integer))");

    csvsqldb::AggregationOperatorNode operatorNode(fixture.context, fixture.symbolTable, expressions);
    operatorNode.connect(scanOperatorNode);

    scanOperatorNode->setProducer([](csvsqldb::BlockProducer& producer) {
      for (auto n = 0; n < 10; ++n) {
        producer.addReal(n, false);
        producer.addBool(n % 10, false);
        producer.addInt(n * 100, false);
        producer.nextRow();
      }
    });

    const auto* values = operatorNode.getNextRow();
    REQUIRE(values);
    REQUIRE(values->size() == 3);
    CHECK(csvsqldb::valueToVariant(*(values->at(0))).asInt() == 4500);
    CHECK(csvsqldb::valueToVariant(*(values->at(1))).asDouble() == Approx(4.5));
    CHECK(csvsqldb::valueToVariant(*(values->at(2))).asInt() == 1);
  }

  SECTION("Wrong aggregation function")
  {
    csvsqldb::SymbolInfo info = fixture.addTable("TEST", {{"A", csvsqldb::INT}});
    auto scanOperatorNode = std::make_shared<ScanOperatorNodeMock>(fixture.context, fixture.symbolTable, info);

    csvsqldb::Expressions expressions;
    fixture.addExpression(expressions, "cast(a as integer)");

    csvsqldb::AggregationOperatorNode operatorNode(fixture.context, fixture.symbolTable, expressions);
    CHECK_THROWS_WITH(operatorNode.connect(scanOperatorNode), "no aggregation on other than aggregation functions");
  }

  SECTION("Dump")
  {
    csvsqldb::SymbolInfo info = fixture.addTable("TEST", {{"A", csvsqldb::INT}, {"B", csvsqldb::INT}, {"C", csvsqldb::INT}});
    auto scanOperatorNode = std::make_shared<ScanOperatorNodeMock>(fixture.context, fixture.symbolTable, info);

    csvsqldb::Expressions expressions;
    fixture.addExpression(expressions, "sum(c)");
    fixture.addExpression(expressions, "avg(a)");
    fixture.addExpression(expressions, "max(cast(b as integer))");

    csvsqldb::AggregationOperatorNode operatorNode(fixture.context, fixture.symbolTable, expressions);
    operatorNode.connect(scanOperatorNode);

    std::stringstream ss;
    operatorNode.dump(ss);
    CHECK(ss.str() == "AggregationOperator (SUM,AVG,MAX)\n-->MockScanOperatorNode()\n");
  }
}
