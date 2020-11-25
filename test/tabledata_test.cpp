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


#include <csvsqldb/sql_parser.h>
#include <csvsqldb/tabledata.h>

#include <catch2/catch.hpp>


TEST_CASE("Table Data Test", "[tables]")
{
  SECTION("encode decode")
  {
    csvsqldb::TableData tabledata("TestTable");

    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);
    csvsqldb::ASTExprNodePtr check;
    tabledata.addColumn("id", csvsqldb::INT, true, false, false, std::any(), check, 0);
    std::any defaultValue = std::string("Lars");
    tabledata.addColumn("name", csvsqldb::STRING, false, false, false, defaultValue, check, 25);
    check = parser.parseExpression("age >= 18");
    tabledata.addColumn("age", csvsqldb::INT, false, false, false, std::any(), check, 0);

    check = csvsqldb::ASTExprNodePtr();
    csvsqldb::StringVector primaryKeys = {"id", "name"};
    tabledata.addConstraint(primaryKeys, csvsqldb::StringVector(), check);
    csvsqldb::StringVector unique = {"age"};
    tabledata.addConstraint(csvsqldb::StringVector(), unique, check);
    check = parser.parseExpression("id > 4711 AND name = 'Lars'");
    tabledata.addConstraint(csvsqldb::StringVector(), csvsqldb::StringVector(), check);

    std::string json = tabledata.asJson();
    std::stringstream ss(json);
    csvsqldb::TableData decoded = csvsqldb::TableData::fromJson(ss);
    CHECK(json == decoded.asJson());
  }

  SECTION("json decode fail")
  {
    std::string invalidTableData = R"(
    { "Table" :
      { "My cool dump name" : "EMPLOYEES",
        "columns" : [
          { "name" : "EMP_NO", "type" : "INTEGER", "primary key" : false, "not null" : false, "unique" : false, "default" : "", "check" : "", "length" : 0 },
          { "name" : "BIRTH_DATE", "type" : "DATE", "primary key" : false, "not null" : true, "unique" : false, "default" : "", "check" : "", "length" : 0 },
          { "name" : "FIRST_NAME", "type" : "VARCHAR", "primary key" : false, "not null" : true, "unique" : false, "default" : "", "check" : "", "length" : 25 },
          { "name" : "LAST_NAME", "type" : "VARCHAR", "primary key" : false, "not null" : true, "unique" : false, "default" : "", "check" : "", "length" : 50 },
          { "name" : "GENDER", "type" : "VARCHAR", "primary key" : false, "not null" : false, "unique" : false, "default" : "", "check" : "", "length" : 0 },
          { "name" : "HIRE_DATE", "type" : "DATE", "primary key" : false, "not null" : false, "unique" : false, "default" : "", "check" : "", "length" : 0 }
        ],
        "constraints" : [
          { "primary keys" : [ "EMP_NO" ],
            "unique keys" : [  ],
            "check" : ""
          }
        ]
      }
    }
    )";

    std::stringstream ss(invalidTableData);
    CHECK_THROWS_AS(csvsqldb::TableData::fromJson(ss), csvsqldb::Exception);

    std::string brokenJson = R"(
    { "Table" : }
    )";

    std::stringstream ss1(brokenJson);
    CHECK_THROWS_AS(csvsqldb::TableData::fromJson(ss1), csvsqldb::Exception);
  }
}
