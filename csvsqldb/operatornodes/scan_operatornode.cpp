//
//  scan_operatornode.cpp
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

#include "scan_operatornode.h"


namespace csvsqldb
{
  ScanOperatorNode::ScanOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable,
                                     const SymbolInfo& tableInfo)
  : RowOperatorNode(context, symbolTable)
  , _tableData(_context._database.getTable(tableInfo._identifier))
  , _tableInfo(tableInfo)
  {
    for (size_t n = 0; n < _tableData.columnCount(); ++n) {
      _types.push_back(_tableData.getColumn(n)._type);
    }
  }

  void ScanOperatorNode::getColumnInfos(SymbolInfos& outputSymbols)
  {
    outputSymbols.clear();

    for (size_t n = 0; n < _tableData.columnCount(); ++n) {
      if (getSymbolTable().hasSymbolNameForTable(_tableInfo._name, _tableData.getColumn(n)._name)) {
        const SymbolInfoPtr& info = getSymbolTable().findSymbolNameForTable(_tableInfo._name, _tableData.getColumn(n)._name);
        outputSymbols.push_back(info);
      }
    }
  }
}
