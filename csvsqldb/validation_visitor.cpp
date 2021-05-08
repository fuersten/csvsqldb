//
//  validation_visitor.cpp
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

#include "validation_visitor.h"


namespace csvsqldb
{
  ASTValidationVisitor::ASTValidationVisitor(const Database& database)
  : _database(database)
  {
  }

  void ASTValidationVisitor::visit(ASTCreateTableNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTMappingNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTDropMappingNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTAlterTableAddColumnNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTAlterTableDropColumnNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTDropTableNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTExplainNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTUnionNode& node)
  {
    node._rhs->symbolTable()->typeSymbolTable(_database);
    node._lhs->symbolTable()->typeSymbolTable(_database);

    RelationOutputParameter lo = node._lhs->outputParameter();
    RelationOutputParameter ro = node._rhs->outputParameter();

    if (lo.size() < 1) {
      CSVSQLDB_THROW(SqlParserException, "a relation in a UNION must have at least one element in the select list");
    }
    if (lo.size() != ro.size()) {
      CSVSQLDB_THROW(SqlParserException, "both sides of a UNION must have the same select list");
    }

    for (size_t n = 0; n < lo.size(); ++n) {
      if (lo[n] != ro[n]) {
        CSVSQLDB_THROW(SqlParserException, "both sides of a UNION must have the same select list");
      }
    }

    node._rhs->accept(*this);
    node._lhs->accept(*this);
  }

  void ASTValidationVisitor::visit(ASTQueryNode& node)
  {
    node.symbolTable()->typeSymbolTable(_database);
    node._query->accept(*this);
  }

  void ASTValidationVisitor::visit(ASTQuerySpecificationNode& node)
  {
    for (const auto& derivedColumn : node._nodes) {
      derivedColumn->accept(*this);
    }
    node._tableExpression->accept(*this);
  }

  void ASTValidationVisitor::visit(ASTTableExpressionNode& node)
  {
    node._from->accept(*this);
    if (node._where) {
      node._where->accept(*this);
    }
  }

  void ASTValidationVisitor::visit(ASTBinaryNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTUnaryNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTValueNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTLikeNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTBetweenNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTInNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTFunctionNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTAggregateFunctionNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTIdentifier&)
  {
  }

  void ASTValidationVisitor::visit(ASTQualifiedAsterisk&)
  {
  }

  void ASTValidationVisitor::visit(ASTFromNode& node)
  {
    if (node._tableReferences.size() > 1) {
      CSVSQLDB_THROW(SqlParserException, "implicit joins not allowed");
    }
  }

  void ASTValidationVisitor::visit(ASTTableIdentifierNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTTableSubqueryNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTCrossJoinNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTNaturalJoinNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTInnerJoinNode& node)
  {
    if (node._expression->type() != BOOLEAN) {
      CSVSQLDB_THROW(SqlParserException, "on condition has to be a boolean value expression");
    }
  }

  void ASTValidationVisitor::visit(ASTLeftJoinNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTRightJoinNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTFullJoinNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTWhereNode& node)
  {
    if (node._exp->type() != BOOLEAN) {
      CSVSQLDB_THROW(SqlParserException, "where condition has to be a boolean value expression");
    }
  }

  void ASTValidationVisitor::visit(ASTGroupByNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTHavingNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTOrderByNode&)
  {
  }

  void ASTValidationVisitor::visit(ASTLimitNode&)
  {
  }
}
