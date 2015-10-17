//
//  execution_plan_creator.h
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

#ifndef csvsqldb_execution_plan_creator_h
#define csvsqldb_execution_plan_creator_h

#include "libcsvsqldb/inc.h"

#include "sql_ast.h"
#include "sql_astdump.h"
#include "table_executions.h"
#include "operatornode.h"


namespace csvsqldb
{

    class CSVSQLDB_EXPORT QueryExecutionNode : public ExecutionNode
    {
    public:
        QueryExecutionNode(const OperatorContext& context);

        const OperatorContext& getContext() const;

        void setRootOperatorNode(const RootOperatorNodePtr& queryOperationRoot);

        virtual int64_t execute();

        virtual void dump(std::ostream& stream) const;

    private:
        const OperatorContext& _context;
        RootOperatorNodePtr _queryOperationRoot;
    };

    class CSVSQLDB_EXPORT ExplainExecutionNode : public ExecutionNode
    {
    public:
        ExplainExecutionNode(OperatorContext& context, eDescriptionType descType, const ASTQueryNodePtr& query);

        virtual int64_t execute();

        virtual void dump(std::ostream& stream) const;

    private:
        OperatorContext& _context;
        eDescriptionType _descType;
        ASTQueryNodePtr _query;
    };


    template <typename OperatorFactory>
    class ExecutionPlanVisitor : public ASTNodeVisitor
    {
    public:
        ExecutionPlanVisitor(OperatorContext& context, ExecutionPlan& executionPlan, std::ostream& outputStream)
        : _context(context)
        , _executionPlan(executionPlan)
        , _outputStream(outputStream)
        {
        }

        virtual void visit(ASTCreateTableNode& node)
        {
            ExecutionNode::UniquePtr execNode(
            new CreateTableExecutionNode(_context._database, node._tableName, node._columnDefinitions, node._tableConstraints, node._createIfNotExists));
            _executionPlan.addExecutionNode(execNode);
        }

        virtual void visit(ASTMappingNode& node)
        {
            ExecutionNode::UniquePtr execNode(new CreateMappingExecutionNode(_context._database, node._tableName, node._mappings));
            _executionPlan.addExecutionNode(execNode);
        }

        virtual void visit(ASTDropMappingNode& node)
        {
            ExecutionNode::UniquePtr execNode(new DropMappingExecutionNode(_context._database, node._tableName));
            _executionPlan.addExecutionNode(execNode);
        }

        virtual void visit(ASTAlterTableAddNode& node)
        {
        }

        virtual void visit(ASTAlterTableDropNode& node)
        {
        }

        virtual void visit(ASTDropTableNode& node)
        {
            ExecutionNode::UniquePtr execNode(new DropTableExecutionNode(_context._database, node._tableName));
            _executionPlan.addExecutionNode(execNode);
        }

        virtual void visit(ASTExplainNode& node)
        {
            ExecutionNode::UniquePtr execNode(new ExplainExecutionNode(_context, node._descType, node._query));
            _executionPlan.addExecutionNode(execNode);
        }

        virtual void visit(ASTQueryNode& node)
        {
            std::unique_ptr<QueryExecutionNode> execNode(new QueryExecutionNode(_context));
            node._query->accept(*this);

            RootOperatorNodePtr output = OperatorFactory::createOutputRowOperatorNode(_context, node._query->symbolTable(), _outputStream);
            output->connect(_currentRowOperator);

            execNode->setRootOperatorNode(output);

            ExecutionNode::UniquePtr epn(std::move(execNode));
            _executionPlan.addExecutionNode(epn);
        }

        virtual void visit(ASTUnionNode& node)
        {
            RowOperatorNodePtr unionop = OperatorFactory::createUnionOperatorNode(_context, node._rhs->symbolTable());
            node._rhs->accept(*this);
            unionop->connect(_currentRowOperator);

            node._lhs->accept(*this);
            unionop->connect(_currentRowOperator);
            _currentRowOperator = unionop;
        }

        virtual void visit(ASTQuerySpecificationNode& node)
        {
            node._tableExpression->accept(*this);

            RowOperatorNodePtr projection;

            if(node._tableExpression->_group) {
                projection = OperatorFactory::createGroupingOperatorNode(_context,
                                                                         node._nodes[0]->symbolTable(),
                                                                         node._nodes,
                                                                         node._tableExpression->_group->_identifiers);
            } else if(std::dynamic_pointer_cast<ASTAggregateFunctionNode>(node._nodes[0])) {
                for(const auto& aggrNode : node._nodes) {
                    ASTAggregateFunctionNodePtr aggr = std::dynamic_pointer_cast<ASTAggregateFunctionNode>(aggrNode);
                    if(!aggr) {
                        CSVSQLDB_THROW(csvsqldb::Exception, "no aggregation on other than aggregation functions");
                    }
                }
                projection = OperatorFactory::createAggregationOperatorNode(_context, node._nodes[0]->symbolTable(), node._nodes);
            } else {
                projection = OperatorFactory::createExtendedProjectionOperatorNode(_context, node._nodes[0]->symbolTable(), node._nodes);
            }

            projection->connect(_currentRowOperator);
            _currentRowOperator = projection;

            if(node._tableExpression->_order) {
                node._tableExpression->_order->accept(*this);
            }

            if(node._tableExpression->_limit) {
                node._tableExpression->_limit->accept(*this);
            }
        }

