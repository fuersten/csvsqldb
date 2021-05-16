//
//  operatornode.h
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

#include <csvsqldb/inc.h>

#include <csvsqldb/block.h>
#include <csvsqldb/database.h>
#include <csvsqldb/function_registry.h>
#include <csvsqldb/sql_ast.h>
#include <csvsqldb/stack_machine.h>
#include <csvsqldb/symboltable.h>


namespace csvsqldb
{
  struct CSVSQLDB_EXPORT OperatorContext {
    OperatorContext(Database& database, const FunctionRegistry& functions, BlockManager& blockManager,
                    const csvsqldb::StringVector& files)
    : _database(database)
    , _functions(functions)
    , _blockManager(blockManager)
    , _files(files)
    {
    }

    Database& _database;
    const FunctionRegistry& _functions;
    BlockManager& _blockManager;
    const csvsqldb::StringVector& _files;
    bool _showHeaderLine{true};
  };


  class OperatorBaseNode;
  using OperatorBaseNodePtr = std::shared_ptr<OperatorBaseNode>;

  class RowOperatorNode;
  using RowOperatorNodePtr = std::shared_ptr<RowOperatorNode>;

  class RootOperatorNode;
  using RootOperatorNodePtr = std::shared_ptr<RootOperatorNode>;


  class CSVSQLDB_EXPORT OperatorBaseNode
  {
  public:
    using VariableIndexMapping = std::vector<std::pair<size_t, size_t>>;
    using OutputInputMapping = std::map<size_t, size_t>;

    struct StackMachineType {
      StackMachineType() = default;

      StackMachineType(StackMachine sm, VariableIndexMapping variableMappings)
      : _sm(std::move(sm))
      , _variableMappings(std::move(variableMappings))
      {
      }

      StackMachine _sm;
      VariableStore _store;
      VariableIndexMapping _variableMappings;
    };

    using StackMachines = std::vector<StackMachineType>;

    OperatorBaseNode(const OperatorContext& context, const SymbolTablePtr& symbolTable)
    : _context(context)
    , _symbolTable(symbolTable)
    {
    }

    virtual ~OperatorBaseNode() = default;

    OperatorBaseNode(const OperatorBaseNode&) = delete;
    OperatorBaseNode& operator=(const OperatorBaseNode&) = delete;
    OperatorBaseNode(OperatorBaseNode&&) = delete;
    OperatorBaseNode& operator=(OperatorBaseNode&&) = delete;

    virtual bool connect(const RowOperatorNodePtr& input) = 0;

    const OperatorContext& getContext() const
    {
      return _context;
    }

    const SymbolTable& getSymbolTable() const
    {
      return *_symbolTable;
    }

    virtual void getColumnInfos(SymbolInfos& outputSymbols) = 0;

    virtual BlockManager& getBlockManager()
    {
      return _context._blockManager;
    }

    virtual void dump(std::ostream& stream) const = 0;

    static VariableIndexMapping getIndexMapping(const IdentifierSet& expressionVariables, const SymbolInfos& infos,
                                                const VariableStore::VariableMapping& mapping)
    {
      VariableIndexMapping varIndexMapping;
      for (const auto& variable : expressionVariables) {
        bool found = false;
        for (size_t n = 0; !found && n < infos.size(); ++n) {
          const SymbolInfoPtr& info = infos[n];

          if (variable._info->_name == info->_name) {
            varIndexMapping.push_back(std::make_pair(VariableStore::getMapping(variable.getQualifiedIdentifier(), mapping), n));
            found = true;
          }
        }
        if (!found) {
          CSVSQLDB_THROW(csvsqldb::Exception, "variable '" << variable.getQualifiedIdentifier() << "' not found");
        }
      }
      return varIndexMapping;
    }

  protected:
    const OperatorContext& _context;
    SymbolTablePtr _symbolTable;
  };


  class CSVSQLDB_EXPORT RowOperatorNode
  : public OperatorBaseNode
  , public RowProvider
  {
  public:
    RowOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable)
    : OperatorBaseNode(context, symbolTable)
    {
    }

    virtual void setOutputAlias(const std::string& alias)
    {
      _outputAlias = alias;
    }

  protected:
    void remapOutputSymbols(SymbolInfos& outputSymbols) const
    {
      if (!_outputAlias.empty()) {
        for (auto& info : outputSymbols) {
          info->_prefix = _outputAlias;
          info->_qualifiedIdentifier = info->_prefix + "." + info->_identifier;
        }
      }
    }

    std::string _outputAlias;
  };


  class CSVSQLDB_EXPORT RootOperatorNode : public OperatorBaseNode
  {
  public:
    RootOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable)
    : OperatorBaseNode(context, symbolTable)
    {
    }

    virtual int64_t process() = 0;
  };
}
