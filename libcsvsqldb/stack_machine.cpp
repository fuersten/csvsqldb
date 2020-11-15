//
//  stack_machine.cpp
//  csvsqldb
//
//  BSD 3-Clause License
//  Copyright (c) 2015, Lars-Christian Fürstenberg
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

#include "stack_machine.h"

#include "typeoperations.h"

#include <algorithm>
#include <functional>


namespace csvsqldb
{
  CSVSQLDB_IMPLEMENT_EXCEPTION(StackMachineException, csvsqldb::Exception);


  VariableStore::VariableStore()
  {
  }

  const Variant& VariableStore::operator[](size_t index) const
  {
    return _variables[index];
  }

  void VariableStore::addVariable(size_t index, const Variant& value)
  {
    if (_variables.capacity() < (index + 1)) {
      size_t n = (index - _variables.size()) + 1;
      _variables.resize(n + _variables.size());
    }
    _variables[index] = value;
  }


  void StackMachine::addInstruction(const Instruction& instruction)
  {
    _instructions.emplace(_instructions.end(), instruction);
  }

  Variant& StackMachine::getTopValue()
  {
    if (_valueStack.empty()) {
      CSVSQLDB_THROW(StackMachineException, "Cannot get next value, no more elements on stack");
    }
    return _valueStack.top();
  }

  const Variant StackMachine::getNextValue()
  {
    if (_valueStack.empty()) {
      CSVSQLDB_THROW(StackMachineException, "Cannot get next value, no more elements on stack");
    }
    const Variant val = _valueStack.top();
    _valueStack.pop();
    return val;
  }

  void StackMachine::reset()
  {
    while (!_valueStack.empty()) {
      _valueStack.pop();
    }
  }

  void StackMachine::dump(std::ostream& stream) const
  {
    for (const auto& element : _instructions) {
      switch (element._opCode) {
        case NOP:
          stream << "NOP" << std::endl;
          break;
        case PUSH:
          stream << "PUSH " << element._value.toString() << std::endl;
          break;
        case PUSHVAR:
          stream << "PUSHVAR " << element._value.toString() << std::endl;
          break;
        case ADD:
          stream << "ADD" << std::endl;
          break;
        case SUB:
          stream << "SUB" << std::endl;
          break;
        case DIV:
          stream << "DIV" << std::endl;
          break;
        case MOD:
          stream << "MOD" << std::endl;
          break;
        case MUL:
          stream << "MUL" << std::endl;
          break;
        case NOT:
          stream << "NOT" << std::endl;
          break;
        case PLUS:
          stream << "PLUS" << std::endl;
          break;
        case MINUS:
          stream << "MINUS" << std::endl;
          break;
        case EQ:
          stream << "EQ" << std::endl;
          break;
        case NEQ:
          stream << "NEQ" << std::endl;
          break;
        case GT:
          stream << "GT" << std::endl;
          break;
        case GE:
          stream << "GE" << std::endl;
          break;
        case LT:
          stream << "LT" << std::endl;
          break;
        case LE:
          stream << "LE" << std::endl;
          break;
        case AND:
          stream << "AND" << std::endl;
          break;
        case OR:
          stream << "OR" << std::endl;
          break;
        case BETWEEN:
          stream << "BETWEEN" << std::endl;
          break;
        case FUNC:
          stream << "FUNC" << std::endl;
          break;
        case CAST:
          stream << "CAST " << typeToString(element._value.getType()) << std::endl;
          break;
        case CONCAT:
          stream << "CONCAT" << std::endl;
          break;
        case IN:
          stream << "IN" << std::endl;
          break;
        case IS:
          stream << "IS" << std::endl;
          break;
        case ISNOT:
          stream << "IS NOT" << std::endl;
          break;
        case LIKE:
          stream << "LIKE" << std::endl;
          break;
      }
    }
  }

