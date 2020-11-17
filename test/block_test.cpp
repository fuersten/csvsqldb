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


#include "libcsvsqldb/base/tribool.h"
#include "libcsvsqldb/block.h"
#include "libcsvsqldb/block_iterator.h"
#include "libcsvsqldb/sql_astdump.h"
#include "libcsvsqldb/sql_parser.h"
#include "libcsvsqldb/stack_machine.h"
#include "libcsvsqldb/visitor.h"

#include <catch2/catch.hpp>


class MyBlockProvider : public csvsqldb::BlockProvider
{
public:
  MyBlockProvider(csvsqldb::BlockPtr block, csvsqldb::BlockManager& manager)
  : _block(block)
  , _manager(manager)
  {
  }

  virtual csvsqldb::BlockPtr getNextBlock()
  {
    return _block;
  }

  virtual csvsqldb::BlockManager& getBlockManager()
  {
    return _manager;
  }

private:
  csvsqldb::BlockPtr _block;
  csvsqldb::BlockManager& _manager;
};

namespace
{
  csvsqldb::BlockPtr setupBlock(csvsqldb::BlockManager& blockManager)
  {
    csvsqldb::BlockPtr block = blockManager.createBlock();
    block->addValue(csvsqldb::Variant(4711));
    block->addValue(csvsqldb::Variant(csvsqldb::Date(1970, csvsqldb::Date::September, 23)));
    block->addValue(csvsqldb::Variant("Lars"));
    block->addValue(csvsqldb::Variant("Fürstenberg"));
    block->addValue(csvsqldb::Variant("M"));
    block->addValue(csvsqldb::Variant(csvsqldb::Date(2012, csvsqldb::Date::February, 1)));
    block->nextRow();
    block->addValue(csvsqldb::Variant(815));
    block->addValue(csvsqldb::Variant(csvsqldb::Date(1969, csvsqldb::Date::May, 17)));
    block->addValue(csvsqldb::Variant("Mark"));
    block->addValue(csvsqldb::Variant("Fürstenberg"));
    block->addValue(csvsqldb::Variant("M"));
    block->addValue(csvsqldb::Variant(csvsqldb::Date(2003, csvsqldb::Date::April, 15)));
    block->nextRow();
    block->endBlocks();

    return block;
  }
}

TEST_CASE("Block Test", "[block]")
{
  SECTION("row")
  {
    csvsqldb::BlockManager blockManager;

    csvsqldb::Types types;
    types.push_back(csvsqldb::INT);
    types.push_back(csvsqldb::DATE);
    types.push_back(csvsqldb::STRING);
    types.push_back(csvsqldb::STRING);
    types.push_back(csvsqldb::STRING);
    types.push_back(csvsqldb::DATE);

    {
      csvsqldb::FunctionRegistry functions;
      csvsqldb::SQLParser parser(functions);
      csvsqldb::VariableStore store;

      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("hire_date > DATE'2012-01-01'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);

      csvsqldb::BlockPtr block = setupBlock(blockManager);
      MyBlockProvider blockProvider(block, blockManager);
      csvsqldb::BlockIteratorPtr iterator = std::make_shared<csvsqldb::BlockIterator>(types, blockProvider, blockManager);

      {
        const csvsqldb::Values& rowValues = *(iterator->getNextRow());

        const csvsqldb::Value* val = rowValues[0];
        CHECK("4711" == val->toString());
        val = rowValues[1];
        CHECK("1970-09-23" == val->toString());
        val = rowValues[2];
        CHECK("Lars" == val->toString());
        val = rowValues[3];
        CHECK("Fürstenberg" == val->toString());
        val = rowValues[4];
        CHECK("M" == val->toString());
        val = rowValues[5];
        CHECK("2012-02-01" == val->toString());

        store.addVariable(0, csvsqldb::Variant(dynamic_cast<const csvsqldb::ValDate*>(val)->asDate()));
        CHECK(sm.evaluate(store, functions).asBool());
      }

      {
        const csvsqldb::Values& rowValues = *(iterator->getNextRow());

        const csvsqldb::Value* val = rowValues[0];
        CHECK("815" == val->toString());
        val = rowValues[1];
        CHECK("1969-05-17" == val->toString());
        val = rowValues[2];
        CHECK("Mark" == val->toString());
        val = rowValues[3];
        CHECK("Fürstenberg" == val->toString());
        val = rowValues[4];
        CHECK("M" == val->toString());
        val = rowValues[5];
        CHECK("2003-04-15" == val->toString());

        store.addVariable(0, csvsqldb::Variant(dynamic_cast<const csvsqldb::ValDate*>(val)->asDate()));
        CHECK_FALSE(sm.evaluate(store, functions).asBool());
      }
    }

    {
      csvsqldb::FunctionRegistry functions;
      csvsqldb::SQLParser parser(functions);
      csvsqldb::VariableStore store;

      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("emp.last_name = 'Fürstenberg'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);

      csvsqldb::BlockPtr block = setupBlock(blockManager);
      MyBlockProvider blockProvider(block, blockManager);
      csvsqldb::BlockIteratorPtr iterator = std::make_shared<csvsqldb::BlockIterator>(types, blockProvider, blockManager);

      const csvsqldb::Values* row = nullptr;
      int count = 0;
      while ((row = iterator->getNextRow())) {
        if (count == 0) {
          CHECK("4711" == (*row)[0]->toString());
          CHECK("1970-09-23" == (*row)[1]->toString());
          CHECK("Lars" == (*row)[2]->toString());
          CHECK("Fürstenberg" == (*row)[3]->toString());
          CHECK("M" == (*row)[4]->toString());
          CHECK("2012-02-01" == (*row)[5]->toString());

          store.addVariable(0, csvsqldb::Variant(dynamic_cast<const csvsqldb::ValString*>((*row)[3])->asString()));
          CHECK(sm.evaluate(store, functions).asBool());
        } else if (count == 1) {
          CHECK("815" == (*row)[0]->toString());
          CHECK("1969-05-17" == (*row)[1]->toString());
          CHECK("Mark" == (*row)[2]->toString());
          CHECK("Fürstenberg" == (*row)[3]->toString());
          CHECK("M" == (*row)[4]->toString());
          CHECK("2003-04-15" == (*row)[5]->toString());

          store.addVariable(0, csvsqldb::Variant(dynamic_cast<const csvsqldb::ValString*>((*row)[3])->asString()));
          CHECK(sm.evaluate(store, functions).asBool());
        }
        ++count;
      }
    }
  }
}
