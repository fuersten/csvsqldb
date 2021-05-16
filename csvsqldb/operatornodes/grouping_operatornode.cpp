//
//  grouping_operatornode.cpp
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

#include "grouping_operatornode.h"


namespace csvsqldb
{
  GroupingOperatorNode::GroupingOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable,
                                             const Expressions& nodes, const Identifiers& groupByIdentifiers)
  : RowOperatorNode(context, symbolTable)
  , _nodes(nodes)
  , _groupByIdentifiers(groupByIdentifiers)
  {
  }

  const Values* GroupingOperatorNode::getNextRow()
  {
    return _iterator->getNextRow();
  }

  void GroupingOperatorNode::addPathThrough(const ASTIdentifierPtr& ident, csvsqldb::IndexVector& groupingIndices,
                                            csvsqldb::IndexVector& outputColumns, bool suppress)
  {
    bool found = false;
    for (size_t n = 0; !found && n < _inputSymbols.size(); ++n) {
      const SymbolInfoPtr& info = _inputSymbols[n];

      if ((!ident->_info->_name.empty() && (ident->_info->_name == info->_name)) ||
          (!ident->_info->_qualifiedIdentifier.empty() && (ident->_info->_qualifiedIdentifier == info->_qualifiedIdentifier)) ||
          (ident->_info->_prefix.empty() && ident->_info->_identifier == info->_identifier)) {
        groupingIndices.push_back(n);
        outputColumns.push_back(n);
        if (!suppress) {
          _outputSymbols.push_back(info);
          _types.push_back(info->_type);
        }
        _aggregateFunctions.push_back(std::make_shared<PaththroughAggregationFunction>(suppress));
        found = true;
      }
    }
    if (!found) {
      CSVSQLDB_THROW(csvsqldb::Exception, "group expression '" << ident->_info->_qualifiedIdentifier << "' not found in context");
    }
  }

  bool GroupingOperatorNode::connect(const RowOperatorNodePtr& input)
  {
    _input = input;
    _input->getColumnInfos(_inputSymbols);

    csvsqldb::IndexVector groupingIndices;
    csvsqldb::IndexVector outputColumns;
    Identifiers foundGroupByIdentifiers;

    for (const auto& groupExp : _nodes) {
      ASTIdentifierPtr ident = std::dynamic_pointer_cast<ASTIdentifier>(groupExp);
      if (ident) {
        addPathThrough(ident, groupingIndices, outputColumns, false);
        auto result =
          std::find_if(_groupByIdentifiers.begin(), _groupByIdentifiers.end(), [&](const ASTIdentifierPtr& identifier) {
            return ident->_info->_qualifiedIdentifier == identifier->_info->_qualifiedIdentifier;
          });
        if (result == std::end(_groupByIdentifiers)) {
          CSVSQLDB_THROW(csvsqldb::Exception,
                         "all elements of the select list of a group by have to be aggregations or contained in the "
                         "group by expressions");
        }
        foundGroupByIdentifiers.push_back(*result);
      } else if (std::dynamic_pointer_cast<ASTAggregateFunctionNode>(groupExp)) {
        ASTAggregateFunctionNodePtr aggr = std::dynamic_pointer_cast<ASTAggregateFunctionNode>(groupExp);
        size_t rowValueId = 0;
        eType type = INT;

        if (aggr->_aggregateFunction != COUNT_STAR) {
          if (!std::dynamic_pointer_cast<ASTIdentifier>(aggr->_parameter->_exp)) {
            CSVSQLDB_THROW(csvsqldb::Exception, "currently only identifier allowed as aggregation parameter");
          }
          std::string param = std::dynamic_pointer_cast<ASTIdentifier>(aggr->_parameter->_exp)->getQualifiedIdentifier();
          bool found = false;
          for (const auto& info : _inputSymbols) {
            if (info->_name == param) {
              found = true;
              type = info->_type;
              break;
            }
            ++rowValueId;
          }
          if (!found) {
            CSVSQLDB_THROW(csvsqldb::Exception, "aggregation parameter '" << param << "' not found");
          }
        }

        _aggregateFunctions.push_back(AggregationFunction::create(aggr->_aggregateFunction, type));
        if (aggr->_aggregateFunction == COUNT || aggr->_aggregateFunction == COUNT_STAR) {
          type = INT;
        }

        _types.push_back(type);
        const SymbolInfoPtr& outputInfo = getSymbolTable().findSymbol(groupExp->_symbolName);
        _outputSymbols.push_back(outputInfo);
        outputColumns.push_back(rowValueId);
      } else {
        CSVSQLDB_THROW(csvsqldb::Exception, "only grouping values and aggregate function allowed in group by");
      }
    }
    for (const auto& groupBy : _groupByIdentifiers) {
      auto result =
        std::find_if(foundGroupByIdentifiers.begin(), foundGroupByIdentifiers.end(), [&](const ASTIdentifierPtr& identifier) {
          return groupBy->_qualifiedIdentifier == identifier->_qualifiedIdentifier;
        });
      if (result == std::end(foundGroupByIdentifiers)) {
        addPathThrough(groupBy, groupingIndices, outputColumns, true);
      }
    }

    _iterator = std::make_shared<GroupingBlockIterator>(_types, groupingIndices, outputColumns, _aggregateFunctions, *_input,
                                                        getBlockManager());

    return true;
  }

  void GroupingOperatorNode::getColumnInfos(SymbolInfos& outputSymbols)
  {
    remapOutputSymbols(_outputSymbols);
    outputSymbols = _outputSymbols;
  }

  void GroupingOperatorNode::dump(std::ostream& stream) const
  {
    stream << "GroupingOperator (";
    bool first(true);
    for (const auto& entry : _aggregateFunctions) {
      if (first) {
        first = false;
      } else {
        stream << ",";
      }
      stream << entry->toString();
    }
    stream << " -> ";
    first = true;
    for (const auto& group : _groupByIdentifiers) {
      if (first) {
        first = false;
      } else {
        stream << ",";
      }
      stream << group->_identifier;
    }
    stream << ")\n";
    stream << "-->";
    _input->dump(stream);
  }
}
