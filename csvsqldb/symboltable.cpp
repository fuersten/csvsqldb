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

#include "symboltable.h"

#include "sql_ast.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>


namespace csvsqldb
{
  std::string symbolTypeToString(eSymbolType symbolType)
  {
    switch (symbolType) {
      case CALC:
        return "CALC";
      case FUNCTION:
        return "FUNCTION";
      case NOSYM:
        return "NOSYM";
      case PLAIN:
        return "PLAIN";
      case TABLE:
        return "TABLE";
      case SUBQUERY:
        return "SUBQUERY";
    }
    throw std::runtime_error("just to make VC2013 happy");
  }

  SymbolTablePtr SymbolTable::createSymbolTable(const SymbolTablePtr& parent)
  {
    return SymbolTablePtr(new SymbolTable(parent));
  }

  const SymbolInfoPtr& SymbolTable::findSymbol(const std::string& name) const
  {
    const auto& symbol = internFindSymbol(name);
    if (!symbol) {
      throw std::runtime_error("symbol '" + name + "' not found in scope");
    }

    return symbol;
  }

  const SymbolInfoPtr& SymbolTable::internFindSymbol(const std::string& name) const
  {
    static SymbolInfoPtr emptySymbol;
    auto iter =
      std::find_if(_symbols.begin(), _symbols.end(), [&](const SymbolInfoPtr& symbol) { return symbol->_name == name; });
    if (iter != _symbols.end()) {
      return *iter;
    }

    return emptySymbol;
  }

  bool SymbolTable::hasSymbol(const std::string& name) const
  {
    return internFindSymbol(name).get() != nullptr;
  }

  bool SymbolTable::addSymbol(const std::string& name, const SymbolInfoPtr& info)
  {
    if (hasSymbol(name)) {
      return false;
    }
    _symbols.push_back(info);
    return true;
  }

  void SymbolTable::replaceSymbol(const std::string& toreplace, const std::string& name, const SymbolInfoPtr& info)
  {
    auto iter =
      std::find_if(_symbols.begin(), _symbols.end(), [&](const SymbolInfoPtr& symbol) { return symbol->_name == toreplace; });
    if (iter != _symbols.end()) {
      *(*iter) = *info;
    } else {
      addSymbol(name, info);
    }
  }

  std::string SymbolTable::getNextAlias()
  {
    return "$alias_" + std::to_string(_aliasCount++);
  }

  SymbolInfos SymbolTable::getTables() const
  {
    SymbolInfos tables;
    std::copy_if(_symbols.begin(), _symbols.end(), std::back_inserter(tables),
                 [](const auto& symbol) { return symbol->_symbolType == TABLE; });

    return tables;
  }

  SymbolInfos SymbolTable::getSubqueries() const
  {
    SymbolInfos subqueries;
    std::copy_if(_symbols.begin(), _symbols.end(), std::back_inserter(subqueries),
                 [](const auto& symbol) { return symbol->_symbolType == SUBQUERY; });

    return subqueries;
  }

  bool SymbolTable::hasTableSymbol(const std::string& tableNameOrAlias) const
  {
    try {
      findTableSymbol(tableNameOrAlias);
    } catch (const std::exception&) {
      return false;
    }

    return true;
  }

  const SymbolInfoPtr& SymbolTable::findTableSymbol(const std::string& tableNameOrAlias) const
  {
    auto it = std::find_if(_symbols.begin(), _symbols.end(), [&tableNameOrAlias](const auto& symbol) {
      return symbol->_symbolType == TABLE &&
             (symbol->_name == tableNameOrAlias || symbol->_identifier == tableNameOrAlias || symbol->_alias == tableNameOrAlias);
    });

    if (it == _symbols.end()) {
      throw std::runtime_error("symbol '" + tableNameOrAlias + "' not found in scope");
    }

    return *it;
  }

