//
//  sql_parser.cpp
//  csv db
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


#include "sql_parser.h"

#include "base/logging.h"
#include "base/string_helper.h"


namespace csvsqldb
{
  SQLParser::SQLParser(const FunctionRegistry& functionRegistry)
  : _functionRegistry(functionRegistry)
  {
    _currentToken._token = TOK_NONE;
  }

  void SQLParser::reportUnexpectedToken(const std::string& message, const csvsqldb::lexer::Token& token)
  {
    CSVSQLDB_THROW(SqlParserException,
                   message << "'" << token._value << "' at line " << token._lineCount << ":" << token._charCount);
  }

  std::string SQLParser::expect(eToken tok)
  {
    if (_currentToken._token != tok) {
      CSVSQLDB_THROW(SqlParserException, "expected '" << tokenToString(tok) << "' but found '"
                                                      << tokenToString(eToken(_currentToken._token)) << "' ("
                                                      << _currentToken._value << ") at line " << _currentToken._lineCount << ":"
                                                      << _currentToken._charCount);
    }
    std::string val = _currentToken._value;
    parseNext();
    return val;
  }

  bool SQLParser::canExpect(eToken tok)
  {
    if (_currentToken._token == tok) {
      expect(tok);
      return true;
    }

    return false;
  }

  csvsqldb::lexer::Token SQLParser::parseNext()
  {
    if (_currentToken._token == csvsqldb::lexer::EOI) {
      CSVSQLDB_THROW(SqlParserException, "already at end of input");
    }
    csvsqldb::lexer::Token tok = _lexer.next();
    while (tok._token == TOK_COMMENT) {
      tok = _lexer.next();
    }
    _currentToken = tok;

    return _currentToken;
  }

  void SQLParser::setInput(const std::string& input)
  {
    _lexer.setInput(input);
    _currentToken = csvsqldb::lexer::Token();
    _currentToken._token = TOK_NONE;
  }

  ASTNodePtr SQLParser::parse(const std::string& input)
  {
    setInput(input);
    return parse();
  }

  ASTNodePtr SQLParser::parse()
  {
    ASTNodePtr astnode;

    if (_currentToken._token == TOK_NONE) {
      parseNext();
    }

    if (_currentToken._token != csvsqldb::lexer::EOI) {
      if (_currentToken._token == TOK_SELECT || _currentToken._token == TOK_LEFT_PAREN) {
        astnode = parseQuery();
      } else if (_currentToken._token == TOK_CREATE) {
        expect(TOK_CREATE);
        if (_currentToken._token == TOK_TABLE) {
          astnode = parseCreateTable();
        } else if (_currentToken._token == TOK_MAPPING) {
          astnode = parseCreateMapping();
        } else {
          reportUnexpectedToken("expected 'TABLE' or 'MAPPING', but found ", _currentToken);
        }
      } else if (_currentToken._token == TOK_ALTER) {
        astnode = parseAlterTable();
      } else if (_currentToken._token == TOK_DROP) {
        expect(TOK_DROP);
        if (_currentToken._token == TOK_TABLE) {
          astnode = parseDropTable();
        } else if (_currentToken._token == TOK_MAPPING) {
          astnode = parseDropMapping();
        } else {
          reportUnexpectedToken("expected 'TABLE' or 'MAPPING', but found ", _currentToken);
        }
      } else if (_currentToken._token == TOK_EXPLAIN) {
        astnode = parseExplain();
      } else {
        reportUnexpectedToken("unexpected token, found ", _currentToken);
      }
      if (_currentToken._token == TOK_SEMICOLON) {
        expect(TOK_SEMICOLON);
      } else if (_currentToken._token != csvsqldb::lexer::EOI) {
        // there are more tokens, but there is no semicolon and we are ready with parsing, so this is not good
        expect(TOK_SEMICOLON);
      }
    }

    return astnode;
  }

  ASTExprNodePtr SQLParser::parseExpression(const std::string& expression)
  {
    setInput(expression);
    parseNext();
    return parseExpression(SymbolTable::createSymbolTable());
  }

  ASTDescribeNodePtr SQLParser::parseExplain()
  {
    expect(TOK_EXPLAIN);
    eDescriptionType desc = AST;
    if (canExpect(TOK_AST)) {
      desc = AST;
    } else if (canExpect(TOK_EXEC)) {
      desc = EXEC;
    } else {
      reportUnexpectedToken("expected 'AST' or 'EXEC', but found ", _currentToken);
    }
    ASTQueryNodePtr query = parseQuery();

    return std::make_shared<ASTExplainNode>(query->symbolTable(), desc, query);
  }

  ASTMappingNodePtr SQLParser::parseCreateMapping()
  {
    expect(TOK_MAPPING);
    bool quoted = false;
    std::string name = parseQuotedIdentifier(quoted);
    expect(TOK_LEFT_PAREN);
    FileMapping::Mappings mappings;

    std::string value = _currentToken._value;
    if (!canExpect(TOK_CONST_STRING)) {
      expect(TOK_QUOTED_IDENTIFIER);
    }
    char delimiter = ',';
    bool skipFirstLine = false;
    if (canExpect(TOK_COMMA)) {
      delimiter = expect(TOK_CONST_CHAR)[0];

      if (canExpect(TOK_COMMA)) {
        skipFirstLine = csvsqldb::stringToBool(expect(TOK_CONST_BOOLEAN));
      }
    }
    mappings.push_back({value, delimiter, skipFirstLine});
    expect(TOK_RIGHT_PAREN);

    return std::make_shared<ASTMappingNode>(SymbolTable::createSymbolTable(), name, mappings);
  }

  ASTDropMappingNodePtr SQLParser::parseDropMapping()
  {
    expect(TOK_MAPPING);
    bool quoted = false;
    std::string name = parseQuotedIdentifier(quoted);

    return std::make_shared<ASTDropMappingNode>(SymbolTable::createSymbolTable(), name);
  }

