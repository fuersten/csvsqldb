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


namespace
{
  csvsqldb::Types setupTypes(const csvsqldb::SystemTable& table)
  {
    csvsqldb::Types types;
    for (size_t n = 0; n < table.getTableData().columnCount(); ++n) {
      types.push_back(table.getTableData().getColumn(n)._type);
    }

    return types;
  }
}

TEST_CASE("Data Provider Test", "[system tables]")
{
  TemporaryDirectoryGuard tmpDir;
  auto path = tmpDir.temporaryDirectoryPath();
  csvsqldb::FileMapping mapping;
  csvsqldb::BlockManager blockManager;
  csvsqldb::BlockProducer producer{blockManager};
  csvsqldb::Database database{path, mapping};

  SECTION("SYSTEM_DUAL provider")
  {
    csvsqldb::SystemDualTable table;
    table.setUp();
    auto provider = database.getSystemTables().createDataProvider("SYSTEM_DUAL", database);
    REQUIRE(provider);
    producer.start([&provider](csvsqldb::BlockProducer& blockProducer) { provider->produce(blockProducer); });
    csvsqldb::Types types = setupTypes(table);
    csvsqldb::BlockIterator iterator(types, producer, blockManager);
    auto* values = iterator.getNextRow();
    REQUIRE(values);
    REQUIRE(1 == values->size());
    CHECK(csvsqldb::BOOLEAN == values->at(0)->getType());
  }
  SECTION("SYSTEM_TABLES provider")
  {
    csvsqldb::SystemTableMeta table;
    table.setUp();
    auto provider = database.getSystemTables().createDataProvider("SYSTEM_TABLES", database);
    REQUIRE(provider);
    producer.start([&provider](csvsqldb::BlockProducer& blockProducer) { provider->produce(blockProducer); });
    csvsqldb::Types types = setupTypes(table);
    csvsqldb::BlockIterator iterator(types, producer, blockManager);
    auto* values = iterator.getNextRow();
    REQUIRE(values);
    REQUIRE(2 == values->size());
    CHECK(csvsqldb::STRING == values->at(0)->getType());
    CHECK(csvsqldb::BOOLEAN == values->at(1)->getType());
  }
  SECTION("SYSTEM_COLUMNS provider")
  {
    csvsqldb::SystemColumnMeta table;
    table.setUp();
    auto provider = database.getSystemTables().createDataProvider("SYSTEM_COLUMNS", database);
    REQUIRE(provider);
    producer.start([&provider](csvsqldb::BlockProducer& blockProducer) { provider->produce(blockProducer); });
    csvsqldb::Types types = setupTypes(table);
    csvsqldb::BlockIterator iterator(types, producer, blockManager);
    auto* values = iterator.getNextRow();
    REQUIRE(values);
    REQUIRE(8 == values->size());
    CHECK(csvsqldb::STRING == values->at(0)->getType());
    CHECK(csvsqldb::STRING == values->at(1)->getType());
    CHECK(csvsqldb::STRING == values->at(2)->getType());
    CHECK(csvsqldb::BOOLEAN == values->at(3)->getType());
    CHECK(csvsqldb::BOOLEAN == values->at(4)->getType());
    CHECK(csvsqldb::STRING == values->at(5)->getType());
    CHECK(csvsqldb::STRING == values->at(6)->getType());
    CHECK(csvsqldb::INT == values->at(7)->getType());
  }
  SECTION("SYSTEM_FUNCTIONS provider")
  {
    csvsqldb::SystemFunctionMeta table;
    table.setUp();
    auto provider = database.getSystemTables().createDataProvider("SYSTEM_FUNCTIONS", database);
    REQUIRE(provider);
    producer.start([&provider](csvsqldb::BlockProducer& blockProducer) { provider->produce(blockProducer); });
    csvsqldb::Types types = setupTypes(table);
    csvsqldb::BlockIterator iterator(types, producer, blockManager);
    auto* values = iterator.getNextRow();
    REQUIRE(values);
    REQUIRE(1 == values->size());
    CHECK(csvsqldb::STRING == values->at(0)->getType());
  }
  SECTION("SYSTEM_PARAMETERS provider")
  {
    csvsqldb::SystemParameterMeta table;
    table.setUp();
    auto provider = database.getSystemTables().createDataProvider("SYSTEM_PARAMETERS", database);
    REQUIRE(provider);
    producer.start([&provider](csvsqldb::BlockProducer& blockProducer) { provider->produce(blockProducer); });
    csvsqldb::Types types = setupTypes(table);
    csvsqldb::BlockIterator iterator(types, producer, blockManager);
    auto* values = iterator.getNextRow();
    REQUIRE(values);
    REQUIRE(4 == values->size());
    CHECK(csvsqldb::STRING == values->at(0)->getType());
    CHECK(csvsqldb::STRING == values->at(1)->getType());
    CHECK(csvsqldb::INT == values->at(2)->getType());
    CHECK(csvsqldb::BOOLEAN == values->at(3)->getType());
  }
  SECTION("SYSTEM_MAPPINGS provider")
  {
    csvsqldb::FileMapping::Mappings mappings;
    mappings.push_back({"friends.csv->friends", ',', false});
    mapping.initialize(mappings);

    database.addMapping("FRIENDS", mapping);

    csvsqldb::SystemMappingMeta table;
    table.setUp();
    auto provider = database.getSystemTables().createDataProvider("SYSTEM_MAPPINGS", database);
    REQUIRE(provider);
    producer.start([&provider](csvsqldb::BlockProducer& blockProducer) { provider->produce(blockProducer); });
    csvsqldb::Types types = setupTypes(table);
    csvsqldb::BlockIterator iterator(types, producer, blockManager);
    auto* values = iterator.getNextRow();
    REQUIRE(values);
    REQUIRE(2 == values->size());
    CHECK(csvsqldb::STRING == values->at(0)->getType());
    CHECK(csvsqldb::STRING == values->at(1)->getType());
  }
  SECTION("no table")
  {
    CHECK_THROWS_WITH(database.getSystemTables().createDataProvider("NO SYSTEM TABLE", database),
                      "system table 'NO SYSTEM TABLE' not found");
  }
}