  Variant& StackMachine::evaluate(const VariableStore& store, const FunctionRegistry& functions)
  {
    reset();

    for (const auto& instruction : _instructions) {
      switch (instruction._opCode) {
        case NOP: {
          break;
        }
        case PUSH: {
          _valueStack.emplace(instruction._value);
          break;
        }
        case PUSHVAR: {
          if (instruction._value.getType() != INT) {
            CSVSQLDB_THROW(StackMachineException, "expected an INT as variable index");
          }

          int64_t index = instruction._value.asInt();
          _valueStack.emplace(store[static_cast<size_t>(index)]);
          break;
        }
        case ADD:
        case SUB:
        case DIV:
        case MOD:
        case MUL:
        case EQ:
        case NEQ:
        case IS:
        case ISNOT:
        case GT:
        case GE:
        case LT:
        case LE:
        case AND:
        case OR:
        case CONCAT: {
          const Variant lhs(getNextValue());
          Variant& rhs(getTopValue());
          rhs = binaryOperation(mapOpCodeToBinaryOperationType(instruction._opCode), lhs, rhs);
          break;
        }
        case NOT: {
          Variant& rhs(getTopValue());
          rhs = unaryOperation(OP_NOT, BOOLEAN, rhs);
          break;
        }
        case PLUS: {
          // this is a nop, as the value will not change, so just leave it on the stack
          break;
        }
        case MINUS: {
          Variant& rhs = getTopValue();
          rhs = unaryOperation(OP_MINUS, rhs.getType(), rhs);
          break;
        }
        case BETWEEN: {
          const Variant lhs = getNextValue();
          const Variant from = getNextValue();
          Variant& to = getTopValue();

          Variant result(BOOLEAN);
          if (not(lhs.isNull() || from.isNull() || to.isNull())) {
            if (binaryOperation(OP_GE, to, from).asBool()) {
              result = binaryOperation(OP_GE, lhs, from);
              if (result.asBool()) {
                result = binaryOperation(OP_LE, lhs, to);
              }
            } else {
              result = binaryOperation(OP_GE, lhs, to);
              if (result.asBool()) {
                result = binaryOperation(OP_LE, lhs, from);
              }
            }
          }
          to = result;
          break;
        }
        case FUNC: {
          if (instruction._value.getType() != STRING) {
            CSVSQLDB_THROW(StackMachineException, "expected a string as variable name");
          }

          std::string funcname = instruction._value.asString();
          Function::Ptr func = functions.getFunction(funcname);
          if (!func) {
            CSVSQLDB_THROW(StackMachineException, "function '" << funcname << "' not found");
          }
          Variants parameter;
          size_t count = func->getParameterTypes().size();
          for (const auto& param : func->getParameterTypes()) {
            Variant v = getNextValue();
            if (param != v.getType()) {
              try {
                v = unaryOperation(OP_CAST, param, v);
              } catch (const std::exception&) {
                CSVSQLDB_THROW(StackMachineException, "calling function '" << funcname << "' with wrong parameter");
              }
            }
            parameter.emplace(parameter.end(), v);
            --count;
          }
          if (count) {
            CSVSQLDB_THROW(StackMachineException, "too much parameters for function '" << funcname << "'");
          }
          _valueStack.emplace(func->call(parameter));
          break;
        }
        case CAST: {
          Variant& rhs = getTopValue();
          rhs = unaryOperation(OP_CAST, instruction._value.getType(), rhs);
          break;
        }
        case IN: {
          size_t count = static_cast<size_t>(instruction._value.asInt());
          const Variant lhs = getNextValue();
          bool found(false);
          for (size_t n = 0; n < count; ++n) {
            Variant result = binaryOperation(OP_EQ, lhs, getNextValue());
            if (result.asBool()) {
              found = true;
              ++n;
              for (; n < count; ++n) {
                // remove rest of the values from stack
                _valueStack.pop();
              }
              break;
            }
          }
          if (found) {
            _valueStack.emplace(Variant(true));
          } else {
            _valueStack.emplace(Variant(false));
          }
          break;
        }
        case LIKE: {
          if (!instruction._r) {
            CSVSQLDB_THROW(StackMachineException, "expected a regexp in LIKE expression");
          }
          Variant lhs = getTopValue();
          if (lhs.getType() != STRING) {
            lhs = unaryOperation(OP_CAST, STRING, lhs);
            CSVSQLDB_THROW(StackMachineException, "can only do like operations on strings");
          }
          if (instruction._r->match(lhs.asString())) {
            _valueStack.emplace(Variant(true));
          } else {
            _valueStack.emplace(Variant(false));
          }
          break;
        }
      }
    }

    return _valueStack.top();
  }
}
