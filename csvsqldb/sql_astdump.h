//
//  sql_astdump.h
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

#include <csvsqldb/sql_ast.h>

#include <memory>
#include <ostream>
#include <vector>


namespace csvsqldb
{
  class CSVSQLDB_EXPORT ASTNodeSQLPrintVisitor : public ASTNodeVisitor
  {
  public:
    ASTNodeSQLPrintVisitor() = default;

    std::string toString() const
    {
      return _ss.str();
    }

    void reset()
    {
      _ss.str("");
      _ss.clear();
    }

    void printOp(eOperationType op)
    {
      switch (op) {
        case OP_CAST:
        case OP_LIKE:
        case OP_BETWEEN:
        case OP_IN:
          CSVSQLDB_THROW(SqlException, "Cannot print CAST,LIKE,BETWEEN or IN operators");
        default:
          _ss << " " << operationTypeToString(op) << " ";
      }
    }

    void visit(ASTExplainNode& node) override
    {
      _ss << "EXPLAIN AST ";
      node._query->accept(*this);
    }

    void visit(ASTMappingNode&) override
    {
    }

    void visit(ASTDropMappingNode&) override
    {
    }

    void visit(ASTQualifiedAsterisk& node) override
    {
      _ss << node.getQualifiedQuotedIdentifier();
    }

    void visit(ASTCreateTableNode&) override
    {
    }

    void visit(ASTAlterTableAddColumnNode&) override
    {
    }

    void visit(ASTAlterTableDropColumnNode&) override
    {
    }

    void visit(ASTDropTableNode&) override
    {
    }

    void visit(ASTQueryNode& node) override
    {
      node._query->accept(*this);
    }

    void visit(ASTUnionNode& node) override
    {
      node._lhs->accept(*this);
      _ss << " UNION " << (node._quantifier == DISTINCT ? "DISTINCT " : "");
      _ss << "(";
      node._rhs->accept(*this);
      _ss << ")";
    }

    void visit(ASTQuerySpecificationNode& node) override
    {
      _ss << "SELECT ";
      if (node._quantifier == DISTINCT) {
        _ss << "DISTINCT ";
      }
      bool first = true;
      for (const auto& exp : node._nodes) {
        if (!first) {
          _ss << ",";
        } else {
          first = false;
        }
        exp->accept(*this);
      }
      node._tableExpression->accept(*this);
    }

    void visit(ASTTableExpressionNode& node) override
    {
      node._from->accept(*this);
      if (node._where) {
        node._where->accept(*this);
      }
      if (node._group) {
        node._group->accept(*this);
      }
      if (node._order) {
        node._order->accept(*this);
      }
      if (node._limit) {
        node._limit->accept(*this);
      }
    }

    void visit(ASTFromNode& node) override
    {
      _ss << " FROM ";
      bool first = true;
      for (const auto& ref : node._tableReferences) {
        if (!first) {
          _ss << ",";
        } else {
          first = false;
        }
        ref->accept(*this);
      }
    }

    void visit(ASTTableIdentifierNode& node) override
    {
      node._factor->accept(*this);
    }

    void visit(ASTTableSubqueryNode& node) override
    {
      _ss << "(";
      node._query->accept(*this);
      _ss << ")";
    }

    void visit(ASTCrossJoinNode& node) override
    {
      node._tableReference->accept(*this);
      _ss << " CROSS JOIN ";
      node._factor->accept(*this);
    }

    void visit(ASTNaturalJoinNode& node) override
    {
      node._tableReference->accept(*this);
      _ss << " " << naturalJoinToString(node._joinType);
      _ss << " JOIN ";
      node._factor->accept(*this);
    }

    void visit(ASTInnerJoinNode& node) override
    {
      node._tableReference->accept(*this);
      _ss << " INNER JOIN ";
      node._factor->accept(*this);
      _ss << " ON ";
      node._expression->accept(*this);
    }

