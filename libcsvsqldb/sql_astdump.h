//
//  sql_astdump.h
//  csvsqldb
//
//  BSD 3-Clause License
//  Copyright (c) 2015, Lars-Christian Fürstenberg
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

#ifndef csvsqldb_astdump_h
#define csvsqldb_astdump_h

#include "libcsvsqldb/inc.h"

#include "sql_ast.h"

#include <iostream>
#include <memory>
#include <vector>


namespace csvsqldb
{
    
    class CSVSQLDB_EXPORT ASTNodeDumpVisitor : public ASTNodeVisitor
    {
    public:
        ASTNodeDumpVisitor()
        : _indent(0)
        {}
        
        virtual void visit(ASTCreateTableNode& node)
        {
            std::cout << "ASTCreateTable" << std::endl;
            _indent += 2;
            indent();
            std::cout << node._tableName << " -> " << (node._createIfNotExists? "create if not exists":"") << std::endl;
            _indent += 2;
            for(const auto& definition : node._columnDefinitions) {
                indent();
                std::cout << definition._name << " -> " << typeToString(definition._type) << " ";
                std::cout << (definition._length ? "length " + std::to_string(definition._length) + " " : "");
                std::cout << (definition._primaryKey ? "Primary Key ":"");
                std::cout << (definition._notNull ? "Not Null ":"");
                std::cout << (definition._unique ? "Unique ":"");
                if(!definition._defaultValue.empty()) {
                    std::cout << printType(definition._type, definition._defaultValue);
                }
                if(definition._check) {
                    std::cout << std::endl;
                    indent();
                    std::cout << "check" << std::endl;
                    _indent += 2;
                    indent();
                    definition._check->accept(*this);
                    _indent -= 2;
                }
                std::cout << std::endl;
            }
            for(const auto& constraint : node._tableConstraints) {
                if(!constraint._name.empty()) {
                    indent();
                    std::cout << constraint._name << std::endl;
                }
                if(!constraint._primaryKeys.empty()) {
                    indent();
                    std::cout << "primary keys [ ";
                    for(const auto& pk : constraint._primaryKeys) {
                        std::cout << pk << " ";
                    }
                    std::cout << " ]" << std::endl;
                }
                if(!constraint._uniqueKeys.empty()) {
                    indent();
                    std::cout << "unique keys [ ";
                    for(const auto& uk : constraint._uniqueKeys) {
                        std::cout << uk << " ";
                    }
                    std::cout << " ]" << std::endl;
                }
                if(constraint._check) {
                    indent();
                    std::cout << "check" << std::endl;
                    _indent += 2;
                    indent();
                    constraint._check->accept(*this);
                    _indent -= 2;
                }
                std::cout << std::endl;
            }
            _indent -= 4;
        }
        
        virtual void visit(ASTMappingNode& node)
        {
            std::cout << "ASTMappingNode" << std::endl;
        }
        
        virtual void visit(ASTAlterTableAddNode& node)
        {
            std::cout << "ASTAlterTableAdd" << std::endl;
            _indent += 2;
            indent();
            std::cout << node._definition._name << " -> " << typeToString(node._definition._type) << " ";
            std::cout << (node._definition._primaryKey ? "Primary Key":"");
            std::cout << (node._definition._notNull ? "Not Null":"");
            std::cout << (node._definition._unique ? "Unique":"");
            if(!node._definition._defaultValue.empty()) {
                std::cout << printType(node._definition._type, node._definition._defaultValue);
            }
            std::cout << std::endl;
            _indent -= 2;
        }
        
        virtual void visit(ASTAlterTableDropNode& node)
        {
            std::cout << "ASTAlterTableDrop" << std::endl;
            _indent += 2;
            indent();
            std::cout << "drop column " << node._columnName << std::endl;
            _indent -= 2;
        }
        
