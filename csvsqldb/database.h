//
//  database.h
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

#include <csvsqldb/file_mapping.h>
#include <csvsqldb/tabledata.h>

#include <filesystem>


namespace csvsqldb
{
  namespace fs = std::filesystem;

  class CSVSQLDB_EXPORT Database
  {
  public:
    Database(fs::path path, FileMapping mappings);

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    Database(Database&&) = delete;
    Database& operator=(Database&&) = delete;

    fs::path path() const
    {
      return _path;
    }
    fs::path tablePath() const
    {
      return _path / "tables";
    }
    fs::path functionPath() const
    {
      return _path / "functions";
    }
    fs::path mappingPath() const
    {
      return _path / "mappings";
    }

    bool hasTable(const std::string& tableName) const;
    const TableData& getTable(const std::string& tableName) const;

    void addTable(TableData&& table, bool persist = true);
    void dropTable(const std::string& tableName);

    const Mapping& getMappingForTable(const std::string& tableName) const;
    void addMapping(const std::string& tableName, const FileMapping& mapping);
    void removeMapping(const std::string& tableName);

    std::vector<TableData> getTables() const
    {
      return _tables;
    }
    csvsqldb::StringVector getMappings() const
    {
      return _mappings.asStringVector();
    }

  private:
    void setUp();
    void addSystemTables();
    void readTablesFromPath();
    void readMappingsFromPath();

    fs::path _path;
    std::vector<TableData> _tables;
    FileMapping _mappings;
  };
}
