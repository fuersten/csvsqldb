//
//  tabledata.cpp
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

#include "tabledata.h"

#include "base/json.h"
#include "sql_astexpressionvisitor.h"
#include "sql_parser.h"


namespace csvsqldb
{
  TableData::TableData(std::string tableName)
  : _tableName(std::move(tableName))
  {
  }

  bool TableData::hasColumn(const std::string& name) const
  {
    const auto iter = std::find_if(_columns.begin(), _columns.end(), [&name](const Column& col) { return name == col._name; });
    if (iter == _columns.end()) {
      return false;
    }
    return true;
  }

  const TableData::Column& TableData::getColumn(const std::string& name) const
  {
    const auto iter = std::find_if(_columns.begin(), _columns.end(), [&name](const Column& col) { return name == col._name; });
    if (iter == _columns.end()) {
      CSVSQLDB_THROW(SqlException, "column '" << name << "' not found in table " << _tableName);
    }
    return *iter;
  }

  const TableData::Column& TableData::getColumn(size_t index) const
  {
    if (index >= _columns.size()) {
      CSVSQLDB_THROW(csvsqldb::IndexException, "index '" << index << "' is out of range");
    }

    return _columns[index];
  }

  void TableData::addColumn(const std::string& name, eType type, bool primaryKey, bool unique, bool notNull,
                            std::any defaultValue, const ASTExprNodePtr& check, uint32_t length)
  {
    Column column;
    column._name = name;
    column._type = type;
    column._primaryKey = primaryKey;
    column._unique = unique;
    column._notNull = notNull;
    column._defaultValue = defaultValue;
    column._check = check;
    column._length = length;

    _columns.push_back(column);
  }

  void TableData::addConstraint(const csvsqldb::StringVector& primaryKey, const csvsqldb::StringVector& unique,
                                const ASTExprNodePtr& check)
  {
    TableConstraint constraint;
    constraint._primaryKey = primaryKey;
    constraint._unique = unique;
    constraint._check = check;

    _constraints.push_back(constraint);
  }

  std::string TableData::asJson() const
  {
    json j;
    j["Table"]["name"] = _tableName;

    json columnArray = json::array();
    for (const auto& column : _columns) {
      json entry{{"name", column._name},        {"type", typeToString(column._type)}, {"primary key", column._primaryKey},
                 {"not null", column._notNull}, {"unique", column._unique},           {"length", column._length}};

      if (column._defaultValue.has_value()) {
        entry.push_back({"default", typedValueToString(TypedValue(column._type, column._defaultValue))});
      } else {
        entry.push_back({"default", ""});
      }

      if (column._check) {
        ASTExpressionVisitor visitor;
        column._check->accept(visitor);
        entry.push_back({"check", visitor.toString()});
      } else {
        entry.push_back({"check", ""});
      }
      columnArray.push_back(entry);
    }
    j["Table"]["columns"] = columnArray;

    json cconstraintsArray = json::array();
    for (const auto& constraint : _constraints) {
      json constraintEntry;

      json primaryKeys = json::array();
      for (const auto& pk : constraint._primaryKey) {
        primaryKeys.push_back(pk);
      }
      constraintEntry["primary keys"] = primaryKeys;

      json uniqueKeys = json::array();
      for (const auto& pk : constraint._unique) {
        uniqueKeys.push_back(pk);
      }
      constraintEntry["unique keys"] = uniqueKeys;

      if (constraint._check) {
        ASTExpressionVisitor visitor;
        constraint._check->accept(visitor);
        constraintEntry["check"] = visitor.toString();
      } else {
        constraintEntry["check"] = "";
      }
      cconstraintsArray.push_back(constraintEntry);
    }
    j["Table"]["constraints"] = cconstraintsArray;

    std::stringstream ss;
    ss << std::setw(2) << j << std::endl;
    return ss.str();
  }

  TableData TableData::fromJson(std::istream& stream)
  {
    try {
      json j;
      stream >> j;

      auto table = j["Table"];
      auto tableName = table["name"].get<std::string>();

      TableData tabledata(tableName);

      for (const auto& column : table["columns"]) {
        auto name = column["name"].get<std::string>();
        eType type = stringToType(column["type"].get<std::string>());
        bool primaryKey = column["primary key"].get<bool>();
        bool notNull = column["not null"].get<bool>();
        bool unique = column["unique"].get<bool>();
        std::any defaultValue;
        if (!column["default"].get<std::string>().empty()) {
          defaultValue = TypedValue::createValue(type, column["default"].get<std::string>())._value;
        }
        ASTExprNodePtr check;
        if (!column["check"].get<std::string>().empty()) {
          // TODO LFG: needs the real function registry
          FunctionRegistry registry;
          SQLParser sqlparser(registry);
          check = sqlparser.parseExpression(column["check"].get<std::string>());
        }
        uint32_t length = column["length"].get<uint32_t>();
        tabledata.addColumn(name, type, primaryKey, unique, notNull, defaultValue, check, length);
      }

      for (const auto& constraint : table["constraints"]) {
        csvsqldb::StringVector primary;
        for (const auto& key : constraint["primary keys"]) {
          primary.push_back(key.get<std::string>());
        }

        csvsqldb::StringVector unique;
        for (const auto& key : constraint["unique keys"]) {
          unique.push_back(key.get<std::string>());
        }

        ASTExprNodePtr check;
        if (!constraint["check"].get<std::string>().empty()) {
          // TODO LFG: needs the concrete function registry
          FunctionRegistry registry;
          SQLParser sqlparser(registry);
          check = sqlparser.parseExpression(constraint["check"].get<std::string>());
        }
        tabledata.addConstraint(primary, unique, check);
      }

      return tabledata;
    } catch (const std::exception& ex) {
      CSVSQLDB_THROW(JsonException, "No valid table data: " << ex.what());
    }
  }

  TableData TableData::fromCreateAST(const ASTCreateTableNodePtr& createNode)
  {
    TableData tabledata(createNode->_tableName);
    for (const auto& definition : createNode->_columnDefinitions) {
      tabledata.addColumn(definition._name, definition._type, definition._primaryKey, definition._unique, definition._notNull,
                          definition._defaultValue, definition._check, definition._length);
    }
    for (const auto& constraint : createNode->_tableConstraints) {
      tabledata.addConstraint(constraint._primaryKeys, constraint._uniqueKeys, constraint._check);
    }
    return tabledata;
  }
}
