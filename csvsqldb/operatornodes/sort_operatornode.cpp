//
//  sort_operatornode.cpp
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

#include "sort_operatornode.h"

#include "sql_astexpressionvisitor.h"

namespace csvsqldb
{
  SortOperatorNode::SortOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable,
                                     OrderExpressions orderExpressions)
  : RowOperatorNode(context, symbolTable)
  , _orderExpressions(std::move(orderExpressions))
  {
  }

  const Values* SortOperatorNode::getNextRow()
  {
    return _iterator->getNextRow();
  }

  bool SortOperatorNode::connect(const RowOperatorNodePtr& input)
  {
    _input = input;
    _input->getColumnInfos(_inputSymbols);

    SortingBlockIterator::SortOrders sortOrders;

    for (const auto& orderExp : _orderExpressions) {
      ASTIdentifierPtr ident = std::dynamic_pointer_cast<ASTIdentifier>(orderExp.first);
      if (ident) {
        bool found = false;
        for (size_t n = 0; !found && n < _inputSymbols.size(); ++n) {
          const SymbolInfoPtr& info = _inputSymbols[n];

          if ((!ident->_info->_name.empty() && (ident->_info->_name == info->_name)) ||
              (!ident->_info->_qualifiedIdentifier.empty() &&
               (ident->_info->_qualifiedIdentifier == info->_qualifiedIdentifier)) ||
              (ident->_info->_prefix.empty() && ident->_info->_identifier == info->_identifier)) {
            sortOrders.push_back({n, orderExp.second});
            found = true;
          }
        }
        if (!found) {
          CSVSQLDB_THROW(csvsqldb::Exception, "order expression '" << ident->_info->_name << "' not found in context");
        }
      } else {
        CSVSQLDB_THROW(csvsqldb::Exception, "complex order expression not supported yet");
      }
    }

    for (const auto& info : _inputSymbols) {
      _types.push_back(info->_type);
    }
    _iterator = std::make_shared<SortingBlockIterator>(_types, sortOrders, *_input, getBlockManager());

    return true;
  }

  void SortOperatorNode::getColumnInfos(SymbolInfos& outputSymbols)
  {
    remapOutputSymbols(_inputSymbols);
    outputSymbols = _inputSymbols;
  }

  void SortOperatorNode::dump(std::ostream& stream) const
  {
    stream << "SortOperator (";
    bool first(true);
    for (const auto& entry : _orderExpressions) {
      if (first) {
        first = false;
      } else {
        stream << ",";
      }
      ASTExpressionVisitor visitor;
      entry.first->accept(visitor);
      stream << visitor.toString();
      stream << " " << orderToString(entry.second);
    }
    stream << ")\n-->";
    _input->dump(stream);
  }
}
