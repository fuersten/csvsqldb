//
//  sql_ast.h
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

#pragma once

#include <csvsqldb/inc.h>

#include <csvsqldb/function_registry.h>
#include <csvsqldb/symboltable.h>
#include <csvsqldb/typeoperations.h>

#include <memory>
#include <vector>


namespace csvsqldb
{
  class ASTNode;
  class ASTExplainNode;
  class ASTCreateTableNode;
  class ASTMappingNode;
  class ASTDropMappingNode;
  class ASTAlterTableNode;
  class ASTAlterTableAddColumnNode;
  class ASTAlterTableDropColumnNode;
  class ASTDropTableNode;
  class ASTQueryNode;
  class ASTQueryExpressionNode;
  class ASTUnionNode;
  class ASTQuerySpecificationNode;
  class ASTTableExpressionNode;
  class ASTTableReferenceNode;
  class ASTTableFactorNode;
  class ASTTableIdentifierNode;
  class ASTTableSubqueryNode;
  class ASTJoinNode;
  class ASTCrossJoinNode;
  class ASTNaturalJoinNode;
  class ASTJoinWithCondition;
  class ASTInnerJoinNode;
  class ASTLeftJoinNode;
  class ASTRightJoinNode;
  class ASTFullJoinNode;
  class ASTExprNode;
  class ASTBinaryNode;
  class ASTUnaryNode;
  class ASTValueNode;
  class ASTLikeNode;
  class ASTBetweenNode;
  class ASTInNode;
  class ASTFunctionNode;
  class ASTAggregateFunctionNode;
  class ASTIdentifier;
  class ASTQualifiedAsterisk;
  class ASTFromNode;
  class ASTWhereNode;
  class ASTGroupByNode;
  class ASTHavingNode;
  class ASTOrderByNode;
  class ASTLimitNode;

  using ASTNodePtr = std::shared_ptr<ASTNode>;
  using ASTDescribeNodePtr = std::shared_ptr<ASTExplainNode>;
  using ASTCreateTableNodePtr = std::shared_ptr<ASTCreateTableNode>;
  using ASTMappingNodePtr = std::shared_ptr<ASTMappingNode>;
  using ASTDropMappingNodePtr = std::shared_ptr<ASTDropMappingNode>;
  using ASTAlterTableNodePtr = std::shared_ptr<ASTAlterTableNode>;
  using ASTDropTableNodePtr = std::shared_ptr<ASTDropTableNode>;
  using ASTQueryNodePtr = std::shared_ptr<ASTQueryNode>;
  using ASTQueryExpressionNodePtr = std::shared_ptr<ASTQueryExpressionNode>;
  using ASTUnionNodePtr = std::shared_ptr<ASTUnionNode>;
  using ASTQuerySpecificationNodePtr = std::shared_ptr<ASTQuerySpecificationNode>;
  using ASTTableExpressionNodePtr = std::shared_ptr<ASTTableExpressionNode>;
  using ASTTableReferenceNodePtr = std::shared_ptr<ASTTableReferenceNode>;
  using ASTTableFactorNodePtr = std::shared_ptr<ASTTableFactorNode>;
  using ASTTableIdentifierNodePtr = std::shared_ptr<ASTTableIdentifierNode>;
  using ASTTableSubqueryNodePtr = std::shared_ptr<ASTTableSubqueryNode>;
  using ASTJoinNodePtr = std::shared_ptr<ASTJoinNode>;
  using ASTCrossJoinNodePtr = std::shared_ptr<ASTCrossJoinNode>;
  using ASTNaturalJoinNodePtr = std::shared_ptr<ASTNaturalJoinNode>;
  using ASTJoinWithConditionPtr = std::shared_ptr<ASTJoinWithCondition>;
  using ASTInnerJoinNodePtr = std::shared_ptr<ASTInnerJoinNode>;
  using ASTLeftJoinNodePtr = std::shared_ptr<ASTLeftJoinNode>;
  using ASTRightJoinNodePtr = std::shared_ptr<ASTRightJoinNode>;
  using ASTFullJoinNodePtr = std::shared_ptr<ASTFullJoinNode>;
  using ASTExprNodePtr = std::shared_ptr<ASTExprNode>;
  using ASTBinaryNodePtr = std::shared_ptr<ASTBinaryNode>;
  using ASTUnaryNodePtr = std::shared_ptr<ASTUnaryNode>;
  using ASTValueNodePtr = std::shared_ptr<ASTValueNode>;
  using ASTLikeNodePtr = std::shared_ptr<ASTLikeNode>;
  using ASTBetweenNodePtr = std::shared_ptr<ASTBetweenNode>;
  using ASTInNodePtr = std::shared_ptr<ASTInNode>;
  using ASTFunctionNodePtr = std::shared_ptr<ASTFunctionNode>;
  using ASTAggregateFunctionNodePtr = std::shared_ptr<ASTAggregateFunctionNode>;
  using ASTIdentifierPtr = std::shared_ptr<ASTIdentifier>;
  using ASTQualifiedAsteriskPtr = std::shared_ptr<ASTQualifiedAsterisk>;
  using ASTFromNodePtr = std::shared_ptr<ASTFromNode>;
  using ASTWhereNodePtr = std::shared_ptr<ASTWhereNode>;
  using ASTGroupByNodePtr = std::shared_ptr<ASTGroupByNode>;
  using ASTHavingNodePtr = std::shared_ptr<ASTHavingNode>;
  using ASTOrderByNodePtr = std::shared_ptr<ASTOrderByNode>;
  using ASTLimitNodePtr = std::shared_ptr<ASTLimitNode>;