        virtual void visit(ASTDropTableNode& node)
        {
            std::cout << "ASTDropTable" << std::endl;
            _indent += 2;
            indent();
            std::cout << "drop table " << node._tableName << std::endl;
            _indent -= 2;
        }
        
        virtual void visit(ASTExplainNode& node)
        {
            std::cout << "ASTExplainNode" << std::endl;
        }
        
        virtual void visit(ASTUnionNode& node)
        {
            std::cout << "ASTUnion" << std::endl;
            _indent += 2;
            switch(node._quantifier) {
                case DISTINCT:
                    indent();
                    std::cout << "DISTINCT" << std::endl;
                case ALL:
                    break;
            }
            indent();
            node._lhs->accept(*this);
            indent();
            node._rhs->accept(*this);
            _indent -= 2;
        }
        
        virtual void visit(ASTQueryNode& node)
        {
            node._query->accept(*this);
        }
        
        virtual void visit(ASTQuerySpecificationNode& select)
        {
            std::cout << "ASTQuerySpecification" << std::endl;
            _indent += 2;
            switch(select._quantifier) {
                case DISTINCT:
                    indent();
                    std::cout << "DISTINCT" << std::endl;
                case ALL:
                    break;
            }
            _indent += 2;
            for(const auto& node: select._nodes) {
                indent();
                node->accept(*this);
            }
            _indent -= 2;
            select._tableExpression->accept(*this);
            _indent -= 2;
        }
        
        virtual void visit(ASTTableExpressionNode& node)
        {
            indent();
            std::cout << "ASTTableExpression" << std::endl;
            _indent += 2;
            indent();
            node._from->accept(*this);
            
            if(node._where) {
                indent();
                node._where->accept(*this);
            }
            if(node._group) {
                indent();
                node._group->accept(*this);
            }
            if(node._order) {
                indent();
                node._order->accept(*this);
            }
            if(node._limit) {
                indent();
                node._limit->accept(*this);
            }
            _indent -= 2;
        }
        
        virtual void visit(ASTBinaryNode& node)
        {
            std::cout << "ASTBinary" << std::endl;
            
            _indent += 2;
            indent();
            std::cout << operationTypeToString(node._op);
            std::cout << std::endl;
            _indent += 2;
            indent();
            node._lhs->accept(*this);
            indent();
            node._rhs->accept(*this);
            _indent -= 2;
            if(!node._symbolName.empty()) {
                indent();
                std::cout << "-> " << node._symbolName;
                const SymbolInfoPtr& info = node.symbolTable()->findSymbol(node._symbolName);
                if(!info->_alias.empty()) {
                    std::cout << " alias: " << info->_alias;
                }
                std::cout << std::endl;
            }
            _indent -= 2;
        }
        
        virtual void visit(ASTUnaryNode& node)
        {
            std::cout << "ASTUnary" << std::endl;
            _indent += 2;
            indent();
            std::cout << operationTypeToString(node._op);
            std::cout << std::endl;
            _indent += 2;
            indent();
            node._rhs->accept(*this);
            if(node._op == OP_CAST) {
                indent();
                std::cout << "AS " << typeToString(node._castType) << std::endl;
            }
            _indent -= 4;
        }
        
        virtual void visit(ASTValueNode& node)
        {
            std::cout << "ASTValue -> ";
            std::cout << typeToString(node._value._type) << " ";
            std::cout << printType(node._value);
            std::cout << std::endl;
        }
        
        virtual void visit(ASTLikeNode& node)
        {
            std::cout << "ASTLike" << std::endl;
            
            _indent += 2;
            indent();
            node._lhs->accept(*this);
            indent();
            std::cout << "'" << node._like << "'" << std::endl;
            _indent -= 2;
        }
        
        virtual void visit(ASTBetweenNode& node)
        {
            std::cout << "ASTBetween" << std::endl;
            
            _indent += 2;
            indent();
            node._lhs->accept(*this);
            indent();
            std::cout << "between" << std::endl;
            _indent += 2;
            indent();
            node._from->accept(*this);
            _indent -= 2;
            indent();
            std::cout << "and" << std::endl;
            _indent += 2;
            indent();
            node._to->accept(*this);
            _indent -= 4;
        }
        
