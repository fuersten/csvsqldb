//
//  validation_visitor.h
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

#ifndef csvsqldb_validation_visitor_h
#define csvsqldb_validation_visitor_h

#include "libcsvsqldb/inc.h"

#include "database.h"
#include "sql_ast.h"


namespace csvsqldb
{

    class CSVSQLDB_EXPORT ASTValidationVisitor : public ASTNodeVisitor
    {
    public:
        ASTValidationVisitor(const Database& database);

        virtual void visit(ASTCreateTableNode& node);

        virtual void visit(ASTMappingNode& node);

        virtual void visit(ASTDropMappingNode& node);

        virtual void visit(ASTAlterTableAddNode& node);

        virtual void visit(ASTAlterTableDropNode& node);

        virtual void visit(ASTDropTableNode& node);

        virtual void visit(ASTExplainNode& node);

        virtual void visit(ASTUnionNode& node);

        virtual void visit(ASTQueryNode& node);

        virtual void visit(ASTQuerySpecificationNode& node);

        virtual void visit(ASTTableExpressionNode& node);

        virtual void visit(ASTBinaryNode& node);

        virtual void visit(ASTUnaryNode& node);

        virtual void visit(ASTValueNode& node);

        virtual void visit(ASTLikeNode& node);

        virtual void visit(ASTBetweenNode& node);

        virtual void visit(ASTInNode& node);

        virtual void visit(ASTFunctionNode& node);

        virtual void visit(ASTAggregateFunctionNode& node);

        virtual void visit(ASTIdentifier& node);

        virtual void visit(ASTQualifiedAsterisk& node);

        virtual void visit(ASTFromNode& node);

        virtual void visit(ASTTableIdentifierNode& node);

        virtual void visit(ASTTableSubqueryNode& node);

        virtual void visit(ASTCrossJoinNode& node);

        virtual void visit(ASTNaturalJoinNode& node);

        virtual void visit(ASTInnerJoinNode& node);

        virtual void visit(ASTLeftJoinNode& node);

        virtual void visit(ASTRightJoinNode& node);

        virtual void visit(ASTFullJoinNode& node);

        virtual void visit(ASTWhereNode& node);

        virtual void visit(ASTGroupByNode& node);

        virtual void visit(ASTHavingNode& node);

        virtual void visit(ASTOrderByNode& node);

        virtual void visit(ASTLimitNode& node);

    private:
        const Database& _database;
    };
}

#endif
