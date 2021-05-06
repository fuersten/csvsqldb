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


#include <csvsqldb/system_tables.h>

#include "test/test_util.h"

#include <catch2/catch.hpp>


TEST_CASE("System Tables Test", "[system tables]")
{
  SECTION("Check tables")
  {
    csvsqldb::SystemTables tables;
    CHECK(tables.getSystemTables().size() == 2);
  }
  SECTION("Is system table")
  {
    csvsqldb::SystemTables tables;
    CHECK(tables.isSystemTable("SYSTEM_DUAL"));
    CHECK(tables.isSystemTable("SYSTEM_TABLES"));

    CHECK_FALSE(tables.isSystemTable("SYSTEM"));
    CHECK_FALSE(tables.isSystemTable("EMPLOYEE"));
  }
  SECTION("System Dual")
  {
    csvsqldb::SystemDualTable table;
    CHECK(table.getName() == "SYSTEM_DUAL");
    table.setUp();

    auto column = table.getTableData().getColumn("x");
    CHECK(column._type == csvsqldb::BOOLEAN);
    CHECK_FALSE(column._primaryKey);
    CHECK_FALSE(column._unique);
    CHECK_FALSE(column._notNull);
    CHECK_FALSE(column._defaultValue.has_value());
    CHECK_FALSE(column._check);
    CHECK(0 == column._length);
  }
  SECTION("System Table Meta")
  {
    csvsqldb::SystemTableMeta table;
    CHECK(table.getName() == "SYSTEM_TABLES");
    table.setUp();

    auto tableData = table.getTableData();
    auto column = tableData.getColumn("NAME");
    CHECK(column._type == csvsqldb::STRING);
    CHECK(column._primaryKey);
    CHECK(column._unique);
    CHECK(column._notNull);
    CHECK_FALSE(column._defaultValue.has_value());
    CHECK_FALSE(column._check);
    CHECK(0 == column._length);

    column = tableData.getColumn("SYSTEM");
    CHECK(column._type == csvsqldb::BOOLEAN);
    CHECK_FALSE(column._primaryKey);
    CHECK_FALSE(column._unique);
    CHECK(column._notNull);
    CHECK_FALSE(column._defaultValue.has_value());
    CHECK_FALSE(column._check);
    CHECK(0 == column._length);
  }
}