  using Expressions = std::vector<ASTExprNodePtr>;
  using OrderExpression = std::pair<ASTExprNodePtr, eOrder>;
  using OrderExpressions = std::vector<OrderExpression>;
  using Identifiers = std::vector<ASTIdentifierPtr>;
  using TableReferences = std::vector<ASTTableReferenceNodePtr>;
  using IdentifierSet = std::set<ASTIdentifier>;

  struct CSVSQLDB_EXPORT ColumnDefinition {
    ColumnDefinition(const std::string& name, eType type)
    : _name(name)
    , _type{type}
    {
    }

    std::string _name;
    eType _type{NONE};
    bool _primaryKey{false};
    bool _unique{false};
    bool _notNull{false};
    std::any _defaultValue;
    ASTExprNodePtr _check;
    uint32_t _length{0};
  };

  using ColumnDefinitions = std::vector<ColumnDefinition>;

  struct CSVSQLDB_EXPORT TableConstraint {
    std::string _name;
    csvsqldb::StringVector _primaryKeys;
    csvsqldb::StringVector _uniqueKeys;
    ASTExprNodePtr _check;
  };

  using TableConstraints = std::vector<TableConstraint>;

  struct CSVSQLDB_EXPORT Parameter {
    ASTExprNodePtr _exp;
  };

  using Parameters = std::vector<Parameter>;

  struct CSVSQLDB_EXPORT NamedType {
    std::string _name;
    eType _type;

    bool operator!=(const NamedType& rhs) const
    {
      return _name != rhs._name && _type == rhs._type;
    }
  };

  using RelationOutputParameter = std::vector<NamedType>;


  class CSVSQLDB_EXPORT ASTNodeVisitor
  {
  public:
    ASTNodeVisitor() = default;
    virtual ~ASTNodeVisitor() = default;