  ASTCreateTableNodePtr SQLParser::parseCreateTable()
  {
    bool createIfNotExists = false;

    expect(TOK_TABLE);
    if (canExpect(TOK_IF)) {
      expect(TOK_NOT);
      expect(TOK_EXISTS);
      createIfNotExists = true;
    }
    bool quoted = false;
    std::string name = parseQuotedIdentifier(quoted);
    expect(TOK_LEFT_PAREN);

    ColumnDefinitions columns;
    TableConstraints constraints;

    while (_currentToken._token == TOK_IDENTIFIER || _currentToken._token == TOK_CONSTRAINT ||
           _currentToken._token == TOK_UNIQUE || _currentToken._token == TOK_PRIMARY || _currentToken._token == TOK_CHECK) {
      if (_currentToken._token == TOK_IDENTIFIER) {
        columns.push_back(parseColumnDefinition());
      } else {
        constraints.push_back(parseTableConstraint());
      }
      if (canExpect(TOK_COMMA)) {
        if (!(_currentToken._token == TOK_IDENTIFIER || _currentToken._token == TOK_CONSTRAINT ||
              _currentToken._token == TOK_UNIQUE || _currentToken._token == TOK_PRIMARY || _currentToken._token == TOK_CHECK)) {
          CSVSQLDB_THROW(SqlParserException, "expected a table element but found '"
                                               << tokenToString(eToken(_currentToken._token)) << "' (" << _currentToken._value
                                               << ") at line " << _currentToken._lineCount << ":" << _currentToken._charCount);
        }
      } else {
        break;
      }
    }

    expect(TOK_RIGHT_PAREN);

    return std::make_shared<ASTCreateTableNode>(SymbolTable::createSymbolTable(), name, columns, constraints, createIfNotExists);
  }

  eType SQLParser::parseType()
  {
    eType type;

    switch (_currentToken._token) {
      case TOK_BOOL:
        expect(TOK_BOOL);
        type = BOOLEAN;
        break;
      case TOK_INT:
        expect(TOK_INT);
        type = INT;
        break;
      case TOK_REAL:
        expect(TOK_REAL);
        type = REAL;
        break;
      case TOK_STRING:
        expect(TOK_STRING);
        if (canExpect(TOK_LEFT_PAREN)) {
          expect(TOK_CONST_INTEGER);
          expect(TOK_RIGHT_PAREN);
        }
        type = STRING;
        break;
      case TOK_CHAR:
        expect(TOK_CHAR);
        type = STRING;
        break;
      case TOK_DATE:
      case TOK_TIME:
      case TOK_TIMESTAMP:
        // TODO LCF: need a type for every category
        expect(TOK_DATE);
        type = DATE;
        break;
      default:
        CSVSQLDB_THROW(SqlParserException, "expected a type but found '"
                                             << tokenToString(eToken(_currentToken._token)) << "' (" << _currentToken._value
                                             << ") at line " << _currentToken._lineCount << ":" << _currentToken._charCount);
    }

    return type;
  }

  ColumnDefinition SQLParser::parseColumnDefinition()
  {
    ColumnDefinition definition(_currentToken._value);
    expect(TOK_IDENTIFIER);

    switch (_currentToken._token) {
      case TOK_BOOL:
        expect(TOK_BOOL);
        definition._type = BOOLEAN;
        break;
      case TOK_INT:
        expect(TOK_INT);
        definition._type = INT;
        break;
      case TOK_REAL:
        expect(TOK_REAL);
        definition._type = REAL;
        break;
      case TOK_STRING:
        expect(TOK_STRING);
        expect(TOK_LEFT_PAREN);
        definition._length = static_cast<uint32_t>(std::stol(expect(TOK_CONST_INTEGER)));
        expect(TOK_RIGHT_PAREN);
        definition._type = STRING;
        break;
      case TOK_CHAR:
        expect(TOK_CHAR);
        if (canExpect(TOK_LEFT_PAREN)) {
          definition._length = static_cast<uint32_t>(std::stol(expect(TOK_CONST_INTEGER)));
          expect(TOK_RIGHT_PAREN);
        } else if (canExpect(TOK_VARYING)) {
          expect(TOK_LEFT_PAREN);
          definition._length = static_cast<uint32_t>(std::stol(expect(TOK_CONST_INTEGER)));
          expect(TOK_RIGHT_PAREN);
        }
        definition._type = STRING;
        break;
      case TOK_DATE:
        expect(TOK_DATE);
        definition._type = DATE;
        break;
      case TOK_TIME:
        expect(TOK_TIME);
        definition._type = TIME;
        break;
      case TOK_TIMESTAMP:
        expect(TOK_TIMESTAMP);
        definition._type = TIMESTAMP;
        break;
      default:
        CSVSQLDB_THROW(SqlParserException, "expected a type but found '"
                                             << tokenToString(eToken(_currentToken._token)) << "' (" << _currentToken._value
                                             << ") at line " << _currentToken._lineCount << ":" << _currentToken._charCount);
    }
    if (_currentToken._token == TOK_DEFAULT) {
      expect(TOK_DEFAULT);

      if (_currentToken._token != TOK_CONST_STRING && _currentToken._token != TOK_CONST_INTEGER &&
          _currentToken._token != TOK_CONST_BOOLEAN && _currentToken._token != TOK_CONST_DATE &&
          _currentToken._token != TOK_CONST_REAL && _currentToken._token != TOK_CONST_CHAR) {
        CSVSQLDB_THROW(SqlParserException, "expected a constant but found '"
                                             << tokenToString(eToken(_currentToken._token)) << "' (" << _currentToken._value
                                             << ") at line " << _currentToken._lineCount << ":" << _currentToken._charCount);
      }

      definition._defaultValue = _currentToken._value;
      parseNext();
    }
    if (canExpect(TOK_CONSTRAINT)) {
      definition._name = expect(TOK_IDENTIFIER);
      parseConstraint(definition);
    } else {
      parseConstraint(definition);
    }

    return definition;
  }

  csvsqldb::StringVector SQLParser::parseColumnList()
  {
    csvsqldb::StringVector columns;

    do {
      columns.push_back(expect(TOK_IDENTIFIER));
    } while (canExpect(TOK_COMMA));

    return columns;
  }

  TableConstraint SQLParser::parseTableConstraint()
  {
    TableConstraint constraint;

    if (canExpect(TOK_CONSTRAINT)) {
      constraint._name = expect(TOK_IDENTIFIER);
    }

    if (canExpect(TOK_PRIMARY)) {
      expect(TOK_KEY);
      expect(TOK_LEFT_PAREN);
      csvsqldb::StringVector keys = parseColumnList();
      expect(TOK_RIGHT_PAREN);
      constraint._primaryKeys = keys;
    } else if (canExpect(TOK_UNIQUE)) {
      expect(TOK_LEFT_PAREN);
      csvsqldb::StringVector keys = parseColumnList();
      expect(TOK_RIGHT_PAREN);
      constraint._uniqueKeys = keys;
    }
    if (canExpect(TOK_CHECK)) {
      expect(TOK_LEFT_PAREN);
      constraint._check = parseExpression(SymbolTable::createSymbolTable());
      expect(TOK_RIGHT_PAREN);
    }

    return constraint;
  }

