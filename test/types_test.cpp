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


#include "csvsqldb/sql_ast.h"

#include <catch2/catch.hpp>


TEST_CASE("Types Test", "[types]")
{
  SECTION("identifiers")
  {
    csvsqldb::IdentifierSet identifiers;

    csvsqldb::SymbolTablePtr symbolTable = csvsqldb::SymbolTable::createSymbolTable();
    csvsqldb::SymbolInfoPtr info = std::make_shared<csvsqldb::SymbolInfo>();
    info->_name = "BIRTHDATE";

    csvsqldb::ASTIdentifier bd(symbolTable, info, "", "BIRTHDATE", false);

    identifiers.insert(bd);
    CHECK(identifiers.size() == 1);

    csvsqldb::ASTIdentifier id(symbolTable, info, "", "ID", false);
    identifiers.insert(id);
    CHECK(identifiers.size() == 2);
    identifiers.insert(id);
    CHECK(identifiers.size() == 2);

    csvsqldb::ASTIdentifier prefix_bd(symbolTable, info, "EMPLOYEES", "BIRTHDATE", false);
    identifiers.insert(prefix_bd);
    CHECK(identifiers.size() == 3);
  }

  SECTION("string to bool")
  {
    CHECK(csvsqldb::stringToBool("true"));
    CHECK_FALSE(csvsqldb::stringToBool("false"));
    CHECK(csvsqldb::stringToBool("TRUE"));
    CHECK_FALSE(csvsqldb::stringToBool("FaLsE"));
    CHECK(csvsqldb::stringToBool("1"));
    CHECK_FALSE(csvsqldb::stringToBool("0"));
  }
  SECTION("description type to string")
  {
    CHECK("AST" == csvsqldb::descriptionTypeToString(csvsqldb::AST));
    CHECK("EXEC" == csvsqldb::descriptionTypeToString(csvsqldb::EXEC));
    CHECK_THROWS_AS(csvsqldb::descriptionTypeToString(static_cast<csvsqldb::eDescriptionType>(42)), std::runtime_error);
  }
  SECTION("order to string")
  {
    CHECK("ASC" == csvsqldb::orderToString(csvsqldb::ASC));
    CHECK("DESC" == csvsqldb::orderToString(csvsqldb::DESC));
    CHECK_THROWS_AS(csvsqldb::orderToString(static_cast<csvsqldb::eOrder>(42)), std::runtime_error);
  }
}