        virtual void visit(ASTInNode& node)
        {
            std::cout << "ASTIn" << std::endl;
            
            _indent += 2;
            indent();
            node._lhs->accept(*this);
            indent();
            std::cout << "in" << std::endl;
            _indent += 2;
            for(const auto& exp : node._expressions) {
                indent();
                exp->accept(*this);
            }
            _indent -= 4;
        }
        
        virtual void visit(ASTFunctionNode& node)
        {
            std::cout << "ASTFunction" << std::endl;
            _indent += 2;
            indent();
            std::cout << node._function->getName() << "(" << std::endl;
            _indent += 2;
            for(const auto& param : node._parameters) {
                indent();
                param._exp->accept(*this);
            }
            _indent -= 2;
            indent();
            std::cout << ")" << std::endl;
            _indent -= 2;
        }
        
        virtual void visit(ASTAggregateFunctionNode& node)
        {
            std::cout << "ASTAggregateFunction" << std::endl;
            _indent += 2;
            indent();
            std::cout << aggregateFunctionToString(node._aggregateFunction) << "(" << std::endl;
            _indent += 2;
            if(node._aggregateFunction == COUNT_STAR) {
                indent();
                std::cout << "*" << std::endl;
            }
            switch(node._quantifier) {
                case DISTINCT:
                    indent();
                    std::cout << "DISTINCT ";
                case ALL:
                    break;
            }
            for(const auto& param : node._parameters) {
                param._exp->accept(*this);
            }
            _indent -= 2;
            indent();
            std::cout << ")" << std::endl;
            _indent -= 2;
        }
        
        virtual void visit(ASTIdentifier& node)
        {
            std::cout << "ASTIdentifier -> " << node.getQualifiedIdentifier();
            if(!node._info->_alias.empty()) {
                std::cout << " alias: " << node._info->_alias;
            }
            std::cout << std::endl;
        }
        
        virtual void visit(ASTQualifiedAsterisk& node)
        {
            std::cout << "ASTQualifiedAsterisk -> " << node.getQualifiedIdentifier() << std::endl;
        }
        
        virtual void visit(ASTFromNode& node)
        {
            std::cout << "ASTFrom" << std::endl;
            
            _indent += 2;
            for(const auto& reference : node._tableReferences) {
                indent();
                reference->accept(*this);
            }
            _indent -= 2;
        }
        
        virtual void visit(ASTTableIdentifierNode& node)
        {
            std::cout << "ASTTableIdentifier" << std::endl;
            
            _indent += 2;
            indent();
            std::cout << node._factor->getQualifiedIdentifier();
            if(!node._factor->_info->_alias.empty()) {
                std::cout << " alias: " << node._factor->_info->_alias;
            }
            _indent -= 2;
            std::cout << std::endl;
        }
        
        virtual void visit(ASTTableSubqueryNode& node)
        {
            std::cout << "ASTTableSubquery" << std::endl;
            
            _indent += 2;
            indent();
            node._query->accept(*this);
            _indent -= 2;
        }
        
        virtual void visit(ASTCrossJoinNode& node)
        {
            std::cout << "ASTCrossJoin" << std::endl;
            
            _indent += 2;
            indent();
            node._tableReference->accept(*this);
            indent();
            std::cout << "JOIN" << std::endl;
            indent();
            node._factor->accept(*this);
            _indent -= 2;
            std::cout << std::endl;
        }
        
        virtual void visit(ASTNaturalJoinNode& node)
        {
            std::cout << "ASTNaturalJoin" << std::endl;
            
            _indent += 2;
            indent();
            node._tableReference->accept(*this);
            indent();
            std::cout << "JOIN" << std::endl;
            indent();
            node._factor->accept(*this);
            _indent -= 2;
            std::cout << std::endl;
        }
        
