//
//  table_executions.h
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

#ifndef csvsqldb_table_executions_h
#define csvsqldb_table_executions_h

#include "libcsvsqldb/inc.h"

#include "database.h"
#include "execution_plan.h"
#include "sql_ast.h"


namespace csvsqldb
{
  class CSVSQLDB_EXPORT CreateTableExecutionNode : public ExecutionNode
  {
  public:
    CreateTableExecutionNode(Database& database, const std::string& tableName, const ColumnDefinitions& columnDefinitions,
                             const TableConstraints& constraints, bool createIfNotExists);

    virtual int64_t execute();

    virtual void dump(std::ostream& stream) const;

  private:
    Database& _database;
    std::string _tableName;
    ColumnDefinitions _columnDefinitions;
    TableConstraints _constraints;
    bool _createIfNotExists;
  };

  class CSVSQLDB_EXPORT DropTableExecutionNode : public ExecutionNode
  {
  public:
    DropTableExecutionNode(Database& database, const std::string& tableName);

    virtual int64_t execute();

    virtual void dump(std::ostream& stream) const;

  private:
    Database& _database;
    std::string _tableName;
  };

  class CSVSQLDB_EXPORT CreateMappingExecutionNode : public ExecutionNode
  {
  public:
    CreateMappingExecutionNode(Database& database, const std::string& tableName, const FileMapping::Mappings& mappings);

    virtual int64_t execute();

    virtual void dump(std::ostream& stream) const;

  private:
    Database& _database;
    std::string _tableName;
    FileMapping::Mappings _mappings;
  };

  class CSVSQLDB_EXPORT DropMappingExecutionNode : public ExecutionNode
  {
  public:
    DropMappingExecutionNode(Database& database, const std::string& tableName);

    virtual int64_t execute();

    virtual void dump(std::ostream& stream) const;

  private:
    Database& _database;
    std::string _tableName;
  };
}

#endif