  void SQLParser::parseConstraint(ColumnDefinition& definition)
  {
    if (canExpect(TOK_PRIMARY)) {
      expect(TOK_KEY);
      definition._primaryKey = true;
    } else if (canExpect(TOK_NOT)) {
      expect(TOK_NULL);
      definition._notNull = true;
    } else if (canExpect(TOK_UNIQUE)) {
      definition._unique = true;
    }
    if (canExpect(TOK_CHECK)) {
      expect(TOK_LEFT_PAREN);
      definition._check = parseExpression(SymbolTable::createSymbolTable());
      expect(TOK_RIGHT_PAREN);
    }
  }

  ASTAlterTableNodePtr SQLParser::parseAlterTable()
  {
    ASTAlterTableNodePtr alterNode;

    expect(TOK_ALTER);
    expect(TOK_TABLE);
    expect(TOK_IDENTIFIER);
    if (canExpect(TOK_ADD_KEYWORD)) {
      canExpect(TOK_COLUMN);
      alterNode = std::make_shared<ASTAlterTableAddColumnNode>(SymbolTable::createSymbolTable(), parseColumnDefinition());
    } else if (canExpect(TOK_DROP)) {
      canExpect(TOK_COLUMN);
      alterNode = std::make_shared<ASTAlterTableDropColumnNode>(SymbolTable::createSymbolTable(), expect(TOK_IDENTIFIER));
    } else {
      CSVSQLDB_THROW(SqlParserException, "expected add or drop but found '"
                                           << tokenToString(eToken(_currentToken._token)) << "' (" << _currentToken._value
                                           << ") at line " << _currentToken._lineCount << ":" << _currentToken._charCount);
    }

    return alterNode;
  }

  ASTDropTableNodePtr SQLParser::parseDropTable()
  {
    ASTDropTableNodePtr node;
    expect(TOK_TABLE);

    return std::make_shared<ASTDropTableNode>(SymbolTable::createSymbolTable(), expect(TOK_IDENTIFIER));
  }

  ASTQueryNodePtr SQLParser::parseQuery()
  {
    ASTQueryExpressionNodePtr query = parseQueryExpression(SymbolTablePtr());

    return std::make_shared<ASTQueryNode>(query->symbolTable(), query);
  }

  ASTQueryExpressionNodePtr SQLParser::parseQueryExpression(const SymbolTablePtr& symboltable)
  {
    SymbolTablePtr nestedSymbolable = symboltable;

    if (!symboltable) {
      nestedSymbolable = SymbolTable::createSymbolTable();
    } else {
      nestedSymbolable = SymbolTable::createSymbolTable(symboltable);
    }

    bool expectRightParen = false;
    if (canExpect(TOK_LEFT_PAREN)) {
      expectRightParen = true;
    }
    expect(TOK_SELECT);

    eQuantifier quantifier = ALL;
    if (canExpect(TOK_DISTINCT)) {
      quantifier = DISTINCT;
    } else if (canExpect(TOK_ALL)) {
      quantifier = ALL;
    }

    Expressions nodes = parseSelectList(nestedSymbolable);
    ASTTableExpressionNodePtr tableExpression = parseTableExpression(nestedSymbolable);

    if (expectRightParen) {
      expect(TOK_RIGHT_PAREN);
    }

    ASTQueryExpressionNodePtr query =
      std::make_shared<ASTQuerySpecificationNode>(nestedSymbolable, quantifier, nodes, tableExpression);

    while (_currentToken._token == TOK_UNION) {
      expect(TOK_UNION);
      quantifier = ALL;
      if (canExpect(TOK_DISTINCT)) {
        quantifier = DISTINCT;
      } else if (canExpect(TOK_ALL)) {
        quantifier = ALL;
      }
      expect(TOK_LEFT_PAREN);
      ASTQueryExpressionNodePtr rhs = parseQueryExpression(nestedSymbolable);
      expect(TOK_RIGHT_PAREN);
      query = std::make_shared<ASTUnionNode>(nestedSymbolable, quantifier, query, rhs);
    }

    return query;
  }

  ASTTableExpressionNodePtr SQLParser::parseTableExpression(const SymbolTablePtr& symboltable)
  {
    ASTFromNodePtr from = parseFrom(symboltable);
    ASTWhereNodePtr where = parseWhere(symboltable);
    ASTGroupByNodePtr group = parseGroupBy(symboltable);
    ASTHavingNodePtr having = parseHaving(symboltable);
    ASTOrderByNodePtr order = parseOrderBy(symboltable);
    ASTLimitNodePtr limit = parseLimit(symboltable);

    return std::make_shared<ASTTableExpressionNode>(symboltable, from, where, group, having, order, limit);
  }

  ASTFromNodePtr SQLParser::parseFrom(const SymbolTablePtr& symboltable)
  {
    TableReferences tables;

    expect(TOK_FROM);

    do {
      ASTTableReferenceNodePtr reference = parseTableReference(symboltable);
      tables.push_back(reference);
    } while (canExpect(TOK_COMMA));

    return std::make_shared<ASTFromNode>(symboltable, tables);
  }

  bool SQLParser::isJoin()
  {
    switch (_currentToken._token) {
      case TOK_INNER:
      case TOK_LEFT:
      case TOK_RIGHT:
      case TOK_FULL:
      case TOK_NATURAL:
      case TOK_JOIN:
      case TOK_CROSS:
        return true;
    }
    return false;
  }

  ASTTableReferenceNodePtr SQLParser::parseTableReference(const SymbolTablePtr& symboltable)
  {
    ASTTableReferenceNodePtr reference = parseTableFactor(symboltable);

    if (isJoin()) {
      while (isJoin()) {
        eJoinType type = parseJoinType();
        reference = parseJoinClause(reference, type);
      }
    }

    return reference;
  }

  ASTTableFactorNodePtr SQLParser::parseTableFactor(const SymbolTablePtr& symboltable)
  {
    ASTTableFactorNodePtr node;

    if (canExpect(TOK_LEFT_PAREN)) {
      ASTQueryExpressionNodePtr query = parseQueryExpression(symboltable);
      expect(TOK_RIGHT_PAREN);
      std::string subqueryAlias;
      if (canExpect(TOK_AS)) {
        subqueryAlias = expect(TOK_IDENTIFIER);
      }

      SymbolTablePtr parent = query->symbolTable()->getParent();
      SymbolInfoPtr info = std::make_shared<SymbolInfo>();
      info->_name = subqueryAlias;
      info->_symbolType = SUBQUERY;
      info->_subquery = query->symbolTable();
      parent->addSymbol(subqueryAlias, info);
      node = std::make_shared<ASTTableSubqueryNode>(symboltable, query, subqueryAlias);
    } else {
      ASTIdentifierPtr identifier = parseAliasedIdentifier(symboltable);
      node = std::make_shared<ASTTableIdentifierNode>(symboltable, identifier);
    }

    return node;
  }