    void visit(ASTLeftJoinNode& node) override
    {
      node._tableReference->accept(*this);
      _ss << " LEFT OUTER JOIN ";
      node._factor->accept(*this);
      _ss << " ON ";
      node._expression->accept(*this);
    }

    void visit(ASTRightJoinNode& node) override
    {
      node._tableReference->accept(*this);
      _ss << " RIGHT OUTER JOIN ";
      node._factor->accept(*this);
      _ss << " ON ";
      node._expression->accept(*this);
    }

    void visit(ASTFullJoinNode& node) override
    {
      node._tableReference->accept(*this);
      _ss << " FULL OUTER JOIN ";
      node._factor->accept(*this);
      _ss << " ON ";
      node._expression->accept(*this);
    }

    void visit(ASTWhereNode& node) override
    {
      _ss << " WHERE ";
      node._exp->accept(*this);
    }

    void visit(ASTGroupByNode& node) override
    {
      _ss << " GROUP BY ";
      bool first = true;
      for (const auto& ident : node._identifiers) {
        if (!first) {
          _ss << ",";
        } else {
          first = false;
        }
        ident->accept(*this);
      }
    }

    void visit(ASTHavingNode&) override
    {
    }

    void visit(ASTOrderByNode& node) override
    {
      _ss << " ORDER BY ";
      bool first = true;
      for (const auto& exp : node._orderExpressions) {
        if (!first) {
          _ss << ",";
        } else {
          first = false;
        }
        exp.first->accept(*this);
        switch (exp.second) {
          case ASC:
            _ss << " ASC";
            break;
          case DESC:
            _ss << " DESC";
            break;
        }
      }
    }

    void visit(ASTLimitNode& node) override
    {
      _ss << " LIMIT ";
      node._limit->accept(*this);
      if (node._offset) {
        _ss << " OFFSET ";
        node._offset->accept(*this);
      }
    }

    void visit(ASTBinaryNode& node) override
    {
      _ss << "(";
      node._lhs->accept(*this);
      printOp(node._op);
      node._rhs->accept(*this);
      _ss << ")";
      if (!node._symbolName.empty()) {
        const SymbolInfoPtr info = node.symbolTable()->findSymbol(node._symbolName);
        if (!info->_alias.empty()) {
          _ss << " AS " << info->_alias;
        }
      }
    }

    void visit(ASTUnaryNode& node) override
    {
      if (node._op == OP_CAST) {
        _ss << "CAST(";
        node._rhs->accept(*this);
        _ss << " AS " << typeToString(node._castType);
        /*
         if(node._castType == STRING) {
         // this just a workaround
         // we need to parse STRING types also without length
         // and we should store the length of the type in the value
         _ss << "(255)";
         }*/
        _ss << ")";
        if (!node._symbolName.empty()) {
          const SymbolInfoPtr info = node.symbolTable()->findSymbol(node._symbolName);
          if (!info->_alias.empty()) {
            _ss << " AS " << info->_alias;
          }
        }
      } else {
        printOp(node._op);
        node._rhs->accept(*this);
      }
    }

    void visit(ASTValueNode& node) override
    {
      if (node._value._type == STRING) {
        _ss << "'";
      }
      _ss << typedValueToString(node._value);
      if (node._value._type == STRING) {
        _ss << "'";
      }
    }

    void visit(ASTLikeNode& node) override
    {
      node._lhs->accept(*this);
      _ss << " LIKE '" << node._like << "'";
    }

    void visit(ASTBetweenNode& node) override
    {
      node._lhs->accept(*this);
      _ss << " BETWEEN ";
      node._from->accept(*this);
      _ss << " AND ";
      node._to->accept(*this);
    }

    void visit(ASTInNode& node) override
    {
      node._lhs->accept(*this);
      _ss << " IN (";
      bool first = true;
      for (const auto& exp : node._expressions) {
        if (!first) {
          _ss << ",";
        } else {
          first = false;
        }
        exp->accept(*this);
      }
      _ss << ")";
    }

