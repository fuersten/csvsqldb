//
//  symboltable.cpp
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

#include "symboltable.h"
#include "sql_ast.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>


namespace csvsqldb
{
    
    std::string symbolTypeToString(eSymbolType symbolType)
    {
        switch(symbolType) {
            case CALC:
                return "CALC";
            case FUNCTION:
                return "FUNCTION";
            case NOSYM:
                return "NOSYM";
            case PLAIN:
                return "PLAIN";
            case TABLE:
                return "TABLE";
            case SUBQUERY:
                return "SUBQUERY";
        }
        throw std::runtime_error("just to make VC2013 happy");
    }
    
    SymbolTablePtr SymbolTable::createSymbolTable(const SymbolTablePtr& parent)
    {
        return SymbolTablePtr(new SymbolTable(parent));
    }
    
    void SymbolTable::addSymbolsFrom(const SymbolTablePtr& symbolTable, const std::string& prefix)
    {
        bool hasPrefix = !prefix.empty();
        for(const auto& info : symbolTable->_symbols) {
            if(info->_symbolType == TABLE) {
                SymbolInfoPtr newInfo = info->clone();
                if(hasPrefix) {
                    newInfo->_alias = prefix;
                    newInfo->_name = prefix;
                }
                addSymbol(newInfo->_name, newInfo);
            } else {
                if(hasPrefix) {
                    SymbolInfoPtr newInfo = info->clone();
                    if(newInfo->_qualifiedIdentifier.empty()) {
                        newInfo->_qualifiedIdentifier = prefix + "." + info->_name;
                    } else {
                        newInfo->_qualifiedIdentifier = prefix + "." + info->_qualifiedIdentifier;
                    }
                    newInfo->_alias = prefix;
                    addSymbol(info->_qualifiedIdentifier, newInfo);
                } else {
                    SymbolInfoPtr oldSymbol = internFindSymbol(info->_name);
                    if(oldSymbol) {
                        oldSymbol->_expressionNode = info->_expressionNode;
                        oldSymbol->_symbolType = info->_symbolType;
                        replaceSymbol(oldSymbol->_name, oldSymbol->_name, oldSymbol);
                    } else {
                        addSymbol(info->_qualifiedIdentifier, info->clone());
                    }
                }
            }
        }
    }
    
    const SymbolInfoPtr& SymbolTable::findSymbol(const std::string& name) const
    {
        auto iter =  std::find_if(_symbols.begin(), _symbols.end(), [&](const SymbolInfoPtr& symbol){ return symbol->_name == name; });
        if(iter != _symbols.end())
        {
            return *iter;
        }
        
        throw std::runtime_error("symbol '" + name + "' not found in scope");
    }
    
    SymbolInfoPtr SymbolTable::internFindSymbol(const std::string& name)
    {
        auto iter =  std::find_if(_symbols.begin(), _symbols.end(), [&](const SymbolInfoPtr& symbol){ return symbol->_name == name; });
        if(iter != _symbols.end())
        {
            return *iter;
        }
        
        return SymbolInfoPtr();
    }
    
    bool SymbolTable::hasSymbol(const std::string& name) const
    {
        auto iter =  std::find_if(_symbols.begin(), _symbols.end(), [&](const SymbolInfoPtr& symbol){ return symbol->_name == name; });
        if(iter != _symbols.end())
        {
            return true;
        }
        
        return false;
    }
    
    bool SymbolTable::addSymbol(const std::string& name, const SymbolInfoPtr& info)
    {
        if(hasSymbol(name))
        {
            return false;
        }
        _symbols.push_back(info);
        return true;
    }
    
    void SymbolTable::replaceSymbol(const std::string& toreplace, const std::string& name, SymbolInfoPtr& info)
    {
        auto iter =  std::find_if(_symbols.begin(), _symbols.end(), [&](const SymbolInfoPtr& symbol){ return symbol->_name == toreplace; });
        if(iter != _symbols.end())
        {
            *(*iter) = *info;
        } else {
            addSymbol(name, info);
        }
    }
    
    std::string SymbolTable::getNextAlias()
    {
        return "$alias_" + std::to_string(_aliasCount++);
    }
    
    SymbolInfos SymbolTable::getTables() const
    {
        SymbolInfos tables;
        for(const auto& info : _symbols) {
            if(info->_symbolType == TABLE) {
                tables.push_back(info);
            }
        }
        return tables;
    }
    
