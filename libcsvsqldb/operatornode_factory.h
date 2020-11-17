//
//  operationnode_factory.h
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

#ifndef csvsqldb_operation_node_factory_h
#define csvsqldb_operation_node_factory_h

#include "libcsvsqldb/inc.h"

#include "operatornode.h"


namespace csvsqldb
{
  struct OperatorNodeFactory {
  public:
    OperatorNodeFactory() = delete;

    static CSVSQLDB_EXPORT RootOperatorNodePtr createOutputRowOperatorNode(OperatorContext& context,
                                                                           const SymbolTablePtr& symbolTable,
                                                                           std::ostream& stream);

    static CSVSQLDB_EXPORT RowOperatorNodePtr createLimitOperatorNode(OperatorContext& context, const SymbolTablePtr& symbolTable,
                                                                      const ASTExprNodePtr& limit, const ASTExprNodePtr& offset);

    static CSVSQLDB_EXPORT RowOperatorNodePtr createSortOperatorNode(OperatorContext& context, const SymbolTablePtr& symbolTable,
                                                                     OrderExpressions orderExpressions);

    static CSVSQLDB_EXPORT RowOperatorNodePtr createGroupingOperatorNode(OperatorContext& context,
                                                                         const SymbolTablePtr& symbolTable,
                                                                         const Expressions& nodes,
                                                                         const Identifiers& groupByIdentifiers);

    static CSVSQLDB_EXPORT RowOperatorNodePtr createAggregationOperatorNode(OperatorContext& context,
                                                                            const SymbolTablePtr& symbolTable,
                                                                            const Expressions& nodes);

    static CSVSQLDB_EXPORT RowOperatorNodePtr createExtendedProjectionOperatorNode(OperatorContext& context,
                                                                                   const SymbolTablePtr& symbolTable,
                                                                                   const Expressions& nodes);

    static CSVSQLDB_EXPORT RowOperatorNodePtr createCrossJoinOperatorNode(OperatorContext& context,
                                                                          const SymbolTablePtr& symbolTable);

    static CSVSQLDB_EXPORT RowOperatorNodePtr createInnerJoinOperatorNode(OperatorContext& context,
                                                                          const SymbolTablePtr& symbolTable,
                                                                          const ASTExprNodePtr& exp);

    static CSVSQLDB_EXPORT RowOperatorNodePtr createInnerHashJoinOperatorNode(OperatorContext& context,
                                                                              const SymbolTablePtr& symbolTable,
                                                                              const ASTExprNodePtr& exp);

    static CSVSQLDB_EXPORT RowOperatorNodePtr createUnionOperatorNode(OperatorContext& context,
                                                                      const SymbolTablePtr& symbolTable);

    static CSVSQLDB_EXPORT RowOperatorNodePtr createSelectOperatorNode(OperatorContext& context,
                                                                       const SymbolTablePtr& symbolTable,
                                                                       const ASTExprNodePtr& exp);

    static CSVSQLDB_EXPORT RowOperatorNodePtr createScanOperatorNode(OperatorContext& context, const SymbolTablePtr& symbolTable,
                                                                     const SymbolInfo& tableInfo);
  };
}

#endif
