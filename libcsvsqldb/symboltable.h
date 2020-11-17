//
//  symboltable.h
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

#ifndef csvsqldb_symboltable_h
#define csvsqldb_symboltable_h

#include "libcsvsqldb/inc.h"

#include "database.h"
#include "types.h"

#include <memory.h>
#include <vector>


namespace csvsqldb
{
  enum eSymbolType { NOSYM, CALC, PLAIN, FUNCTION, TABLE, SUBQUERY };

  CSVSQLDB_EXPORT std::string symbolTypeToString(eSymbolType symbolType);


  struct SymbolInfo;
  typedef std::shared_ptr<SymbolInfo> SymbolInfoPtr;
  typedef std::vector<SymbolInfoPtr> SymbolInfos;

  class SymbolTable;
  typedef std::shared_ptr<SymbolTable> SymbolTablePtr;
  typedef std::vector<SymbolInfoPtr> SymbolMap;


  struct CSVSQLDB_EXPORT SymbolInfo {
    typedef std::shared_ptr<ASTExprNode> ASTExprNodePtr;

    SymbolInfo()
    : _symbolType(NOSYM)
    , _type(NONE)
    {
    }

    SymbolInfoPtr clone()
    {
      SymbolInfoPtr info = std::make_shared<SymbolInfo>();
      info->_symbolType = _symbolType;
      info->_type = _type;
      info->_name = _name;
      info->_alias = _alias;
      info->_prefix = _prefix;
      info->_identifier = _identifier;
      info->_relation = _relation;
      info->_qualifiedIdentifier = _qualifiedIdentifier;
      info->_expressionNode = _expressionNode;
      info->_subquery = _subquery;

      return info;
    }

    eSymbolType _symbolType;
    eType _type;
    std::string _name;  //!< real context name
    std::string _alias;
    std::string _prefix;
    std::string _identifier;  //!< parsed identifier name
    std::string _relation;
    std::string _qualifiedIdentifier;
    ASTExprNodePtr _expressionNode;
    SymbolTablePtr _subquery;
  };


  class CSVSQLDB_EXPORT SymbolTable
  : public std::enable_shared_from_this<SymbolTable>
  , private csvsqldb::noncopyable
  {
  public:
    SymbolTablePtr getParent()
    {
      return _parent;
    }

    void addSymbolsFrom(const SymbolTablePtr& symbolTable, const std::string& prefix);
    const SymbolInfoPtr& findSymbolNameForTable(const std::string& tableName, const std::string& columnName) const;
    bool hasSymbolNameForTable(const std::string& tableName, const std::string& columnName) const;
    const SymbolInfoPtr& findSymbol(const std::string& name) const;
    bool hasTableSymbol(const std::string& tableNameOrAlias) const;
    const SymbolInfoPtr& findTableSymbol(const std::string& tableNameOrAlias) const;
    const SymbolInfoPtr& findAliasedSymbol(const std::string& alias) const;
    SymbolInfos findAllSymbolsForTable(const std::string& tableName) const;

    bool hasSymbol(const std::string& name) const;
    bool addSymbol(const std::string& name, const SymbolInfoPtr& info);
    void replaceSymbol(const std::string& toreplace, const std::string& name, SymbolInfoPtr& info);

    void dump(size_t indent = 0) const;

    SymbolInfos getTables() const;

    void typeSymbolTable(const Database& database);

    std::string getNextAlias();

    static SymbolTablePtr createSymbolTable(const SymbolTablePtr& parent = SymbolTablePtr());

  private:
    SymbolTable(const SymbolTablePtr& parent)
    : _aliasCount(1)
    , _parent(parent)
    {
    }

    bool fillInfoFromTablePrefix(const Database& database, const SymbolInfos& tables, SymbolInfoPtr info);
    bool fillInfoFromTable(const Database& database, const SymbolInfos& tables, SymbolInfoPtr info);
    bool fillInfoFromSubquery(const Database& database, const SymbolInfos& subqueries, SymbolInfoPtr info);
    SymbolInfos getSubqueries() const;
    void fillWithTableData(const Database& database, const SymbolInfos& tables);

    SymbolInfoPtr internFindSymbol(const std::string& name);

    SymbolMap _symbols;
    uint32_t _aliasCount;
    SymbolTablePtr _parent;
  };
}

#endif