  ASTIdentifierPtr SQLParser::parseAliasedIdentifier(const SymbolTablePtr& symboltable)
  {
    bool quoted = false;
    std::string identifier = parseQuotedIdentifier(quoted);
    SymbolInfoPtr info = std::make_shared<SymbolInfo>();
    info->_identifier = identifier;
    info->_symbolType = TABLE;
    info->_type = NONE;

    if (_currentToken._token == TOK_AS || _currentToken._token == TOK_IDENTIFIER ||
        _currentToken._token == TOK_QUOTED_IDENTIFIER) {
      canExpect(TOK_AS);
      std::string alias = parseQuotedIdentifier(quoted);
      info->_alias = alias;
      identifier = alias;
      quoted = _currentToken._token == TOK_QUOTED_IDENTIFIER;
    }
    info->_name = identifier;
    symboltable->addSymbol(identifier, info);

    return std::make_shared<ASTIdentifier>(symboltable, symboltable->findSymbol(identifier), "", info->_identifier, quoted);
  }

  eJoinType SQLParser::parseJoinType()
  {
    if (canExpect(TOK_CROSS)) {
      expect(TOK_JOIN);
      return CROSS_JOIN;
    } else if (canExpect(TOK_INNER)) {
      expect(TOK_JOIN);
      return INNER_JOIN;
    } else if (canExpect(TOK_NATURAL)) {
      if (canExpect(TOK_LEFT)) {
        canExpect(TOK_OUTER);
        expect(TOK_JOIN);
        return NATURAL_LEFT_JOIN;
      } else if (canExpect(TOK_RIGHT)) {
        canExpect(TOK_OUTER);
        expect(TOK_JOIN);
        return NATURAL_RIGHT_JOIN;
      } else if (canExpect(TOK_FULL)) {
        canExpect(TOK_OUTER);
        expect(TOK_JOIN);
        return NATURAL_FULL_JOIN;
      }
      canExpect(TOK_INNER);
      expect(TOK_JOIN);
      return NATURAL_JOIN;
    } else if (canExpect(TOK_LEFT)) {
      canExpect(TOK_OUTER);
      expect(TOK_JOIN);
      return LEFT_JOIN;
    } else if (canExpect(TOK_RIGHT)) {
      canExpect(TOK_OUTER);
      expect(TOK_JOIN);
      return RIGHT_JOIN;
    } else if (canExpect(TOK_FULL)) {
      canExpect(TOK_OUTER);
      expect(TOK_JOIN);
      return FULL_JOIN;
    }

    expect(TOK_JOIN);
    return INNER_JOIN;
  }

  ASTJoinNodePtr SQLParser::parseJoinClause(const ASTTableReferenceNodePtr& reference, eJoinType joinType)
  {
    ASTJoinNodePtr join;

    switch (joinType) {
      case INNER_JOIN: {
        ASTTableFactorNodePtr factor = parseTableFactor(reference->symbolTable());
        expect(TOK_ON);
        ASTExprNodePtr exp = parseExpression(factor->symbolTable());
        join = std::make_shared<ASTInnerJoinNode>(factor->symbolTable(), reference, factor, exp);
        break;
      }
      case CROSS_JOIN: {
        ASTTableFactorNodePtr factor = parseTableFactor(reference->symbolTable());
        join = std::make_shared<ASTCrossJoinNode>(factor->symbolTable(), reference, factor);
        break;
      }
      case NATURAL_JOIN: {
        ASTTableFactorNodePtr factor = parseTableFactor(reference->symbolTable());
        join = std::make_shared<ASTNaturalJoinNode>(factor->symbolTable(), NATURAL, reference, factor);
        break;
      }
      case NATURAL_LEFT_JOIN: {
        ASTTableFactorNodePtr factor = parseTableFactor(reference->symbolTable());
        join = std::make_shared<ASTNaturalJoinNode>(factor->symbolTable(), LEFT, reference, factor);
        break;
      }
      case NATURAL_RIGHT_JOIN: {
        ASTTableFactorNodePtr factor = parseTableFactor(reference->symbolTable());
        join = std::make_shared<ASTNaturalJoinNode>(factor->symbolTable(), RIGHT, reference, factor);
        break;
      }
      case NATURAL_FULL_JOIN: {
        ASTTableFactorNodePtr factor = parseTableFactor(reference->symbolTable());
        join = std::make_shared<ASTNaturalJoinNode>(factor->symbolTable(), FULL, reference, factor);
        break;
      }
      case LEFT_JOIN: {
        ASTTableFactorNodePtr factor = parseTableFactor(reference->symbolTable());
        expect(TOK_ON);
        ASTExprNodePtr exp = parseExpression(factor->symbolTable());
        join = std::make_shared<ASTLeftJoinNode>(factor->symbolTable(), reference, factor, exp);
        break;
      }
      case RIGHT_JOIN: {
        ASTTableFactorNodePtr factor = parseTableFactor(reference->symbolTable());
        expect(TOK_ON);
        ASTExprNodePtr exp = parseExpression(factor->symbolTable());
        join = std::make_shared<ASTRightJoinNode>(factor->symbolTable(), reference, factor, exp);
        break;
      }
      case FULL_JOIN: {
        ASTTableFactorNodePtr factor = parseTableFactor(reference->symbolTable());
        expect(TOK_ON);
        ASTExprNodePtr exp = parseExpression(factor->symbolTable());
        join = std::make_shared<ASTFullJoinNode>(factor->symbolTable(), reference, factor, exp);
        break;
      }
    }

    return join;
  }

  ASTWhereNodePtr SQLParser::parseWhere(const SymbolTablePtr& symboltable)
  {
    ASTWhereNodePtr where;

    if (canExpect(TOK_WHERE)) {
      ASTExprNodePtr exp = parseExpression(symboltable);
      where = std::make_shared<ASTWhereNode>(symboltable, exp);
    }

    return where;
  }