    void visit(ASTFunctionNode& node) override
    {
      _ss << node._function->getName() << "(";
      bool first = true;
      for (const auto& param : node._parameters) {
        if (!first) {
          _ss << ",";
        } else {
          first = false;
        }
        param._exp->accept(*this);
      }
      _ss << ")";
    }

    void visit(ASTAggregateFunctionNode& node) override
    {
      _ss << aggregateFunctionToString(node._aggregateFunction) << "(";
      bool first = true;
      if (node._aggregateFunction == COUNT_STAR) {
        _ss << "*";
      }
      _ss << (node._quantifier == DISTINCT ? "DISTINCT " : "");
      for (const auto& param : node._parameters) {
        if (!first) {
          _ss << ",";
        } else {
          first = false;
        }
        param._exp->accept(*this);
      }
      _ss << ")";
      if (!node._symbolName.empty()) {
        const SymbolInfoPtr info = node.symbolTable()->findSymbol(node._symbolName);
        if (!info->_alias.empty()) {
          _ss << " AS " << info->_alias;
        }
      }
    }

    void visit(ASTIdentifier& node) override
    {
      _ss << node.getQualifiedQuotedIdentifier();
      if (node._info && !node._info->_alias.empty() && node.getQualifiedQuotedIdentifier() != node._info->_alias) {
        _ss << " AS " << node._info->_alias;
      }
    }

  private:
    std::stringstream _ss;
  };


  class CSVSQLDB_EXPORT ASTNodeDumpVisitor : public ASTNodeVisitor
  {
  public:
    explicit ASTNodeDumpVisitor(std::ostream& out)
    : _out{out}
    {
    }

    void visit(ASTCreateTableNode& node) override
    {
      _out << "ASTCreateTable" << std::endl;
      _indent += 2;
      indent();
      _out << node._tableName << " ->" << (node._createIfNotExists ? " create if not exists" : "") << std::endl;
      _indent += 2;
      for (const auto& definition : node._columnDefinitions) {
        indent();
        _out << definition._name << " -> " << typeToString(definition._type);
        _out << (definition._length ? " length " + std::to_string(definition._length) : "");
        _out << (definition._primaryKey ? " Primary Key" : "");
        _out << (definition._notNull ? " Not Null" : "");
        _out << (definition._unique ? " Unique" : "");
        if (definition._defaultValue.has_value()) {
          _out << typedValueToString(TypedValue(definition._type, definition._defaultValue));
        }
        if (definition._check) {
          _out << std::endl;
          indent();
          _out << "check" << std::endl;
          _indent += 2;
          indent();
          definition._check->accept(*this);
          _indent -= 2;
        }
        _out << std::endl;
      }
      for (const auto& constraint : node._tableConstraints) {
        if (!constraint._name.empty()) {
          indent();
          _out << constraint._name << std::endl;
        }
        if (!constraint._primaryKeys.empty()) {
          indent();
          _out << "primary keys [ ";
          for (const auto& pk : constraint._primaryKeys) {
            _out << pk << " ";
          }
          _out << " ]" << std::endl;
        }
        if (!constraint._uniqueKeys.empty()) {
          indent();
          _out << "unique keys [ ";
          for (const auto& uk : constraint._uniqueKeys) {
            _out << uk << " ";
          }
          _out << " ]" << std::endl;
        }
        if (constraint._check) {
          indent();
          _out << "check" << std::endl;
          _indent += 2;
          indent();
          constraint._check->accept(*this);
          _indent -= 2;
        }
        _out << std::endl;
      }
      _indent -= 4;
    }

    void visit(ASTMappingNode& node) override
    {
      _out << "ASTMappingNode" << std::endl;
      _indent += 2;
      indent();
      _out << node._tableName << " ->";
      _indent += 2;
      std::for_each(node._mappings.begin(), node._mappings.end(), [&](const auto& mapping) {
        _out << std::endl;
        indent();
        _out << mapping._mapping;
      });
      _out << std::endl;
      _indent -= 4;
    }

