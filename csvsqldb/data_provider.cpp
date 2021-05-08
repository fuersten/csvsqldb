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
  bool SystemDualDataProvider::produce(BlockProducer& producer)
  {
    producer.addBool(false, false);
    producer.nextRow();
    return true;
  }


  SystemTablesDataProvider::SystemTablesDataProvider(Database& database)
  : DataProvider()
  , _database{database}
  {
  }

  bool SystemTablesDataProvider::produce(BlockProducer& producer)
  {
    const auto& systemTables{_database.getSystemTables()};
    for (const auto& table : _database.getTables()) {
      producer.addValue(Variant(table.name()));
      producer.addValue(Variant(systemTables.isSystemTable(table.name())));
      producer.nextRow();
    }
    return true;
  }


  SystemColumnsDataProvider::SystemColumnsDataProvider(Database& database)
  : DataProvider()
  , _database{database}
  {
  }

  bool SystemColumnsDataProvider::produce(BlockProducer& producer)
  {
    for (const auto& table : _database.getTables()) {
      const auto& tableName = table.name();
      for (auto n = 0u; n < table.columnCount(); ++n) {
        const auto& column = table.getColumn(n);
        producer.addValue(Variant(tableName));
        producer.addValue(Variant(column._name));
        producer.addValue(Variant(typeToString(column._type)));
        producer.addValue(Variant(column._primaryKey));
        producer.addValue(Variant(column._unique));
        if (column._defaultValue.has_value()) {
          producer.addValue(Variant(typedValueToString(TypedValue(column._type, column._defaultValue))));
        } else {
          producer.addValue(Variant{STRING});
        }
        if (column._check) {
          ASTExpressionVisitor visitor;
          column._check->accept(visitor);
          producer.addValue(Variant(visitor.toString()));
        } else {
          producer.addValue(Variant{STRING});
        }
        producer.addValue(Variant(static_cast<int64_t>(column._length)));
        producer.nextRow();
      }
    }

    return true;
  }


  SystemFunctionsDataProvider::SystemFunctionsDataProvider(Database&)
  : DataProvider()
  {
  }

  bool SystemFunctionsDataProvider::produce(BlockProducer& producer)
  {
    FunctionRegistry functionRegistry;
    initBuildInFunctions(functionRegistry);
    for (const auto& func : functionRegistry.getFunctions()) {
      producer.addValue(Variant(func->getName()));
      producer.nextRow();
    }

    return true;
  }


  SystemParametersDataProvider::SystemParametersDataProvider(Database&)
  : DataProvider()
  {
  }

  bool SystemParametersDataProvider::produce(BlockProducer& producer)
  {
    FunctionRegistry functionRegistry;
    initBuildInFunctions(functionRegistry);
    for (const auto& func : functionRegistry.getFunctions()) {
      const auto& funcName = func->getName();
      int64_t index{0u};
      for (const auto& param : func->getParameterTypes()) {
        producer.addValue(Variant(funcName));
        producer.addValue(typeToString(param));
        producer.addValue(Variant{index++});
        producer.addValue(Variant(false));
        producer.nextRow();
      }
      producer.addValue(Variant(funcName));
      producer.addValue(typeToString(func->getReturnType()));
      producer.addValue(Variant{-1});
      producer.addValue(Variant(true));
      producer.nextRow();
    }

    return true;
  }


  SystemMappingsDataProvider::SystemMappingsDataProvider(Database& database)
  : DataProvider()
  , _database{database}
  {
  }

  bool SystemMappingsDataProvider::produce(BlockProducer& producer)
  {
    static auto r = std::regex{R"((.+)->(.+))"};

    std::smatch m;
    for (const auto& mapping : _database.getMappings()) {
      if (std::regex_match(mapping, m, r) && m.size() == 3) {
        producer.addValue(Variant(m[1].str()));
        producer.addValue(Variant(m[2].str()));
        producer.nextRow();
      }
    }

    return true;
  }
}
