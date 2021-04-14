//
//  visitor.h
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

#include <csvsqldb/sql_ast.h>
#include <csvsqldb/stack_machine.h>

#include <algorithm>


namespace csvsqldb
{
  class CSVSQLDB_EXPORT ASTInstructionStackVisitor : public ASTExpressionNodeVisitor
  {
  public:
    ASTInstructionStackVisitor(StackMachine& sm, StackMachine::VariableMapping& mapping)
    : _sm(sm)
    , _mapping(mapping)
    , _index(0)
    {
    }

    void visit(ASTBinaryNode& node) override
    {
      if (node._op == OP_CONCAT) {
        node._rhs->accept(*this);
        node._lhs->accept(*this);
        _sm.addInstruction(StackMachine::Instruction(StackMachine::CONCAT));
      } else if (node._op == OP_ADD) {
        node._rhs->accept(*this);
        node._lhs->accept(*this);
        _sm.addInstruction(StackMachine::Instruction(StackMachine::ADD));
      } else if (node._op == OP_SUB) {
        node._rhs->accept(*this);
        node._lhs->accept(*this);
        _sm.addInstruction(StackMachine::Instruction(StackMachine::SUB));
      } else if (node._op == OP_MUL) {
        node._rhs->accept(*this);
        node._lhs->accept(*this);
        _sm.addInstruction(StackMachine::Instruction(StackMachine::MUL));
      } else if (node._op == OP_DIV) {
        node._rhs->accept(*this);
        node._lhs->accept(*this);
        _sm.addInstruction(StackMachine::Instruction(StackMachine::DIV));
      } else if (node._op == OP_MOD) {
        node._rhs->accept(*this);
        node._lhs->accept(*this);
        _sm.addInstruction(StackMachine::Instruction(StackMachine::MOD));
      } else if (node._op == OP_EQ) {
        node._rhs->accept(*this);
        node._lhs->accept(*this);
        _sm.addInstruction(StackMachine::Instruction(StackMachine::EQ));
      } else if (node._op == OP_NEQ) {
        node._rhs->accept(*this);
        node._lhs->accept(*this);
        _sm.addInstruction(StackMachine::Instruction(StackMachine::NEQ));
      } else if (node._op == OP_IS) {
        node._rhs->accept(*this);
        node._lhs->accept(*this);
        _sm.addInstruction(StackMachine::Instruction(StackMachine::IS));
      } else if (node._op == OP_ISNOT) {
        node._rhs->accept(*this);
        node._lhs->accept(*this);
        _sm.addInstruction(StackMachine::Instruction(StackMachine::ISNOT));
      } else if (node._op == OP_AND) {
        // TODO LCF: try to implement some shortcut code for bools
        node._lhs->accept(*this);
        node._rhs->accept(*this);
        _sm.addInstruction(StackMachine::Instruction(StackMachine::AND));
      } else if (node._op == OP_OR) {
        // TODO LCF: try to implement some shortcut code for bools
        node._lhs->accept(*this);
        node._rhs->accept(*this);
        _sm.addInstruction(StackMachine::Instruction(StackMachine::OR));
      } else if (node._op == OP_GT) {
        node._rhs->accept(*this);
        node._lhs->accept(*this);
        _sm.addInstruction(StackMachine::Instruction(StackMachine::GT));
      } else if (node._op == OP_LT) {
        node._rhs->accept(*this);
        node._lhs->accept(*this);
        _sm.addInstruction(StackMachine::Instruction(StackMachine::LT));
      } else if (node._op == OP_GE) {
        node._rhs->accept(*this);
        node._lhs->accept(*this);
        _sm.addInstruction(StackMachine::Instruction(StackMachine::GE));
      } else if (node._op == OP_LE) {
        node._rhs->accept(*this);
        node._lhs->accept(*this);
        _sm.addInstruction(StackMachine::Instruction(StackMachine::LE));
      } else {
        CSVSQLDB_THROW(StackMachineException, "not implemented");
      }
    }

