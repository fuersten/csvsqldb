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

#include <csvsqldb/inc.h>

#include <csvsqldb/types.h>
#include <csvsqldb/values.h>
#include <csvsqldb/variant.h>

#include <memory>
#include <vector>


namespace csvsqldb
{
  using StoreType = char;

  class Block;
  using BlockPtr = Block*;
  using Blocks = std::vector<BlockPtr>;

  class BlockProvider;
  using BlockProviderPtr = std::shared_ptr<BlockProvider>;

  class RowProvider;
  using RowProviderPtr = std::shared_ptr<RowProvider>;

  static constexpr size_t sDefaultMaxActiveBlocks = 100;
  static constexpr size_t sDefaultBlockCapacity = 1 * 1024 * 1024;

  static constexpr StoreType sValueMarker = static_cast<char>(0xAA);
  static constexpr StoreType sRowMarker = static_cast<char>(0xBB);
  static constexpr StoreType sBlockMarker = static_cast<char>(0xCC);
  static constexpr StoreType sEndMarker = static_cast<char>(0xDD);


  class CSVSQLDB_EXPORT BlockManager
  {
  public:
    BlockManager(size_t maxActiveBlocks = sDefaultMaxActiveBlocks, size_t blockCapacity = sDefaultBlockCapacity);

    ~BlockManager();

    BlockManager(const BlockManager&) = delete;
    BlockManager& operator=(const BlockManager&) = delete;
    BlockManager(BlockManager&&) = delete;
    BlockManager& operator=(BlockManager&&) = delete;

    BlockPtr createBlock();
    BlockPtr getBlock(size_t blockNumber) const;

    void release(BlockPtr& block);

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

    inline static size_t sBlockNumber{0};
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

    ~Block() = default;

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

    inline size_t offset() const
    {
      return _offset;
    }

    inline StoreType* getRawBuffer()
    {
      return &_store[_offset];
    }

    inline StoreType* getRawBuffer(size_t offset)
    {
#if defined BLOCK_RANGE_CHECK
      if (offset >= _offset) {
        CSVSQLDB_THROW(csvsqldb::Exception, "block offset out of range");
      }
#endif
      return &_store[offset];
    }

    inline bool isValue(size_t offset) const
    {
#if defined BLOCK_RANGE_CHECK
      if (offset >= _offset) {
        CSVSQLDB_THROW(csvsqldb::Exception, "block offset out of range");
      }
#endif
      return (*(&_store[0] + offset) == sValueMarker);
    }

    inline bool isRow(size_t offset) const
    {
#if defined BLOCK_RANGE_CHECK
      if (offset >= _offset) {
        CSVSQLDB_THROW(csvsqldb::Exception, "block offset out of range");
      }
#endif
      return (*(&_store[0] + offset) == sRowMarker);
    }

    inline bool isBlock(size_t offset) const
    {
#if defined BLOCK_RANGE_CHECK
      if (offset >= _offset) {
        CSVSQLDB_THROW(csvsqldb::Exception, "block offset out of range");
      }
#endif
      return (*(&_store[0] + offset) == sBlockMarker);
    }

    inline bool isEnd(size_t offset) const
    {
#if defined BLOCK_RANGE_CHECK
      if (offset >= _offset) {
        CSVSQLDB_THROW(csvsqldb::Exception, "block offset out of range");
      }
#endif
      return (*(&_store[0] + offset) == sEndMarker);
    }

    inline bool hasSizeFor(size_t size) const
    {
      if (size >= std::numeric_limits<size_t>::max() - 2) {
        return false;
      }
      return _offset + size + 2 < _capacity;
    }

    template<typename T>
    T* allocate()
    {
      if (!hasSizeFor(sizeof(T))) {
        return nullptr;
      }
      T* t = new (&_store[_offset]) T();
      _offset += sizeof(T);
      return t;
    }

    template<typename T, typename A>
    T* allocate(A arg)
    {
      if (!hasSizeFor(sizeof(T))) {
        return nullptr;
      }
      T* t = new (&_store[_offset]) T(arg);
      _offset += sizeof(T);
      return t;
    }

  private:
    void markValue();

    size_t _capacity{0};
    size_t _offset{0};
    size_t _blockNumber{0};
    std::unique_ptr<StoreType[]> _store;
  };
}
