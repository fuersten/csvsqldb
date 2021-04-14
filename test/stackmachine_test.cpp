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


#include <csvsqldb/base/float_helper.h>
#include <csvsqldb/buildin_functions.h>
#include <csvsqldb/sql_astdump.h>
#include <csvsqldb/sql_parser.h>
#include <csvsqldb/stack_machine.h>
#include <csvsqldb/visitor.h>

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
    csvsqldb::Variant doCall(const csvsqldb::Variants& parameter) const override
    {
      return csvsqldb::Variant(csvsqldb::Date::now());
    }
  };
}


TEST_CASE("Stackmachine Test", "[stackmachine]")
{
  csvsqldb::FunctionRegistry functions;
  initBuildInFunctions(functions);
  csvsqldb::SQLParser parser(functions);
  csvsqldb::VariableStore store;


  SECTION("evaluate exp")
  {
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
    {
      csvsqldb::Variant aVar(100);
      store.addVariable(0, aVar);
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("a + (37 * 5) / 3");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK(161 == sm.evaluate(store, functions).asInt());
    }

    {
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("a * a + b * b");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);

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

      csvsqldb::Variant aVar(100);
      store.addVariable(0, aVar);

      CHECK(100l == sm.evaluate(store, functions).asInt());
    }
  }

  SECTION("expression with function")
  {
    csvsqldb::Function::Ptr func = std::make_shared<csvsqldb::PowerFunction>();
    functions.registerFunction(func);
    func = std::make_shared<MyCurrentDateFunction>();
    functions.registerFunction(func);

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
      CHECK(sm.evaluate(store, functions).asBool());
    }

    {
      store.addVariable(0, b_true);
      csvsqldb::ASTExprNodePtr exp = parser.parseExpression("a IS NULL");
      csvsqldb::StackMachine::VariableMapping mapping;
      csvsqldb::StackMachine sm;
      csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
      exp->accept(visitor);
      CHECK_FALSE(sm.evaluate(store, functions).asBool());
    }
  }

  SECTION("null operations")
  {
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
    {
      csvsqldb::StackMachine sm;
      sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::NOP));
      sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::PUSH, csvsqldb::Variant(csvsqldb::BOOLEAN)));
      CHECK(sm.evaluate(store, functions).isNull());
    }

    {
      csvsqldb::StackMachine sm;
      sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::PUSH, 42));
      sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::PLUS, nullptr));
      CHECK(42 == sm.evaluate(store, functions).asInt());
    }
  }

  SECTION("like")
  {
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

TEST_CASE("Stackmachine Dump Test", "[stackmachine]")
{
  csvsqldb::StackMachine sm;
  std::ostringstream os;
  os << std::endl;

  SECTION("push operations")
  {
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::PUSH, 42));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::PUSHVAR, 815));

    sm.dump(os);
    std::string expected = R"(
PUSH 42
PUSHVAR 815
)";
    CHECK(expected == os.str());
  }

  SECTION("dump arithmetic operations")
  {
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::ADD));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::DIV));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::MINUS));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::MOD));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::MUL));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::PLUS));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::SUB));

    sm.dump(os);
    std::string expected = R"(
ADD
DIV
MINUS
MOD
MUL
PLUS
SUB
)";
    CHECK(expected == os.str());
  }

  SECTION("dump bool and compare operations")
  {
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::AND));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::EQ));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::GE));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::GT));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::LE));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::LT));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::NEQ));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::NOT));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::OR));

    sm.dump(os);
    std::string expected = R"(
AND
EQ
GE
GT
LE
LT
NEQ
NOT
OR
)";
    CHECK(expected == os.str());
  }

  SECTION("dump sql operations")
  {
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::BETWEEN));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::CAST, csvsqldb::Variant{csvsqldb::REAL}));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::CONCAT));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::FUNC, "UPPER"));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::IN, 7));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::IS));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::ISNOT));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::LIKE, new csvsqldb::RegExp("test")));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::NOP));

    sm.dump(os);
    std::string expected = R"(
BETWEEN
CAST REAL
CONCAT
FUNC 'UPPER'
IN 7
IS
IS NOT
LIKE
NOP
)";
    CHECK(expected == os.str());
  }
}

TEST_CASE("Stackmachine Error Test", "[stackmachine]")
{
  csvsqldb::FunctionRegistry functions;
  initBuildInFunctions(functions);
  csvsqldb::VariableStore store;
  csvsqldb::StackMachine sm;

  SECTION("PUSHVAR not of int type")
  {
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::PUSHVAR, "Test"));
    CHECK_THROWS_WITH(sm.evaluate(store, functions), "expected an INT as variable index");
  }
  SECTION("FUNC name not of string type")
  {
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::FUNC, 4711));
    CHECK_THROWS_WITH(sm.evaluate(store, functions), "expected a string as function name");
  }
  SECTION("Unkown FUNC")
  {
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::FUNC, "check4711"));
    CHECK_THROWS_WITH(sm.evaluate(store, functions), "function 'check4711' not found");
  }
  SECTION("Wrong FUNC parameter")
  {
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::PUSH, 4711));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::FUNC, "UPPER"));
    CHECK_THROWS_WITH(sm.evaluate(store, functions), "calling function 'UPPER' with wrong parameter");
  }
  SECTION("LIKE without regexp")
  {
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::LIKE, nullptr));
    CHECK_THROWS_WITH(sm.evaluate(store, functions), "expected a regexp in LIKE expression");
  }
  SECTION("LIKE without regexp")
  {
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::PUSH, 4711));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::LIKE, new csvsqldb::RegExp("test")));
    CHECK_THROWS_WITH(sm.evaluate(store, functions), "can only do like operations on strings");
  }
  SECTION("Empty stack")
  {
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::ADD, nullptr));
    CHECK_THROWS_WITH(sm.evaluate(store, functions), "Cannot get next value, no more elements on stack");
  }
  SECTION("Empty top of stack")
  {
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::PUSH, 4711));
    sm.addInstruction(csvsqldb::StackMachine::Instruction(csvsqldb::StackMachine::ADD, nullptr));
    CHECK_THROWS_WITH(sm.evaluate(store, functions), "Cannot get next value, no more elements on top of stack");
  }
}
