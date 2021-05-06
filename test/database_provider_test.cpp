//
//  csvsqldb test
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

#include <csvsqldb/block_iterator.h>
#include <csvsqldb/data_provider.h>

#include "temporary_directory.h"
#include "test/test_util.h"

#include <catch2/catch.hpp>


TEST_CASE("Data Provider Test", "[system tables]")
{
  TemporaryDirectoryGuard tmpDir;
  auto path = tmpDir.temporaryDirectoryPath();
  csvsqldb::FileMapping mapping;
  csvsqldb::BlockManager blockManager;
  csvsqldb::Database database{path, mapping};

  SECTION("SYSTEM_DUAL construction")
  {
    csvsqldb::SystemDualTable table;
    table.setUp();
    auto provider = database.getSystemTables().createDataProvider("SYSTEM_DUAL", database, blockManager);
    REQUIRE(provider);
    csvsqldb::Types types;
    for (size_t n = 0; n < table.getTableData().columnCount(); ++n) {
      types.push_back(table.getTableData().getColumn(n)._type);
    }
    csvsqldb::BlockIterator iterator(types, *provider, blockManager);
    auto* values = iterator.getNextRow();
    REQUIRE(values);
    REQUIRE(1 == values->size());
    CHECK(csvsqldb::BOOLEAN == values->at(0)->getType());
  }
  SECTION("SYSTEM_TABLE construction")
  {
    csvsqldb::SystemTableMeta table;
    table.setUp();
    auto provider = database.getSystemTables().createDataProvider("SYSTEM_TABLES", database, blockManager);
    REQUIRE(provider);
    csvsqldb::Types types;
    for (size_t n = 0; n < table.getTableData().columnCount(); ++n) {
      types.push_back(table.getTableData().getColumn(n)._type);
    }
    csvsqldb::BlockIterator iterator(types, *provider, blockManager);
    auto* values = iterator.getNextRow();
    REQUIRE(values);
    REQUIRE(2 == values->size());
    CHECK(csvsqldb::STRING == values->at(0)->getType());
    CHECK(csvsqldb::BOOLEAN == values->at(1)->getType());
  }
  SECTION("no table")
  {
    CHECK_THROWS_WITH(database.getSystemTables().createDataProvider("NO SYSTEM TABLE", database, blockManager),
                      "system table 'NO SYSTEM TABLE' not found");
  }
}