    virtual void visit(ASTCreateTableNode& node) = 0;
    virtual void visit(ASTMappingNode& node) = 0;
    virtual void visit(ASTDropMappingNode& node) = 0;
    virtual void visit(ASTAlterTableAddColumnNode& node) = 0;
    virtual void visit(ASTAlterTableDropColumnNode& node) = 0;
    virtual void visit(ASTDropTableNode& node) = 0;
    virtual void visit(ASTUnionNode& node) = 0;
    virtual void visit(ASTExplainNode& node) = 0;
    virtual void visit(ASTQueryNode& node) = 0;
    virtual void visit(ASTQuerySpecificationNode& node) = 0;
    virtual void visit(ASTTableExpressionNode& node) = 0;
    virtual void visit(ASTBinaryNode& node) = 0;
    virtual void visit(ASTUnaryNode& node) = 0;
    virtual void visit(ASTValueNode& node) = 0;
    virtual void visit(ASTLikeNode& node) = 0;
    virtual void visit(ASTBetweenNode& node) = 0;
    virtual void visit(ASTInNode& node) = 0;
    virtual void visit(ASTFunctionNode& node) = 0;
    virtual void visit(ASTAggregateFunctionNode& node) = 0;
    virtual void visit(ASTIdentifier& node) = 0;
    virtual void visit(ASTQualifiedAsterisk& node) = 0;
    virtual void visit(ASTFromNode& node) = 0;
    virtual void visit(ASTTableIdentifierNode& node) = 0;
    virtual void visit(ASTTableSubqueryNode& node) = 0;
    virtual void visit(ASTCrossJoinNode& node) = 0;
    virtual void visit(ASTNaturalJoinNode& node) = 0;
    virtual void visit(ASTInnerJoinNode& node) = 0;
    virtual void visit(ASTLeftJoinNode& node) = 0;
    virtual void visit(ASTRightJoinNode& node) = 0;
    virtual void visit(ASTFullJoinNode& node) = 0;
    virtual void visit(ASTWhereNode& node) = 0;
    virtual void visit(ASTGroupByNode& node) = 0;
    virtual void visit(ASTHavingNode& node) = 0;
    virtual void visit(ASTOrderByNode& node) = 0;
    virtual void visit(ASTLimitNode& node) = 0;
  };


  class CSVSQLDB_EXPORT ASTExpressionNodeVisitor : public ASTNodeVisitor
  {
  public:
    ASTExpressionNodeVisitor() = default;

    void visit(ASTBinaryNode& node) override
    {
    }
    void visit(ASTUnaryNode& node) override
    {
    }
    void visit(ASTValueNode& node) override
    {
    }
    void visit(ASTLikeNode& node) override
    {
    }
    void visit(ASTBetweenNode& node) override
    {
    }
    void visit(ASTInNode& node) override
    {
    }
    void visit(ASTFunctionNode& node) override
    {
    }
    void visit(ASTAggregateFunctionNode& node) override
    {
    }
    void visit(ASTIdentifier& node) override
    {
    }

  private:
    void visit(ASTExplainNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTQualifiedAsterisk& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTCreateTableNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTMappingNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTDropMappingNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTAlterTableAddColumnNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTAlterTableDropColumnNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTDropTableNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTUnionNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTQueryNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTQuerySpecificationNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTTableExpressionNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTFromNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTTableIdentifierNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTTableSubqueryNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTCrossJoinNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTNaturalJoinNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTInnerJoinNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTLeftJoinNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTRightJoinNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTFullJoinNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTWhereNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTGroupByNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTHavingNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTOrderByNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
    void visit(ASTLimitNode& node) override
    {
      CSVSQLDB_THROW(SqlParserException, "Visting non expression node");
    }
  };


  class CSVSQLDB_EXPORT ASTNode
  {
  public:
    ASTNode(const SymbolTablePtr& symbolTable)
    : _symbolTable(symbolTable)
    {
    }

    virtual ~ASTNode() = default;

    SymbolTablePtr symbolTable() const
    {
      return _symbolTable;
    }

    virtual void typeSymbolTable(const Database&)
    {
    }

    virtual void accept(ASTNodeVisitor& visitor) = 0;

  protected:
    SymbolTablePtr _symbolTable;
  };


  class CSVSQLDB_EXPORT ASTExprNode : public ASTNode
  {
  public:
    ASTExprNode(const SymbolTablePtr& symbolTable)
    : ASTNode(symbolTable)
    {
    }

    virtual eType type() const = 0;

    std::string _symbolName;
  };


  class CSVSQLDB_EXPORT ASTQualifiedAsterisk : public ASTExprNode
  {
  public:
    ASTQualifiedAsterisk(const SymbolTablePtr& symbolTable, const std::string& prefix, bool quoted)
    : ASTExprNode(symbolTable)
    , _prefix(prefix)
    , _quoted(quoted)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    std::string getQualifiedIdentifier() const
    {
      return !_prefix.empty() ? _prefix + ".*" : "*";
    }

