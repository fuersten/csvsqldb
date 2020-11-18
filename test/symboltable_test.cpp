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


#include "libcsvsqldb/database.h"
#include "libcsvsqldb/sql_parser.h"
#include "libcsvsqldb/symboltable.h"
#include "libcsvsqldb/tabledata.h"
#include "libcsvsqldb/validation_visitor.h"

#include "temporary_directory.h"

#include <catch2/catch.hpp>


TEST_CASE("Symbol Table Test", "[engine]")
{
  TemporaryDirectoryGuard tmpDir;
  auto path = tmpDir.temporaryDirectoryPath();

  SECTION("add symbol")
  {
    csvsqldb::SymbolTablePtr st = csvsqldb::SymbolTable::createSymbolTable();

    {
      csvsqldb::SymbolInfoPtr info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "emp";
      info->_alias = "emp";
      info->_identifier = "emp_no";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::INT;
      info->_prefix = "employees";
      st->addSymbol("emp", info);
    }

    CHECK(!st->hasSymbol("emp_no"));
    CHECK(st->hasSymbol("emp"));

    {
      const csvsqldb::SymbolInfoPtr& info = st->findSymbol("emp");
      CHECK("emp" == info->_alias);
      CHECK(csvsqldb::PLAIN == info->_symbolType);
      CHECK(csvsqldb::INT == info->_type);
      CHECK("employees" == info->_prefix);
      CHECK("emp_no" == info->_identifier);
    }
  }

  SECTION("update symbol")
  {
    csvsqldb::SymbolTablePtr st = csvsqldb::SymbolTable::createSymbolTable();

    {
      csvsqldb::SymbolInfoPtr info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "emp";
      info->_alias = "emp";
      info->_identifier = "emp_no";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::INT;
      info->_prefix = "employees";
      st->addSymbol("emp", info);
    }

    {
      csvsqldb::SymbolInfoPtr info = st->findSymbol("emp");
      info->_symbolType = csvsqldb::FUNCTION;
      info->_name = "function";
      info->_type = csvsqldb::REAL;
      info->_prefix = "";
      st->replaceSymbol("emp", "function", info);
    }

    {
      CHECK(!st->hasSymbol("emp"));
      const csvsqldb::SymbolInfoPtr& info = st->findSymbol("function");
      CHECK("emp" == info->_alias);
      CHECK(csvsqldb::FUNCTION == info->_symbolType);
      CHECK(csvsqldb::REAL == info->_type);
      CHECK("" == info->_prefix);
      CHECK("function" == info->_name);
    }
  }

  SECTION("next symbol")
  {
    csvsqldb::SymbolTablePtr st = csvsqldb::SymbolTable::createSymbolTable();

    CHECK("$alias_1" == st->getNextAlias());
  }

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

    csvsqldb::SymbolTablePtr st = csvsqldb::SymbolTable::createSymbolTable();
    {
      csvsqldb::SymbolInfoPtr info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "first_name";
      info->_alias = "";
      info->_identifier = "first_name";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::NONE;
      info->_prefix = "";
      info->_relation = "";
      st->addSymbol(info->_name, info);
    }
    {
      csvsqldb::SymbolInfoPtr info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "name";
      info->_alias = "name";
      info->_identifier = "last_name";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::NONE;
      info->_prefix = "";
      info->_relation = "";
      st->addSymbol(info->_name, info);
    }
    csvsqldb::SymbolTablePtr nestedSt = csvsqldb::SymbolTable::createSymbolTable(st);
    {
      csvsqldb::SymbolInfoPtr info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "first_name";
      info->_alias = "";
      info->_identifier = "first_name";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::NONE;
      info->_prefix = "";
      info->_relation = "";
      nestedSt->addSymbol(info->_name, info);
    }
    {
      csvsqldb::SymbolInfoPtr info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "last_name";
      info->_alias = "";
      info->_identifier = "last_name";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::NONE;
      info->_prefix = "";
      info->_relation = "";
      nestedSt->addSymbol(info->_name, info);
    }
    {
      csvsqldb::SymbolInfoPtr info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_identifier = "employees";
      info->_symbolType = csvsqldb::TABLE;
      info->_type = csvsqldb::NONE;
      info->_alias = "";
      info->_name = "";
      nestedSt->addSymbol(info->_name, info);
    }
    {
      csvsqldb::SymbolInfoPtr info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "";
      info->_symbolType = csvsqldb::SUBQUERY;
      info->_subquery = nestedSt;
      nestedSt->getParent()->addSymbol(info->_name, info);
    }

    st->typeSymbolTable(database);

    {
      CHECK(st->hasSymbol("first_name"));
      const csvsqldb::SymbolInfoPtr& info = st->findSymbol("first_name");
      CHECK("" == info->_alias);
      CHECK(csvsqldb::PLAIN == info->_symbolType);
      CHECK(csvsqldb::STRING == info->_type);
      CHECK("" == info->_prefix);
      CHECK("first_name" == info->_identifier);
    }
    {
      CHECK(st->hasSymbol("name"));
      const csvsqldb::SymbolInfoPtr& info = st->findSymbol("name");
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

    csvsqldb::SymbolTablePtr st = csvsqldb::SymbolTable::createSymbolTable();
    {
      csvsqldb::SymbolInfoPtr info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "emp.first_name";
      info->_alias = "";
      info->_identifier = "first_name";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::NONE;
      info->_prefix = "emp";
      info->_relation = "";
      st->addSymbol(info->_name, info);
    }
    {
      csvsqldb::SymbolInfoPtr info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "name";
      info->_alias = "name";
      info->_identifier = "last_name";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::NONE;
      info->_prefix = "emp";
      info->_relation = "";
      st->addSymbol(info->_name, info);
    }
    csvsqldb::SymbolTablePtr nestedSt = csvsqldb::SymbolTable::createSymbolTable(st);
    {
      csvsqldb::SymbolInfoPtr info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "first_name";
      info->_alias = "";
      info->_identifier = "first_name";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::NONE;
      info->_prefix = "";
      info->_relation = "";
      nestedSt->addSymbol(info->_name, info);
    }
    {
      csvsqldb::SymbolInfoPtr info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "last_name";
      info->_alias = "";
      info->_identifier = "last_name";
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = csvsqldb::NONE;
      info->_prefix = "";
      info->_relation = "";
      nestedSt->addSymbol(info->_name, info);
    }
    {
      csvsqldb::SymbolInfoPtr info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_identifier = "employees";
      info->_symbolType = csvsqldb::TABLE;
      info->_type = csvsqldb::NONE;
      info->_alias = "";
      info->_name = "";
      nestedSt->addSymbol(info->_name, info);
    }
    {
      csvsqldb::SymbolInfoPtr info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = "emp";
      info->_symbolType = csvsqldb::SUBQUERY;
      info->_subquery = nestedSt;
      nestedSt->getParent()->addSymbol("emp", info);
    }

    st->typeSymbolTable(database);

    {
      CHECK(st->hasSymbol("emp.first_name"));
      const csvsqldb::SymbolInfoPtr& info = st->findSymbol("emp.first_name");
      CHECK("" == info->_alias);
      CHECK(csvsqldb::PLAIN == info->_symbolType);
      CHECK(csvsqldb::STRING == info->_type);
      CHECK("emp" == info->_prefix);
      CHECK("first_name" == info->_identifier);
    }
    {
      CHECK(st->hasSymbol("name"));
      const csvsqldb::SymbolInfoPtr& info = st->findSymbol("name");
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

    csvsqldb::SymbolTablePtr symbolTable = node->symbolTable();
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

    csvsqldb::SymbolTablePtr symbolTable = node->symbolTable();

    CHECK_THROWS_AS(node->accept(validationVisitor), csvsqldb::SqlParserException);
  }
}
