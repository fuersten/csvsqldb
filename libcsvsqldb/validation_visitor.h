//
//  validation_visitor.h
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

#include "libcsvsqldb/inc.h"

#include "database.h"
#include "sql_ast.h"


namespace csvsqldb
{
  class CSVSQLDB_EXPORT ASTValidationVisitor : public ASTNodeVisitor
  {
  public:
    ASTValidationVisitor(const Database& database);

    void visit(ASTCreateTableNode& node) override;

    void visit(ASTMappingNode& node) override;

    void visit(ASTDropMappingNode& node) override;

    void visit(ASTAlterTableAddNode& node) override;

    void visit(ASTAlterTableDropNode& node) override;

    void visit(ASTDropTableNode& node) override;

    void visit(ASTExplainNode& node) override;

    void visit(ASTUnionNode& node) override;

    void visit(ASTQueryNode& node) override;

    void visit(ASTQuerySpecificationNode& node) override;

    void visit(ASTTableExpressionNode& node) override;

    void visit(ASTBinaryNode& node) override;

    void visit(ASTUnaryNode& node) override;

    void visit(ASTValueNode& node) override;

    void visit(ASTLikeNode& node) override;

    void visit(ASTBetweenNode& node) override;

    void visit(ASTInNode& node) override;

    void visit(ASTFunctionNode& node) override;

    void visit(ASTAggregateFunctionNode& node) override;

    void visit(ASTIdentifier& node) override;

    void visit(ASTQualifiedAsterisk& node) override;

    void visit(ASTFromNode& node) override;

    void visit(ASTTableIdentifierNode& node) override;

    void visit(ASTTableSubqueryNode& node) override;

    void visit(ASTCrossJoinNode& node) override;

    void visit(ASTNaturalJoinNode& node) override;

    void visit(ASTInnerJoinNode& node) override;

    void visit(ASTLeftJoinNode& node) override;

    void visit(ASTRightJoinNode& node) override;

    void visit(ASTFullJoinNode& node) override;

    void visit(ASTWhereNode& node) override;

    void visit(ASTGroupByNode& node) override;

    void visit(ASTHavingNode& node) override;

    void visit(ASTOrderByNode& node) override;

    void visit(ASTLimitNode& node) override;

  private:
    const Database& _database;
  };
}