  ASTGroupByNodePtr SQLParser::parseGroupBy(const SymbolTablePtr& symboltable)
  {
    ASTGroupByNodePtr group;
    if (canExpect(TOK_GROUP)) {
      expect(TOK_BY);
      eQuantifier quantifier = ALL;
      if (canExpect(TOK_DISTINCT)) {
        quantifier = DISTINCT;
      } else if (canExpect(TOK_ALL)) {
        quantifier = ALL;
      }
      Identifiers identifiers = parseIdentifierList(symboltable);
      group = std::make_shared<ASTGroupByNode>(symboltable, quantifier, identifiers);
    }

    return group;
  }

  ASTHavingNodePtr SQLParser::parseHaving(const SymbolTablePtr& symboltable)
  {
    ASTHavingNodePtr having;
    if (canExpect(TOK_HAVING)) {
      ASTExprNodePtr exp = parseExpression(symboltable);
      having = std::make_shared<ASTHavingNode>(symboltable, exp);
    }

    return having;
  }

  ASTOrderByNodePtr SQLParser::parseOrderBy(const SymbolTablePtr& symboltable)
  {
    ASTOrderByNodePtr order;

    if (canExpect(TOK_ORDER)) {
      expect(TOK_BY);

      OrderExpressions orderExpressions;
      OrderExpression orderExp;

      do {
        orderExp.first = parseExpression(symboltable);
        orderExp.second = ASC;

        if (canExpect(TOK_ASC)) {
          orderExp.second = ASC;
        } else if (canExpect(TOK_DESC)) {
          orderExp.second = DESC;
        }
        orderExpressions.push_back(orderExp);
      } while (canExpect(TOK_COMMA));

      order = std::make_shared<ASTOrderByNode>(symboltable, orderExpressions);
    }

    return order;
  }

  ASTLimitNodePtr SQLParser::parseLimit(const SymbolTablePtr& symboltable)
  {
    ASTLimitNodePtr limit;

    if (canExpect(TOK_LIMIT)) {
      ASTExprNodePtr expStart = parseExpression(symboltable);
      ASTExprNodePtr expEnd;
      if (canExpect(TOK_OFFSET)) {
        expEnd = parseExpression(symboltable);
      }
      limit = std::make_shared<ASTLimitNode>(symboltable, expStart, expEnd);
    }

    return limit;
  }

  ASTExprNodePtr SQLParser::parseExpression(const SymbolTablePtr& symboltable)
  {
    ASTExprNodePtr lhs = parseJoin(symboltable);

    while (_currentToken._token == TOK_OR) {
      eOperationType op;
      switch (_currentToken._token) {
        case TOK_OR:
          expect(TOK_OR);
          op = OP_OR;
          break;
        default:
          CSVSQLDB_THROW(SqlParserException, "wrong operator for or operation");
      }
      ASTExprNodePtr rhs = parseJoin(symboltable);
      lhs = std::make_shared<ASTBinaryNode>(symboltable, op, lhs, rhs);
    }

    return lhs;
  }

  ASTExprNodePtr SQLParser::parseJoin(const SymbolTablePtr& symboltable)
  {
    ASTExprNodePtr lhs = parseEquality(symboltable);
    while (_currentToken._token == TOK_AND) {
      eOperationType op;
      switch (_currentToken._token) {
        case TOK_AND:
          expect(TOK_AND);
          op = OP_AND;
          break;
        default:
          CSVSQLDB_THROW(SqlParserException, "wrong operator for and operation");
      }
      ASTExprNodePtr rhs = parseEquality(symboltable);
      lhs = std::make_shared<ASTBinaryNode>(symboltable, op, lhs, rhs);
    }

    return lhs;
  }

  ASTExprNodePtr SQLParser::parseEquality(const SymbolTablePtr& symboltable)
  {
    ASTExprNodePtr lhs = parseRelation(symboltable);
    while (_currentToken._token == TOK_EQUAL || _currentToken._token == TOK_NOTEQUAL || _currentToken._token == TOK_LIKE ||
           _currentToken._token == TOK_BETWEEN || _currentToken._token == TOK_IN || _currentToken._token == TOK_IS) {
      eOperationType op;
      switch (_currentToken._token) {
        case TOK_EQUAL:
          expect(TOK_EQUAL);
          op = OP_EQ;
          break;
        case TOK_NOTEQUAL:
          expect(TOK_NOTEQUAL);
          op = OP_NEQ;
          break;
        case TOK_LIKE:
          expect(TOK_LIKE);
          op = OP_LIKE;
          break;
        case TOK_BETWEEN:
          expect(TOK_BETWEEN);
          op = OP_BETWEEN;
          break;
        case TOK_IN:
          expect(TOK_IN);
          op = OP_IN;
          break;
        case TOK_IS:
          op = OP_IS;
          break;
        default:
          CSVSQLDB_THROW(SqlParserException, "wrong operator for equal operation");
      }
      if (op == OP_LIKE) {
        std::string value = expect(TOK_CONST_STRING);
        std::stringstream regexp;
        for (const auto& c : value) {
          switch (c) {
            case '%':
              regexp << ".*";
              break;
            case '_':
              regexp << ".";
              break;
            case '.':
            case '*':
            case '?':
            case '(':
            case ')':
              regexp << "\\" << c;
              break;
            default:
              regexp << c;
          }
        }
        lhs = std::make_shared<ASTLikeNode>(symboltable, lhs, regexp.str());
      } else if (op == OP_BETWEEN) {
        ASTExprNodePtr from = parseFactor(symboltable);
        expect(TOK_AND);
        ASTExprNodePtr to = parseFactor(symboltable);
        lhs = std::make_shared<ASTBetweenNode>(symboltable, lhs, from, to);
      } else if (op == OP_IN) {
        Expressions expressions;
        expect(TOK_LEFT_PAREN);
        while (_currentToken._token != TOK_RIGHT_PAREN) {
          expressions.push_back(parseFactor(symboltable));
          if (canExpect(TOK_COMMA)) {
            if (_currentToken._token == TOK_RIGHT_PAREN) {
              CSVSQLDB_THROW(SqlParserException, "expected an expression");
            }
          }
        }
        expect(TOK_RIGHT_PAREN);
        lhs = std::make_shared<ASTInNode>(symboltable, lhs, expressions);
      } else if (canExpect(TOK_IS)) {
        if (canExpect(TOK_NOT)) {
          op = OP_ISNOT;
        }
        ASTExprNodePtr rhs;
        if (_currentToken._token == TOK_CONST_BOOLEAN) {
          rhs = std::make_shared<ASTValueNode>(symboltable, BOOLEAN, expect(TOK_CONST_BOOLEAN));
        } else if (canExpect(TOK_NULL)) {
          rhs = std::make_shared<ASTValueNode>(symboltable, BOOLEAN, "UNKNOWN");
        } else {
          CSVSQLDB_THROW(SqlParserException, "expected a BOOLEAN value or NULL");
        }
        lhs = std::make_shared<ASTBinaryNode>(symboltable, op, lhs, rhs);
      } else {
        ASTExprNodePtr rhs = parseRelation(symboltable);
        lhs = std::make_shared<ASTBinaryNode>(symboltable, op, lhs, rhs);
      }
    }

    return lhs;
  }

