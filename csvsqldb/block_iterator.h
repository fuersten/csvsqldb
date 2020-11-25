//
//  block_iterator.h
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

#include <csvsqldb/aggregation_functions.h>
#include <csvsqldb/block.h>

#include <unordered_map>


namespace csvsqldb
{
  struct CSVSQLDB_EXPORT GroupingElement {
    GroupingElement() = default;

    GroupingElement(const Variants& groupingValues);

    void disconnect();
    size_t getHash() const;
    bool operator==(const GroupingElement& rhs) const;

    Variants _groupingValues;
  };
}

namespace std
{
  template<>
  struct hash<csvsqldb::GroupingElement> {
    size_t operator()(csvsqldb::GroupingElement const& element) const
    {
      return element.getHash();
    }
  };
}

namespace csvsqldb
{
  class BlockIterator;
  using BlockIteratorPtr = std::shared_ptr<BlockIterator>;

  class CachingBlockIterator;
  using CachingBlockIteratorPtr = std::shared_ptr<CachingBlockIterator>;

  class SortingBlockIterator;
  using SortingBlockIteratorPtr = std::shared_ptr<SortingBlockIterator>;

  class GroupingBlockIterator;
  using GroupingBlockIteratorPtr = std::shared_ptr<GroupingBlockIterator>;

  class HashingBlockIterator;
  using HashingBlockIteratorPtr = std::shared_ptr<HashingBlockIterator>;

  struct CSVSQLDB_EXPORT BlockPosition {
    size_t _block{0};
    size_t _offset{0};
  };

  using HashTable = std::unordered_multimap<Variant, BlockPosition>;

  struct CSVSQLDB_EXPORT HashingBlockIteratorContext {
    HashTable::const_iterator _it;
    HashTable::const_iterator _end;
  };


  class CSVSQLDB_EXPORT BlockIterator
  {
  public:
    BlockIterator(const Types& types, BlockProvider& blockProvider, BlockManager& blockManager);

    ~BlockIterator();

    BlockIterator(const BlockIterator&) = delete;
    BlockIterator(BlockIterator&&) = delete;
    BlockIterator& operator=(const BlockIterator& iterator) = delete;
    BlockIterator& operator=(BlockIterator&&) = delete;

    const Values* getNextRow();

  private:
    const Value* getNextValue();
    void checkNextBlock();

    BlockProvider& _blockProvider;
    BlockManager& _blockManager;
    Values _row;
    Types _types;
    BlockPtr _block{nullptr};
    BlockPtr _previousBlock{nullptr};
    size_t _offset{0};
    size_t _endOffset{0};
    Types::iterator _typeOffset;
  };


  class CSVSQLDB_EXPORT CachingBlockIterator
  {
  public:
    CachingBlockIterator(const Types& types, RowProvider& rowProvider, BlockManager& blockManager);

    ~CachingBlockIterator();

    CachingBlockIterator(const CachingBlockIterator&) = delete;
    CachingBlockIterator(CachingBlockIterator&&) = delete;
    CachingBlockIterator& operator=(const CachingBlockIterator& iterator) = delete;
    CachingBlockIterator& operator=(CachingBlockIterator&&) = delete;

    const Values* getNextRow();

    void rewind();

  private:
    Value* getNextValue();
    void getNextBlock();

    RowProvider& _rowProvider;
    BlockManager& _blockManager;
    Values _row;
    Types _types;
    Blocks _blocks;
    size_t _currentBlock{0};
    size_t _offset{0};
    size_t _endOffset{0};
    bool _useCache{false};
    Types::iterator _typeOffset;
  };


  class CSVSQLDB_EXPORT SortingBlockIterator
  {
  public:
    struct SortOrder {
      size_t _index{0};
      eOrder _order{ASC};
    };

    using SortOrders = std::vector<SortOrder>;

    SortingBlockIterator(const Types& types, const SortOrders& sortOrders, RowProvider& rowProvider, BlockManager& blockManager);

    ~SortingBlockIterator();

    SortingBlockIterator(const SortingBlockIterator&) = delete;
    SortingBlockIterator(SortingBlockIterator&&) = delete;
    SortingBlockIterator& operator=(const SortingBlockIterator& iterator) = delete;
    SortingBlockIterator& operator=(SortingBlockIterator&&) = delete;

    const Values* getNextRow();

  private:
    typedef std::vector<BlockPosition> Rows;

    Value* getNextValue();
    void getNextBlock();

    RowProvider& _rowProvider;
    BlockManager& _blockManager;
    Values _row;
    const Types& _types;
    Blocks _blocks;
    size_t _currentBlock{0};
    size_t _offset{0};
    size_t _endOffset{0};
    Rows _rows;
    Rows::const_iterator _rowIter;
    bool _initialize{true};
    Types::const_iterator _typeOffset;
    const SortOrders _sortOrders;
  };


  class CSVSQLDB_EXPORT GroupingBlockIterator
  {
  public:
    GroupingBlockIterator(const Types& types, const csvsqldb::IndexVector groupingIndices,
                          const csvsqldb::IndexVector outputIndices, AggregationFunctions& aggregateFunctions,
                          RowProvider& rowProvider, BlockManager& blockManager);

    ~GroupingBlockIterator();

    GroupingBlockIterator(const GroupingBlockIterator&) = delete;
    GroupingBlockIterator(GroupingBlockIterator&&) = delete;
    GroupingBlockIterator& operator=(const GroupingBlockIterator& iterator) = delete;
    GroupingBlockIterator& operator=(GroupingBlockIterator&&) = delete;

    const Values* getNextRow();

  private:
    using AggregationFunctionPtrs = std::vector<AggregationFunction*>;
    using GroupMap = std::unordered_map<GroupingElement, AggregationFunctionPtrs>;

    Value* getNextValue();
    void getNextBlock();

    RowProvider& _rowProvider;
    BlockManager& _blockManager;
    Values _row;
    Types _types;
    Blocks _blocks;
    size_t _currentBlock{0};
    size_t _offset{0};
    size_t _endOffset{0};
    Blocks _aggrFuncBlocks;
    bool _useCache{false};
    GroupMap _groupMap;
    const csvsqldb::IndexVector _groupingIndices;
    const csvsqldb::IndexVector _outputIndices;
    Types::iterator _typeOffset;
    AggregationFunctions _aggregateFunctions;
  };


  class CSVSQLDB_EXPORT HashingBlockIterator
  {
  public:
    HashingBlockIterator(const Types& types, RowProvider& rowProvider, BlockManager& blockManager, size_t hashTableKeyPosition);

    ~HashingBlockIterator();

    HashingBlockIterator(const HashingBlockIterator&) = delete;
    HashingBlockIterator(HashingBlockIterator&&) = delete;
    HashingBlockIterator& operator=(const HashingBlockIterator& iterator) = delete;
    HashingBlockIterator& operator=(HashingBlockIterator&&) = delete;

    const Values* getNextRow();

    void setContextForKeyValue(const Value& keyValue);
    const Values* getNextKeyValueRow();

    void reset();

  private:
    const Value* getNextValue();
    void getNextBlock();

    RowProvider& _rowProvider;
    BlockManager& _blockManager;
    Values _row;
    Types _types;
    Blocks _blocks;
    size_t _currentBlock{0};
    size_t _offset{0};
    size_t _endOffset{0};
    bool _useCache{false};
    HashTable _hashTable;
    size_t _hashTableKeyPosition;
    HashingBlockIteratorContext _context;
    Types::iterator _typeOffset;
  };
}