    void visit(ASTDropMappingNode& node) override
    {
      _out << "ASTDropMappingNode" << std::endl;
      _indent += 2;
      indent();
      _out << node._tableName;
      _out << std::endl;
      _indent -= 2;
    }

    void visit(ASTAlterTableAddColumnNode& node) override
    {
      _out << "ASTAlterTableAdd" << std::endl;
      _indent += 2;
      indent();
      _out << node._definition._name << " -> " << typeToString(node._definition._type) << " ";
      _out << (node._definition._primaryKey ? "Primary Key" : "");
      _out << (node._definition._notNull ? "Not Null" : "");
      _out << (node._definition._unique ? "Unique" : "");
      if (node._definition._defaultValue.has_value()) {
        _out << " Default " << typedValueToString(TypedValue(node._definition._type, node._definition._defaultValue));
      }
      _out << std::endl;
      _indent -= 2;
    }

    void visit(ASTAlterTableDropColumnNode& node) override
    {
      _out << "ASTAlterTableDrop" << std::endl;
      _indent += 2;
      indent();
      _out << "drop column " << node._columnName << std::endl;
      _indent -= 2;
    }

    void visit(ASTDropTableNode& node) override
    {
      _out << "ASTDropTable" << std::endl;
      _indent += 2;
      indent();
      _out << "drop table " << node._tableName << std::endl;
      _indent -= 2;
    }

    void visit(ASTExplainNode& node) override
    {
      _out << "ASTExplainNode" << std::endl;
      _indent += 2;
      indent();
      _out << "Type " << descriptionTypeToString(node._descType) << std::endl;

      ASTNodeSQLPrintVisitor visitor;
      node._query->accept(visitor);
      indent();
      _out << "Query: " << visitor.toString() << std::endl;
      _indent -= 2;
    }

    void visit(ASTUnionNode& node) override
    {
      _out << "ASTUnion" << std::endl;
      _indent += 2;
      switch (node._quantifier) {
        case DISTINCT:
          indent();
          _out << "DISTINCT" << std::endl;
        case ALL:
          break;
      }
      indent();
      node._lhs->accept(*this);
      indent();
      node._rhs->accept(*this);
      _indent -= 2;
    }

    void visit(ASTQueryNode& node) override
    {
      node._query->accept(*this);
    }

    void visit(ASTQuerySpecificationNode& select) override
    {
      _out << "ASTQuerySpecification" << std::endl;
      _indent += 2;
      switch (select._quantifier) {
        case DISTINCT:
          indent();
          _out << "DISTINCT" << std::endl;
        case ALL:
          break;
      }
      _indent += 2;
      for (const auto& node : select._nodes) {
        indent();
        node->accept(*this);
      }
      _indent -= 2;
      select._tableExpression->accept(*this);
      _indent -= 2;
    }

    void visit(ASTTableExpressionNode& node) override
    {
      indent();
      _out << "ASTTableExpression" << std::endl;
      _indent += 2;
      indent();
      node._from->accept(*this);

      if (node._where) {
        indent();
        node._where->accept(*this);
      }
      if (node._group) {
        indent();
        node._group->accept(*this);
      }
      if (node._order) {
        indent();
        node._order->accept(*this);
      }
      if (node._limit) {
        indent();
        node._limit->accept(*this);
      }
      _indent -= 2;
    }

    void visit(ASTBinaryNode& node) override
    {
      _out << "ASTBinary" << std::endl;

      _indent += 2;
      indent();
      _out << operationTypeToString(node._op);
      _out << std::endl;
      _indent += 2;
      indent();
      node._lhs->accept(*this);
      indent();
      node._rhs->accept(*this);
      _indent -= 2;
      if (!node._symbolName.empty()) {
        indent();
        _out << "-> " << node._symbolName;
        const SymbolInfoPtr& info = node.symbolTable()->findSymbol(node._symbolName);
        if (!info->_alias.empty()) {
          _out << " alias: " << info->_alias;
        }
        _out << std::endl;
      }
      _indent -= 2;
    }