  ASTExprNodePtr SQLParser::parseRelation(const SymbolTablePtr& symboltable)
  {
    ASTExprNodePtr lhs = parseAdd(symboltable);
    while (_currentToken._token == TOK_GREATER || _currentToken._token == TOK_GREATEREQUAL ||
           _currentToken._token == TOK_SMALLER || _currentToken._token == TOK_SMALLEREQUAL) {
      eOperationType op;
      switch (_currentToken._token) {
        case TOK_GREATER:
          expect(TOK_GREATER);
          op = OP_GT;
          break;
        case TOK_GREATEREQUAL:
          expect(TOK_GREATEREQUAL);
          op = OP_GE;
          break;
        case TOK_SMALLER:
          expect(TOK_SMALLER);
          op = OP_LT;
          break;
        case TOK_SMALLEREQUAL:
          expect(TOK_SMALLEREQUAL);
          op = OP_LE;
          break;
        default:
          CSVSQLDB_THROW(SqlParserException, "wrong operator for relation operation");
      }
      ASTExprNodePtr rhs = parseAdd(symboltable);
      lhs = std::make_shared<ASTBinaryNode>(symboltable, op, lhs, rhs);
    }

    return lhs;
  }

  ASTExprNodePtr SQLParser::parseAdd(const SymbolTablePtr& symboltable)
  {
    ASTExprNodePtr lhs = parseMul(symboltable);
    while (_currentToken._token == TOK_ADD || _currentToken._token == TOK_SUB || _currentToken._token == TOK_CONCAT) {
      eOperationType op;
      switch (_currentToken._token) {
        case TOK_ADD:
          expect(TOK_ADD);
          op = OP_ADD;
          break;
        case TOK_SUB:
          expect(TOK_SUB);
          op = OP_SUB;
          break;
        case TOK_CONCAT:
          expect(TOK_CONCAT);
          op = OP_CONCAT;
          break;
        default:
          CSVSQLDB_THROW(SqlParserException, "wrong operator for add operation");
      }
      ASTExprNodePtr rhs = parseMul(symboltable);
      lhs = std::make_shared<ASTBinaryNode>(symboltable, op, lhs, rhs);
    }

    return lhs;
  }

  ASTExprNodePtr SQLParser::parseMul(const SymbolTablePtr& symboltable)
  {
    ASTExprNodePtr lhs = parseFactor(symboltable);
    while (_currentToken._token == TOK_ASTERISK || _currentToken._token == TOK_DIV || _currentToken._token == TOK_MOD) {
      eOperationType op;
      switch (_currentToken._token) {
        case TOK_ASTERISK:
          expect(TOK_ASTERISK);
          op = OP_MUL;
          break;
        case TOK_DIV:
          expect(TOK_DIV);
          op = OP_DIV;
          break;
        case TOK_MOD:
          expect(TOK_MOD);
          op = OP_MOD;
          break;
        default:
          CSVSQLDB_THROW(SqlParserException, "wrong operator for mul operation");
      }
      ASTExprNodePtr rhs = parseFactor(symboltable);
      lhs = std::make_shared<ASTBinaryNode>(symboltable, op, lhs, rhs);
    }

    return lhs;
  }

