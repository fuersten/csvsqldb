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


#include <csvsqldb/table_executions.h>

#include "data_test_framework.h"

#include <catch2/catch.hpp>


TEST_CASE("Table Executions Test", "[engine]")
{
  DatabaseTestWrapper dbWrapper;
  csvsqldb::Database& database{dbWrapper.getDatabase()};

  csvsqldb::ColumnDefinitions columnDefinitions;
  columnDefinitions.emplace_back(csvsqldb::ColumnDefinition("name", csvsqldb::STRING));
  columnDefinitions.emplace_back(csvsqldb::ColumnDefinition("id", csvsqldb::INT));
  columnDefinitions.emplace_back(csvsqldb::ColumnDefinition("age", csvsqldb::INT));

  csvsqldb::TableConstraints constraints;
  csvsqldb::TableConstraint constraint;
  constraint._name = "FRIENDS_CONSTRAINT";
  constraint._primaryKeys = {"id"};
  constraints.emplace_back(constraint);

  csvsqldb::FileMapping::Mappings mappings;
  mappings.push_back({"friends.csv->friends", ',', false});

  SECTION("create table execute")
  {
    CHECK_FALSE(database.hasTable("FRIENDS"));
    csvsqldb::CreateTableExecutionNode createTableNode(database, "FRIENDS", columnDefinitions, constraints, false);
    CHECK(0 == createTableNode.execute());
    CHECK(database.hasTable("FRIENDS"));
  }

  SECTION("create table already created")
  {
    csvsqldb::CreateTableExecutionNode createTableNode(database, "FRIENDS", columnDefinitions, constraints, false);
    createTableNode.execute();
    CHECK_THROWS_WITH(createTableNode.execute(), "table 'FRIENDS' does already exist");
  }

  SECTION("create table if not exist with already created table")
  {
    csvsqldb::CreateTableExecutionNode createTableNode(database, "FRIENDS", columnDefinitions, constraints, true);
    createTableNode.execute();
    CHECK(0 == createTableNode.execute());
  }

  SECTION("create table dump")
  {
    csvsqldb::CreateTableExecutionNode createTableNode(database, "FRIENDS", columnDefinitions, constraints, true);
    std::ostringstream os;
    createTableNode.dump(os);
    std::string expected = R"(CreateTableExecution (FRIENDS [create] -> name: VARCHAR, id: INTEGER, age: INTEGER))";
    CHECK(expected == os.str());
  }

  SECTION("drop table execute")
  {
    CHECK_FALSE(database.hasTable("FRIENDS"));
    csvsqldb::CreateTableExecutionNode createTableNode(database, "FRIENDS", columnDefinitions, constraints, false);
    createTableNode.execute();
    CHECK(database.hasTable("FRIENDS"));

    csvsqldb::DropTableExecutionNode dropTableNode(database, "FRIENDS");
    CHECK(0 == dropTableNode.execute());
    CHECK_FALSE(database.hasTable("FRIENDS"));
  }

  SECTION("drop table dump")
  {
    csvsqldb::DropTableExecutionNode dropTableNode(database, "FRIENDS");
    std::ostringstream os;
    dropTableNode.dump(os);
    std::string expected = R"(DropTableExecution (FRIENDS))";
    CHECK(expected == os.str());
  }

  SECTION("create mapping")
  {
    csvsqldb::CreateMappingExecutionNode createMappingNode(database, "FRIENDS", mappings);
    CHECK(database.getMappings().empty());
    CHECK(0 == createMappingNode.execute());
    CHECK_FALSE(database.getMappings().empty());
  }

  SECTION("create mapping dump")
  {
    csvsqldb::CreateMappingExecutionNode createMappingNode(database, "FRIENDS", mappings);
    std::ostringstream os;
    createMappingNode.dump(os);
    std::string expected = R"(CreateMappingExecution (friends.csv->friends -> FRIENDS))";
    CHECK(expected == os.str());
  }

  SECTION("drop mapping")
  {
    csvsqldb::CreateMappingExecutionNode createMappingNode(database, "FRIENDS", mappings);
    createMappingNode.execute();
    CHECK_FALSE(database.getMappings().empty());
    csvsqldb::DropMappingExecutionNode dropMappingNode(database, "FRIENDS");
    CHECK(0 == dropMappingNode.execute());
    CHECK(database.getMappings().empty());
  }

  SECTION("drop mapping dump")
  {
    csvsqldb::DropMappingExecutionNode dropMappingNode(database, "FRIENDS");
    std::ostringstream os;
    dropMappingNode.dump(os);
    std::string expected = R"(DropMappingExecution (FRIENDS))";
    CHECK(expected == os.str());
  }
}