    SymbolInfos SymbolTable::getSubqueries() const
    {
        SymbolInfos subqueries;
        for(const auto& info : _symbols) {
            if(info->_symbolType == SUBQUERY) {
                subqueries.push_back(info);
            }
        }
        return subqueries;
    }
    
    bool SymbolTable::hasTableSymbol(const std::string& tableNameOrAlias) const
    {
        for(const auto& info : _symbols) {
            if(info->_symbolType == TABLE &&
               (info->_name == tableNameOrAlias ||
                info->_identifier == tableNameOrAlias ||
                info->_alias == tableNameOrAlias
                ) ) {
                   return true;
               }
        }
        return false;
    }
    
    const SymbolInfoPtr& SymbolTable::findTableSymbol(const std::string& tableNameOrAlias) const
    {
        for(const auto& info : _symbols) {
            if(info->_symbolType == TABLE &&
               (info->_name == tableNameOrAlias ||
                info->_identifier == tableNameOrAlias ||
                info->_alias == tableNameOrAlias
                ) ) {
                   return info;
               }
        }
        throw std::runtime_error("symbol '" + tableNameOrAlias + "' not found in scope");
    }
    
    bool SymbolTable::hasSymbolNameForTable(const std::string& tableName, const std::string& columnName) const
    {
        for(auto& info : _symbols) {
            if(info->_symbolType == PLAIN) {
                if(info->_relation == tableName &&
                   info->_identifier == columnName
                   ) {
                    return true;
                }
            }
        }
        return false;
    }
    
    const SymbolInfoPtr& SymbolTable::findSymbolNameForTable(const std::string& tableName, const std::string& columnName) const
    {
        for(auto& info : _symbols) {
            if(info->_symbolType == PLAIN) {
                if(info->_relation == tableName &&
                   info->_identifier == columnName
                   ) {
                    return info;
                }
            }
        }
        throw std::runtime_error("symbol '" + tableName + "." + columnName + "' not found in scope");
    }
    
    const SymbolInfoPtr& SymbolTable::findAliasedSymbol(const std::string& alias) const
    {
        for(auto& info : _symbols) {
            if(info->_alias == alias) {
                return info;
            }
        }
        throw std::runtime_error("alias symbol '" + alias + "' not found in scope");
    }
    
    SymbolInfos SymbolTable::findAllSymbolsForTable(const std::string& tableName) const
    {
        SymbolInfos symbols;
        
        for(auto& info : _symbols) {
            if(info->_symbolType == PLAIN && info->_relation == tableName) {
                symbols.push_back(info);
            }
        }
        return symbols;
    }
    
    void SymbolTable::fillWithTableData(const Database& database, const SymbolInfos& tables)
    {
        for(const auto& info : tables) {
            const TableData& tableData = database.getTable(info->_identifier);
            for(size_t n = 0; n < tableData.columnCount(); ++n) {
                if(!hasSymbol(info->_name + "." + tableData.getColumn(n)._name)) {
                    SymbolInfoPtr elementInfo = std::make_shared<SymbolInfo>();
                    elementInfo->_name = info->_name + "." + tableData.getColumn(n)._name;
                    elementInfo->_identifier = tableData.getColumn(n)._name;
                    elementInfo->_symbolType = PLAIN;
                    elementInfo->_type = tableData.getColumn(n)._type;
                    elementInfo->_relation = info->_name;
                    elementInfo->_qualifiedIdentifier = info->_name + "." + tableData.getColumn(n)._name;
                    
                    addSymbol(elementInfo->_name, elementInfo);
                }
            }
        }
    }
    
    bool SymbolTable::fillInfoFromTablePrefix(const Database& database, const SymbolInfos& tables, SymbolInfoPtr info)
    {
        std::string table = info->_prefix;
        std::string relation = info->_prefix;
        
        if(!database.hasTable(info->_prefix)) {
            // this could be some kind of alias, search for alias name of table
            table.clear();
            for(const auto& tableInfo : tables) {
                if(tableInfo->_symbolType == TABLE) {
                    if(tableInfo->_alias == info->_prefix) {
                        table = tableInfo->_identifier;
                        relation = tableInfo->_name;
                        break;
                    }
                }
            }
            if(table.empty()) {
                return false;
            }
        }
        const TableData& tableData = database.getTable(table);
        const TableData::Column& column = tableData.getColumn(info->_identifier);
        info->_type = column._type;
        info->_relation = relation;
        info->_qualifiedIdentifier = info->_prefix + "." + column._name;
        
        return true;
    }
    
