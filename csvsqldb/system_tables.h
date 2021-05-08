//
//  system_tables.h
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

#include <csvsqldb/block_producer.h>
#include <csvsqldb/tabledata.h>

#include <memory>


namespace csvsqldb
{
  class Database;
  class DataProvider;
  class SystemTable;
  using SystemTablePtr = std::shared_ptr<const SystemTable>;

  class CSVSQLDB_EXPORT SystemTable
  {
  public:
    explicit SystemTable(std::string name);

    SystemTable(const SystemTable&) = delete;
    SystemTable(SystemTable&&) = delete;
    SystemTable& operator=(const SystemTable&) = delete;
    SystemTable& operator=(SystemTable&&) = delete;
    virtual ~SystemTable() = default;

    void setUp();
    std::unique_ptr<DataProvider> createDataProvider(Database& database) const;

    TableData getTableData() const;
    const std::string& getName() const;

  private:
    virtual void doSetUp() = 0;
    virtual std::unique_ptr<DataProvider> doCreateDataProvider(Database& database) const = 0;

  protected:
    TableData _tableData;
  };


  class CSVSQLDB_EXPORT SystemDualTable : public SystemTable
  {
  public:
    SystemDualTable();

  private:
    void doSetUp() override;
    std::unique_ptr<DataProvider> doCreateDataProvider(Database& database) const override;
  };


  class CSVSQLDB_EXPORT SystemTableMeta : public SystemTable
  {
  public:
    SystemTableMeta();

  private:
    void doSetUp() override;
    std::unique_ptr<DataProvider> doCreateDataProvider(Database& database) const override;
  };


  class CSVSQLDB_EXPORT SystemColumnMeta : public SystemTable
  {
  public:
    SystemColumnMeta();

  private:
    void doSetUp() override;
    std::unique_ptr<DataProvider> doCreateDataProvider(Database& database) const override;
  };


  class CSVSQLDB_EXPORT SystemFunctionMeta : public SystemTable
  {
  public:
    SystemFunctionMeta();

  private:
    void doSetUp() override;
    std::unique_ptr<DataProvider> doCreateDataProvider(Database& database) const override;
  };


  class CSVSQLDB_EXPORT SystemParameterMeta : public SystemTable
  {
  public:
    SystemParameterMeta();

  private:
    void doSetUp() override;
    std::unique_ptr<DataProvider> doCreateDataProvider(Database& database) const override;
  };


  class CSVSQLDB_EXPORT SystemMappingMeta : public SystemTable
  {
  public:
    SystemMappingMeta();

  private:
    void doSetUp() override;
    std::unique_ptr<DataProvider> doCreateDataProvider(Database& database) const override;
  };


  class CSVSQLDB_EXPORT SystemTables
  {
  public:
    SystemTables();

    SystemTables(const SystemTables&) = delete;
    SystemTables(SystemTables&&) = delete;
    SystemTables& operator=(const SystemTables&) = delete;
    SystemTables& operator=(SystemTables&&) = delete;

    std::vector<SystemTablePtr> getSystemTables() const;

    bool isSystemTable(const std::string& name) const;
    std::unique_ptr<DataProvider> createDataProvider(const std::string& name, Database& database) const;

  private:
    void addSystemTables();

    std::vector<SystemTablePtr> _systemTables;
  };
}
