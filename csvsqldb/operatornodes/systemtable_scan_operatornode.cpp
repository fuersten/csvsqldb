//
//  systemtable_scan_operatornode.cpp
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

#include "systemtable_scan_operatornode.h"


namespace csvsqldb
{
  SystemTableScanOperatorNode::SystemTableScanOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable,
                                                           const SymbolInfo& tableInfo)
  : ScanOperatorNode(context, symbolTable, tableInfo)
  {
  }

  void SystemTableScanOperatorNode::dump(std::ostream& stream) const
  {
    stream << "SystemTableScanOperatorNode(" << _tableInfo._identifier << ")\n";
  }

  const Values* SystemTableScanOperatorNode::getNextRow()
  {
    if (!_currentBlock) {
      _iterator = std::make_shared<BlockIterator>(_types, *this, _context._blockManager);
    }
    return _iterator->getNextRow();
  }

  BlockPtr SystemTableScanOperatorNode::getNextBlock()
  {
    _currentBlock = _context._blockManager.createBlock();

    if (_tableInfo._identifier == "SYSTEM_DUAL") {
      _currentBlock->addValue(Variant(false));
      _currentBlock->nextRow();
    } else if (_tableInfo._identifier == "SYSTEM_TABLES") {
      const auto& systemTables{_context._database.getSystemTables()};
      for (const auto& table : _context._database.getTables()) {
        _currentBlock->addValue(Variant(table.name()));
        _currentBlock->addValue(Variant(systemTables.isSystemTable(table.name())));
        _currentBlock->nextRow();
      }
    }
    _currentBlock->endBlocks();

    return _currentBlock;
  }
}
