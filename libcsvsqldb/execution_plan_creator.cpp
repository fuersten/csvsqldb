//
//  execution_plan_creator.cpp
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

#include "execution_plan_creator.h"
#include "operatornode_factory.h"
#include "validation_visitor.h"


namespace csvsqldb
{

    QueryExecutionNode::QueryExecutionNode(const OperatorContext& context)
    : _context(context)
    {
    }

    const OperatorContext& QueryExecutionNode::getContext() const
    {
        return _context;
    }

    void QueryExecutionNode::setRootOperatorNode(const RootOperatorNodePtr& queryOperationRoot)
    {
        _queryOperationRoot = queryOperationRoot;
    }

    int64_t QueryExecutionNode::execute()
    {
        return _queryOperationRoot->process();
    }

    void QueryExecutionNode::dump(std::ostream& stream) const
    {
        _queryOperationRoot->dump(stream);
    }


    ExplainExecutionNode::ExplainExecutionNode(OperatorContext& context, eDescriptionType descType, const ASTQueryNodePtr& query)
    : _context(context)
    , _descType(descType)
    , _query(query)
    {
    }

    int64_t ExplainExecutionNode::execute()
    {
        switch(_descType) {
            case AST: {
                ASTNodeDumpVisitor visitor;
                _query->accept(visitor);
                std::cout << std::endl;
                break;
            }
            case EXEC: {
                std::stringstream ss;
                ExecutionPlan execPlan;
                ASTValidationVisitor validationVisitor(_context._database);
                _query->accept(validationVisitor);
                ExecutionPlanVisitor<OperatorNodeFactory> execVisitor(_context, execPlan, ss);
                _query->accept(execVisitor);
                execPlan.dump(std::cout);
                break;
            }
        }

        return 0;
    }

    void ExplainExecutionNode::dump(std::ostream& stream) const
    {
    }
}
