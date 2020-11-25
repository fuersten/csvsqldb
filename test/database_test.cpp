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


#include <csvsqldb/database.h>

#include "temporary_directory.h"
#include "test/test_util.h"

#include <catch2/catch.hpp>


TEST_CASE("Database Test", "[database]")
{
  TemporaryDirectoryGuard tmpDir;
  auto path = tmpDir.temporaryDirectoryPath();
  csvsqldb::FileMapping mapping;

  SECTION("construction")
  {
    csvsqldb::Database database{path, mapping};

    CHECK(path == database.path());
    CHECK(path / "functions" == database.functionPath());
    CHECK(path / "mappings" == database.mappingPath());
    CHECK(path / "tables" == database.tablePath());

    std::error_code ec;
    CHECK(std::filesystem::exists(path / "functions", ec));
    CHECK(std::filesystem::exists(path / "mappings", ec));
    CHECK(std::filesystem::exists(path / "tables", ec));

    REQUIRE(database.hasTable("SYSTEM_DUAL"));
    auto systemTable = database.getTable("SYSTEM_DUAL");
    CHECK("SYSTEM_DUAL" == systemTable.name());
    auto column = systemTable.getColumn("x");
    CHECK(column._type == csvsqldb::BOOLEAN);
    CHECK_FALSE(column._primaryKey);
    CHECK_FALSE(column._unique);
    CHECK_FALSE(column._notNull);
    CHECK_FALSE(column._defaultValue.has_value());
    CHECK_FALSE(column._check);
    CHECK(0 == column._length);
  }
  SECTION("construction no path")
  {
    auto testPath = path / "test";

    csvsqldb::Database database{testPath, mapping};

    CHECK(testPath == database.path());
    CHECK(testPath / "functions" == database.functionPath());
    CHECK(testPath / "mappings" == database.mappingPath());
    CHECK(testPath / "tables" == database.tablePath());

    std::error_code ec;
    CHECK(std::filesystem::exists(testPath / "functions", ec));
    CHECK(std::filesystem::exists(testPath / "mappings", ec));
    CHECK(std::filesystem::exists(testPath / "tables", ec));
  }
  SECTION("tables")
  {
    {
      csvsqldb::Database database{path, mapping};

      CHECK_FALSE(database.hasTable("FRIENDS"));
      std::error_code ec;
      CHECK_FALSE(std::filesystem::exists(path / "tables" / "FRIENDS", ec));

      csvsqldb::TableData tabledata("FRIENDS");
      csvsqldb::ASTExprNodePtr check;
      tabledata.addColumn("id", csvsqldb::INT, true, false, false, std::any(), check, 0);
      tabledata.addColumn("name", csvsqldb::STRING, false, false, false, std::any(), check, 25);
      tabledata.addColumn("age", csvsqldb::INT, false, false, false, std::any(), check, 0);

      csvsqldb::TableData sameTable{tabledata};

      database.addTable(std::move(tabledata));
      REQUIRE(database.hasTable("FRIENDS"));
      CHECK_THROWS_AS(database.addTable(std::move(sameTable)), csvsqldb::Exception);
      CHECK(std::filesystem::exists(path / "tables" / "FRIENDS", ec));

      CHECK("FRIENDS" == database.getTable("FRIENDS").name());

      csvsqldb::TableData otherTable("PETS");
      otherTable.addColumn("id", csvsqldb::INT, true, false, false, std::any(), check, 0);
      otherTable.addColumn("name", csvsqldb::STRING, false, false, false, std::any(), check, 25);
      otherTable.addColumn("age", csvsqldb::INT, false, false, false, std::any(), check, 0);
      database.addTable(std::move(otherTable));
      CHECK(database.hasTable("PETS"));

      CHECK(3 == database.getTables().size());
    }

    csvsqldb::Database database{path, mapping};
    CHECK(database.hasTable("FRIENDS"));

    database.dropTable("FRIENDS");
    CHECK_FALSE(database.hasTable("FRIENDS"));
    CHECK_THROWS_AS(database.getTable("FRIENDS"), csvsqldb::Exception);
    std::error_code ec;
    CHECK_FALSE(std::filesystem::exists(path / "tables" / "FRIENDS", ec));
  }
  SECTION("drop tables error")
  {
    csvsqldb::Database database{path, mapping};
    CHECK_THROWS_AS(database.dropTable("SYSTEM_DUAL"), csvsqldb::Exception);
    CHECK_THROWS_AS(database.dropTable("NON EXISTENT TABLE"), csvsqldb::Exception);
  }
  SECTION("mappings")
  {
    {
      csvsqldb::Database database{path, mapping};

      CHECK(0 == database.getMappings().size());
      std::error_code ec;
      CHECK_FALSE(std::filesystem::exists(path / "mappings" / "FRIENDS", ec));

      csvsqldb::FileMapping::Mappings mappings;
      mappings.push_back({"friends.csv->friends", ',', false});
      mapping.initialize(mappings);

      database.addMapping("FRIENDS", mapping);
      CHECK_NOTHROW(database.addMapping("FRIENDS", mapping));
      CHECK(std::filesystem::exists(path / "mappings" / "FRIENDS", ec));
      CHECK_NOTHROW(database.getMappingForTable("FRIENDS"));
    }

    csvsqldb::Database database{path, mapping};
    CHECK_NOTHROW(database.getMappingForTable("FRIENDS"));
    database.removeMapping("FRIENDS");
    CHECK_THROWS_AS(database.getMappingForTable("FRIENDS"), csvsqldb::Exception);
    std::error_code ec;
    CHECK_FALSE(std::filesystem::exists(path / "mappings" / "FRIENDS", ec));
  }
  SECTION("add mapping error")
  {
    csvsqldb::Database database{path, mapping};
    csvsqldb::FileMapping::Mappings mappings;
    mappings.push_back({"system.csv->SYSTEM_DUAL", ',', false});
    mapping.initialize(mappings);
    CHECK_THROWS_AS(database.addMapping("SYSTEM_DUAL", mapping), csvsqldb::Exception);
  }
  SECTION("drop mapping error")
  {
    csvsqldb::Database database{path, mapping};
    CHECK_THROWS_AS(database.removeMapping("SYSTEM_DUAL"), csvsqldb::Exception);
  }
}
