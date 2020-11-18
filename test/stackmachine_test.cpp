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


#include "libcsvsqldb/base/float_helper.h"
#include "libcsvsqldb/buildin_functions.h"
#include "libcsvsqldb/sql_astdump.h"
#include "libcsvsqldb/sql_parser.h"
#include "libcsvsqldb/stack_machine.h"
#include "libcsvsqldb/visitor.h"

#include <catch2/catch.hpp>


namespace
{
  class MyCurrentDateFunction : public csvsqldb::Function
  {
  public:
    MyCurrentDateFunction()
    : Function("CURRENT_DATE", csvsqldb::DATE, csvsqldb::Types({}))
    {
    }

  private:
    virtual const csvsqldb::Variant doCall(const csvsqldb::Variants& parameter) const
    {
      return csvsqldb::Variant(csvsqldb::Date::now());
    }
  };
}


TEST_CASE("Stackmachine Test", "[stackmachine]")
{
  SECTION("evaluate exp")
  {
    csvsqldb::VariableStore store;
    csvsqldb::FunctionRegistry functions;
    csvsqldb::StackMachine sm;

    csvsqldb::Variant i1(42);
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::PUSH, i1));

    csvsqldb::Variant i2(815);
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::PUSH, i2));

    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::ADD));

    CHECK(857 == sm.evaluate(store, functions).asInt());
  }

  SECTION("expression")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);
    csvsqldb::VariableStore store;

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("(37 * 5) / 3");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(61 == sm.evaluate(store, functions).asInt());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("5 - 3 * 10 + 7");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(-18 == sm.evaluate(store, functions).asInt());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("5 between 3 and 17");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("not 5 between 3 and 17");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK_FALSE(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("5 between 17 and 3");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("not 5 between 17 and 3");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK_FALSE(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("- 3 * 10");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(-30 == sm.evaluate(store, functions).asInt());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("3 <> 4");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("4 <> 4");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK_FALSE(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("121 % 10");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(1 == sm.evaluate(store, functions).asInt());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("CAST(4.5 AS INT) = 4");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("4 <> 4 or 5 between 3 and 17 and CAST(4.5 AS INT) = 4");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);

      //            std::cout << std::endl;
      //            sm.dump(std::cout);

      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("4 = 4 or 5 between 3 and 17 and CAST(4.5 AS INT) = 4");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);

      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("'Lars' || ' ' || 'Fürstenberg'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK("Lars Fürstenberg" == sm.evaluate(store, functions).toString());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("DATE '2014-12-30' > DATE '1970-09-23'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("DATE '2014-12-30' <> DATE '1970-09-23'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("DATE '1970-09-23' = DATE '1970-09-23'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("TIME '13:40:25' > TIME '08:09:11'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("TIME '13:40:25' <> TIME '08:09:11'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("TIME '08:09:11' = TIME '08:09:11'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("TIMESTAMP '2014-12-30T13:40:25' > TIMESTAMP '1970-09-23T08:09:11'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("TIMESTAMP '2014-12-30T13:40:25' <> TIMESTAMP '1970-09-23T08:09:11'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("TIMESTAMP '1970-09-23T08:09:11' = TIMESTAMP '1970-09-23T08:09:11'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("'Lars' = 'Lars'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("'Lars' = 'Mark'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK_FALSE(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("'Lars' <> 'Lars'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK_FALSE(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("'Lars' <> 'Mark'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("'Lars' > 'Mark'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK_FALSE(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("'Lars' < 'Mark'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("'Lars' >= 'Mark'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK_FALSE(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("'Lars' <= 'Mark'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("DATE'2015-01-01' - DATE'1970-09-23'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(16171 == sm.evaluate(store, functions).asInt());
    }
  }

  SECTION("in")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);
    csvsqldb::VariableStore store;

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("8 IN(1,2,3,4,5,6,7,8,9,10)");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("11 IN(1,2,3,4,5,6,7,8,9,10)");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK_FALSE(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp =
        parser.parseExpression("DATE'1970-09-23' IN(DATE'1963-03-06',DATE'1969-05-17',DATE'1946-05-14',DATE'1970-09-23')");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }
  }

  SECTION("expression with variable")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    {
      csvsqldb::VariableStore store;
      csvsqldb::Variant aVar(100);
      store.addVariable(0, aVar);
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("a + (37 * 5) / 3");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);

      // std::cout << std::endl;
      // sm.dump(std::cout);

      CHECK(161 == sm.evaluate(store, functions).asInt());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("a * a + b * b");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      csvsqldb::VariableStore store;

      {
        csvsqldb::Variant aVar(100);
        csvsqldb::Variant bVar(10);
        store.addVariable(0, aVar);
        store.addVariable(1, bVar);

        CHECK(10100 == sm.evaluate(store, functions).asInt());
      }
      {
        csvsqldb::Variant aVar(5);
        csvsqldb::Variant bVar(7);
        store.addVariable(0, aVar);
        store.addVariable(1, bVar);

        CHECK(74 == sm.evaluate(store, functions).asInt());
      }
      {
        csvsqldb::VariableStore mixedStore;
        csvsqldb::Variant aVar(5.0);
        csvsqldb::Variant bVar(7);
        mixedStore.addVariable(0, aVar);
        mixedStore.addVariable(1, bVar);

        CHECK(74.0 == Approx(sm.evaluate(mixedStore, functions).asDouble()));
      }
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("a");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      csvsqldb::VariableStore store;

      {
        csvsqldb::Variant aVar(100);
        store.addVariable(0, aVar);

        CHECK(100l == sm.evaluate(store, functions).asInt());
      }
    }
  }

  SECTION("expression with function")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);
    csvsqldb::Function::Ptr func = std::make_shared<csvsqldb::PowerFunction>();
    functions.registerFunction(func);
    func = std::make_shared<MyCurrentDateFunction>();
    functions.registerFunction(func);
    csvsqldb::VariableStore store;

    {
      csvsqldb::Variant bVar(4);
      store.addVariable(0, bVar);
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("(pow(CAST(10 AS REAL),2.0) * 2) / b");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(50 == Approx(sm.evaluate(store, functions).asDouble()));
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("CURRENT_DATE");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(csvsqldb::Date::now() == sm.evaluate(store, functions).asDate());
    }
  }

  SECTION("build in functions")
  {
    csvsqldb::FunctionRegistry functions;
    initBuildInFunctions(functions);
    csvsqldb::SQLParser parser(functions);
    csvsqldb::VariableStore store;

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("CURRENT_DATE");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(csvsqldb::Date::now() == sm.evaluate(store, functions).asDate());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("CURRENT_TIMESTAMP");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(csvsqldb::Timestamp::now() == sm.evaluate(store, functions).asTimestamp());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("EXTRACT(YEAR FROM CURRENT_TIMESTAMP)");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(csvsqldb::Timestamp::now().year() == sm.evaluate(store, functions).asInt());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("EXTRACT(MONTH FROM CURRENT_DATE)");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(csvsqldb::Date::now().month() == sm.evaluate(store, functions).asInt());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("EXTRACT(DAY FROM CURRENT_DATE)");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(csvsqldb::Date::now().day() == sm.evaluate(store, functions).asInt());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("EXTRACT(HOUR FROM CURRENT_TIME)");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(csvsqldb::Time::now().hour() == sm.evaluate(store, functions).asInt());
    }

    {
      csvsqldb::Variant bVar(4);
      store.addVariable(0, bVar);
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("(pow(CAST(10 AS REAL),2.0) * 2) / b");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(50 == Approx(sm.evaluate(store, functions).asDouble()));
    }

    {
      csvsqldb::Variant bVar(4);
      store.addVariable(0, bVar);
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("(pow(10, 2.0) * 2) / b");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(50 == Approx(sm.evaluate(store, functions).asDouble()));
    }

    {
      csvsqldb::Variant bVar("Hutzli");
      store.addVariable(0, bVar);
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("upper(s)");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK("HUTZLI" == sm.evaluate(store, functions).toString());
    }

    {
      csvsqldb::Variant bVar("HutZli");
      store.addVariable(0, bVar);
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("lower(s)");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK("hutzli" == sm.evaluate(store, functions).toString());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("DATE_FORMAT(DATE'1970-09-23', '%d.%m.%Y')");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK("23.09.1970" == sm.evaluate(store, functions).toString());
    }
  }

  SECTION("exclude middle")
  {
    csvsqldb::FunctionRegistry functions;
    initBuildInFunctions(functions);
    csvsqldb::SQLParser parser(functions);
    csvsqldb::VariableStore store;

    csvsqldb::Variant b_true(true);
    csvsqldb::Variant b_false(false);
    csvsqldb::Variant null(csvsqldb::BOOLEAN);

    {
      store.addVariable(0, b_true);
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("a OR NOT a");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      store.addVariable(0, b_false);
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("a OR NOT a");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      store.addVariable(0, null);
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("a OR NOT a");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).isNull());
    }
  }

  SECTION("null functions")
  {
    csvsqldb::FunctionRegistry functions;
    initBuildInFunctions(functions);
    csvsqldb::SQLParser parser(functions);
    csvsqldb::VariableStore store;

    csvsqldb::Variant b_true(true);
    csvsqldb::Variant b_false(false);
    csvsqldb::Variant null(csvsqldb::BOOLEAN);

    {
      store.addVariable(0, b_true);
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("a IS TRUE");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      store.addVariable(0, b_true);
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("a IS FALSE");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK_FALSE(sm.evaluate(store, functions).asBool());
    }

    {
      store.addVariable(0, b_true);
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("a IS NOT FALSE");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      //           std::cout << std::endl;
      //           sm.dump(std::cout);

      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      store.addVariable(0, b_true);
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("a IS NULL");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      //           std::cout << std::endl;
      //           sm.dump(std::cout);
      CHECK_FALSE(sm.evaluate(store, functions).asBool());
    }
  }

  SECTION("null operations")
  {
    csvsqldb::FunctionRegistry functions;
    initBuildInFunctions(functions);
    csvsqldb::SQLParser parser(functions);
    csvsqldb::VariableStore store;

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("cast(null as boolean)");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).isNull());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("cast(null as boolean) and false");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK_FALSE(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("cast(null as boolean) and true");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).isNull());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("cast(null as boolean) and cast(null as boolean)");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).isNull());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("cast(null as boolean) or false");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).isNull());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("cast(null as boolean) or true");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("cast(null as boolean) or cast(null as boolean)");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).isNull());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("not cast(null as boolean)");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).isNull());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("NULL BETWEEN 2 AND 4");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).isNull());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("2 BETWEEN NULL AND 4");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).isNull());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("2 BETWEEN 1 AND NULL");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).isNull());
    }
  }

  SECTION("nop")
  {
    csvsqldb::FunctionRegistry functions;
    initBuildInFunctions(functions);
    csvsqldb::VariableStore store;

    csvsqldb::StackMachine::VariableMapping mapping;
    csvsqldb::StackMachine sm;
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::NOP));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::PUSH, csvsqldb::Variant(csvsqldb::BOOLEAN)));
    CHECK(sm.evaluate(store, functions).isNull());
  }

  SECTION("like")
  {
    csvsqldb::FunctionRegistry functions;
    initBuildInFunctions(functions);
    csvsqldb::SQLParser parser(functions);
    csvsqldb::VariableStore store;

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("'Han Solo' like '%an So%'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("'Darth Vader' like '%an So%'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK_FALSE(sm.evaluate(store, functions).asBool());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("'Darth Vader' not like '%an So%'");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(sm.evaluate(store, functions).asBool());
    }
  }
}