    void visit(ASTUnaryNode& node) override
    {
      _out << "ASTUnary" << std::endl;
      _indent += 2;
      indent();
      _out << operationTypeToString(node._op);
      _out << std::endl;
      _indent += 2;
      indent();
      node._rhs->accept(*this);
      if (node._op == OP_CAST) {
        indent();
        _out << "AS " << typeToString(node._castType) << std::endl;
      }
      _indent -= 4;
    }

    void visit(ASTValueNode& node) override
    {
      _out << "ASTValue -> ";
      _out << typeToString(node._value._type) << " ";
      _out << typedValueToString(node._value);
      _out << std::endl;
    }

    void visit(ASTLikeNode& node) override
    {
      _out << "ASTLike" << std::endl;

      _indent += 2;
      indent();
      node._lhs->accept(*this);
      indent();
      _out << "'" << node._like << "'" << std::endl;
      _indent -= 2;
    }

    void visit(ASTBetweenNode& node) override
    {
      _out << "ASTBetween" << std::endl;

      _indent += 2;
      indent();
      node._lhs->accept(*this);
      indent();
      _out << "between" << std::endl;
      _indent += 2;
      indent();
      node._from->accept(*this);
      _indent -= 2;
      indent();
      _out << "and" << std::endl;
      _indent += 2;
      indent();
      node._to->accept(*this);
      _indent -= 4;
    }

    void visit(ASTInNode& node) override
    {
      _out << "ASTIn" << std::endl;

      _indent += 2;
      indent();
      node._lhs->accept(*this);
      indent();
      _out << "in" << std::endl;
      _indent += 2;
      for (const auto& exp : node._expressions) {
        indent();
        exp->accept(*this);
      }
      _indent -= 4;
    }

    void visit(ASTFunctionNode& node) override
    {
      _out << "ASTFunction" << std::endl;
      _indent += 2;
      indent();
      _out << node._function->getName() << "(" << std::endl;
      _indent += 2;
      for (const auto& param : node._parameters) {
        indent();
        param._exp->accept(*this);
      }
      _indent -= 2;
      indent();
      _out << ")" << std::endl;
      _indent -= 2;
    }

    void visit(ASTAggregateFunctionNode& node) override
    {
      _out << "ASTAggregateFunction" << std::endl;
      _indent += 2;
      indent();
      _out << aggregateFunctionToString(node._aggregateFunction) << "(" << std::endl;
      _indent += 2;
      if (node._aggregateFunction == COUNT_STAR) {
        indent();
        _out << "*" << std::endl;
      }
      switch (node._quantifier) {
        case DISTINCT:
          indent();
          _out << "DISTINCT ";
        case ALL:
          break;
      }
      for (const auto& param : node._parameters) {
        param._exp->accept(*this);
      }
      _indent -= 2;
      indent();
      _out << ")" << std::endl;
      _indent -= 2;
    }

    void visit(ASTIdentifier& node) override
    {
      _out << "ASTIdentifier -> " << node.getQualifiedIdentifier();
      if (!node._info->_alias.empty()) {
        _out << " alias: " << node._info->_alias;
      }
      _out << std::endl;
    }

    void visit(ASTQualifiedAsterisk& node) override
    {
      _out << "ASTQualifiedAsterisk -> " << node.getQualifiedIdentifier() << std::endl;
    }

    void visit(ASTFromNode& node) override
    {
      _out << "ASTFrom" << std::endl;

      _indent += 2;
      for (const auto& reference : node._tableReferences) {
        indent();
        reference->accept(*this);
      }
      _indent -= 2;
    }

