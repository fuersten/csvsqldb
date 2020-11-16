//
//  table_executions.cpp
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

#include "table_executions.h"

#include "file_mapping.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;


namespace csvsqldb
{
  CSVSQLDB_DECLARE_EXCEPTION(CreateTableException, SqlException);
  CSVSQLDB_IMPLEMENT_EXCEPTION(CreateTableException, SqlException);

  CSVSQLDB_DECLARE_EXCEPTION(CreateMappingException, SqlException);
  CSVSQLDB_IMPLEMENT_EXCEPTION(CreateMappingException, SqlException);

  CSVSQLDB_DECLARE_EXCEPTION(DropMappingException, SqlException);
  CSVSQLDB_IMPLEMENT_EXCEPTION(DropMappingException, SqlException);


  CreateTableExecutionNode::CreateTableExecutionNode(Database& database, const std::string& tableName,
                                                     const ColumnDefinitions& columnDefinitions,
                                                     const TableConstraints& constraints, bool createIfNotExists)
  : _database(database)
  , _tableName(tableName)
  , _columnDefinitions(columnDefinitions)
  , _constraints(constraints)
  , _createIfNotExists(createIfNotExists)
  {
  }

  int64_t CreateTableExecutionNode::execute()
  {
    if (_database.hasTable(_tableName)) {
      if (_createIfNotExists) {
        return 0;
      } else {
        CSVSQLDB_THROW(CreateTableException, "table '" << _tableName << "' does already exist");
      }
    }

    TableData tabledata(_tableName);
    for (const auto& definition : _columnDefinitions) {
      tabledata.addColumn(definition._name, definition._type, definition._primaryKey, definition._unique, definition._notNull,
                          definition._defaultValue, definition._check, definition._length);
    }
    for (const auto& constraint : _constraints) {
      tabledata.addConstraint(constraint._primaryKeys, constraint._uniqueKeys, constraint._check);
    }

    std::ofstream table((_database.tablePath() / _tableName).string());
    if (!table.good()) {
      CSVSQLDB_THROW(CreateTableException, "cant open table file");
    }

    table << tabledata.asJson();
    table.close();

    _database.addTable(tabledata);

    return 0;
  }

  void CreateTableExecutionNode::dump(std::ostream& stream) const
  {
  }


  DropTableExecutionNode::DropTableExecutionNode(Database& database, const std::string& tableName)
  : _database(database)
  , _tableName(tableName)
  {
  }

  int64_t DropTableExecutionNode::execute()
  {
    _database.dropTable(_tableName);
    return 0;
  }

  void DropTableExecutionNode::dump(std::ostream& stream) const
  {
  }


  CreateMappingExecutionNode::CreateMappingExecutionNode(Database& database, const std::string& tableName,
                                                         const FileMapping::Mappings& mappings)
  : _database(database)
  , _tableName(tableName)
  , _mappings(mappings)
  {
  }

  int64_t CreateMappingExecutionNode::execute()
  {
    if (_tableName.substr(0, 7) == "SYSTEM_") {
      CSVSQLDB_THROW(CreateMappingException, "cant add mapping for system tables");
    }

    {
      std::ofstream mappingFile((_database.mappingPath() / _tableName).string());
      if (!mappingFile.good()) {
        CSVSQLDB_THROW(CreateMappingException, "cant open mapping file");
      }

      for (auto& mapping : _mappings) {
        mapping._mapping = mapping._mapping + "->" + _tableName;
      }

      mappingFile << FileMapping::asJson(_tableName, _mappings);
      mappingFile.close();
    }

    std::ifstream mappingFile((_database.mappingPath() / _tableName).string());
    if (!mappingFile.good()) {
      CSVSQLDB_THROW(CreateMappingException, "cant open mapping file");
    }

    _database.addMapping(FileMapping::fromJson(mappingFile));

    mappingFile.close();

    return 0;
  }

  void CreateMappingExecutionNode::dump(std::ostream& stream) const
  {
  }


  DropMappingExecutionNode::DropMappingExecutionNode(Database& database, const std::string& tableName)
  : _database(database)
  , _tableName(tableName)
  {
  }

  int64_t DropMappingExecutionNode::execute()
  {
    if (_tableName.substr(0, 7) == "SYSTEM_") {
      CSVSQLDB_THROW(DropMappingException, "cant drop mapping for system tables");
    }

    fs::path mappingFile(_database.mappingPath() / _tableName);
    if (!fs::exists(mappingFile)) {
      CSVSQLDB_THROW(DropMappingException, "mapping file does not exist");
    }

    std::error_code ec;
    fs::remove(mappingFile, ec);
    if (ec) {
      CSVSQLDB_THROW(DropMappingException, "could not remove mapping file (" << ec.message() << ")");
    }

    _database.removeMapping(_tableName);

    return 0;
  }

  void DropMappingExecutionNode::dump(std::ostream& stream) const
  {
  }
}
