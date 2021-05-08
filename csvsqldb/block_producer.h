//
//  block_producer.h
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

#include <csvsqldb/block.h>

#include <condition_variable>
#include <queue>
#include <thread>


namespace csvsqldb
{
  class CSVSQLDB_EXPORT BlockProducer : public BlockProvider
  {
  public:
    explicit BlockProducer(BlockManager& blockManager);

    BlockProducer(const BlockProducer&) = delete;
    BlockProducer(BlockProducer&&) = delete;
    BlockProducer& operator=(const BlockProducer&) = delete;
    BlockProducer& operator=(BlockProducer&&) = delete;

    ~BlockProducer();

    void start(std::function<void(BlockProducer& producer)> reader);

    BlockPtr getNextBlock() override;

    void nextRow();
    void addValue(const Variant& value);
    void addInt(int64_t num, bool isNull);
    void addReal(double num, bool isNull);
    void addString(const char* s, size_t len, bool isNull);
    void addBool(bool b, bool isNull);
    void addDate(const csvsqldb::Date& date, bool isNull);
    void addTime(const csvsqldb::Time& time, bool isNull);
    void addTimestamp(const csvsqldb::Timestamp& timestamp, bool isNull);

  private:
    void addBlock();
    void readBlocks();

    using Blocks = std::queue<BlockPtr>;

    BlockManager& _blockManager;
    Blocks _blocks;
    BlockPtr _block{nullptr};
    std::thread _readThread;
    std::condition_variable _cv;
    std::mutex _queueMutex;
    std::string _error;
    std::function<void(BlockProducer& producer)> _reader;
  };
}
