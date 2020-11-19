//
//  block.h
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

#include "libcsvsqldb/inc.h"

#include "types.h"
#include "values.h"
#include "variant.h"

#include <memory>
#include <vector>


namespace csvsqldb
{
  using StoreType = char*;

  class Block;
  typedef Block* BlockPtr;
  typedef std::vector<BlockPtr> Blocks;

  class BlockProvider;
  using BlockProviderPtr = std::shared_ptr<BlockProvider>;

  class RowProvider;
  using RowProviderPtr = std::shared_ptr<RowProvider>;


  class CSVSQLDB_EXPORT BlockManager
  {
  public:
    BlockManager(size_t maxActiveBlocks = 100, size_t blockCapacity = 1 * 1024 * 1024);

    BlockManager(const BlockManager&) = delete;
    BlockManager& operator=(const BlockManager&) = delete;
    BlockManager(BlockManager&&) = delete;
    BlockManager& operator=(BlockManager&&) = delete;

    BlockPtr createBlock();
    BlockPtr getBlock(size_t blockNumber) const;

    void release(BlockPtr& block);
    void cache(const BlockPtr block);

    size_t getActiveBlocks() const;
    size_t getMaxActiveBlocks() const;
    size_t getMaxUsedBlocks() const;
    size_t getBlockCapacity() const;
    size_t getTotalBlocks() const;

  private:
    Blocks _blocks;
    size_t _blockCapacity;
    size_t _maxActiveBlocks;
    size_t _activeBlocks{0};
    size_t _maxCountActiveBlocks{0};
    size_t _totalBlocks{0};

    static size_t sBlockNumber;
  };


  class CSVSQLDB_EXPORT BlockProvider
  {
  public:
    virtual BlockPtr getNextBlock() = 0;
  };


  class CSVSQLDB_EXPORT RowProvider
  {
  public:
    virtual const Values* getNextRow() = 0;
  };


  class CSVSQLDB_EXPORT Block
  {
  public:
    Block(size_t blockNumber, size_t capacity);

    ~Block();

    Block(const Block&) = delete;
    Block& operator=(const Block&) = delete;
    Block(Block&&) = delete;
    Block& operator=(Block&&) = delete;

    Value* addValue(const Variant& value);
    Value* addValue(const Value& value);

    Value* addInt(int64_t num, bool isNull);
    Value* addReal(double num, bool isNull);
    Value* addString(const char* s, size_t len, bool isNull);
    Value* addBool(bool b, bool isNull);
    Value* addDate(const csvsqldb::Date& date, bool isNull);
    Value* addTime(const csvsqldb::Time& time, bool isNull);
    Value* addTimestamp(const csvsqldb::Timestamp& timestamp, bool isNull);

    void nextRow();

    void endBlocks();

    void markNextBlock();

    size_t getBlockNumber() const
    {
      return _blockNumber;
    }

    size_t offset() const
    {
      return _offset;
    }

    void moveOffset(size_t add)
    {
      _offset += add;
    }

    StoreType getRawBuffer()
    {
      return &_store[_offset];
    }

    bool hasSizeFor(size_t size) const;

  private:
    void markValue();

    size_t _capacity;
    StoreType _store;
    size_t _offset{0};
    size_t _blockNumber;

    friend class BlockIterator;
    friend class CachingBlockIterator;
    friend class SortingBlockIterator;
    friend class GroupingBlockIterator;
    friend class HashingBlockIterator;
    friend struct SortOperation;
    friend struct GroupingElement;
  };
}