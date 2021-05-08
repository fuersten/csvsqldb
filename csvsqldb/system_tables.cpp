//
//  system_tables.cpp
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

#include "system_tables.h"

#include "base/exception.h"
#include "data_provider.h"


namespace csvsqldb
{
  SystemTable::SystemTable(std::string name)
  : _tableData{std::move(name)}
  {
  }

  void SystemTable::setUp()
  {
    doSetUp();
  }

  TableData SystemTable::getTableData() const
  {
    return _tableData;
  }

  const std::string& SystemTable::getName() const
  {
    return _tableData.name();
  }

  std::unique_ptr<DataProvider> SystemTable::createDataProvider(Database& database) const
  {
    return doCreateDataProvider(database);
  }


  SystemDualTable::SystemDualTable()
  : SystemTable("SYSTEM_DUAL")
  {
  }

  void SystemDualTable::doSetUp()
  {
    _tableData.addColumn("x", BOOLEAN, false, false, false, std::any(), ASTExprNodePtr(), 0);
  }

  std::unique_ptr<DataProvider> SystemDualTable::doCreateDataProvider(Database&) const
  {
    return std::make_unique<SystemDualDataProvider>();
  }


  SystemTableMeta::SystemTableMeta()
  : SystemTable("SYSTEM_TABLES")
  {
  }

  void SystemTableMeta::doSetUp()
  {
    _tableData.addColumn("NAME", STRING, true, true, true, std::any(), ASTExprNodePtr(), 0);
    _tableData.addColumn("SYSTEM", BOOLEAN, false, false, true, std::any(), ASTExprNodePtr(), 0);
  }

  std::unique_ptr<DataProvider> SystemTableMeta::doCreateDataProvider(Database& database) const
  {
    return std::make_unique<SystemTablesDataProvider>(database);
  }


  SystemColumnMeta::SystemColumnMeta()
  : SystemTable("SYSTEM_COLUMNS")
  {
  }

  void SystemColumnMeta::doSetUp()
  {
    _tableData.addColumn("TABLE_NAME", STRING, false, false, true, std::any(), ASTExprNodePtr(), 0);
    _tableData.addColumn("COLUMN_NAME", STRING, false, false, true, std::any(), ASTExprNodePtr(), 0);
    _tableData.addColumn("TYPE", STRING, false, false, true, std::any(), ASTExprNodePtr(), 0);
    _tableData.addColumn("PRIMARY_KEY", BOOLEAN, false, false, true, std::any(), ASTExprNodePtr(), 0);
    _tableData.addColumn("UNIQUE", BOOLEAN, false, false, true, std::any(), ASTExprNodePtr(), 0);
    _tableData.addColumn("DEFAULT_VALUE", STRING, false, false, false, std::any(), ASTExprNodePtr(), 0);
    _tableData.addColumn("CHECK", STRING, false, false, false, std::any(), ASTExprNodePtr(), 0);
    _tableData.addColumn("LENGTH", INT, false, false, false, std::any(), ASTExprNodePtr(), 0);
  }

  std::unique_ptr<DataProvider> SystemColumnMeta::doCreateDataProvider(Database& database) const
  {
    return std::make_unique<SystemColumnsDataProvider>(database);
  }


  SystemFunctionMeta::SystemFunctionMeta()
  : SystemTable("SYSTEM_FUNCTIONS")
  {
  }

  void SystemFunctionMeta::doSetUp()
  {
    _tableData.addColumn("NAME", STRING, true, true, true, std::any(), ASTExprNodePtr(), 0);
  }

  std::unique_ptr<DataProvider> SystemFunctionMeta::doCreateDataProvider(Database& database) const
  {
    return std::make_unique<SystemFunctionsDataProvider>(database);
  }


  SystemParameterMeta::SystemParameterMeta()
  : SystemTable("SYSTEM_PARAMETERS")
  {
  }

  void SystemParameterMeta::doSetUp()
  {
    _tableData.addColumn("FUNCTION_NAME", STRING, false, false, true, std::any(), ASTExprNodePtr(), 0);
    _tableData.addColumn("TYPE", STRING, false, false, true, std::any(), ASTExprNodePtr(), 0);
    _tableData.addColumn("INDEX", INT, false, false, true, std::any(), ASTExprNodePtr(), 0);
    _tableData.addColumn("RETURN", BOOLEAN, false, false, true, std::any(), ASTExprNodePtr(), 0);
  }

  std::unique_ptr<DataProvider> SystemParameterMeta::doCreateDataProvider(Database& database) const
  {
    return std::make_unique<SystemParametersDataProvider>(database);
  }


  SystemMappingMeta::SystemMappingMeta()
  : SystemTable("SYSTEM_MAPPINGS")
  {
  }

  void SystemMappingMeta::doSetUp()
  {
    _tableData.addColumn("PATTERN", STRING, false, false, true, std::any(), ASTExprNodePtr(), 0);
    _tableData.addColumn("TABLE_NAME", STRING, true, true, true, std::any(), ASTExprNodePtr(), 0);
  }

  std::unique_ptr<DataProvider> SystemMappingMeta::doCreateDataProvider(Database& database) const
  {
    return std::make_unique<SystemMappingsDataProvider>(database);
  }


  template<typename T>
  SystemTablePtr makeSystemTable()
  {
    std::shared_ptr<SystemTable> table{std::make_shared<T>()};
    table->setUp();
    return table;
  }

  SystemTables::SystemTables()
  {
    addSystemTables();
  }

  void SystemTables::addSystemTables()
  {
    _systemTables.emplace_back(makeSystemTable<SystemDualTable>());
    _systemTables.emplace_back(makeSystemTable<SystemTableMeta>());
    _systemTables.emplace_back(makeSystemTable<SystemColumnMeta>());
    _systemTables.emplace_back(makeSystemTable<SystemFunctionMeta>());
    _systemTables.emplace_back(makeSystemTable<SystemParameterMeta>());
    _systemTables.emplace_back(makeSystemTable<SystemMappingMeta>());
  }

  std::vector<SystemTablePtr> SystemTables::getSystemTables() const
  {
    return _systemTables;
  }

  bool SystemTables::isSystemTable(const std::string& name) const
  {
    auto result =
      std::find_if(_systemTables.begin(), _systemTables.end(), [&name](const auto& table) { return name == table->getName(); });
    return result != _systemTables.end();
  }

  std::unique_ptr<DataProvider> SystemTables::createDataProvider(const std::string& name, Database& database) const
  {
    auto result =
      std::find_if(_systemTables.begin(), _systemTables.end(), [&name](const auto& table) { return name == table->getName(); });
    if (result == _systemTables.end()) {
      CSVSQLDB_THROW(Exception, "system table '" << name << "' not found");
    }
    return (*result)->createDataProvider(database);
  }
}
