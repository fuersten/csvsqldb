//
//  symboltable.cpp
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

#include "data_provider.h"

#include "buildin_functions.h"
#include "function_registry.h"
#include "sql_astexpressionvisitor.h"

#include <regex>


namespace csvsqldb
{
  SystemDualDataProvider::SystemDualDataProvider(BlockManager& blockManager)
  : _blockManager{blockManager}
  {
  }

  BlockPtr SystemDualDataProvider::getNextBlock()
  {
    auto block = _blockManager.createBlock();
    block->addValue(Variant(false));
    block->nextRow();
    block->endBlocks();
    return block;
  }


  SystemTablesDataProvider::SystemTablesDataProvider(Database& database, BlockManager& blockManager)
  : _database{database}
  , _blockManager{blockManager}
  {
  }

  BlockPtr SystemTablesDataProvider::getNextBlock()
  {
    auto block = _blockManager.createBlock();
    const auto& systemTables{_database.getSystemTables()};
    for (const auto& table : _database.getTables()) {
      block->addValue(Variant(table.name()));
      block->addValue(Variant(systemTables.isSystemTable(table.name())));
      block->nextRow();
    }
    block->endBlocks();
    return block;
  }


  SystemColumnsDataProvider::SystemColumnsDataProvider(Database& database, BlockManager& blockManager)
  : _database{database}
  , _blockManager{blockManager}
  {
  }

  BlockPtr SystemColumnsDataProvider::getNextBlock()
  {
    auto block = _blockManager.createBlock();
    for (const auto& table : _database.getTables()) {
      const auto& tableName = table.name();
      for (auto n = 0u; n < table.columnCount(); ++n) {
        const auto& column = table.getColumn(n);
        block->addValue(Variant(tableName));
        block->addValue(Variant(column._name));
        block->addValue(Variant(typeToString(column._type)));
        block->addValue(Variant(column._primaryKey));
        block->addValue(Variant(column._unique));
        if (column._defaultValue.has_value()) {
          block->addValue(Variant(typedValueToString(TypedValue(column._type, column._defaultValue))));
        } else {
          block->addValue(Variant{column._type});
        }
        if (column._check) {
          ASTExpressionVisitor visitor;
          column._check->accept(visitor);
          block->addValue(Variant(visitor.toString()));
        } else {
          block->addValue(Variant{column._type});
        }
        block->addValue(Variant(static_cast<int64_t>(column._length)));
        block->nextRow();
      }
    }
    block->endBlocks();
    return block;
  }


  SystemFunctionsDataProvider::SystemFunctionsDataProvider(Database&, BlockManager& blockManager)
  : _blockManager{blockManager}
  {
  }

  BlockPtr SystemFunctionsDataProvider::getNextBlock()
  {
    auto block = _blockManager.createBlock();

    FunctionRegistry functionRegistry;
    initBuildInFunctions(functionRegistry);
    for (const auto& func : functionRegistry.getFunctions()) {
      block->addValue(Variant(func->getName()));
      block->nextRow();
    }
    block->endBlocks();
    return block;
  }


  SystemParametersDataProvider::SystemParametersDataProvider(Database&, BlockManager& blockManager)
  : _blockManager{blockManager}
  {
  }

  BlockPtr SystemParametersDataProvider::getNextBlock()
  {
    auto block = _blockManager.createBlock();

    FunctionRegistry functionRegistry;
    initBuildInFunctions(functionRegistry);
    for (const auto& func : functionRegistry.getFunctions()) {
      const auto& funcName = func->getName();
      int64_t index{0u};
      for (const auto& param : func->getParameterTypes()) {
        block->addValue(Variant(funcName));
        block->addValue(typeToString(param));
        block->addValue(Variant{index++});
        block->addValue(Variant(false));
        block->nextRow();
      }
      block->addValue(Variant(funcName));
      block->addValue(typeToString(func->getReturnType()));
      block->addValue(Variant{-1});
      block->addValue(Variant(true));
      block->nextRow();
    }
    block->endBlocks();
    return block;
  }


  SystemMappingsDataProvider::SystemMappingsDataProvider(Database& database, BlockManager& blockManager)
  : _database{database}
  , _blockManager{blockManager}
  {
  }

  BlockPtr SystemMappingsDataProvider::getNextBlock()
  {
    static auto r = std::regex{R"((.+)->(.+))"};
    auto block = _blockManager.createBlock();

    std::smatch m;
    for (const auto& mapping : _database.getMappings()) {
      if (std::regex_match(mapping, m, r) && m.size() == 3) {
        block->addValue(Variant(m[1].str()));
        block->addValue(Variant(m[2].str()));
        block->nextRow();
      }
    }
    block->endBlocks();
    return block;
  }

}
