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
#include <csvsqldb/sql_parser.h>
#include <csvsqldb/symboltable.h>
#include <csvsqldb/tabledata.h>
#include <csvsqldb/validation_visitor.h>

#include "temporary_directory.h"

#include <catch2/catch.hpp>


TEST_CASE("Symbol Table Utils Test", "[symbols]")
{
  SECTION("symbol type to string")
  {
    CHECK("CALC" == csvsqldb::symbolTypeToString(csvsqldb::CALC));
    CHECK("FUNCTION" == csvsqldb::symbolTypeToString(csvsqldb::FUNCTION));
    CHECK("NOSYM" == csvsqldb::symbolTypeToString(csvsqldb::NOSYM));
    CHECK("PLAIN" == csvsqldb::symbolTypeToString(csvsqldb::PLAIN));
    CHECK("SUBQUERY" == csvsqldb::symbolTypeToString(csvsqldb::SUBQUERY));
    CHECK("TABLE" == csvsqldb::symbolTypeToString(csvsqldb::TABLE));

    CHECK_THROWS(csvsqldb::symbolTypeToString(static_cast<csvsqldb::eSymbolType>(4711)));
  }
}

TEST_CASE("Simple Symbol Table Test", "[symbols]")
{
  csvsqldb::SymbolTablePtr symbolTable = csvsqldb::SymbolTable::createSymbolTable();

  SECTION("nested table")
  {
    auto table = csvsqldb::SymbolTable::createSymbolTable(symbolTable);
    REQUIRE(table);
    REQUIRE(table->getParent());
    CHECK(table->getParent().get() == symbolTable.get());
  }

  SECTION("add symbol")
  {
    CHECK_FALSE(symbolTable->hasSymbol("emp"));

    {
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "emp";
      info->_alias = "emp";
      info->_identifier = "emp_no";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::INT;
      info->_prefix = "employees";
      info->_relation = "emps";
      symbolTable->addSymbol("emp", info);
    }

    CHECK_FALSE(symbolTable->hasSymbol("emp_no"));
    CHECK(symbolTable->hasSymbol("emp"));

    {
      const auto& info = symbolTable->findSymbol("emp");
      CHECK("emp" == info->_alias);
      CHECK(csvsqldb::PLAIN == info->_symbolType);
      CHECK(csvsqldb::INT == info->_type);
      CHECK("employees" == info->_prefix);
      CHECK("emp_no" == info->_identifier);
      CHECK("emps" == info->_relation);
    }
  }

  SECTION("replace symbol")
  {
    {
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "emp";
      info->_alias = "emp";
      info->_identifier = "emp_no";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::INT;
      info->_prefix = "employees";
      symbolTable->addSymbol("emp", info);
    }

    {
      auto info = symbolTable->findSymbol("emp")->clone();
      info->_symbolType = csvsqldb::FUNCTION;
      info->_name = "function";
      info->_type = csvsqldb::REAL;
      info->_prefix = "";
      symbolTable->replaceSymbol("emp", "function", info);
    }

    {
      CHECK_FALSE(symbolTable->hasSymbol("emp"));
      const auto& info = symbolTable->findSymbol("function");
      CHECK("emp" == info->_alias);
      CHECK(csvsqldb::FUNCTION == info->_symbolType);
      CHECK(csvsqldb::REAL == info->_type);
      CHECK("" == info->_prefix);
      CHECK("function" == info->_name);
    }

    {
      CHECK_FALSE(symbolTable->hasSymbol("non-existent"));
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_symbolType = csvsqldb::PLAIN;
      info->_name = "check";
      info->_type = csvsqldb::BOOLEAN;
      info->_prefix = "";
      symbolTable->replaceSymbol("non-existent", "check", info);
    }

    CHECK_FALSE(symbolTable->hasSymbol("non-existent"));
    CHECK(symbolTable->hasSymbol("check"));
  }

  SECTION("next symbol")
  {
    CHECK("$alias_1" == symbolTable->getNextAlias());
    CHECK("$alias_2" == symbolTable->getNextAlias());
    CHECK("$alias_3" == symbolTable->getNextAlias());
    CHECK("$alias_4" == symbolTable->getNextAlias());

    csvsqldb::SymbolTablePtr otherSymbolTable = csvsqldb::SymbolTable::createSymbolTable();
    CHECK("$alias_1" == otherSymbolTable->getNextAlias());
    CHECK("$alias_2" == otherSymbolTable->getNextAlias());
    CHECK("$alias_3" == otherSymbolTable->getNextAlias());
    CHECK("$alias_4" == otherSymbolTable->getNextAlias());
  }

  SECTION("find error")
  {
    CHECK_THROWS_WITH(symbolTable->findSymbol("emp"), "symbol 'emp' not found in scope");
  }
}

