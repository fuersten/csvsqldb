//
//  tabledata.h
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

#include "csvsqldb/inc.h"

#include "types.h"

#include <memory>
#include <vector>


namespace csvsqldb
{
  class ASTExprNode;
  using ASTExprNodePtr = std::shared_ptr<ASTExprNode>;
  class ASTCreateTableNode;
  using ASTCreateTableNodePtr = std::shared_ptr<ASTCreateTableNode>;

  class CSVSQLDB_EXPORT TableData
  {
  public:
    struct CSVSQLDB_EXPORT Column {
      Column() = default;

      std::string _name;
      eType _type{NONE};
      bool _primaryKey{false};
      bool _unique{false};
      bool _notNull{false};
      std::any _defaultValue;
      ASTExprNodePtr _check;
      uint32_t _length{0};
    };

    TableData(std::string tableName);

    void addColumn(const std::string name, eType type, bool primaryKey, bool unique, bool notNull, std::any defaultValue,
                   const ASTExprNodePtr& check, uint32_t length);
    void addConstraint(const csvsqldb::StringVector& primaryKey, const csvsqldb::StringVector& unique,
                       const ASTExprNodePtr& check);

    std::string asJson() const;
    static TableData fromJson(std::istream& stream);

    static TableData fromCreateAST(const ASTCreateTableNodePtr& createNode);

    const std::string& name() const
    {
      return _tableName;
    }
    size_t columnCount() const
    {
      return _columns.size();
    }
    const Column& getColumn(const std::string& name) const;
    const Column& getColumn(size_t index) const;
    bool hasColumn(const std::string& name) const;

  private:
    struct TableConstraint {
      csvsqldb::StringVector _primaryKey;
      csvsqldb::StringVector _unique;
      ASTExprNodePtr _check;
    };

    std::string _tableName;
    std::vector<Column> _columns;
    std::vector<TableConstraint> _constraints;
  };
}