    std::string getQualifiedQuotedIdentifier() const
    {
      if (_quoted) {
        return !_prefix.empty() ? "\"" + _prefix + "\"" + ".*" : "*";
      }
      return getQualifiedIdentifier();
    }

    eType type() const override
    {
      return NONE;
    }

    std::string _prefix;
    bool _quoted;
  };


  class CSVSQLDB_EXPORT ASTExplainNode : public ASTNode
  {
  public:
    ASTExplainNode(const SymbolTablePtr& symbolTable, eDescriptionType descType, const ASTQueryNodePtr& query)
    : ASTNode(symbolTable)
    , _descType(descType)
    , _query(query)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    eDescriptionType _descType;
    ASTQueryNodePtr _query;
  };


  class CSVSQLDB_EXPORT ASTMappingNode : public ASTNode
  {
  public:
    ASTMappingNode(const SymbolTablePtr& symbolTable, const std::string& tableName, FileMapping::Mappings& mappings)
    : ASTNode(symbolTable)
    , _tableName(tableName)
    , _mappings(mappings)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    std::string _tableName;
    FileMapping::Mappings _mappings;
  };


  class CSVSQLDB_EXPORT ASTDropMappingNode : public ASTNode
  {
  public:
    ASTDropMappingNode(const SymbolTablePtr& symbolTable, const std::string& tableName)
    : ASTNode(symbolTable)
    , _tableName(tableName)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    std::string _tableName;
  };


  class CSVSQLDB_EXPORT ASTQueryNode : public ASTNode
  {
  public:
    ASTQueryNode(const SymbolTablePtr& symbolTable, const ASTQueryExpressionNodePtr& query)
    : ASTNode(symbolTable)
    , _query(query)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    ASTQueryExpressionNodePtr _query;
  };


  class CSVSQLDB_EXPORT ASTQueryExpressionNode : public ASTNode
  {
  public:
    ASTQueryExpressionNode(const SymbolTablePtr& symbolTable)
    : ASTNode(symbolTable)
    {
    }

    virtual RelationOutputParameter outputParameter() const = 0;
  };


  class CSVSQLDB_EXPORT ASTUnionNode : public ASTQueryExpressionNode
  {
  public:
    ASTUnionNode(const SymbolTablePtr& symbolTable, eQuantifier quantifier, const ASTQueryExpressionNodePtr& lhs,
                 const ASTQueryExpressionNodePtr& rhs)
    : ASTQueryExpressionNode(symbolTable)
    , _quantifier(quantifier)
    , _lhs(lhs)
    , _rhs(rhs)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    RelationOutputParameter outputParameter() const override
    {
      return _lhs->outputParameter();
    }

    eQuantifier _quantifier;
    ASTQueryExpressionNodePtr _lhs;
    ASTQueryExpressionNodePtr _rhs;
  };


  class CSVSQLDB_EXPORT ASTQuerySpecificationNode : public ASTQueryExpressionNode
  {
  public:
    ASTQuerySpecificationNode(const SymbolTablePtr& symbolTable, eQuantifier quantifier, const Expressions& nodes,
                              const ASTTableExpressionNodePtr tableExpression)
    : ASTQueryExpressionNode(symbolTable)
    , _quantifier(quantifier)
    , _nodes(nodes)
    , _tableExpression(tableExpression)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    void typeSymbolTable(const Database& database) override
    {
      _symbolTable->typeSymbolTable(database);
    }

    RelationOutputParameter outputParameter() const override
    {
      RelationOutputParameter outputParameter;

      for (const auto& exp : _nodes) {
        if (std::dynamic_pointer_cast<ASTQualifiedAsterisk>(exp)) {
          // find all symbols in the table associated with the table / star
          // and put them in the list
          std::string tableName = std::dynamic_pointer_cast<ASTQualifiedAsterisk>(exp)->_prefix;

          if (tableName.empty()) {
            tableName = _symbolTable->getTables()[0]->_identifier;
          }
          SymbolInfos infos = _symbolTable->findAllSymbolsForTable(tableName);
          for (const auto& info : infos) {
            NamedType nt;
            nt._type = info->_type;
            nt._name = info->_name;
            outputParameter.push_back(nt);
          }
        } else {
          NamedType nt;
          nt._type = exp->type();
          nt._name = exp->_symbolName;
          outputParameter.push_back(nt);
        }
      }

      return outputParameter;
    }

