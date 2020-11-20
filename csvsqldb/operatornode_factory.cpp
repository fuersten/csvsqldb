//
//  operationnode_factory.cpp
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

#include "operatornode_factory.h"


namespace csvsqldb
{
  RootOperatorNodePtr OperatorNodeFactory::createOutputRowOperatorNode(OperatorContext& context,
                                                                       const SymbolTablePtr& symbolTable, std::ostream& stream)
  {
    return std::make_shared<OutputRowOperatorNode>(context, symbolTable, stream);
  }

  RowOperatorNodePtr OperatorNodeFactory::createLimitOperatorNode(OperatorContext& context, const SymbolTablePtr& symbolTable,
                                                                  const ASTExprNodePtr& limit, const ASTExprNodePtr& offset)
  {
    return std::make_shared<LimitOperatorNode>(context, symbolTable, limit, offset);
  }

  RowOperatorNodePtr OperatorNodeFactory::createSortOperatorNode(OperatorContext& context, const SymbolTablePtr& symbolTable,
                                                                 OrderExpressions orderExpressions)
  {
    return std::make_shared<SortOperatorNode>(context, symbolTable, orderExpressions);
  }

  RowOperatorNodePtr OperatorNodeFactory::createGroupingOperatorNode(OperatorContext& context, const SymbolTablePtr& symbolTable,
                                                                     const Expressions& nodes,
                                                                     const Identifiers& groupByIdentifiers)
  {
    return std::make_shared<GroupingOperatorNode>(context, symbolTable, nodes, groupByIdentifiers);
  }

  RowOperatorNodePtr OperatorNodeFactory::createAggregationOperatorNode(OperatorContext& context,
                                                                        const SymbolTablePtr& symbolTable,
                                                                        const Expressions& nodes)
  {
    return std::make_shared<AggregationOperatorNode>(context, symbolTable, nodes);
  }

  RowOperatorNodePtr OperatorNodeFactory::createExtendedProjectionOperatorNode(OperatorContext& context,
                                                                               const SymbolTablePtr& symbolTable,
                                                                               const Expressions& nodes)
  {
    return std::make_shared<ExtendedProjectionOperatorNode>(context, symbolTable, nodes);
  }

  RowOperatorNodePtr OperatorNodeFactory::createCrossJoinOperatorNode(OperatorContext& context, const SymbolTablePtr& symbolTable)
  {
    return std::make_shared<CrossJoinOperatorNode>(context, symbolTable);
  }

  RowOperatorNodePtr OperatorNodeFactory::createInnerJoinOperatorNode(OperatorContext& context, const SymbolTablePtr& symbolTable,
                                                                      const ASTExprNodePtr& exp)
  {
    return std::make_shared<InnerJoinOperatorNode>(context, symbolTable, exp);
  }

  RowOperatorNodePtr OperatorNodeFactory::createInnerHashJoinOperatorNode(OperatorContext& context,
                                                                          const SymbolTablePtr& symbolTable,
                                                                          const ASTExprNodePtr& exp)
  {
    return std::make_shared<InnerHashJoinOperatorNode>(context, symbolTable, exp);
  }

  RowOperatorNodePtr OperatorNodeFactory::createUnionOperatorNode(OperatorContext& context, const SymbolTablePtr& symbolTable)
  {
    return std::make_shared<UnionOperatorNode>(context, symbolTable);
  }

  RowOperatorNodePtr OperatorNodeFactory::createSelectOperatorNode(OperatorContext& context, const SymbolTablePtr& symbolTable,
                                                                   const ASTExprNodePtr& exp)
  {
    return std::make_shared<SelectOperatorNode>(context, symbolTable, exp);
  }

  RowOperatorNodePtr OperatorNodeFactory::createScanOperatorNode(OperatorContext& context, const SymbolTablePtr& symbolTable,
                                                                 const SymbolInfo& tableInfo)
  {
    return std::make_shared<TableScanOperatorNode>(context, symbolTable, tableInfo);
  }
}