        virtual void visit(ASTTableExpressionNode& node)
        {
            node._from->accept(*this);
            if(node._where) {
                node._where->accept(*this);
            }
        }

        virtual void visit(ASTBinaryNode& node)
        {
        }

        virtual void visit(ASTUnaryNode& node)
        {
        }

        virtual void visit(ASTValueNode& node)
        {
        }

        virtual void visit(ASTLikeNode& node)
        {
        }

        virtual void visit(ASTBetweenNode& node)
        {
        }

        virtual void visit(ASTInNode& node)
        {
        }

        virtual void visit(ASTFunctionNode& node)
        {
        }

        virtual void visit(ASTAggregateFunctionNode& node)
        {
        }

        virtual void visit(ASTIdentifier& node)
        {
        }

        virtual void visit(ASTQualifiedAsterisk& node)
        {
        }

        virtual void visit(ASTFromNode& node)
        {
            for(const auto& reference : node._tableReferences) {
                reference->accept(*this);
            }
        }

        virtual void visit(ASTTableIdentifierNode& node)
        {
            RowOperatorNodePtr scan;
            if(node._factor->getQualifiedIdentifier().substr(0, 7) == "SYSTEM_") {
                scan = std::make_shared<SystemTableScanOperatorNode>(_context, node.symbolTable(), *node._factor->_info);
            } else {
                scan = OperatorFactory::createScanOperatorNode(_context, node.symbolTable(), *node._factor->_info);
            }
            _currentRowOperator = scan;
        }

        virtual void visit(ASTTableSubqueryNode& node)
        {
            node._query->accept(*this);
            // here the _currentRowOperator needs to be told, that it maybe has to supply an alias to its output symbols
            _currentRowOperator->setOutputAlias(node._queryAlias);
        }

        virtual void visit(ASTCrossJoinNode& node)
        {
            RowOperatorNodePtr join = OperatorFactory::createCrossJoinOperatorNode(_context, node._factor->symbolTable());
            node._tableReference->accept(*this);
            join->connect(_currentRowOperator);

            node._factor->accept(*this);
            join->connect(_currentRowOperator);
            _currentRowOperator = join;
        }

        virtual void visit(ASTNaturalJoinNode& node)
        {
        }

        virtual void visit(ASTInnerJoinNode& node)
        {
            RowOperatorNodePtr join;

            ASTBinaryNodePtr binaryExpression = std::dynamic_pointer_cast<ASTBinaryNode>(node._expression);
            if(binaryExpression && binaryExpression->_op == OP_EQ && std::dynamic_pointer_cast<ASTIdentifier>(binaryExpression->_lhs)
               && std::dynamic_pointer_cast<ASTIdentifier>(binaryExpression->_rhs)) {
                // only equi joins can be performed as hash joins
                join = OperatorFactory::createInnerHashJoinOperatorNode(_context, node._factor->symbolTable(), node._expression);
            } else {
                join = OperatorFactory::createInnerJoinOperatorNode(_context, node._factor->symbolTable(), node._expression);
            }

            node._tableReference->accept(*this);
            join->connect(_currentRowOperator);

            node._factor->accept(*this);
            join->connect(_currentRowOperator);
            _currentRowOperator = join;
        }

        virtual void visit(ASTLeftJoinNode& node)
        {
        }

        virtual void visit(ASTRightJoinNode& node)
        {
        }

        virtual void visit(ASTFullJoinNode& node)
        {
        }

        virtual void visit(ASTWhereNode& node)
        {
            RowOperatorNodePtr select = OperatorFactory::createSelectOperatorNode(_context, node.symbolTable(), node._exp);
            select->connect(_currentRowOperator);
            _currentRowOperator = select;
        }

        virtual void visit(ASTGroupByNode& node)
        {
        }

        virtual void visit(ASTHavingNode& node)
        {
        }

        virtual void visit(ASTOrderByNode& node)
        {
            RowOperatorNodePtr sort = OperatorFactory::createSortOperatorNode(_context, node.symbolTable(), node._orderExpressions);
            sort->connect(_currentRowOperator);
            _currentRowOperator = sort;
        }

        virtual void visit(ASTLimitNode& node)
        {
            RowOperatorNodePtr limit = OperatorFactory::createLimitOperatorNode(_context, node.symbolTable(), node._limit, node._offset);
            limit->connect(_currentRowOperator);
            _currentRowOperator = limit;
        }

    private:
        OperatorContext& _context;
        ExecutionPlan& _executionPlan;
        RowOperatorNodePtr _currentRowOperator;
        std::ostream& _outputStream;
    };
}

#endif
