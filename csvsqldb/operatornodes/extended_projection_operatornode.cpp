//
//  operatornode.cpp
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

#include "extended_projection_operatornode.h"

#include <csvsqldb/sql_astexpressionvisitor.h>
#include <csvsqldb/visitor.h>


namespace csvsqldb
{
  ExtendedProjectionOperatorNode::ExtendedProjectionOperatorNode(const OperatorContext& context,
                                                                 const SymbolTablePtr& symbolTable, const Expressions& nodes)
  : RowOperatorNode(context, symbolTable)
  , _nodes(nodes)
  {
  }

  const Values* ExtendedProjectionOperatorNode::getNextRow()
  {
    return _iterator->getNextRow();
  }

  bool ExtendedProjectionOperatorNode::connect(const RowOperatorNodePtr& input)
  {
    _input = input;
    _input->getColumnInfos(_inputSymbols);

    size_t index = 0;
    for (const auto& exp : _nodes) {
      if (std::dynamic_pointer_cast<ASTIdentifier>(exp)) {
        ASTIdentifierPtr ident = std::dynamic_pointer_cast<ASTIdentifier>(exp);

        bool found = false;
        for (size_t n = 0; !found && n < _inputSymbols.size(); ++n) {
          const SymbolInfoPtr& info = _inputSymbols[n];

          if ((!ident->_info->_name.empty() && (ident->_info->_name == info->_name)) ||
              (!ident->_info->_qualifiedIdentifier.empty() &&
               (ident->_info->_qualifiedIdentifier == info->_qualifiedIdentifier)) ||
              (ident->_info->_prefix.empty() && (ident->_info->_identifier == info->_identifier))) {
            _outputSymbols.push_back(ident->_info);
            _types.push_back(ident->_info->_type);
            _outputInputMapping.insert(std::make_pair(index, n));
            found = true;
          }
        }
        if (!found) {
          CSVSQLDB_THROW(csvsqldb::Exception, "select list expression '" << ident->_info->_name << "' not found in context");
        }
      } else if (std::dynamic_pointer_cast<ASTQualifiedAsterisk>(exp)) {
        const std::string& prefixName = std::dynamic_pointer_cast<ASTQualifiedAsterisk>(exp)->_prefix;

        SymbolInfoPtr table;
        if (!prefixName.empty() && getSymbolTable().hasTableSymbol(prefixName)) {
          table = getSymbolTable().findTableSymbol(prefixName);
        }

        if (table) {
          if (!_context._database.hasTable(table->_identifier)) {
            CSVSQLDB_THROW(csvsqldb::Exception, "cannot find table '" << table->_identifier << "'");
          }
          // TODO LCF: this is not quite ok, as we could have a different sorting, so better go through the
          // _inputSymbols and find the right table symbols
          const TableData& tableData = _context._database.getTable(table->_identifier);

          for (size_t n = 0; n < tableData.columnCount(); ++n) {
            const SymbolInfoPtr& info = getSymbolTable().findSymbolNameForTable(table->_identifier, tableData.getColumn(n)._name);
            auto result = std::find(_inputSymbols.begin(), _inputSymbols.end(), info);
            if (result == _inputSymbols.end()) {
              CSVSQLDB_THROW(csvsqldb::Exception, "could not find output symbol '" << info->_identifier << "' in input symbols");
            }
            _outputSymbols.push_back(*result);
            _types.push_back(info->_type);
          }
        } else {
          for (const auto& info : _inputSymbols) {
            _outputSymbols.push_back(info);
            _types.push_back(info->_type);
          }
        }
      } else {
        const SymbolInfoPtr& outputInfo = getSymbolTable().findSymbol(exp->_symbolName);
        _outputSymbols.push_back(outputInfo);
        _types.push_back(outputInfo->_type);

        StackMachine sm;
        IdentifierSet expressionVariables;

        StackMachine::VariableMapping mapping;
        {
          ASTInstructionStackVisitor visitor(sm, mapping);
          exp->accept(visitor);
        }

        {
          ASTExpressionVariableVisitor visitor(expressionVariables);
          exp->accept(visitor);
        }

        VariableMapping varMapping;
        for (const auto& variable : expressionVariables) {
          bool found = false;
          for (size_t n = 0; !found && n < _inputSymbols.size(); ++n) {
            const SymbolInfoPtr& info = _inputSymbols[n];

            if (variable._info->_name == info->_name) {
              varMapping.push_back(std::make_pair(getMapping(variable.getQualifiedIdentifier(), mapping), n));
              found = true;
            }
          }
          if (!found) {
            CSVSQLDB_THROW(csvsqldb::Exception, "variable '" << variable.getQualifiedIdentifier() << "' not found");
          }
        }
        _sms.push_back(StackMachineType(sm, varMapping));
      }
      ++index;
    }

    _block = _context._blockManager.createBlock();
    _iterator = std::make_shared<BlockIterator>(_types, *this, getBlockManager());

    return true;
  }

