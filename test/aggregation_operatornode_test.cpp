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

#include <csvsqldb/block_iterator.h>
#include <csvsqldb/block_producer.h>
#include <csvsqldb/operatornodes/aggregation_operatornode.h>
#include <csvsqldb/operatornodes/scan_operatornode.h>
#include <csvsqldb/sql_parser.h>
#include <csvsqldb/validation_visitor.h>

#include "temporary_directory.h"
#include "test/test_util.h"

#include <catch2/catch.hpp>


namespace
{
  class MockScanOperatorNode
  : public csvsqldb::ScanOperatorNode
  , public csvsqldb::BlockProvider
  {
  public:
    MockScanOperatorNode(const csvsqldb::OperatorContext& context, const csvsqldb::SymbolTablePtr& symbolTable,
                         const csvsqldb::SymbolInfo& tableInfo)
    : ScanOperatorNode(context, symbolTable, tableInfo)
    , _producer(context._blockManager)
    {
    }

    void setProducer(std::function<void(csvsqldb::BlockProducer& producer)> produce)
    {
      _produce = produce;
    }

    const csvsqldb::Values* getNextRow() override
    {
      if (!_iterator) {
        _producer.start([&](csvsqldb::BlockProducer& producer) {
          if (_produce) {
            _produce(producer);
          }
        });
        _iterator = std::make_shared<csvsqldb::BlockIterator>(_types, *this, _context._blockManager);
      }
      return _iterator->getNextRow();
    }

    csvsqldb::BlockPtr getNextBlock() override
    {
      return _producer.getNextBlock();
    }

    void dump(std::ostream& stream) const override
    {
      stream << "MockScanOperatorNode()\n";
    }

  private:
    std::function<void(csvsqldb::BlockProducer& producer)> _produce;

    csvsqldb::BlockIteratorPtr _iterator;
    csvsqldb::BlockProducer _producer;
  };
}


TEST_CASE("Aggregation Operator Node Test", "[operatornodes]")
{
  TemporaryDirectoryGuard tmpDir;
  auto path = tmpDir.temporaryDirectoryPath();
  csvsqldb::FileMapping mapping;
  csvsqldb::Database database{path, mapping};
  csvsqldb::FunctionRegistry functions;
  csvsqldb::BlockManager blockManager;
  csvsqldb::OperatorContext context{database, functions, blockManager, {}};
  auto symbolTable = csvsqldb::SymbolTable::createSymbolTable();
  csvsqldb::FunctionRegistry registry;
  csvsqldb::SQLParser parser{registry};

  SECTION("Process")
  {
    csvsqldb::TableData tabledata("TEST");
    csvsqldb::ASTExprNodePtr check;
    tabledata.addColumn("A", csvsqldb::INT, true, false, false, std::any(), check, 0);
    database.addTable(std::move(tabledata));

    {
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "A";
      info->_identifier = "A";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::INT;
      info->_relation = "TEST";
      symbolTable->addSymbol("A", info);
    }
    {
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_identifier = "TEST";
      info->_name = "TEST";
      info->_symbolType = csvsqldb::TABLE;
      symbolTable->addSymbol("TEST", info);
    }

    auto expr = parser.parseExpression("sum(a)", symbolTable);
    {
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_symbolType = csvsqldb::CALC;
      info->_type = csvsqldb::NONE;
      info->_expressionNode = expr;
      auto symbolName = symbolTable->getNextAlias();
      info->_name = symbolName;
      symbolTable->addSymbol(symbolName, info);
      expr->_symbolName = symbolName;
    }

    csvsqldb::SymbolInfo info;
    info._identifier = "TEST";
    info._name = "TEST";
    info._symbolType = csvsqldb::TABLE;
    auto scanOperatorNode = std::make_shared<MockScanOperatorNode>(context, symbolTable, info);

    expr->symbolTable()->typeSymbolTable(database);

    csvsqldb::Expressions expressions;
    expressions.emplace_back(expr);

    csvsqldb::AggregationOperatorNode operatorNode(context, symbolTable, expressions);
    operatorNode.connect(scanOperatorNode);

    scanOperatorNode->setProducer([](csvsqldb::BlockProducer& producer) {
      for (auto n = 0; n < 10; ++n) {
        producer.addInt(n, false);
        producer.nextRow();
      }
    });

    const auto* values = operatorNode.getNextRow();
    REQUIRE(values);
    CHECK(values->size() == 1);
    CHECK(csvsqldb::valueToVariant(*(values->at(0))) == csvsqldb::Variant{45});
  }
}
