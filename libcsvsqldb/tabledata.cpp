//
//  tabledata.cpp
//  csvsqldb
//
//  BSD 3-Clause License
//  Copyright (c) 2015, Lars-Christian Fürstenberg
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

#include "base/json_object.h"
#include "sql_astexpressionvisitor.h"
#include "sql_parser.h"


namespace csvsqldb
{
  TableData::TableData(const std::string& tableName)
  : _tableName(tableName)
  {
  }

  bool TableData::hasColumn(const std::string& name) const
  {
    Columns::const_iterator iter =
      std::find_if(_columns.begin(), _columns.end(), [&name](const Column& col) { return name == col._name; });
    if (iter == _columns.end()) {
      return false;
    }
    return true;
  }

  const TableData::Column& TableData::getColumn(const std::string& name) const
  {
    Columns::const_iterator iter =
      std::find_if(_columns.begin(), _columns.end(), [&name](const Column& col) { return name == col._name; });
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

  void TableData::addColumn(const std::string name, eType type, bool primaryKey, bool unique, bool notNull,
                            csvsqldb::Any defaultValue, const ASTExprNodePtr& check, uint32_t length)
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
    std::stringstream table;

    table << "{ \"Table\" :\n  { \"name\" : \"" << _tableName << "\",\n    \"columns\" : [\n";
    int n = 0;
    for (const auto& column : _columns) {
      if (n > 0) {
        table << ",\n";
      }
      table << "      { ";
      table << "\"name\" : \"" << column._name << "\"";
      table << ", \"type\" : \"" << typeToString(column._type) << "\"";
      table << ", \"primary key\" : ";
      if (column._primaryKey) {
        table << "true";
      } else {
        table << "false";
      }
      table << ", \"not null\" : ";
      if (column._notNull) {
        table << "true";
      } else {
        table << "false";
      }
      table << ", \"unique\" : ";
      if (column._unique) {
        table << "true";
      } else {
        table << "false";
      }
      table << ", \"default\" : ";
      if (!column._defaultValue.empty()) {
        table << "\"" << printType(column._type, column._defaultValue) << "\"";
      } else {
        table << "\"\"";
      }
      table << ", \"check\" : ";
      if (column._check) {
        ASTExpressionVisitor visitor;
        column._check->accept(visitor);
        table << "\"" << visitor.toString() << "\"";
      } else {
        table << "\"\"";
      }
      table << ", \"length\" : " << column._length;
      table << " }";
      ++n;
    }
    table << "\n    ],";
    table << "\n    \"constraints\" : [ ";
    int m = 0;
    for (const auto& constraint : _constraints) {
      if (m > 0) {
        table << ",\n";
      } else {
        table << "\n";
      }
      table << "      { \"primary keys\" : [ ";
      n = 0;
      for (const auto& pk : constraint._primaryKey) {
        if (n > 0) {
          table << ",";
        }
        table << "\"" << pk << "\"";
        ++n;
      }
      table << " ]";
      table << ",\n        \"unique keys\" : [ ";
      n = 0;
      for (const auto& pk : constraint._unique) {
        if (n > 0) {
          table << ",";
        }
        table << "\"" << pk << "\"";
        ++n;
      }
      table << " ]";
      table << ",\n        \"check\" : ";
      if (constraint._check) {
        ASTExpressionVisitor visitor;
        constraint._check->accept(visitor);
        table << "\"" << visitor.toString() << "\"";
      } else {
        table << "\"\"";
      }
      table << "\n      }";
      ++m;
    }
    table << "\n    ]\n  }\n}";

    return table.str();
  }

  TableData TableData::fromJson(std::istream& stream)
  {
    std::shared_ptr<csvsqldb::json::JsonObjectCallback> callback = std::make_shared<csvsqldb::json::JsonObjectCallback>();
    csvsqldb::json::Parser parser(stream, callback);
    parser.parse();
    const csvsqldb::json::JsonObject& obj = callback->getObject();
    csvsqldb::json::JsonObject table = obj["Table"];
    std::string tableName = table["name"].getAsString();

    TableData tabledata(tableName);

    const csvsqldb::json::JsonObject::ObjectArray& columns = table["columns"].getArray();
    for (const auto& column : columns) {
      std::string name = column["name"].getAsString();
      eType type = stringToType(column["type"].getAsString());
      bool primaryKey = column["primary key"].getAsBool();
      bool notNull = column["not null"].getAsBool();
      bool unique = column["unique"].getAsBool();
      csvsqldb::Any defaultValue;
      if (!column["default"].getAsString().empty()) {
        defaultValue = TypedValue::createValue(type, column["default"].getAsString())._value;
      }
      ASTExprNodePtr check;
      if (!column["check"].getAsString().empty()) {
        // TODO LFG: needs the real function registry
        FunctionRegistry registry;
        SQLParser sqlparser(registry);
        check = sqlparser.parseExpression(column["check"].getAsString());
      }
      uint32_t length = static_cast<uint32_t>(column["length"].getAsLong());
      tabledata.addColumn(name, type, primaryKey, unique, notNull, defaultValue, check, length);
    }
    const csvsqldb::json::JsonObject::ObjectArray& constraints = table["constraints"].getArray();
    for (const auto& constraint : constraints) {
      const csvsqldb::json::JsonObject::ObjectArray& primaryKeys = constraint["primary keys"].getArray();
      csvsqldb::StringVector primary;
      for (const auto& key : primaryKeys) {
        primary.push_back(key.getAsString());
      }
      const csvsqldb::json::JsonObject::ObjectArray& uniqueKeys = constraint["unique keys"].getArray();
      csvsqldb::StringVector unique;
      for (const auto& key : uniqueKeys) {
        unique.push_back(key.getAsString());
      }
      ASTExprNodePtr check;
      if (!constraint["check"].getAsString().empty()) {
        // TODO LFG: needs the concrete function registry
        FunctionRegistry registry;
        SQLParser sqlparser(registry);
        check = sqlparser.parseExpression(constraint["check"].getAsString());
      }
      tabledata.addConstraint(primary, unique, check);
    }

    return tabledata;
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