        void printJoin(ASTJoinWithCondition& node)
        {
            _indent += 2;
            indent();
            node._tableReference->accept(*this);
            indent();
            std::cout << "JOIN" << std::endl;
            indent();
            node._factor->accept(*this);
            indent();
            std::cout << "ON" << std::endl;
            _indent += 2;
            indent();
            node._expression->accept(*this);
            _indent -= 4;
            std::cout << std::endl;
        }
        
        virtual void visit(ASTInnerJoinNode& node)
        {
            std::cout << "ASTInnerJoin" << std::endl;
            
            printJoin(node);
        }
        
        virtual void visit(ASTLeftJoinNode& node)
        {
            std::cout << "ASTLeftJoin" << std::endl;
            
            printJoin(node);
        }
        
        virtual void visit(ASTRightJoinNode& node)
        {
            std::cout << "ASTRightJoin" << std::endl;
            
            printJoin(node);
        }
        
        virtual void visit(ASTFullJoinNode& node)
        {
            std::cout << "ASTFullJoin" << std::endl;
            
            printJoin(node);
        }
        
        virtual void visit(ASTWhereNode& node)
        {
            std::cout << "ASTWhere" << std::endl;
            
            _indent += 2;
            indent();
            node._exp->accept(*this);
            _indent -= 2;
        }
        
        virtual void visit(ASTGroupByNode& group)
        {
            std::cout << "ASTGroupBy" << std::endl;
            _indent += 2;
            switch(group._quantifier) {
                case DISTINCT:
                    indent();
                    std::cout << "DISTINCT" << std::endl;
                case ALL:
                    break;
            }
            for(const auto& node: group._identifiers) {
                indent();
                node->accept(*this);
            }
            _indent -= 2;
        }
        
        virtual void visit(ASTHavingNode& node)
        {
            std::cout << "ASTHaving" << std::endl;
            _indent += 2;
            node._exp->accept(*this);
            _indent -= 2;
        }
        
        virtual void visit(ASTOrderByNode& node)
        {
            std::cout << "ASTOrderBy" << std::endl;
            _indent += 2;
            for(const auto& order: node._orderExpressions) {
                indent();
                std::cout << orderToString(order.second) << std::endl;
                indent();
                order.first->accept(*this);
            }
            _indent -= 2;
        }
        
        virtual void visit(ASTLimitNode& node)
        {
            std::cout << "ASTLimit" << std::endl;
            _indent += 2;
            indent();
            node._limit->accept(*this);
            if(node._offset) {
                indent();
                node._offset->accept(*this);
            }
            _indent -= 2;
        }
        
    protected:
        void indent()
        {
            std::cout << std::string(_indent, ' ');
        }
        
        size_t _indent;
    };
    
    
    class CSVSQLDB_EXPORT ASTNodeSQLPrintVisitor : public ASTNodeVisitor
    {
    public:
        ASTNodeSQLPrintVisitor()
        {}
        
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
            switch(op)
            {
                case OP_CAST:
                case OP_LIKE:
                case OP_BETWEEN:
                case OP_IN:
                    CSVSQLDB_THROW(SqlException, "Cannot print CAST,LIKE,BETWEEN or IN operators");
                default:
                    _ss << " " << operationTypeToString(op) << " ";
            }
        }
        
        virtual void visit(ASTExplainNode& node)
        {
            _ss << "EXPLAIN AST ";
            node._query->accept(*this);
        }
        
        virtual void visit(ASTMappingNode& node)
        {
        }
        
        virtual void visit(ASTQualifiedAsterisk& node)
        {
            _ss << node.getQualifiedQuotedIdentifier();
        }
        
        virtual void visit(ASTCreateTableNode& node)
        {
        }
        
        virtual void visit(ASTAlterTableAddNode& node)
        {
        }
        
        virtual void visit(ASTAlterTableDropNode& node)
        {
        }
        