TEST_CASE("Table Symbol Table Test", "[symbols]")
{
  csvsqldb::SymbolTablePtr symbolTable = csvsqldb::SymbolTable::createSymbolTable();

  {
    auto info = std::make_shared<csvsqldb::SymbolInfo>();
    info->_name = "empId";
    info->_alias = "id";
    info->_identifier = "empId";
    info->_symbolType = csvsqldb::PLAIN;
    info->_relation = "employees";
    symbolTable->addSymbol("empId", info);
  }

  {
    auto info = std::make_shared<csvsqldb::SymbolInfo>();
    info->_name = "name";
    info->_identifier = "name";
    info->_symbolType = csvsqldb::PLAIN;
    info->_relation = "employees";
    symbolTable->addSymbol("name", info);
  }

  {
    auto info = std::make_shared<csvsqldb::SymbolInfo>();
    info->_name = "age";
    info->_identifier = "age";
    info->_symbolType = csvsqldb::PLAIN;
    info->_relation = "employees";
    symbolTable->addSymbol("age", info);
  }

  {
    auto info = std::make_shared<csvsqldb::SymbolInfo>();
    info->_name = "test";
    info->_symbolType = csvsqldb::FUNCTION;
    symbolTable->addSymbol("test", info);
  }

  {
    auto info = std::make_shared<csvsqldb::SymbolInfo>();
    info->_name = "employees";
    info->_alias = "emps";
    info->_symbolType = csvsqldb::TABLE;
    symbolTable->addSymbol("employees", info);
  }

  {
    auto info = std::make_shared<csvsqldb::SymbolInfo>();
    info->_name = "salaries";
    info->_symbolType = csvsqldb::TABLE;
    symbolTable->addSymbol("salaries", info);
  }

  SECTION("get tables")
  {
    CHECK(2 == symbolTable->getTables().size());
    auto tables = symbolTable->getTables();
    std::for_each(tables.begin(), tables.end(), [](const auto& symbol) { CHECK(csvsqldb::TABLE == symbol->_symbolType); });
  }

  SECTION("find table symbols")
  {
    CHECK(symbolTable->hasTableSymbol("employees"));
    CHECK(symbolTable->hasTableSymbol("emps"));
    CHECK(symbolTable->hasTableSymbol("salaries"));
    CHECK_FALSE(symbolTable->hasTableSymbol("non-existent"));

    CHECK(symbolTable->findTableSymbol("employees"));
    CHECK(symbolTable->findTableSymbol("emps"));
    CHECK(symbolTable->findTableSymbol("salaries"));
    CHECK_THROWS_WITH(symbolTable->findTableSymbol("non-existent"), "symbol 'non-existent' not found in scope");
  }

  SECTION("find symbols for table")
  {
    CHECK(symbolTable->hasSymbolNameForTable("employees", "empId"));
    CHECK_FALSE(symbolTable->hasSymbolNameForTable("employees", "gender"));
    CHECK_FALSE(symbolTable->hasSymbolNameForTable("salaries", "empId"));

    CHECK(symbolTable->findSymbolNameForTable("employees", "empId"));
    CHECK_THROWS_WITH(symbolTable->findSymbolNameForTable("employees", "gender"), "symbol 'employees.gender' not found in scope");
    CHECK_THROWS_WITH(symbolTable->findSymbolNameForTable("salaries", "empId"), "symbol 'salaries.empId' not found in scope");
  }

  SECTION("find aliased symbols for table")
  {
    CHECK(symbolTable->findAliasedSymbol("id"));
    CHECK_THROWS_WITH(symbolTable->findAliasedSymbol("no-symbol"), "alias symbol 'no-symbol' not found in scope");
  }

  SECTION("find all symbols for table")
  {
    CHECK(3 == symbolTable->findAllSymbolsForTable("employees").size());
    CHECK(0 == symbolTable->findAllSymbolsForTable("salaries").size());
    CHECK(0 == symbolTable->findAllSymbolsForTable("non-existent").size());
  }

  SECTION("dump")
  {
    std::ostringstream os;
    os << std::endl;
    symbolTable->dump(os);

    std::string expected = R"(
Dumping symbol table:
empId symbol type: PLAIN type: none identifier: empId prefix:  name: empId alias: id relation: employees qualified: ,
name symbol type: PLAIN type: none identifier: name prefix:  name: name alias:  relation: employees qualified: ,
age symbol type: PLAIN type: none identifier: age prefix:  name: age alias:  relation: employees qualified: ,
test symbol type: FUNCTION type: none identifier:  prefix:  name: test alias:  relation:  qualified: ,
employees symbol type: TABLE type: none identifier:  prefix:  name: employees alias: emps relation:  qualified: ,
salaries symbol type: TABLE type: none identifier:  prefix:  name: salaries alias:  relation:  qualified: ,
)";
    CHECK(expected == os.str());
  }
}

