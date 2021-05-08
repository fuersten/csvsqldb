//
//  table_scan_operatornode.h
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

#include <csvsqldb/base/csv_parser.h>
#include <csvsqldb/block_iterator.h>
#include <csvsqldb/operatornodes/scan_operatornode.h>

#include <condition_variable>
#include <queue>
#include <iostream>


namespace csvsqldb
{
  class CSVSQLDB_EXPORT BlockReader : public csvsqldb::csv::CSVParserCallback
  {
  public:
    explicit BlockReader(BlockManager& blockManager);

    ~BlockReader() override;

    void initialize(std::unique_ptr<csvsqldb::csv::CSVParser> csvparser);

    bool valid() const
    {
      return _csvparser.get();
    }

    BlockPtr getNextBlock();

    /// CSVParserCallback interface
    void onLong(int64_t num, bool isNull) override;

    void onDouble(double num, bool isNull) override;

    void onString(const char* s, size_t len, bool isNull) override;

    void onDate(const csvsqldb::Date& date, bool isNull) override;

    void onTime(const csvsqldb::Time& time, bool isNull) override;

    void onTimestamp(const csvsqldb::Timestamp& timestamp, bool isNull) override;

    void onBoolean(bool boolean, bool isNull) override;

  private:
    using Blocks = std::queue<BlockPtr>;

    void addBlock();
    void readBlocks();

    std::unique_ptr<csvsqldb::csv::CSVParser> _csvparser;
    BlockManager& _blockManager;
    Blocks _blocks;
    BlockPtr _block{nullptr};
    std::thread _readThread;
    std::condition_variable _cv;
    std::mutex _queueMutex;
    bool _continue{true};
    std::string _error;
  };


  class CSVSQLDB_EXPORT TableScanOperatorNode
  : public ScanOperatorNode
  , public BlockProvider
  {
  public:
    TableScanOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable, const SymbolInfo& tableInfo);

    void dump(std::ostream& stream) const override;

    const Values* getNextRow() override;

    /// BlockProvider interface
    BlockPtr getNextBlock() override;

  private:
    void initializeBlockReader();

    std::unique_ptr<std::istream> _stream;
    csvsqldb::csv::CSVParserContext _csvContext;
    BlockIteratorPtr _iterator;
    BlockReader _blockReader;
  };
}