        virtual void visit(ASTDropTableNode& node)
        {
        }
        
        virtual void visit(ASTQueryNode& node)
        {
            node._query->accept(*this);
        }
        
        virtual void visit(ASTUnionNode& node)
        {
            node._lhs->accept(*this);
            _ss << " UNION " << (node._quantifier == DISTINCT? "DISTINCT ":"");
            _ss << "(";
            node._rhs->accept(*this);
            _ss << ")";
        }
        
        virtual void visit(ASTQuerySpecificationNode& node)
        {
            _ss << "SELECT ";
            if(node._quantifier == DISTINCT) {
                _ss << "DISTINCT ";
            }
            bool first = true;
            for(const auto& exp : node._nodes) {
                if(!first) {
                    _ss << ",";
                } else {
                    first = false;
                }
                exp->accept(*this);
            }
            node._tableExpression->accept(*this);
        }
        
        virtual void visit(ASTTableExpressionNode& node)
        {
            node._from->accept(*this);
            if(node._where) {
                node._where->accept(*this);
            }
            if(node._group) {
                node._group->accept(*this);
            }
            if(node._order) {
                node._order->accept(*this);
            }
            if(node._limit) {
                node._limit->accept(*this);
            }
        }
        
        virtual void visit(ASTFromNode& node)
        {
            _ss << " FROM ";
            bool first = true;
            for(const auto& ref : node._tableReferences) {
                if(!first) {
                    _ss << ",";
                } else {
                    first = false;
                }
                ref->accept(*this);
            }
        }
        
        virtual void visit(ASTTableIdentifierNode& node)
        {
            node._factor->accept(*this);
        }
        
        virtual void visit(ASTTableSubqueryNode& node)
        {
            _ss << "(";
            node._query->accept(*this);
            _ss << ")";
        }
        
        virtual void visit(ASTCrossJoinNode& node)
        {
            node._tableReference->accept(*this);
            _ss << " CROSS JOIN ";
            node._factor->accept(*this);
            
        }
        
        virtual void visit(ASTNaturalJoinNode& node)
        {
            node._tableReference->accept(*this);
            _ss << " " << naturalJoinToString(node._joinType);
            _ss << " JOIN ";
            node._factor->accept(*this);
        }
        
        virtual void visit(ASTInnerJoinNode& node)
        {
            node._tableReference->accept(*this);
            _ss << " INNER JOIN ";
            node._factor->accept(*this);
            _ss << " ON ";
            node._expression->accept(*this);
        }
        
        virtual void visit(ASTLeftJoinNode& node)
        {
            node._tableReference->accept(*this);
            _ss << " LEFT OUTER JOIN ";
            node._factor->accept(*this);
            _ss << " ON ";
            node._expression->accept(*this);
        }
        
        virtual void visit(ASTRightJoinNode& node)
        {
            node._tableReference->accept(*this);
            _ss << " RIGHT OUTER JOIN ";
            node._factor->accept(*this);
            _ss << " ON ";
            node._expression->accept(*this);
        }
        
        virtual void visit(ASTFullJoinNode& node)
        {
            node._tableReference->accept(*this);
            _ss << " FULL OUTER JOIN ";
            node._factor->accept(*this);
            _ss << " ON ";
            node._expression->accept(*this);
        }
        
        virtual void visit(ASTWhereNode& node)
        {
            _ss << " WHERE ";
            node._exp->accept(*this);
        }
        
        virtual void visit(ASTGroupByNode& node)
        {
            _ss << " GROUP BY ";
            bool first = true;
            for(const auto& ident : node._identifiers) {
                if(!first) {
                    _ss << ",";
                } else {
                    first = false;
                }
                ident->accept(*this);
            }
        }
        
        virtual void visit(ASTHavingNode& node)
        {
        }
        