TEST_CASE("Database Symbol Table Test", "[engine]")
{
  TemporaryDirectoryGuard tmpDir;
  auto path = tmpDir.temporaryDirectoryPath();
  csvsqldb::SymbolTablePtr symbolTable = csvsqldb::SymbolTable::createSymbolTable();


  SECTION("nested symbol table")
  {
    csvsqldb::TableData tabledata("EMPLOYEES");
    tabledata.addColumn("id", csvsqldb::INT, true, true, true, std::any(), csvsqldb::ASTExprNodePtr(), 0);
    tabledata.addColumn("first_name", csvsqldb::STRING, false, false, true, std::any(), csvsqldb::ASTExprNodePtr(), 64);
    tabledata.addColumn("last_name", csvsqldb::STRING, false, false, true, std::any(), csvsqldb::ASTExprNodePtr(), 64);
    tabledata.addColumn("birth_date", csvsqldb::DATE, false, false, true, std::any(), csvsqldb::ASTExprNodePtr(), 0);

    csvsqldb::FileMapping mapping;
    csvsqldb::Database database(path, mapping);
    database.addTable(std::move(tabledata), false);

    // simulating a symbol table for this query:
    // select first_name,last_name as name from (select first_name,last_name from employees)

    {
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "first_name";
      info->_alias = "";
      info->_identifier = "first_name";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::NONE;
      info->_prefix = "";
      info->_relation = "";
      symbolTable->addSymbol(info->_name, info);
    }
    {
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "name";
      info->_alias = "name";
      info->_identifier = "last_name";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::NONE;
      info->_prefix = "";
      info->_relation = "";
      symbolTable->addSymbol(info->_name, info);
    }
    csvsqldb::SymbolTablePtr nestedSymbolTable = csvsqldb::SymbolTable::createSymbolTable(symbolTable);
    {
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "first_name";
      info->_alias = "";
      info->_identifier = "first_name";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::NONE;
      info->_prefix = "";
      info->_relation = "";
      nestedSymbolTable->addSymbol(info->_name, info);
    }
    {
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "last_name";
      info->_alias = "";
      info->_identifier = "last_name";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::NONE;
      info->_prefix = "";
      info->_relation = "";
      nestedSymbolTable->addSymbol(info->_name, info);
    }
    {
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_identifier = "employees";
      info->_symbolType = csvsqldb::TABLE;
      info->_type = csvsqldb::NONE;
      info->_alias = "";
      info->_name = "";
      nestedSymbolTable->addSymbol(info->_name, info);
    }
    {
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "";
      info->_symbolType = csvsqldb::SUBQUERY;
      info->_subquery = nestedSymbolTable;
      nestedSymbolTable->getParent()->addSymbol(info->_name, info);
    }

    symbolTable->typeSymbolTable(database);

    {
      CHECK(symbolTable->hasSymbol("first_name"));
      const auto& info = symbolTable->findSymbol("first_name");
      CHECK("" == info->_alias);
      CHECK(csvsqldb::PLAIN == info->_symbolType);
      CHECK(csvsqldb::STRING == info->_type);
      CHECK("" == info->_prefix);
      CHECK("first_name" == info->_identifier);
    }
    {
      CHECK(symbolTable->hasSymbol("name"));
      const auto& info = symbolTable->findSymbol("name");
      CHECK("name" == info->_alias);
      CHECK(csvsqldb::PLAIN == info->_symbolType);
      CHECK(csvsqldb::STRING == info->_type);
      CHECK("" == info->_prefix);
      CHECK("last_name" == info->_identifier);
    }
  }

  SECTION("nested symbol table with alias")
  {
    csvsqldb::TableData tabledata("EMPLOYEES");
    tabledata.addColumn("id", csvsqldb::INT, true, true, true, std::any(), csvsqldb::ASTExprNodePtr(), 0);
    tabledata.addColumn("first_name", csvsqldb::STRING, false, false, true, std::any(), csvsqldb::ASTExprNodePtr(), 64);
    tabledata.addColumn("last_name", csvsqldb::STRING, false, false, true, std::any(), csvsqldb::ASTExprNodePtr(), 64);
    tabledata.addColumn("birth_date", csvsqldb::DATE, false, false, true, std::any(), csvsqldb::ASTExprNodePtr(), 0);

    csvsqldb::FileMapping mapping;
    csvsqldb::Database database(path, mapping);
    database.addTable(std::move(tabledata), false);

    // simulating a symbol table for this query:
    // select emp.first_name,emp.last_name as name from (select first_name,last_name from employees) as emp

    {
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "emp.first_name";
      info->_alias = "";
      info->_identifier = "first_name";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::NONE;
      info->_prefix = "emp";
      info->_relation = "";
      symbolTable->addSymbol(info->_name, info);
    }
    {
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "name";
      info->_alias = "name";
      info->_identifier = "last_name";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::NONE;
      info->_prefix = "emp";
      info->_relation = "";
      symbolTable->addSymbol(info->_name, info);
    }
    csvsqldb::SymbolTablePtr nestedSymbolTable = csvsqldb::SymbolTable::createSymbolTable(symbolTable);
    {
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "first_name";
      info->_alias = "";
      info->_identifier = "first_name";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::NONE;
      info->_prefix = "";
      info->_relation = "";
      nestedSymbolTable->addSymbol(info->_name, info);
    }
    {
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "last_name";
      info->_alias = "";
      info->_identifier = "last_name";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::NONE;
      info->_prefix = "";
      info->_relation = "";
      nestedSymbolTable->addSymbol(info->_name, info);
    }
    {
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_identifier = "employees";
      info->_symbolType = csvsqldb::TABLE;
      info->_type = csvsqldb::NONE;
      info->_alias = "";
      info->_name = "";
      nestedSymbolTable->addSymbol(info->_name, info);
    }
    {
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "emp";
      info->_symbolType = csvsqldb::SUBQUERY;
      info->_subquery = nestedSymbolTable;
      nestedSymbolTable->getParent()->addSymbol("emp", info);
    }

    symbolTable->typeSymbolTable(database);

    {
      CHECK(symbolTable->hasSymbol("emp.first_name"));
      const auto& info = symbolTable->findSymbol("emp.first_name");
      CHECK("" == info->_alias);
      CHECK(csvsqldb::PLAIN == info->_symbolType);
      CHECK(csvsqldb::STRING == info->_type);
      CHECK("emp" == info->_prefix);
      CHECK("first_name" == info->_identifier);
    }
    {
      CHECK(symbolTable->hasSymbol("name"));
      const auto& info = symbolTable->findSymbol("name");
      CHECK("name" == info->_alias);
      CHECK(csvsqldb::PLAIN == info->_symbolType);
      CHECK(csvsqldb::STRING == info->_type);
      CHECK("emp" == info->_prefix);
      CHECK("last_name" == info->_identifier);
    }
  }

  SECTION("parsed symbol table")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    csvsqldb::ASTNodePtr node = parser.parse(
      "CREATE TABLE employees(emp_no INTEGER,birth_date DATE NOT NULL,first_name VARCHAR(25) NOT NULL,last_name VARCHAR(50) "
      "NOT NULL,gender CHAR,hire_date DATE,PRIMARY KEY(emp_no))");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node));
    csvsqldb::ASTCreateTableNodePtr createNode = std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node);

    csvsqldb::TableData tabledata = csvsqldb::TableData::fromCreateAST(createNode);
    csvsqldb::FileMapping mapping;

    csvsqldb::Database database(path, mapping);
    database.addTable(std::move(tabledata), false);

    node =
      parser.parse("SELECT \"emp\".\"first_name\",emp.last_name,birth_date,hire_date FROM \"employees\" AS emp WHERE hire_date < "
                   "DATE'2014-01-01'");
    REQUIRE(node);

    symbolTable->typeSymbolTable(database);

    node = parser.parse("select emp_no no,emp_no as id from employees where last_name = 'Schmiedel' and emp_no > 490000");
    REQUIRE(node);

    symbolTable = node->symbolTable();

    symbolTable->typeSymbolTable(database);
  }

  SECTION("complex symbol table")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    csvsqldb::Database database(path, csvsqldb::FileMapping());

    csvsqldb::ASTNodePtr node = parser.parse(
      "CREATE TABLE employees(emp_no INTEGER,birth_date DATE NOT NULL,first_name VARCHAR(25) NOT NULL,last_name VARCHAR(50) "
      "NOT NULL,gender CHAR,hire_date DATE,PRIMARY KEY(emp_no))");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node));
    csvsqldb::ASTCreateTableNodePtr createNode = std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node);
    csvsqldb::TableData tabledata = csvsqldb::TableData::fromCreateAST(createNode);

    database.addTable(std::move(tabledata), false);

    node = parser.parse(
      "CREATE TABLE salaries(emp_no INTEGER PRIMARY KEY,salary FLOAT CHECK(salary > 0.0),from_date DATE,to_date DATE)");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node));
    createNode = std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node);
    tabledata = csvsqldb::TableData::fromCreateAST(createNode);

    database.addTable(std::move(tabledata), false);

    csvsqldb::ASTValidationVisitor validationVisitor(database);

    node = parser.parse("SELECT * FROM employees e, salaries s WHERE e.emp_no = s.emp_no and s.salary > 20000");
    REQUIRE(node);

    CHECK(node->symbolTable());
    CHECK_THROWS_AS(node->accept(validationVisitor), csvsqldb::SqlParserException);
  }
}
