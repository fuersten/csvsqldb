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

#include <csvsqldb/operatornodes/systemtable_scan_operatornode.h>

#include "temporary_directory.h"
#include "test/test_util.h"

#include <catch2/catch.hpp>


TEST_CASE("System Scan Operator Node Test", "[operatornodes]")
{
  TemporaryDirectoryGuard tmpDir;
  auto path = tmpDir.temporaryDirectoryPath();
  csvsqldb::FileMapping mapping;
  csvsqldb::Database database{path, mapping};
  csvsqldb::FunctionRegistry functions;
  csvsqldb::BlockManager blockManager;
  csvsqldb::OperatorContext context{database, functions, blockManager, {}};
  auto symbolTable = csvsqldb::SymbolTable::createSymbolTable();

  csvsqldb::SymbolInfo info;
  info._identifier = "SYSTEM_TABLES";
  csvsqldb::SystemTableScanOperatorNode operatorNode{context, symbolTable, info};

  SECTION("Scan System Table")
  {
    const auto* values = operatorNode.getNextRow();
    REQUIRE(values);
    REQUIRE(2 == values->size());
    values = operatorNode.getNextRow();
    REQUIRE(values);
    REQUIRE(2 == values->size());
  }

  SECTION("dump")
  {
    std::stringstream ss;
    operatorNode.dump(ss);
    CHECK(ss.str() == "SystemTableScanOperatorNode(SYSTEM_TABLES)\n");
  }
}