    void visit(ASTTableIdentifierNode& node) override
    {
      _out << "ASTTableIdentifier" << std::endl;

      _indent += 2;
      indent();
      _out << node._factor->getQualifiedIdentifier();
      if (!node._factor->_info->_alias.empty()) {
        _out << " alias: " << node._factor->_info->_alias;
      }
      _indent -= 2;
      _out << std::endl;
    }

    void visit(ASTTableSubqueryNode& node) override
    {
      _out << "ASTTableSubquery" << std::endl;

      _indent += 2;
      indent();
      node._query->accept(*this);
      _indent -= 2;
    }

    void visit(ASTCrossJoinNode& node) override
    {
      _out << "ASTCrossJoin" << std::endl;

      _indent += 2;
      indent();
      node._tableReference->accept(*this);
      indent();
      _out << "JOIN" << std::endl;
      indent();
      node._factor->accept(*this);
      _indent -= 2;
      _out << std::endl;
    }

    void visit(ASTNaturalJoinNode& node) override
    {
      _out << "ASTNaturalJoin" << std::endl;

      _indent += 2;
      indent();
      node._tableReference->accept(*this);
      indent();
      _out << "JOIN" << std::endl;
      indent();
      node._factor->accept(*this);
      _indent -= 2;
      _out << std::endl;
    }

    void printJoin(ASTJoinWithCondition& node)
    {
      _indent += 2;
      indent();
      node._tableReference->accept(*this);
      indent();
      _out << "JOIN" << std::endl;
      indent();
      node._factor->accept(*this);
      indent();
      _out << "ON" << std::endl;
      _indent += 2;
      indent();
      node._expression->accept(*this);
      _indent -= 4;
      _out << std::endl;
    }

    void visit(ASTInnerJoinNode& node) override
    {
      _out << "ASTInnerJoin" << std::endl;

      printJoin(node);
    }

    void visit(ASTLeftJoinNode& node) override
    {
      _out << "ASTLeftJoin" << std::endl;

      printJoin(node);
    }

    void visit(ASTRightJoinNode& node) override
    {
      _out << "ASTRightJoin" << std::endl;

      printJoin(node);
    }

    void visit(ASTFullJoinNode& node) override
    {
      _out << "ASTFullJoin" << std::endl;

      printJoin(node);
    }

    void visit(ASTWhereNode& node) override
    {
      _out << "ASTWhere" << std::endl;

      _indent += 2;
      indent();
      node._exp->accept(*this);
      _indent -= 2;
    }

    void visit(ASTGroupByNode& group) override
    {
      _out << "ASTGroupBy" << std::endl;
      _indent += 2;
      switch (group._quantifier) {
        case DISTINCT:
          indent();
          _out << "DISTINCT" << std::endl;
        case ALL:
          break;
      }
      for (const auto& node : group._identifiers) {
        indent();
        node->accept(*this);
      }
      _indent -= 2;
    }

    void visit(ASTHavingNode& node) override
    {
      _out << "ASTHaving" << std::endl;
      _indent += 2;
      node._exp->accept(*this);
      _indent -= 2;
    }

    void visit(ASTOrderByNode& node) override
    {
      _out << "ASTOrderBy" << std::endl;
      _indent += 2;
      for (const auto& order : node._orderExpressions) {
        indent();
        _out << orderToString(order.second) << std::endl;
        indent();
        order.first->accept(*this);
      }
      _indent -= 2;
    }

    void visit(ASTLimitNode& node) override
    {
      _out << "ASTLimit" << std::endl;
      _indent += 2;
      indent();
      node._limit->accept(*this);
      if (node._offset) {
        indent();
        node._offset->accept(*this);
      }
      _indent -= 2;
    }

  protected:
    void indent()
    {
      _out << std::string(_indent, ' ');
    }

    size_t _indent{0};
    std::ostream& _out;
  };
}
