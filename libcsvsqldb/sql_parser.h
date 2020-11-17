//
//  sql_parser.h
//  csvsqldb
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

#ifndef csvsqldb_sql_parser_h
#define csvsqldb_sql_parser_h

#include "libcsvsqldb/inc.h"

#include "function_registry.h"
#include "sql_ast.h"
#include "sql_lexer.h"


namespace csvsqldb
{
  class CSVSQLDB_EXPORT SQLParser
  {
  public:
    SQLParser(const FunctionRegistry& functionRegistry);

    ASTNodePtr parse(const std::string& input);
    ASTNodePtr parse();

    ASTExprNodePtr parseExpression(const std::string& expression);

    void setInput(const std::string& input);

  private:
    void reportUnexpectedToken(const std::string& message, const csvsqldb::lexer::Token& token);

    std::string expect(eToken tok);
    bool canExpect(eToken tok);
    csvsqldb::lexer::Token parseNext();

    ASTCreateTableNodePtr parseCreateTable();
    eType parseType();
    ColumnDefinition parseColumnDefinition();
    TableConstraint parseTableConstraint();
    void parseConstraint(ColumnDefinition& definition);
    csvsqldb::StringVector parseColumnList();

    ASTAlterTableNodePtr parseAlterTable();

    ASTDropTableNodePtr parseDropTable();

    ASTDescribeNodePtr parseExplain();

    ASTMappingNodePtr parseCreateMapping();
    ASTDropMappingNodePtr parseDropMapping();

    ASTQueryNodePtr parseQuery();

    ASTQueryExpressionNodePtr parseQueryExpression(const SymbolTablePtr& symboltable);
    ASTTableExpressionNodePtr parseTableExpression(const SymbolTablePtr& symboltable);
    ASTTableReferenceNodePtr parseTableReference(const SymbolTablePtr& symboltable);
    ASTIdentifierPtr parseAliasedIdentifier(const SymbolTablePtr& symboltable);
    ASTTableFactorNodePtr parseTableFactor(const SymbolTablePtr& symboltable);
    eJoinType parseJoinType();
    bool isJoin();
    ASTJoinNodePtr parseJoinClause(const ASTTableReferenceNodePtr& reference, eJoinType joinType);
    ASTExprNodePtr parseQualifiedIdentifierOrAsterisk(const SymbolTablePtr& symboltable);
    ASTIdentifierPtr parseQualifiedIdentifier(const SymbolTablePtr& symboltable);
    std::string parseQuotedIdentifier(bool& quoted);
    Identifiers parseIdentifierList(const SymbolTablePtr& symboltable);
    Parameters parseParameterList(const SymbolTablePtr& symboltable);
    Expressions parseSelectList(const SymbolTablePtr& symboltable);
    ASTExprNodePtr parseDerivedColumn(const SymbolTablePtr& symboltable);
    ASTExprNodePtr parseExpression(const SymbolTablePtr& symboltable);
    Expressions parseExprList(const SymbolTablePtr& symboltable);
    ASTFromNodePtr parseFrom(const SymbolTablePtr& symboltable);
    ASTWhereNodePtr parseWhere(const SymbolTablePtr& symboltable);
    ASTGroupByNodePtr parseGroupBy(const SymbolTablePtr& symboltable);
    ASTHavingNodePtr parseHaving(const SymbolTablePtr& symboltable);
    ASTOrderByNodePtr parseOrderBy(const SymbolTablePtr& symboltable);
    ASTLimitNodePtr parseLimit(const SymbolTablePtr& symboltable);

    ASTExprNodePtr parseJoin(const SymbolTablePtr& symboltable);
    ASTExprNodePtr parseEquality(const SymbolTablePtr& symboltable);
    ASTExprNodePtr parseRelation(const SymbolTablePtr& symboltable);
    ASTExprNodePtr parseAdd(const SymbolTablePtr& symboltable);
    ASTExprNodePtr parseMul(const SymbolTablePtr& symboltable);
    ASTExprNodePtr parseFactor(const SymbolTablePtr& symboltable);
    ASTExprNodePtr parseUnary(const SymbolTablePtr& symboltable);

    SQLLexer _lexer;
    csvsqldb::lexer::Token _currentToken;
    const FunctionRegistry& _functionRegistry;
  };
}

#endif
