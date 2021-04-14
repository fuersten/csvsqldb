//
//  stack_machine.h
//  csvsqldb
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

#pragma once

#include <csvsqldb/inc.h>

#include <csvsqldb/base/exception.h>
#include <csvsqldb/base/logging.h>
#include <csvsqldb/base/regexp.h>
#include <csvsqldb/function_registry.h>
#include <csvsqldb/variant.h>

#include <stack>
#include <vector>


namespace csvsqldb
{
  CSVSQLDB_DECLARE_EXCEPTION(StackMachineException, csvsqldb::Exception);

  class CSVSQLDB_EXPORT VariableStore
  {
  public:
    VariableStore() = default;

    void addVariable(size_t index, const Variant& value);

    const Variant& operator[](size_t index) const;

  private:
    std::vector<Variant> _variables;
  };


  class CSVSQLDB_EXPORT StackMachine
  {
  public:
    using VariableIndex = std::pair<std::string, size_t>;
    using VariableMapping = std::vector<VariableIndex>;

    enum OpCode {
      ADD,
      AND,
      BETWEEN,
      CAST,
      CONCAT,
      DIV,
      EQ,
      FUNC,
      GE,
      GT,
      IN,
      IS,
      ISNOT,
      LE,
      LIKE,
      LT,
      MINUS,
      MOD,
      MUL,
      NEQ,
      NOP,
      NOT,
      OR,
      PLUS,
      PUSH,
      PUSHVAR,
      SUB
    };

    struct CSVSQLDB_EXPORT Instruction {
      Instruction(OpCode opCode)
      : _opCode(opCode)
      {
      }

      Instruction(OpCode opCode, Variant value)
      : _opCode(opCode)
      , _value(std::move(value))
      {
      }

      Instruction(OpCode opCode, csvsqldb::RegExp* r)
      : _opCode(opCode)
      , _refCount(new RefCount)
      , _r(r)
      {
      }

      Instruction(const Instruction& rhs)
      : _opCode(rhs._opCode)
      , _value(rhs._value)
      , _refCount(rhs._refCount)
      , _r(rhs._r)
      {
        if (_refCount) {
          _refCount->inc();
        }
      }

      ~Instruction()
      {
        if (_refCount) {
          try {
            if (_refCount->dec() == 0) {
              delete _r;
              _r = nullptr;
              delete _refCount;
            }
          } catch (const std::exception& ex) {
            CSVSQLDB_ERRORLOG("Instruction: " << ex.what());
          }
        }
      }

      OpCode _opCode;
      Variant _value{NONE};
      RefCount* _refCount{nullptr};
      csvsqldb::RegExp* _r{nullptr};
    };

    void addInstruction(const Instruction& instruction);

    const Variant& evaluate(const VariableStore& store, const FunctionRegistry& functions);

    void reset();

    void dump(std::ostream& stream) const;

  private:
    Variant& getTopValue();
    Variant getNextValue();
    static eOperationType mapOpCodeToBinaryOperationType(OpCode code)
    {
      switch (code) {
        case ADD:
          return OP_ADD;
        case SUB:
          return OP_SUB;
        case DIV:
          return OP_DIV;
        case MOD:
          return OP_MOD;
        case MUL:
          return OP_MUL;
        case EQ:
          return OP_EQ;
        case NEQ:
          return OP_NEQ;
        case IS:
          return OP_IS;
        case ISNOT:
          return OP_ISNOT;
        case GT:
          return OP_GT;
        case GE:
          return OP_GE;
        case LT:
          return OP_LT;
        case LE:
          return OP_LE;
        case AND:
          return OP_AND;
        case OR:
          return OP_OR;
        case CONCAT:
          return OP_CONCAT;
        default:
          CSVSQLDB_THROW(StackMachineException, "op code " << code << " not supported as binary operation type");
      }
    }

    std::vector<Instruction> _instructions;
    std::stack<Variant> _valueStack;
  };
}