    eQuantifier _quantifier;
    Expressions _nodes;
    ASTTableExpressionNodePtr _tableExpression;
  };


  class CSVSQLDB_EXPORT ASTTableExpressionNode : public ASTNode
  {
  public:
    ASTTableExpressionNode(const SymbolTablePtr& symbolTable, const ASTFromNodePtr& from, const ASTWhereNodePtr& where,
                           const ASTGroupByNodePtr& group, const ASTHavingNodePtr& having, const ASTOrderByNodePtr& order,
                           const ASTLimitNodePtr& limit)
    : ASTNode(symbolTable)
    , _from(from)
    , _where(where)
    , _group(group)
    , _having(having)
    , _order(order)
    , _limit(limit)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    ASTFromNodePtr _from;
    ASTWhereNodePtr _where;
    ASTGroupByNodePtr _group;
    ASTHavingNodePtr _having;
    ASTOrderByNodePtr _order;
    ASTLimitNodePtr _limit;
  };


  class CSVSQLDB_EXPORT ASTTableReferenceNode : public ASTNode
  {
  public:
    ASTTableReferenceNode(const SymbolTablePtr& symbolTable)
    : ASTNode(symbolTable)
    {
    }
  };


  class CSVSQLDB_EXPORT ASTTableFactorNode : public ASTTableReferenceNode
  {
  public:
    ASTTableFactorNode(const SymbolTablePtr& symbolTable)
    : ASTTableReferenceNode(symbolTable)
    {
    }
  };


  class CSVSQLDB_EXPORT ASTTableIdentifierNode : public ASTTableFactorNode
  {
  public:
    ASTTableIdentifierNode(const SymbolTablePtr& symbolTable, const ASTIdentifierPtr& factor)
    : ASTTableFactorNode(symbolTable)
    , _factor(factor)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    ASTIdentifierPtr _factor;
  };


  class CSVSQLDB_EXPORT ASTTableSubqueryNode : public ASTTableFactorNode
  {
  public:
    ASTTableSubqueryNode(const SymbolTablePtr& symbolTable, const ASTQueryExpressionNodePtr& query, const std::string& queryAlias)
    : ASTTableFactorNode(symbolTable)
    , _query(query)
    , _queryAlias(queryAlias)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    ASTQueryExpressionNodePtr _query;
    std::string _queryAlias;
  };


  class CSVSQLDB_EXPORT ASTJoinNode : public ASTTableReferenceNode
  {
  public:
    ASTJoinNode(const SymbolTablePtr& symbolTable, const ASTTableReferenceNodePtr& tableReference)
    : ASTTableReferenceNode(symbolTable)
    , _tableReference(tableReference)
    {
    }

    ASTTableReferenceNodePtr _tableReference;
  };


  class CSVSQLDB_EXPORT ASTCrossJoinNode : public ASTJoinNode
  {
  public:
    ASTCrossJoinNode(const SymbolTablePtr& symbolTable, const ASTTableReferenceNodePtr& tableReference,
                     const ASTTableFactorNodePtr& factor)
    : ASTJoinNode(symbolTable, tableReference)
    , _factor(factor)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    ASTTableFactorNodePtr _factor;
  };


  class CSVSQLDB_EXPORT ASTNaturalJoinNode : public ASTJoinNode
  {
  public:
    ASTNaturalJoinNode(const SymbolTablePtr& symbolTable, eNaturalJoinType joinType,
                       const ASTTableReferenceNodePtr& tableReference, const ASTTableFactorNodePtr& factor)
    : ASTJoinNode(symbolTable, tableReference)
    , _joinType(joinType)
    , _factor(factor)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    eNaturalJoinType _joinType;
    ASTTableFactorNodePtr _factor;
  };


  class CSVSQLDB_EXPORT ASTJoinWithCondition : public ASTJoinNode
  {
  public:
    ASTJoinWithCondition(const SymbolTablePtr& symbolTable, const ASTTableReferenceNodePtr& tableReference,
                         const ASTTableFactorNodePtr& factor, const ASTExprNodePtr& expression)
    : ASTJoinNode(symbolTable, tableReference)
    , _factor(factor)
    , _expression(expression)
    {
    }

