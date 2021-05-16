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

#include <csvsqldb/operatornode.h>
#include <csvsqldb/sql_astexpressionvisitor.h>
#include <csvsqldb/visitor.h>

#include "test/operatornodes/operatornode_test_fixture.h"

#include <catch2/catch.hpp>


TEST_CASE("Operator Node Test", "[operatornodes]")
{
  OperatorNodeTestFixture fixture;
  csvsqldb::SymbolInfo info = fixture.addTable("TEST", {{"A", csvsqldb::INT}, {"B", csvsqldb::INT}});
  auto scanOperatorNode = std::make_shared<ScanOperatorNodeMock>(fixture.context, fixture.symbolTable, info);
  csvsqldb::SymbolInfos infos;
  scanOperatorNode->getColumnInfos(infos);
  csvsqldb::Expressions expressions;

  SECTION("Index mapping")
  {
    fixture.addExpression(expressions, "a + b");

    csvsqldb::StackMachine sm;
    csvsqldb::VariableStore::VariableMapping mapping;
    {
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      expressions[0]->accept(visitor);
    }

    csvsqldb::IdentifierSet expressionVariables;
    {
      csvsqldb::ASTExpressionVariableVisitor visitor(expressionVariables);
      expressions[0]->accept(visitor);
    }

    csvsqldb::OperatorBaseNode::VariableIndexMapping varMapping =
      csvsqldb::OperatorBaseNode::getIndexMapping(expressionVariables, infos, mapping);
    REQUIRE(varMapping.size() == 2);
    CHECK(varMapping[0].first == 1);
    CHECK(varMapping[0].second == 0);
    CHECK(varMapping[1].first == 0);
    CHECK(varMapping[1].second == 1);
  }

  SECTION("Index mapping not found")
  {
    csvsqldb::IdentifierSet expressionVariables;
    auto info = std::make_shared<csvsqldb::SymbolInfo>();
    info->_name = "b";
    info->_identifier = info->_name;
    info->_symbolType = csvsqldb::PLAIN;
    info->_type = csvsqldb::INT;
    info->_relation = "TEST";
    fixture.symbolTable->addSymbol(info->_name, info);
    expressionVariables.emplace(csvsqldb::ASTIdentifier(fixture.symbolTable, info, "", "b", false));
    info = std::make_shared<csvsqldb::SymbolInfo>();
    info->_name = "c";
    info->_identifier = info->_name;
    info->_symbolType = csvsqldb::PLAIN;
    info->_type = csvsqldb::INT;
    info->_relation = "TEST";
    fixture.symbolTable->addSymbol(info->_name, info);
    expressionVariables.emplace(csvsqldb::ASTIdentifier(fixture.symbolTable, info, "", "c", false));

    csvsqldb::VariableStore::VariableMapping mapping;

    CHECK_THROWS_WITH(csvsqldb::OperatorBaseNode::getIndexMapping(expressionVariables, infos, mapping), "variable 'b' not found");
  }
}
