//
//  sql_astexpressionvisitor.h
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

#ifndef csvsqldb_astexpressionvisitor_h
#define csvsqldb_astexpressionvisitor_h

#include "libcsvsqldb/inc.h"

#include "sql_ast.h"

#include <memory>
#include <sstream>


namespace csvsqldb
{
  class CSVSQLDB_EXPORT ASTExpressionVisitor : public ASTExpressionNodeVisitor
  {
  public:
    ASTExpressionVisitor()
    {
    }

    void reset()
    {
      _ss.str("");
      _ss.clear();
    }

    std::string toString() const
    {
      return _ss.str();
    }

    void printOp(eOperationType op)
    {
      switch (op) {
        case OP_CAST:
        case OP_LIKE:
        case OP_BETWEEN:
        case OP_IN:
          CSVSQLDB_THROW(SqlException, "Cannot print CAST,LIKE,BETWEEN or IN operators");
        default:
          _ss << " " << operationTypeToString(op) << " ";
      }
    }

    void visit(ASTBinaryNode& node) override
    {
      _ss << "(";
      node._lhs->accept(*this);
      printOp(node._op);
      node._rhs->accept(*this);
      _ss << ")";
    }

    void visit(ASTUnaryNode& node) override
    {
      if (node._op == OP_CAST) {
        _ss << "CAST(";
        node._rhs->accept(*this);
        _ss << " AS " << typeToString(node._castType) << ")";
      } else {
        printOp(node._op);
        node._rhs->accept(*this);
      }
    }

    void visit(ASTValueNode& node) override
    {
      if (node._value._type == STRING) {
        _ss << "'";
      }
      _ss << printType(node._value._type, node._value._value);
      if (node._value._type == STRING) {
        _ss << "'";
      }
    }

    void visit(ASTLikeNode& node) override
    {
      node._lhs->accept(*this);
      _ss << "'" << node._like << "'";
    }

    void visit(ASTBetweenNode& node) override
    {
      node._lhs->accept(*this);
      _ss << " between ";
      node._from->accept(*this);
      _ss << " and ";
      node._to->accept(*this);
    }

    void visit(ASTInNode& node) override
    {
      node._lhs->accept(*this);
      _ss << " in (";
      bool first = true;
      for (const auto& exp : node._expressions) {
        if (!first) {
          _ss << ",";
        } else {
          first = false;
        }
        exp->accept(*this);
      }
      _ss << ")";
    }

    void visit(ASTFunctionNode& node) override
    {
      _ss << node._function->getName() << "(";
      bool first = true;
      for (const auto& param : node._parameters) {
        if (!first) {
          _ss << ",";
        } else {
          first = false;
        }
        param._exp->accept(*this);
      }
      _ss << ")";
    }

    void visit(ASTAggregateFunctionNode& node) override
    {
      _ss << aggregateFunctionToString(node._aggregateFunction) << "(";
      bool first = true;
      if (node._aggregateFunction == COUNT_STAR) {
        _ss << "*";
      }
      if (node._quantifier == DISTINCT) {
        _ss << "DISTINCT ";
      }
      for (const auto& param : node._parameters) {
        if (!first) {
          _ss << ",";
        } else {
          first = false;
        }
        param._exp->accept(*this);
      }
      _ss << ")";
    }

    void visit(ASTIdentifier& node) override
    {
      _ss << node.getQualifiedIdentifier();
    }

  protected:
    std::stringstream _ss;
  };
}

#endif