    ASTTableFactorNodePtr _factor;
    ASTExprNodePtr _expression;
  };


  class CSVSQLDB_EXPORT ASTInnerJoinNode : public ASTJoinWithCondition
  {
  public:
    ASTInnerJoinNode(const SymbolTablePtr& symbolTable, const ASTTableReferenceNodePtr& tableReference,
                     const ASTTableFactorNodePtr& factor, const ASTExprNodePtr& expression)
    : ASTJoinWithCondition(symbolTable, tableReference, factor, expression)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }
  };


  class CSVSQLDB_EXPORT ASTLeftJoinNode : public ASTJoinWithCondition
  {
  public:
    ASTLeftJoinNode(const SymbolTablePtr& symbolTable, const ASTTableReferenceNodePtr& tableReference,
                    const ASTTableFactorNodePtr& factor, const ASTExprNodePtr& expression)
    : ASTJoinWithCondition(symbolTable, tableReference, factor, expression)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }
  };


  class CSVSQLDB_EXPORT ASTRightJoinNode : public ASTJoinWithCondition
  {
  public:
    ASTRightJoinNode(const SymbolTablePtr& symbolTable, const ASTTableReferenceNodePtr& tableReference,
                     const ASTTableFactorNodePtr& factor, const ASTExprNodePtr& expression)
    : ASTJoinWithCondition(symbolTable, tableReference, factor, expression)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }
  };


  class CSVSQLDB_EXPORT ASTFullJoinNode : public ASTJoinWithCondition
  {
  public:
    ASTFullJoinNode(const SymbolTablePtr& symbolTable, const ASTTableReferenceNodePtr& tableReference,
                    const ASTTableFactorNodePtr& factor, const ASTExprNodePtr& expression)
    : ASTJoinWithCondition(symbolTable, tableReference, factor, expression)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }
  };


  class CSVSQLDB_EXPORT ASTBinaryNode : public ASTExprNode
  {
  public:
    ASTBinaryNode(const SymbolTablePtr& symbolTable, eOperationType op, const ASTExprNodePtr& lhs, const ASTExprNodePtr& rhs)
    : ASTExprNode(symbolTable)
    , _op(op)
    , _lhs(lhs)
    , _rhs(rhs)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    eType type() const override
    {
      return inferTypeOfBinaryOperation(_op, _lhs->type(), _rhs->type());
    }

    eOperationType _op;
    ASTExprNodePtr _lhs;
    ASTExprNodePtr _rhs;
  };


  class CSVSQLDB_EXPORT ASTUnaryNode : public ASTExprNode
  {
  public:
    ASTUnaryNode(const SymbolTablePtr& symbolTable, eOperationType op, eType castType, const ASTExprNodePtr& rhs)
    : ASTExprNode(symbolTable)
    , _op(op)
    , _castType(castType)
    , _rhs(rhs)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    eType type() const override
    {
      eType retType = NONE;
      switch (_op) {
        case OP_NOT:
          retType = BOOLEAN;
          break;
        case OP_PLUS:
        case OP_MINUS:
          retType = _rhs->type();
          break;
        case OP_CAST:
          retType = _castType;
          break;
        default:
          throw std::runtime_error("operation " + operationTypeToString(_op) + " not allowed for unary operations");
      }
      return inferTypeOfUnaryOperation(_op, retType, _rhs->type());
    }

    eOperationType _op;
    eType _castType;
    ASTExprNodePtr _rhs;
  };


  class CSVSQLDB_EXPORT ASTValueNode : public ASTExprNode
  {
  public:
    ASTValueNode(const SymbolTablePtr& symbolTable, eType type, const std::string& value)
    : ASTExprNode(symbolTable)
    , _value(type, TypedValue::createValue(type, value)._value)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    eType type() const override
    {
      return _value._type;
    }

    TypedValue _value;
  };


  class CSVSQLDB_EXPORT ASTLikeNode : public ASTExprNode
  {
  public:
    ASTLikeNode(const SymbolTablePtr& symbolTable, const ASTExprNodePtr& lhs, const std::string& like)
    : ASTExprNode(symbolTable)
    , _lhs(lhs)
    , _like(like)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    eType type() const override
    {
      return BOOLEAN;
    }

    ASTExprNodePtr _lhs;
    std::string _like;
  };


  class CSVSQLDB_EXPORT ASTBetweenNode : public ASTExprNode
  {
  public:
    ASTBetweenNode(const SymbolTablePtr& symbolTable, const ASTExprNodePtr& lhs, const ASTExprNodePtr& from,
                   const ASTExprNodePtr& to)
    : ASTExprNode(symbolTable)
    , _lhs(lhs)
    , _from(from)
    , _to(to)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    eType type() const override
    {
      return BOOLEAN;
    }

    ASTExprNodePtr _lhs;
    ASTExprNodePtr _from;
    ASTExprNodePtr _to;
  };


  class CSVSQLDB_EXPORT ASTInNode : public ASTExprNode
  {
  public:
    ASTInNode(const SymbolTablePtr& symbolTable, const ASTExprNodePtr& lhs, const Expressions& expressions)
    : ASTExprNode(symbolTable)
    , _lhs(lhs)
    , _expressions(expressions)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    eType type() const override
    {
      return BOOLEAN;
    }

    ASTExprNodePtr _lhs;
    Expressions _expressions;
  };


  class CSVSQLDB_EXPORT ASTFunctionNode : public ASTExprNode
  {
  public:
    ASTFunctionNode(const SymbolTablePtr& symbolTable, const FunctionRegistry& functionRegistry, const std::string& functionName,
                    const Parameters& parameters)
    : ASTExprNode(symbolTable)
    , _parameters(parameters)
    {
      _function = functionRegistry.getFunction(functionName);
      if (!_function) {
        CSVSQLDB_THROW(SqlParserException, "function '" << functionName << "' not found");
      }
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    eType type() const override
    {
      return _function->getReturnType();
    }

    Function::Ptr _function;
    Parameters _parameters;
  };


  class CSVSQLDB_EXPORT ASTAggregateFunctionNode : public ASTExprNode
  {
  public:
    ASTAggregateFunctionNode(const SymbolTablePtr& symbolTable, eAggregateFunction aggregateFunction, eQuantifier quantifier,
                             const Parameters& parameters)
    : ASTExprNode(symbolTable)
    , _aggregateFunction(aggregateFunction)
    , _quantifier(quantifier)
    , _parameters(parameters)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    eType type() const override
    {
      switch (_aggregateFunction) {
        case COUNT_STAR:
          return INT;
        case AVG:
          return REAL;
        case COUNT:
          return INT;
        case ARBITRARY:
        case MAX:
        case MIN:
        case SUM:
          return _parameters[0]._exp->type();
      }
      throw std::runtime_error("just to make VC2013 happy");
    }

    eAggregateFunction _aggregateFunction;
    eQuantifier _quantifier;
    Parameters _parameters;
  };


  class CSVSQLDB_EXPORT ASTIdentifier : public ASTExprNode
  {
  public:
    ASTIdentifier(const SymbolTablePtr& symbolTable, const SymbolInfoPtr& info, const std::string& prefix,
                  const std::string& identifier, bool quoted)
    : ASTExprNode(symbolTable)
    , _info(info)
    , _prefix(prefix)
    , _identifier(identifier)
    , _quoted(quoted)
    , _qualifiedIdentifier((!_prefix.empty() ? _prefix + "." : "") + _identifier)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    std::string getQualifiedIdentifier() const
    {
      return _qualifiedIdentifier;
    }

    std::string getQualifiedQuotedIdentifier() const
    {
      if (_quoted) {
        return (!_prefix.empty() ? "\"" + _prefix + "\"" + "." : "") + "\"" + _identifier + "\"";
      }
      return getQualifiedIdentifier();
    }

    eType type() const override
    {
      return _info->_type;
    }

    bool operator<(const ASTIdentifier& rhs) const
    {
      return (_prefix + _identifier) < (rhs._prefix + rhs._identifier);
    }

    SymbolInfoPtr _info;
    std::string _prefix;
    std::string _identifier;
    bool _quoted;
    std::string _qualifiedIdentifier;
  };


  class CSVSQLDB_EXPORT ASTFromNode : public ASTNode
  {
  public:
    ASTFromNode(const SymbolTablePtr& symbolTable, const TableReferences& tableReferences)
    : ASTNode(symbolTable)
    , _tableReferences(tableReferences)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    TableReferences _tableReferences;
  };


  class CSVSQLDB_EXPORT ASTWhereNode : public ASTNode
  {
  public:
    ASTWhereNode(const SymbolTablePtr& symbolTable, const ASTExprNodePtr& exp)
    : ASTNode(symbolTable)
    , _exp(exp)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    ASTExprNodePtr _exp;
  };


  class CSVSQLDB_EXPORT ASTGroupByNode : public ASTNode
  {
  public:
    ASTGroupByNode(const SymbolTablePtr& symbolTable, eQuantifier quantifier, const Identifiers& identifiers)
    : ASTNode(symbolTable)
    , _quantifier(quantifier)
    , _identifiers(identifiers)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    eQuantifier _quantifier;
    Identifiers _identifiers;
  };


  class CSVSQLDB_EXPORT ASTHavingNode : public ASTNode
  {
  public:
    ASTHavingNode(const SymbolTablePtr& symbolTable, const ASTExprNodePtr& exp)
    : ASTNode(symbolTable)
    , _exp(exp)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    ASTExprNodePtr _exp;
  };


  class CSVSQLDB_EXPORT ASTOrderByNode : public ASTNode
  {
  public:
    ASTOrderByNode(const SymbolTablePtr& symbolTable, const OrderExpressions& orderExpressions)
    : ASTNode(symbolTable)
    , _orderExpressions(orderExpressions)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    OrderExpressions _orderExpressions;
  };


  class CSVSQLDB_EXPORT ASTLimitNode : public ASTNode
  {
  public:
    ASTLimitNode(const SymbolTablePtr& symbolTable, const ASTExprNodePtr& limit, const ASTExprNodePtr& offset)
    : ASTNode(symbolTable)
    , _limit(limit)
    , _offset(offset)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    ASTExprNodePtr _limit;
    ASTExprNodePtr _offset;
  };


  class CSVSQLDB_EXPORT ASTCreateTableNode : public ASTNode
  {
  public:
    ASTCreateTableNode(const SymbolTablePtr& symbolTable, const std::string& tableName,
                       const ColumnDefinitions& columnDefinitions, const TableConstraints& tableConstraints,
                       bool createIfNotExists)
    : ASTNode(symbolTable)
    , _tableName(tableName)
    , _columnDefinitions(columnDefinitions)
    , _tableConstraints(tableConstraints)
    , _createIfNotExists(createIfNotExists)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    std::string _tableName;
    ColumnDefinitions _columnDefinitions;
    TableConstraints _tableConstraints;
    bool _createIfNotExists;
  };


  class CSVSQLDB_EXPORT ASTAlterTableNode : public ASTNode
  {
  protected:
    ASTAlterTableNode(const SymbolTablePtr& symbolTable)
    : ASTNode(symbolTable)
    {
    }
  };


  class CSVSQLDB_EXPORT ASTAlterTableAddColumnNode : public ASTAlterTableNode
  {
  public:
    ASTAlterTableAddColumnNode(const SymbolTablePtr& symbolTable, const ColumnDefinition& definition)
    : ASTAlterTableNode(symbolTable)
    , _definition(definition)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    ColumnDefinition _definition;
  };


  class CSVSQLDB_EXPORT ASTAlterTableDropColumnNode : public ASTAlterTableNode
  {
  public:
    ASTAlterTableDropColumnNode(const SymbolTablePtr& symbolTable, const std::string& columnName)
    : ASTAlterTableNode(symbolTable)
    , _columnName(columnName)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    std::string _columnName;
  };


  class CSVSQLDB_EXPORT ASTDropTableNode : public ASTNode
  {
  public:
    ASTDropTableNode(const SymbolTablePtr& symbolTable, const std::string& tableName)
    : ASTNode(symbolTable)
    , _tableName(tableName)
    {
    }

    void accept(ASTNodeVisitor& visitor) override
    {
      visitor.visit(*this);
    }

    std::string _tableName;
  };
}