  bool SymbolTable::hasSymbolNameForTable(const std::string& tableName, const std::string& columnName) const
  {
    try {
      findSymbolNameForTable(tableName, columnName);
    } catch (const std::exception&) {
      return false;
    }

    return true;
  }

  SymbolInfoPtr SymbolTable::findSymbolNameForTable(const std::string& tableName, const std::string& columnName) const
  {
    auto it = std::find_if(_symbols.begin(), _symbols.end(), [&tableName, &columnName](const auto& symbol) {
      return symbol->_symbolType == PLAIN && (symbol->_relation == tableName && symbol->_identifier == columnName);
    });

    if (it == _symbols.end()) {
      throw std::runtime_error("symbol '" + tableName + "." + columnName + "' not found in scope");
    }

    return *it;
  }

  const SymbolInfoPtr& SymbolTable::findAliasedSymbol(const std::string& alias) const
  {
    auto it = std::find_if(_symbols.begin(), _symbols.end(), [&alias](const auto& symbol) { return symbol->_alias == alias; });
    if (it == _symbols.end()) {
      throw std::runtime_error("alias symbol '" + alias + "' not found in scope");
    }

    return *it;
  }

  SymbolInfos SymbolTable::findAllSymbolsForTable(const std::string& tableName) const
  {
    SymbolInfos symbols;

    std::copy_if(_symbols.begin(), _symbols.end(), std::back_inserter(symbols),
                 [&tableName](const auto& symbol) { return symbol->_symbolType == PLAIN && symbol->_relation == tableName; });

    return symbols;
  }

  void SymbolTable::fillWithTableData(const Database& database, const SymbolInfos& tables)
  {
    for (const auto& info : tables) {
      const TableData& tableData = database.getTable(info->_identifier);
      for (size_t n = 0; n < tableData.columnCount(); ++n) {
        if (!hasSymbol(info->_name + "." + tableData.getColumn(n)._name)) {
          SymbolInfoPtr elementInfo = std::make_shared<SymbolInfo>();
          elementInfo->_name = info->_name + "." + tableData.getColumn(n)._name;
          elementInfo->_identifier = tableData.getColumn(n)._name;
          elementInfo->_symbolType = PLAIN;
          elementInfo->_type = tableData.getColumn(n)._type;
          elementInfo->_relation = info->_name;
          elementInfo->_qualifiedIdentifier = info->_name + "." + tableData.getColumn(n)._name;

          addSymbol(elementInfo->_name, elementInfo);
        }
      }
    }
  }

  bool SymbolTable::fillInfoFromTablePrefix(const Database& database, const SymbolInfos& tables, SymbolInfoPtr info) const
  {
    std::string table = info->_prefix;
    std::string relation = info->_prefix;

    if (!database.hasTable(info->_prefix)) {
      // this could be some kind of alias, search for alias name of table
      table.clear();
      for (const auto& tableInfo : tables) {
        if (tableInfo->_symbolType == TABLE && tableInfo->_alias == info->_prefix) {
          table = tableInfo->_identifier;
          relation = tableInfo->_name;
          break;
        }
      }
      if (table.empty()) {
        return false;
      }
    }
    const TableData& tableData = database.getTable(table);
    const TableData::Column& column = tableData.getColumn(info->_identifier);
    info->_type = column._type;
    info->_relation = relation;
    info->_qualifiedIdentifier = info->_prefix + "." + column._name;

    return true;
  }