  void ExtendedProjectionOperatorNode::getColumnInfos(SymbolInfos& outputSymbols)
  {
    remapOutputSymbols(_outputSymbols);
    outputSymbols = _outputSymbols;
  }

  BlockPtr ExtendedProjectionOperatorNode::getNextBlock()
  {
    BlockPtr previousBlock = prepareNextBuffer();
    return previousBlock ? previousBlock : _block;
  }

  BlockPtr ExtendedProjectionOperatorNode::prepareNextBuffer()
  {
    const Values* row = nullptr;
    BlockPtr previousBlock = nullptr;

    while (!previousBlock && (row = _input->getNextRow())) {
      size_t smIndex = 0;
      size_t index = 0;
      for (const auto& exp : _nodes) {
        if (std::dynamic_pointer_cast<ASTIdentifier>(exp)) {
          OutputInputMapping::const_iterator iter = _outputInputMapping.find(index);
          if (iter == _outputInputMapping.end()) {
            CSVSQLDB_THROW(csvsqldb::Exception, "selection expression no. " << index << " not found in output input mappings");
          }
          const Values& rrow = *row;
          if (!_block->addValue(*(rrow[iter->second]))) {
            _block->markNextBlock();
            auto tmpBlock = _context._blockManager.createBlock();
            previousBlock = _block;
            _block = tmpBlock;
            _block->addValue(*(rrow[iter->second]));
          }
        } else if (std::dynamic_pointer_cast<ASTQualifiedAsterisk>(exp)) {
          const std::string& prefixName = std::dynamic_pointer_cast<ASTQualifiedAsterisk>(exp)->_prefix;

          SymbolInfoPtr table;
          if (!prefixName.empty() && getSymbolTable().hasTableSymbol(prefixName)) {
            table = getSymbolTable().findTableSymbol(prefixName);
          }

          if (table) {
            if (!_context._database.hasTable(table->_identifier)) {
              CSVSQLDB_THROW(csvsqldb::Exception, "cannot find table '" << table->_identifier << "'");
            }
            // TODO LCF: this is not quite ok, as we could have a different sorting, so better go through the
            // _inputSymbols and find the right table symbols
            const TableData& tableData = _context._database.getTable(table->_identifier);

            const Values& rrow = *row;
            for (size_t n = 0; n < tableData.columnCount(); ++n) {
              if (!_block->addValue(*(rrow[n]))) {
                _block->markNextBlock();
                auto tmpBlock = _context._blockManager.createBlock();
                previousBlock = _block;
                _block = tmpBlock;
                _block->addValue(*(rrow[n]));
              }
            }
          } else {
            const Values& rrow = *row;
            for (size_t n = 0; n < _inputSymbols.size(); ++n) {
              if (!_block->addValue(*(rrow[n]))) {
                _block->markNextBlock();
                auto tmpBlock = _context._blockManager.createBlock();
                previousBlock = _block;
                _block = tmpBlock;
                _block->addValue(*(rrow[n]));
              }
            }
          }
        } else {
          fillVariableStore(_sms[smIndex]._store, _sms[smIndex]._variableMappings, *row);

          if (!_block->addValue(_sms[smIndex]._sm.evaluate(_sms[smIndex]._store, _context._functions))) {
            _block->markNextBlock();
            auto tmpBlock = _context._blockManager.createBlock();
            previousBlock = _block;
            _block = tmpBlock;
            _block->addValue(_sms[smIndex]._sm.evaluate(_sms[smIndex]._store, _context._functions));
          }
          ++smIndex;
        }
        ++index;
      }
      _block->nextRow();
    }
    if (!row) {
      _block->endBlocks();
    }

    return previousBlock;
  }

  void ExtendedProjectionOperatorNode::dump(std::ostream& stream) const
  {
    stream << "ExtendedProjectionOperator (";
    bool first(true);
    for (const auto& entry : _outputSymbols) {
      if (first) {
        first = false;
      } else {
        stream << ",";
      }
      stream << entry->_name;
    }
    stream << ")\n-->";
    _input->dump(stream);
  }
}