    void visit(ASTUnaryNode& node) override
    {
      node._rhs->accept(*this);

      if (node._op == OP_NOT) {
        _sm.addInstruction(StackMachine::Instruction(StackMachine::NOT));
      } else if (node._op == OP_PLUS) {
        _sm.addInstruction(StackMachine::Instruction(StackMachine::PLUS));
      } else if (node._op == OP_MINUS) {
        _sm.addInstruction(StackMachine::Instruction(StackMachine::MINUS));
      } else if (node._op == OP_CAST) {
        _sm.addInstruction(StackMachine::Instruction(StackMachine::CAST, Variant(node._castType)));
      }
    }

    void visit(ASTValueNode& node) override
    {
      _sm.addInstruction(StackMachine::Instruction(StackMachine::PUSH, typedValueToVariant(node._value)));
    }

    void visit(ASTLikeNode& node) override
    {
      node._lhs->accept(*this);
      _sm.addInstruction(StackMachine::Instruction(StackMachine::LIKE, new csvsqldb::RegExp(node._like)));
    }

    void visit(ASTBetweenNode& node) override
    {
      node._to->accept(*this);
      node._from->accept(*this);
      node._lhs->accept(*this);
      _sm.addInstruction(StackMachine::Instruction(StackMachine::BETWEEN));
    }

    void visit(ASTInNode& node) override
    {
      for (auto i = node._expressions.rbegin(); i != node._expressions.rend(); ++i) {
        (*i)->accept(*this);
      }
      node._lhs->accept(*this);
      _sm.addInstruction(StackMachine::Instruction(StackMachine::IN, Variant(node._expressions.size())));
    }

    void visit(ASTFunctionNode& node) override
    {
      for (Parameters::reverse_iterator iter = node._parameters.rbegin(); iter != node._parameters.rend(); ++iter) {
        (*iter)._exp->accept(*this);
      }
      _sm.addInstruction(StackMachine::Instruction(StackMachine::FUNC, Variant(node._function->getName())));
    }

    void visit(ASTAggregateFunctionNode& node) override
    {
      // TODO LCF: add these expressions to a new collection and create a store variable for them
      CSVSQLDB_THROW(StackMachineException, "aggregation functions in expressions not allowed");
    }

    void visit(ASTIdentifier& node) override
    {
      size_t index = getMapping(node.getQualifiedIdentifier());
      _sm.addInstruction(StackMachine::Instruction(StackMachine::PUSHVAR, Variant(index)));
    }

  protected:
    size_t getMapping(const std::string& variable)
    {
      StackMachine::VariableMapping::iterator iter = std::find_if(
        _mapping.begin(), _mapping.end(), [&](const StackMachine::VariableIndex& var) { return var.first == variable; });

      if (iter == _mapping.end()) {
        _mapping.push_back(std::make_pair(variable, _index));
        return _index++;
      }

      return iter->second;
    }

    StackMachine& _sm;
    StackMachine::VariableMapping& _mapping;
    size_t _index;
  };


  class ASTExpressionVariableVisitor : public ASTExpressionNodeVisitor
  {
  public:
    ASTExpressionVariableVisitor(IdentifierSet& variables)
    : _variables(variables)
    {
    }

    void visit(ASTBinaryNode& node) override
    {
      node._rhs->accept(*this);
      node._lhs->accept(*this);
    }

    void visit(ASTUnaryNode& node) override
    {
      node._rhs->accept(*this);
    }

    void visit(ASTValueNode& node) override
    {
    }

    void visit(ASTLikeNode& node) override
    {
      node._lhs->accept(*this);
    }

    void visit(ASTBetweenNode& node) override
    {
      node._to->accept(*this);
      node._from->accept(*this);
      node._lhs->accept(*this);
    }

    void visit(ASTInNode& node) override
    {
    }

    void visit(ASTFunctionNode& node) override
    {
      for (Parameters::reverse_iterator iter = node._parameters.rbegin(); iter != node._parameters.rend(); ++iter) {
        (*iter)._exp->accept(*this);
      }
    }

    void visit(ASTAggregateFunctionNode& node) override
    {
    }

    void visit(ASTIdentifier& node) override
    {
      _variables.insert(node);
    }

  protected:
    IdentifierSet& _variables;
  };
}
