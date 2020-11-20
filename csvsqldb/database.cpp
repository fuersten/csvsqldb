//
//  database.cpp
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

#include "database.h"

#include "base/exception.h"
#include "base/string_helper.h"
#include "sql_parser.h"

#include <algorithm>
#include <fstream>


namespace csvsqldb
{
  Database::Database(fs::path path, FileMapping mappings)
  : _path(std::move(path))
  , _mappings(std::move(mappings))
  {
    setUp();
  }

  void Database::setUp()
  {
    if (!fs::exists(_path)) {
      fs::create_directory(_path);
      fs::create_directory(tablePath());
      fs::create_directory(functionPath());
      fs::create_directory(mappingPath());
    } else {
      if (!fs::exists(tablePath())) {
        fs::create_directory(tablePath());
      }
      if (!fs::exists(functionPath())) {
        fs::create_directory(functionPath());
      }
      if (!fs::exists(mappingPath())) {
        fs::create_directory(mappingPath());
      }
    }
    addSystemTables();
    readTablesFromPath();
    readMappingsFromPath();
  }

  void Database::addSystemTables()
  {
    TableData tabledata("SYSTEM_DUAL");
    tabledata.addColumn("x", BOOLEAN, false, false, false, std::any(), ASTExprNodePtr(), 0);
    addTable(std::move(tabledata), false);
  }

  void Database::readTablesFromPath()
  {
    std::vector<fs::path> entries;
    std::copy(fs::directory_iterator(tablePath()), fs::directory_iterator(), std::back_inserter(entries));

    for (const auto& entry : entries) {
      std::string tableEntry = entry.string();
      std::ifstream tableStream(tableEntry);

      try {
        TableData tabledata = TableData::fromJson(tableStream);

        if (hasTable(tabledata.name())) {
          CSVSQLDB_THROW(SqlException, "table '" << tabledata.name() << "' already added");
        }
        addTable(std::move(tabledata));
      } catch (const Exception& ex) {
        CSVSQLDB_THROW(SqlException, "Cannot load table file '" << tableEntry << "': " << ex.what());
      }
    }
  }

  void Database::readMappingsFromPath()
  {
    FileMapping::readFromPath(_mappings, mappingPath());
  }

  bool Database::hasTable(const std::string& tableName) const
  {
    auto it = std::find_if(_tables.begin(), _tables.end(),
                           [table = toupper_copy(tableName)](const auto& data) { return table == data.name(); });

    return it != _tables.end();
  }

  const TableData& Database::getTable(const std::string& tableName) const
  {
    auto it = std::find_if(_tables.begin(), _tables.end(),
                           [table = toupper_copy(tableName)](const auto& data) { return table == data.name(); });

    if (it == _tables.end()) {
      CSVSQLDB_THROW(Exception, "table '" << tableName << "' not found");
    }

    return *it;
  }

  void Database::addTable(TableData&& tableData, bool persist)
  {
    auto it = std::find_if(_tables.begin(), _tables.end(),
                           [table = toupper_copy(tableData.name())](const auto& data) { return table == data.name(); });

    if (it != _tables.end()) {
      CSVSQLDB_THROW(Exception, "table '" << tableData.name() << "' does already exist");
    }

    if (persist) {
      std::ofstream table((tablePath() / tableData.name()).string());
      if (!table.good()) {
        CSVSQLDB_THROW(Exception, "cant open table file");
      }

      table << tableData.asJson();
      table.close();
    }

    _tables.push_back(tableData);
  }

  void Database::dropTable(const std::string& tableName)
  {
    if (tableName.substr(0, 7) == "SYSTEM_") {
      CSVSQLDB_THROW(Exception, "table '" << tableName << "' is a system table. Dropping nothing");
    }
    auto it = std::find_if(_tables.begin(), _tables.end(), [&tableName](const auto& data) { return data.name() == tableName; });
    if (it == _tables.end()) {
      CSVSQLDB_THROW(Exception, "table '" << tableName << "' not found. Dropping nothing");
    }
    std::error_code ec;
    fs::remove(tablePath() / tableName, ec);
    if (ec) {
      CSVSQLDB_THROW(Exception, "could not remove table file (" << ec.message() << ")");
    }
    _tables.erase(it);

    _mappings.removeMapping(tableName);
  }

  const Mapping& Database::getMappingForTable(const std::string& tableName) const
  {
    return _mappings.getMappingForTable(tableName);
  }

  void Database::addMapping(const std::string& tableName, const FileMapping& mapping)
  {
    if (tableName.substr(0, 7) == "SYSTEM_") {
      CSVSQLDB_THROW(Exception, "cant add mapping for system tables");
    }

    std::ofstream mappingFile((mappingPath() / tableName).string());
    if (!mappingFile.good()) {
      CSVSQLDB_THROW(Exception, "cant open mapping file");
    }

    mappingFile << mapping.asJson(tableName);
    mappingFile.close();

    _mappings.mergeMapping(mapping);
  }

  void Database::removeMapping(const std::string& tableName)
  {
    if (tableName.substr(0, 7) == "SYSTEM_") {
      CSVSQLDB_THROW(Exception, "cant drop mapping for system tables");
    }

    fs::path mappingFile(mappingPath() / tableName);

    std::error_code ec;
    fs::remove(mappingFile, ec);
    if (ec) {
      CSVSQLDB_THROW(Exception, "could not remove mapping file (" << ec.message() << ")");
    }

    _mappings.removeMapping(tableName);
  }
}
