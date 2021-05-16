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
#include <csvsqldb/validation_visitor.h>

#include "test/operatornodes/scan_operator_node_mock.h"
#include "test/temporary_directory.h"


class OperatorNodeTestFixture
{
public:
  OperatorNodeTestFixture() = default;

  csvsqldb::SymbolInfo addTable(std::string name, const std::vector<std::tuple<std::string, csvsqldb::eType>>& columns)
  {
    csvsqldb::TableData tabledata(name);
    csvsqldb::ASTExprNodePtr check;
    for (const auto column : columns) {
      tabledata.addColumn(std::get<0>(column), std::get<1>(column), false, false, false, std::any(), check, 0);

      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_name = std::get<0>(column);
      info->_identifier = std::get<0>(column);
      info->_symbolType = csvsqldb::PLAIN;
      info->_type = std::get<1>(column);
      info->_relation = name;
      symbolTable->addSymbol(std::get<0>(column), info);
    }
    database.addTable(std::move(tabledata));

    {
      auto info = std::make_shared<csvsqldb::SymbolInfo>();
      info->_identifier = name;
      info->_name = name;
      info->_symbolType = csvsqldb::TABLE;
      symbolTable->addSymbol(name, info);
    }

    csvsqldb::SymbolInfo info;
    info._identifier = name;
    info._name = name;
    info._symbolType = csvsqldb::TABLE;
    return info;
  }

  void addExpression(csvsqldb::Expressions& expressions, std::string expression)
  {
    auto expr = parser.parseExpression(expression, symbolTable);
    addExpressionSymbol(expr);
    expr->symbolTable()->typeSymbolTable(database);
    expressions.emplace_back(expr);
  }

  void addExpressionSymbol(csvsqldb::ASTExprNodePtr expr)
  {
    auto info = std::make_shared<csvsqldb::SymbolInfo>();
    info->_symbolType = csvsqldb::CALC;
    info->_type = csvsqldb::NONE;
    info->_expressionNode = expr;
    auto symbolName = symbolTable->getNextAlias();
    info->_name = symbolName;
    symbolTable->addSymbol(symbolName, info);
    expr->_symbolName = symbolName;
  }

  TemporaryDirectoryGuard tmpDir;
  csvsqldb::FileMapping mapping;
  csvsqldb::Database database{tmpDir.temporaryDirectoryPath(), mapping};
  csvsqldb::FunctionRegistry functions;
  csvsqldb::BlockManager blockManager;
  csvsqldb::OperatorContext context{database, functions, blockManager, {}};
  csvsqldb::SymbolTablePtr symbolTable = csvsqldb::SymbolTable::createSymbolTable();
  csvsqldb::SQLParser parser{functions};
};