  ASTExprNodePtr SQLParser::parseFactor(const SymbolTablePtr& symboltable)
  {
    ASTExprNodePtr node;
    if (canExpect(TOK_LEFT_PAREN)) {
      node = parseExpression(symboltable);
      expect(TOK_RIGHT_PAREN);
    } else if (_currentToken._token == TOK_IDENTIFIER || _currentToken._token == TOK_QUOTED_IDENTIFIER) {
      bool isFunction = false;
      if (_currentToken._token == TOK_IDENTIFIER || _currentToken._token == TOK_QUOTED_IDENTIFIER) {
        isFunction = !!_functionRegistry.getFunction(_currentToken._value);
      }
      if (!isFunction) {
        node = parseQualifiedIdentifierOrAsterisk(symboltable);
        if (std::dynamic_pointer_cast<ASTIdentifier>(node)) {
          ASTIdentifierPtr identifier = std::dynamic_pointer_cast<ASTIdentifier>(node);
          std::string symbolName = identifier->getQualifiedIdentifier();
          if (!symboltable->hasSymbol(symbolName)) {
            SymbolInfoPtr info = std::make_shared<SymbolInfo>();
            info->_name = symbolName;
            info->_symbolType = PLAIN;
            info->_type = NONE;
            info->_prefix = identifier->_prefix;
            info->_identifier = identifier->_identifier;
            symboltable->addSymbol(symbolName, info);
          }
          identifier->_info = symboltable->findSymbol(symbolName);
        }
      } else {
        std::string funcName = _currentToken._value;
        expect(TOK_IDENTIFIER);
        Parameters parameters = parseParameterList(symboltable);
        SymbolInfoPtr info = std::make_shared<SymbolInfo>();
        info->_name = funcName;
        info->_symbolType = FUNCTION;
        info->_type = NONE;
        symboltable->addSymbol(funcName, info);
        node = std::make_shared<ASTFunctionNode>(symboltable, _functionRegistry, funcName, parameters);
      }
    } else if (_currentToken._token == TOK_SUM || _currentToken._token == TOK_COUNT || _currentToken._token == TOK_AVG ||
               _currentToken._token == TOK_MIN || _currentToken._token == TOK_MAX || _currentToken._token == TOK_ARBITRARY) {
      eAggregateFunction aggregateFunction;
      switch (_currentToken._token) {
        case TOK_SUM:
          expect(TOK_SUM);
          aggregateFunction = SUM;
          break;
        case TOK_COUNT:
          expect(TOK_COUNT);
          aggregateFunction = COUNT;
          break;
        case TOK_AVG:
          expect(TOK_AVG);
          aggregateFunction = AVG;
          break;
        case TOK_MAX:
          expect(TOK_MAX);
          aggregateFunction = MAX;
          break;
        case TOK_MIN:
          expect(TOK_MIN);
          aggregateFunction = MIN;
          break;
        case TOK_ARBITRARY:
          expect(TOK_ARBITRARY);
          aggregateFunction = ARBITRARY;
          break;
        default:
          CSVSQLDB_THROW(SqlParserException, "unknown aggregate function '" << _currentToken._value << "'");
      }

      expect(TOK_LEFT_PAREN);
      Parameters parameters;
      if (aggregateFunction == COUNT) {
        if (canExpect(TOK_ASTERISK)) {
          aggregateFunction = COUNT_STAR;
        }
      }
      eQuantifier quantifier = ALL;
      if (aggregateFunction != COUNT_STAR) {
        Parameter param;
        if (canExpect(TOK_DISTINCT)) {
          quantifier = DISTINCT;
        } else if (canExpect(TOK_ALL)) {
          quantifier = ALL;
        }
        param._exp = parseExpression(symboltable);
        parameters.push_back(param);
      }
      expect(TOK_RIGHT_PAREN);

      node = std::make_shared<ASTAggregateFunctionNode>(symboltable, aggregateFunction, quantifier, parameters);
    } else if (_currentToken._token == TOK_CONST_STRING || _currentToken._token == TOK_CONST_CHAR ||
               _currentToken._token == TOK_CONST_BOOLEAN || _currentToken._token == TOK_CONST_DATE ||
               _currentToken._token == TOK_CONST_INTEGER || _currentToken._token == TOK_CONST_REAL ||
               _currentToken._token == TOK_NULL) {
      eType type = NONE;
      std::string value = _currentToken._value;
      switch (_currentToken._token) {
        case TOK_CONST_STRING:
          expect(TOK_CONST_STRING);
          type = STRING;
          break;
        case TOK_CONST_INTEGER:
          expect(TOK_CONST_INTEGER);
          type = INT;
          break;
        case TOK_CONST_BOOLEAN:
          expect(TOK_CONST_BOOLEAN);
          type = BOOLEAN;
          break;
        case TOK_CONST_DATE:
          expect(TOK_CONST_DATE);
          type = DATE;
          break;
        case TOK_CONST_REAL:
          expect(TOK_CONST_REAL);
          type = REAL;
          break;
        case TOK_CONST_CHAR:
          expect(TOK_CONST_CHAR);
          type = STRING;
          break;
        case TOK_NULL:
          expect(TOK_NULL);
          type = NONE;
          break;
        default:
          CSVSQLDB_THROW(SqlParserException, "expected a constant");
      }
      node = std::make_shared<ASTValueNode>(symboltable, type, value);
    } else if (_currentToken._token == TOK_NOT || _currentToken._token == TOK_SUB || _currentToken._token == TOK_ADD ||
               _currentToken._token == TOK_CAST) {
      node = parseUnary(symboltable);
    } else if (_currentToken._token == TOK_DATE || _currentToken._token == TOK_TIME || _currentToken._token == TOK_TIMESTAMP) {
      eType type;
      switch (_currentToken._token) {
        case TOK_DATE:
          expect(TOK_DATE);
          type = DATE;
          break;
        case TOK_TIME:
          expect(TOK_TIME);
          type = TIME;
          break;
        case TOK_TIMESTAMP:
          expect(TOK_TIMESTAMP);
          type = TIMESTAMP;
          break;
        default:
          CSVSQLDB_THROW(SqlParserException, "expected DATE/TIME/TIMESTAMP");
      }
      node = std::make_shared<ASTValueNode>(symboltable, type, expect(TOK_CONST_STRING));
    } else if (_currentToken._token == TOK_CURRENT_DATE || _currentToken._token == TOK_CURRENT_TIME ||
               _currentToken._token == TOK_CURRENT_TIMESTAMP || _currentToken._token == TOK_EXTRACT) {
      std::string funcName;
      Parameters parameters;
      switch (_currentToken._token) {
        case TOK_CURRENT_DATE:
          expect(TOK_CURRENT_DATE);
          funcName = "CURRENT_DATE";
          if (canExpect(TOK_LEFT_PAREN)) {
            expect(TOK_RIGHT_PAREN);
          }
          break;
        case TOK_CURRENT_TIME:
          expect(TOK_CURRENT_TIME);
          funcName = "CURRENT_TIME";
          if (canExpect(TOK_LEFT_PAREN)) {
            expect(TOK_RIGHT_PAREN);
          }
          break;
        case TOK_CURRENT_TIMESTAMP:
          expect(TOK_CURRENT_TIMESTAMP);
          funcName = "CURRENT_TIMESTAMP";
          if (canExpect(TOK_LEFT_PAREN)) {
            expect(TOK_RIGHT_PAREN);
          }
          break;
        case TOK_EXTRACT: {
          expect(TOK_EXTRACT);
          expect(TOK_LEFT_PAREN);
          Parameter param;
          switch (_currentToken._token) {
            case TOK_SECOND:
              param._exp = std::make_shared<ASTValueNode>(symboltable, INT, "1");
              parameters.push_back(param);
              break;
            case TOK_MINUTE:
              param._exp = std::make_shared<ASTValueNode>(symboltable, INT, "2");
              parameters.push_back(param);
              break;
            case TOK_HOUR:
              param._exp = std::make_shared<ASTValueNode>(symboltable, INT, "3");
              parameters.push_back(param);
              break;
            case TOK_DAY:
              param._exp = std::make_shared<ASTValueNode>(symboltable, INT, "4");
              parameters.push_back(param);
              break;
            case TOK_MONTH:
              param._exp = std::make_shared<ASTValueNode>(symboltable, INT, "5");
              parameters.push_back(param);
              break;
            case TOK_YEAR:
              param._exp = std::make_shared<ASTValueNode>(symboltable, INT, "6");
              parameters.push_back(param);
              break;
            default:
              CSVSQLDB_THROW(SqlParserException, "expected SECOND/MINUTE/HOUR/DAY/MONTH/YEAR");
          }
          parseNext();
          expect(TOK_FROM);
          param._exp = parseExpression(symboltable);
          parameters.push_back(param);
          expect(TOK_RIGHT_PAREN);
          funcName = "EXTRACT";
        }
      }
      node = std::make_shared<ASTFunctionNode>(symboltable, _functionRegistry, funcName, parameters);
    } else {
      CSVSQLDB_THROW(SqlParserException, "expected an expression");
    }

    return node;
  }

