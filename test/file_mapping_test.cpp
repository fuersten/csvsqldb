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


#include "libcsvsqldb/base/string_helper.h"
#include "libcsvsqldb/file_mapping.h"

#include <catch2/catch.hpp>


TEST_CASE("Mapping Test", "[engine]")
{
  SECTION("mapping")
  {
    csvsqldb::FileMapping::Mappings mappings;
    mappings.push_back({"employees.csv->employees", ',', false});
    mappings.push_back({"salaries.csv->salaries", ';', true});

    csvsqldb::FileMapping mapping;
    mapping.initialize(mappings);
    CHECK("employees.csv" == mapping.getMappingForTable("EMPLOYEES")._mapping);
    CHECK(',' == mapping.getMappingForTable("EMPLOYEES")._delimiter);
    CHECK("salaries.csv" == mapping.getMappingForTable("SALARIES")._mapping);
    CHECK(';' == mapping.getMappingForTable("SALARIES")._delimiter);
    CHECK(mapping.getMappingForTable("SALARIES")._skipFirstLine);
  }

  SECTION("missing mapping")
  {
    csvsqldb::FileMapping::Mappings mappings;
    mappings.push_back({"employees.csv->employees", ',', false});

    csvsqldb::FileMapping mapping;
    mapping.initialize(mappings);
    CHECK("employees.csv" == mapping.getMappingForTable("EMPLOYEES")._mapping);
    CHECK_THROWS_AS(mapping.getMappingForTable("salaries"), csvsqldb::MappingException);
    CHECK_THROWS_AS(mapping.getMappingForTable("employee"), csvsqldb::MappingException);
  }

  SECTION("as string vector")
  {
    csvsqldb::FileMapping::Mappings mappings;
    mappings.push_back({"employees.csv->employees", ',', false});
    mappings.push_back({"salaries.csv->salaries", ',', false});

    csvsqldb::FileMapping mapping;
    mapping.initialize(mappings);
    CHECK("employees.csv->EMPLOYEES,salaries.csv->SALARIES" == csvsqldb::join(mapping.asStringVector(), ","));
  }

  SECTION("json")
  {
    std::string JSON;

    {
      csvsqldb::FileMapping::Mappings mappings;
      mappings.push_back({"employees.csv->employees", ',', false});
      mappings.push_back({"salaries.csv->salaries", ',', false});

      csvsqldb::FileMapping mapping;
      mapping.initialize(mappings);

      JSON = mapping.asJson("employees", mappings);
    }

    std::stringstream ss(JSON);
    csvsqldb::FileMapping mapping = csvsqldb::FileMapping::fromJson(ss);
    CHECK("employees.csv" == mapping.getMappingForTable("EMPLOYEES")._mapping);
    CHECK_THROWS_AS(mapping.getMappingForTable("SALARIES"), csvsqldb::MappingException);
  }

  SECTION("json decode fail")
  {
    std::string invalidMapping = R"(
    { "Map":
      { "name": "EMPLOYEES",
        "mappings": [
          { "pattern": "employees.csv", "delimiter": ",", "skipFirstLine": true}
        ]
      }
    }
    )";
    std::stringstream ss(invalidMapping);
    CHECK_THROWS_AS(csvsqldb::FileMapping::fromJson(ss), csvsqldb::Exception);

    std::string brokenJson = R"(
    { "Mapping" : }
    )";

    std::stringstream ss1(brokenJson);
    CHECK_THROWS_AS(csvsqldb::FileMapping::fromJson(ss1), csvsqldb::Exception);
  }
}