  bool SymbolTable::fillInfoFromTable(const Database& database, const SymbolInfos& tables, SymbolInfoPtr info) const
  {
    bool found = false;
    for (const auto& table : tables) {
      const TableData& tableData = database.getTable(table->_identifier);
      std::string ident = info->_identifier;
      if (!tableData.hasColumn(ident)) {
        for (const auto& info2 : _symbols) {
          if (info2->_alias == ident) {
            ident = info2->_identifier;
            break;
          }
        }
      }

      if (tableData.hasColumn(ident)) {
        if (found) {
          CSVSQLDB_THROW(SqlException, "ambigous symbol '" << (!info->_prefix.empty() ? info->_prefix + "." : "")
                                                           << info->_identifier << "' found");
        }
        found = true;
        const TableData::Column& column = tableData.getColumn(ident);
        info->_type = column._type;
        info->_relation = table->_name;
        info->_qualifiedIdentifier = table->_name + "." + column._name;
      }
    }
    if (!found) {
      // this could be a CALC field
      try {
        const SymbolInfoPtr& info2 = findAliasedSymbol(info->_identifier);
        if (info2->_symbolType != CALC) {
          return false;
        }
        info->_symbolType = CALC;
      } catch (const std::exception&) {
        return false;
      }
    }

    return true;
  }

  bool SymbolTable::fillInfoFromSubquery(const Database&, const SymbolInfos& subqueries, SymbolInfoPtr info) const
  {
    bool found = false;
    for (const auto& query : subqueries) {
      for (const auto& symbol : query->_subquery->_symbols) {
        if (symbol->_name == info->_name || (!info->_alias.empty() && info->_identifier == symbol->_identifier) ||
            (query->_name == info->_prefix && info->_identifier == symbol->_identifier) ||
            (info->_prefix.empty() && info->_identifier == symbol->_identifier)) {
          if (found) {
            CSVSQLDB_THROW(SqlException, "ambigous symbol '" << (!info->_prefix.empty() ? info->_prefix + "." : "")
                                                             << info->_identifier << "' found");
          }

          info->_type = symbol->_type;
          info->_qualifiedIdentifier = info->_name;
          found = true;
          break;
        }
      }
    }
    return found;
  }

  void SymbolTable::typeSymbolTable(const Database& database)
  {
    // first resolve child symbol tables
    std::for_each(_symbols.begin(), _symbols.end(), [&database](const auto& symbol) {
      if (symbol->_symbolType == SUBQUERY) {
        symbol->_subquery->typeSymbolTable(database);
      }
    });

    SymbolInfos tables = getTables();
    fillWithTableData(database, tables);

    for (const auto& info : _symbols) {
      if (info->_symbolType == PLAIN && info->_qualifiedIdentifier.empty()) {
        if (!info->_prefix.empty()) {
          if (!fillInfoFromTablePrefix(database, tables, info) && !fillInfoFromSubquery(database, getSubqueries(), info)) {
            CSVSQLDB_THROW(SqlException, "symbol '" << (!info->_prefix.empty() ? info->_prefix + "." : "") << info->_identifier
                                                    << "' not found in any table");
          }
        } else {
          if (!fillInfoFromTable(database, tables, info) && !fillInfoFromSubquery(database, getSubqueries(), info)) {
            CSVSQLDB_THROW(SqlException, "symbol '" << (!info->_prefix.empty() ? info->_prefix + "." : "") << info->_identifier
                                                    << "' not found in any table");
          }
        }
      }
    }
    std::for_each(_symbols.begin(), _symbols.end(), [](const auto& symbol) {
      if (symbol->_symbolType == CALC && symbol->_expressionNode) {
        symbol->_type = symbol->_expressionNode->type();
      }
    });
  }

  void SymbolTable::dump(std::ostream& stream, size_t indent) const
  {
    std::string space(indent, ' ');

    stream << space;
    stream << "Dumping symbol table:" << std::endl;
    for (const auto& symbol : _symbols) {
      stream << space;
      stream << symbol->_name << " symbol type: " << symbolTypeToString(symbol->_symbolType)
             << " type: " << typeToString(symbol->_type) << " identifier: " << symbol->_identifier
             << " prefix: " << symbol->_prefix << " name: " << symbol->_name << " alias: " << symbol->_alias
             << " relation: " << symbol->_relation << " qualified: " << symbol->_qualifiedIdentifier << "," << std::endl;

      if (symbol->_symbolType == SUBQUERY) {
        symbol->_subquery->dump(stream, indent + 2);
      }
    }
  }
}