  ASTExprNodePtr SQLParser::parseUnary(const SymbolTablePtr& symboltable)
  {
    ASTExprNodePtr node;

    eOperationType op;
    switch (_currentToken._token) {
      case TOK_NOT:
        expect(TOK_NOT);
        op = OP_NOT;
        break;
      case TOK_ADD:
        expect(TOK_ADD);
        op = OP_PLUS;
        break;
      case TOK_SUB:
        expect(TOK_SUB);
        op = OP_MINUS;
        break;
      case TOK_CAST:
        expect(TOK_CAST);
        op = OP_CAST;
        break;
      default:
        CSVSQLDB_THROW(SqlParserException, "wrong operator for unary operation");
    }
    if (op == OP_CAST) {
      expect(TOK_LEFT_PAREN);
      ASTExprNodePtr exp = parseExpression(symboltable);
      expect(TOK_AS);
      eType castType = parseType();
      expect(TOK_RIGHT_PAREN);
      node = std::make_shared<ASTUnaryNode>(symboltable, op, castType, exp);
    } else {
      node = std::make_shared<ASTUnaryNode>(symboltable, op, NONE, parseExpression(symboltable));
    }

    return node;
  }

  std::string SQLParser::parseQuotedIdentifier(bool& quoted)
  {
    std::string prefix;
    std::string value = _currentToken._value;
    if (canExpect(TOK_IDENTIFIER)) {
      quoted = false;
      return value;
    } else if (canExpect(TOK_QUOTED_IDENTIFIER)) {
      quoted = true;
      return csvsqldb::toupper(value);
    }
    // this is just to trigger an execption with a good text
    expect(TOK_IDENTIFIER);
    return "";
  }

  ASTExprNodePtr SQLParser::parseQualifiedIdentifierOrAsterisk(const SymbolTablePtr& symboltable)
  {
    bool quoted = false;
    std::string prefix;
    std::string identifier = parseQuotedIdentifier(quoted);
    if (canExpect(TOK_DOT)) {
      prefix = identifier;
      if (canExpect(TOK_ASTERISK)) {
        return std::make_shared<ASTQualifiedAsterisk>(symboltable, prefix, quoted);
      }
      identifier = parseQuotedIdentifier(quoted);
    }

    return std::make_shared<ASTIdentifier>(symboltable, nullptr, prefix, identifier, quoted);
  }

  ASTIdentifierPtr SQLParser::parseQualifiedIdentifier(const SymbolTablePtr& symboltable)
  {
    bool quoted = false;
    std::string prefix;
    std::string identifier = parseQuotedIdentifier(quoted);
    if (canExpect(TOK_DOT)) {
      prefix = identifier;
      identifier = parseQuotedIdentifier(quoted);
    }

    return std::make_shared<ASTIdentifier>(symboltable, nullptr, prefix, identifier, quoted);
  }

  Parameters SQLParser::parseParameterList(const SymbolTablePtr& symboltable)
  {
    Parameters parameters;
    expect(TOK_LEFT_PAREN);
    while (_currentToken._token != TOK_RIGHT_PAREN) {
      Parameter param;
      param._exp = parseExpression(symboltable);
      canExpect(TOK_COMMA);
      parameters.push_back(param);
    }
    expect(TOK_RIGHT_PAREN);

    return parameters;
  }

  Identifiers SQLParser::parseIdentifierList(const SymbolTablePtr& symboltable)
  {
    Identifiers identifiers;

    do {
      ASTIdentifierPtr identifier = parseQualifiedIdentifier(symboltable);
      std::string symbolName = identifier->getQualifiedIdentifier();
      if (!symboltable->hasSymbol(symbolName)) {
        SymbolInfoPtr info = std::make_shared<SymbolInfo>();
        info->_name = symbolName;
        info->_symbolType = PLAIN;
        info->_type = NONE;
        info->_prefix = identifier->_prefix;
        info->_identifier = identifier->_identifier;
        symboltable->addSymbol(symbolName, info);
      }
      identifier->_info = symboltable->findSymbol(symbolName);
      identifiers.push_back(identifier);
    } while (canExpect(TOK_COMMA));

    return identifiers;
  }

  Expressions SQLParser::parseSelectList(const SymbolTablePtr& symboltable)
  {
    Expressions nodes;
    ASTExprNodePtr exp;
    std::string prefix;
    bool first = true;

    do {
      if (canExpect(TOK_ASTERISK) && first) {
        exp = std::make_shared<ASTQualifiedAsterisk>(symboltable, prefix, false);
      } else {
        exp = parseDerivedColumn(symboltable);
      }
      first = false;

      nodes.push_back(exp);
    } while (canExpect(TOK_COMMA));

    return nodes;
  }

  ASTExprNodePtr SQLParser::parseDerivedColumn(const SymbolTablePtr& symboltable)
  {
    ASTExprNodePtr exp = parseExpression(symboltable);
    std::string symbolName;
    if (!std::dynamic_pointer_cast<ASTIdentifier>(exp)) {
      SymbolInfoPtr info = std::make_shared<SymbolInfo>();
      info->_symbolType = CALC;
      info->_type = NONE;
      info->_expressionNode = exp;
      symbolName = symboltable->getNextAlias();
      info->_name = symbolName;
      symboltable->addSymbol(symbolName, info);
      exp->_symbolName = symbolName;
    } else if (std::dynamic_pointer_cast<ASTIdentifier>(exp)) {
      ASTIdentifierPtr identifier = std::dynamic_pointer_cast<ASTIdentifier>(exp);
      symbolName = identifier->getQualifiedIdentifier();
      identifier->_info = symboltable->findSymbol(symbolName);
      identifier->_symbolName = symbolName;
    } else {
      expect(TOK_IDENTIFIER);
    }
    if (_currentToken._token == TOK_AS || _currentToken._token == TOK_IDENTIFIER) {
      canExpect(TOK_AS);
      std::string value = _currentToken._value;
      if (canExpect(TOK_IDENTIFIER)) {
        // empty
      } else if (canExpect(TOK_QUOTED_IDENTIFIER)) {
        // empty
      } else {
        expect(TOK_IDENTIFIER);
      }
      SymbolInfoPtr info = symboltable->findSymbol(symbolName)->clone();
      info->_name = csvsqldb::toupper(value);
      info->_alias = info->_name;
      if (exp) {
        exp->_symbolName = info->_alias;
      }
      symboltable->replaceSymbol(symbolName, info->_alias, info);
    }

    return exp;
  }

  Expressions SQLParser::parseExprList(const SymbolTablePtr& symboltable)
  {
    Expressions nodes;

    do {
      ASTExprNodePtr exp = parseExpression(symboltable);
      nodes.push_back(exp);
    } while (canExpect(TOK_COMMA));

    return nodes;
  }
}