    bool SymbolTable::fillInfoFromTable(const Database& database, const SymbolInfos& tables, SymbolInfoPtr info)
    {
        bool found = false;
        for(const auto& table : tables) {
            const TableData& tableData = database.getTable(table->_identifier);
            std::string ident = info->_identifier;
            if(!tableData.hasColumn(ident)) {
                for(auto& info2 : _symbols) {
                    if(info2->_alias == ident) {
                        ident = info2->_identifier;
                        break;
                    }
                }
            }
            
            if(tableData.hasColumn(ident)) {
                if(found) {
                    CSVSQLDB_THROW(SqlException, "ambigous symbol '" << (!info->_prefix.empty()?info->_prefix + "." : "") <<
                                   info->_identifier << "' found");
                }
                found = true;
                const TableData::Column& column = tableData.getColumn(ident);
                info->_type = column._type;
                info->_relation = table->_name;
                info->_qualifiedIdentifier = table->_name + "." + column._name;
            }
        }
        if(!found) {
            // this could be a CALC field
            try {
                const SymbolInfoPtr& info2 = findAliasedSymbol(info->_identifier);
                if(info2->_symbolType != CALC) {
                    return false;
                }
                info->_symbolType = CALC;
            } catch(const std::exception&) {
                return false;
            }
        }
        
        return true;
    }
    
    bool SymbolTable::fillInfoFromSubquery(const Database& database, const SymbolInfos& subqueries, SymbolInfoPtr info)
    {
        bool found = false;
        for(const auto& query : subqueries) {
            for(const auto& symbol : query->_subquery->_symbols) {
                if(symbol->_name == info->_name ||
                   (!info->_alias.empty() && info->_identifier == symbol->_identifier) ||
                   (query->_name == info->_prefix && info->_identifier == symbol->_identifier) ||
                   (info->_prefix.empty() && info->_identifier == symbol->_identifier) ) {
                    if(found) {
                        CSVSQLDB_THROW(SqlException, "ambigous symbol '" << (!info->_prefix.empty()?info->_prefix + "." : "") <<
                                       info->_identifier << "' found");
                    }
                    
                    info->_type = symbol->_type;
                    info->_qualifiedIdentifier = info->_name;
                    found = true;
                    break;
                }
            }
        }
        return found;
    }
    
    void SymbolTable::typeSymbolTable(const Database& database)
    {
        // first resolve child symbol tables
        for(auto& info : _symbols) {
            if(info->_symbolType == SUBQUERY) {
                info->_subquery->typeSymbolTable(database);
            }
        }
        
        SymbolInfos tables = getTables();
        fillWithTableData(database, tables);
        
        for(auto& info : _symbols) {
            if(info->_symbolType == PLAIN && info->_qualifiedIdentifier.empty()) {
                if(!info->_prefix.empty()) {
                    if(!fillInfoFromTablePrefix(database, tables, info)) {
                        if(!fillInfoFromSubquery(database, getSubqueries(), info)) {
                            CSVSQLDB_THROW(SqlException, "symbol '" << (!info->_prefix.empty()?info->_prefix + "." : "") <<
                                           info->_identifier << "' not found in any table");
                        }
                    }
                } else {
                    if(!fillInfoFromTable(database, tables, info)) {
                        if(!fillInfoFromSubquery(database, getSubqueries(), info)) {
                            CSVSQLDB_THROW(SqlException, "symbol '" << (!info->_prefix.empty()?info->_prefix + "." : "") <<
                                           info->_identifier << "' not found in any table");
                        }
                    }
                }
            }
        }
        for(auto& info : _symbols) {
            if(info->_symbolType == CALC && info->_expressionNode) {
                info->_type = info->_expressionNode->type();
            }
        }
    }
    
    void SymbolTable::dump(size_t indent) const
    {
        std::string s(indent, ' ');
        
        std::cout << s;
        std::cout << "Dumping symbol table:" << std::endl;
        for(auto symbol : _symbols)
        {
            std::cout << s;
            std::cout << symbol->_name << " symbol type: " << symbolTypeToString(symbol->_symbolType) << " type: " << typeToString(symbol->_type)
            << " identifier: " << symbol->_identifier << " prefix: " << symbol->_prefix << " name: " << symbol->_name << " alias: " << symbol->_alias << " relation: "
            << symbol->_relation << " qualified: " << symbol->_qualifiedIdentifier <<  std::endl;
            
            if(symbol->_symbolType == SUBQUERY) {
                symbol->_subquery->dump(indent+2);
            }
        }
    }
    
}