        virtual void visit(ASTOrderByNode& node)
        {
            _ss << " ORDER BY ";
            bool first = true;
            for(const auto& exp : node._orderExpressions) {
                if(!first) {
                    _ss << ",";
                } else {
                    first = false;
                }
                exp.first->accept(*this);
                switch(exp.second) {
                    case ASC:
                        _ss << " ASC";
                        break;
                    case DESC:
                        _ss << " DESC";
                        break;
                }
            }
        }
        
        virtual void visit(ASTLimitNode& node)
        {
            _ss << " LIMIT ";
            node._limit->accept(*this);
            if(node._offset) {
                _ss << " OFFSET ";
                node._offset->accept(*this);
            }
        }
        
        virtual void visit(ASTBinaryNode& node)
        {
            _ss << "(";
            node._lhs->accept(*this);
            printOp(node._op);
            node._rhs->accept(*this);
            _ss << ")";
            if(!node._symbolName.empty()) {
                const SymbolInfoPtr info = node.symbolTable()->findSymbol(node._symbolName);
                if(!info->_alias.empty()) {
                    _ss << " AS " << info->_alias;
                }
            }
        }
        
        virtual void visit(ASTUnaryNode& node)
        {
            if(node._op == OP_CAST) {
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
                if(!node._symbolName.empty()) {
                    const SymbolInfoPtr info = node.symbolTable()->findSymbol(node._symbolName);
                    if(!info->_alias.empty()) {
                        _ss << " AS " << info->_alias;
                    }
                }
            } else {
                printOp(node._op);
                node._rhs->accept(*this);
            }
        }
        
        virtual void visit(ASTValueNode& node)
        {
            if(node._value._type == STRING) {
                _ss << "'";
            }
            _ss << printType(node._value._type, node._value._value);
            if(node._value._type == STRING) {
                _ss << "'";
            }
        }
        
        virtual void visit(ASTLikeNode& node)
        {
            node._lhs->accept(*this);
            _ss << "'" << node._like << "'";
        }
        
        virtual void visit(ASTBetweenNode& node)
        {
            node._lhs->accept(*this);
            _ss << " between ";
            node._from->accept(*this);
            _ss << " and ";
            node._to->accept(*this);
        }
        
        virtual void visit(ASTInNode& node)
        {
            node._lhs->accept(*this);
            _ss << " in (";
            bool first = true;
            for(const auto& exp : node._expressions) {
                if(!first) {
                    _ss << ",";
                } else {
                    first = false;
                }
                exp->accept(*this);
            }
            _ss << ")";
        }
        
        virtual void visit(ASTFunctionNode& node)
        {
            _ss << node._function->getName() << "(";
            bool first = true;
            for(const auto& param : node._parameters) {
                if(!first) {
                    _ss << ",";
                } else {
                    first = false;
                }
                param._exp->accept(*this);
            }
            _ss << ")";
        }
        
        virtual void visit(ASTAggregateFunctionNode& node)
        {
            _ss << aggregateFunctionToString(node._aggregateFunction) << "(";
            bool first = true;
            if(node._aggregateFunction == COUNT_STAR) {
                _ss << "*";
            }
            _ss << (node._quantifier == DISTINCT? "DISTINCT ":"");
            for(const auto& param : node._parameters) {
                if(!first) {
                    _ss << ",";
                } else {
                    first = false;
                }
                param._exp->accept(*this);
            }
            _ss << ")";
            if(!node._symbolName.empty()) {
                const SymbolInfoPtr info = node.symbolTable()->findSymbol(node._symbolName);
                if(!info->_alias.empty()) {
                    _ss << " AS " << info->_alias;
                }
            }
        }
        
        virtual void visit(ASTIdentifier& node)
        {
            _ss << node.getQualifiedQuotedIdentifier();
            if(node._info && !node._info->_alias.empty() && node.getQualifiedQuotedIdentifier() != node._info->_alias) {
                _ss << " AS " << node._info->_alias;
            }
        }
        
    private:
        std::stringstream _ss;
    };
    
}

#endif
