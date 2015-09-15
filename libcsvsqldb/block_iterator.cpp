//
//  block.cpp
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

#include "block_iterator.h"
#include "base/hash_helper.h"

#include <algorithm>


namespace csvsqldb
{
    
    BlockIterator::BlockIterator(const Types& types, BlockProvider& blockProvider, BlockManager& blockManager)
    : _blockProvider(blockProvider)
    , _blockManager(blockManager)
    , _types(types)
    , _block(nullptr)
    , _previousBlock(nullptr)
    , _offset(0)
    , _endOffset(0)
    , _typeOffset(_types.begin())
    {
        _row.resize(_types.size());
    }
    
    BlockIterator::~BlockIterator()
    {
        _blockManager.release(_previousBlock);
        _blockManager.release(_block);
    }
    
    const Values* BlockIterator::getNextRow()
    {
        if(!_block) {
            _block = _blockProvider.getNextBlock();
            _offset = 0;
            _endOffset = _block->_offset;
        }
        if(_offset != 0) {
            if(*(&(_block->_store)[0] + _offset) != static_cast<char>(0xBB)) {
                CSVSQLDB_THROW(csvsqldb::Exception, "should be at row delimiter");
            }
            ++_offset;
            _typeOffset = _types.begin();
        }
        if(*(&(_block->_store)[0] + _offset) == static_cast<char>(0xDD)) {
            // no more rows left
            return nullptr;
        }
        
        // look for next block marker
        if(*(&(_block->_store)[0] + _offset) == static_cast<char>(0xCC)) {
            _blockManager.release(_previousBlock);
            _previousBlock = _block;
            _block = _blockProvider.getNextBlock();
            _offset = 0;
            _endOffset = _block->_offset;
        }
        
        if(_offset == _endOffset) {
            CSVSQLDB_THROW(csvsqldb::Exception, "should have found the end marker in the first place");
        }
        
        size_t index = 0;
        for(auto type : _types) {
            Value* val = getNextValue();
            if(!val) {
                CSVSQLDB_THROW(csvsqldb::Exception, "expected more values to fill the row (" << typeToString(type) << ")");
            }
            _row[index++] = val;
        }
        
        return &_row;
    }
    
    Value* BlockIterator::getNextValue()
    {
        if(_offset == _endOffset) {
            CSVSQLDB_THROW(csvsqldb::Exception, "expected more values, but already at end of block");
        }
        
        // look for next block marker
        if(*(&(_block->_store)[0] + _offset) == static_cast<char>(0xCC)) {
            _blockManager.release(_previousBlock);
            _previousBlock = _block;
            _block = _blockProvider.getNextBlock();
            _offset = 0;
            _endOffset = _block->_offset;
        }
        
        // look for next value marker
        if(*(&(_block->_store)[0] + _offset) != static_cast<char>(0xAA)) {
            CSVSQLDB_THROW(csvsqldb::Exception, "missing value separator");
        }
        ++_offset;
        
        Value* val = nullptr;
        eType type = *_typeOffset;
        switch(type) {
            case STRING:
            case DATE:
            case TIME:
            case TIMESTAMP:
            case BOOLEAN:
            case INT:
            case REAL:
                val = reinterpret_cast<Value*>(&(_block->_store)[0] + _offset);
                break;
            case NONE:
                CSVSQLDB_THROW(csvsqldb::Exception, "type not allowed "  << typeToString(type));
        }
        _offset += val->size();
        ++_typeOffset;
        return val;
    }
    
    
    CachingBlockIterator::CachingBlockIterator(const Types& types, RowProvider& rowProvider, BlockManager& blockManager)
    : _rowProvider(rowProvider)
    , _blockManager(blockManager)
    , _types(types)
    , _currentBlock(0)
    , _offset(0)
    , _endOffset(0)
    , _useCache(false)
    , _typeOffset(_types.begin())
    {
        _row.resize(_types.size());
    }
    
    CachingBlockIterator::~CachingBlockIterator()
    {
        for(auto& block : _blocks) {
            _blockManager.release(block);
        }
    }
    
    const Values* CachingBlockIterator::getNextRow()
    {
        if(_blocks.empty()) {
            getNextBlock();
            _currentBlock = 0;
        }
        const Values* row = nullptr;
        if(!_useCache) {
            row = _rowProvider.getNextRow();
            if(row) {
                for(const auto& value : *row) {
                    if(!_blocks[_currentBlock]->addValue(*value)) {
                        _blocks[_currentBlock]->markNextBlock();
                        getNextBlock();
                        _blocks[_currentBlock]->addValue(*value);
                    }
                }
                _blocks[_currentBlock]->nextRow();
            } else {
                _blocks[_currentBlock]->endBlocks();
            }
        } else {
            if(_offset != 0) {
                if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) != static_cast<char>(0xBB)) {
                    CSVSQLDB_THROW(csvsqldb::Exception, "should be at row delimiter");
                }
                ++_offset;
                _typeOffset = _types.begin();
            }
            if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) == static_cast<char>(0xDD)) {
                // no more rows left
                return nullptr;
            }
            
            // look for next block marker
            if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) == static_cast<char>(0xCC)) {
                getNextBlock();
            }
            
            if(_offset == _endOffset) {
                CSVSQLDB_THROW(csvsqldb::Exception, "should have found the end marker in the first place");
            }
            
            size_t index = 0;
            for(auto type : _types) {
                Value* val = getNextValue();
                if(!val) {
                    CSVSQLDB_THROW(csvsqldb::Exception, "expected more values to fill the row (" << typeToString(type) << ")");
                }
                _row[index++] = val;
            }
            row = &_row;
        }
        
        return row;
    }
    
    Value* CachingBlockIterator::getNextValue()
    {
        if(_offset == _endOffset) {
            CSVSQLDB_THROW(csvsqldb::Exception, "expected more values, but already at end of block");
        }
        
        // look for next block marker
        if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) == static_cast<char>(0xCC)) {
            getNextBlock();
        }
        
        // look for next value marker
        if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) != static_cast<char>(0xAA)) {
            CSVSQLDB_THROW(csvsqldb::Exception, "missing value separator");
        }
        ++_offset;
        
        Value* val = nullptr;
        eType type = *_typeOffset;
        switch(type) {
            case STRING:
            case DATE:
            case TIME:
            case TIMESTAMP:
            case BOOLEAN:
            case INT:
            case REAL:
                val = reinterpret_cast<Value*>(&(_blocks[_currentBlock]->_store)[0] + _offset);
                break;
            case NONE:
                CSVSQLDB_THROW(csvsqldb::Exception, "type not allowed "  << typeToString(type));
        }
        _offset += val->size();
        ++_typeOffset;
        return val;
    }
    
    void CachingBlockIterator::rewind()
    {
        _useCache = true;
        _currentBlock = 0;
        _offset = 0;
        _endOffset = _blocks[_currentBlock]->_offset;
    }
    
    void CachingBlockIterator::getNextBlock()
    {
        if(!_useCache) {
            _blocks.push_back(_blockManager.createBlock());
            ++_currentBlock;
            _offset = 0;
        } else {
            ++_currentBlock;
            _offset = 0;
            _endOffset = _blocks[_currentBlock]->_offset;
        }
    }
    
    
    struct SortOperation
    {
        SortOperation(const Types& types, const SortingBlockIterator::SortOrders& sortOrders, const Blocks& blocks)
        : _types(types)
        , _sortOrders(sortOrders)
        , _blocks(blocks)
        , _offset(0)
        , _endOffset(0)
        , _currentBlock(0)
        {
            _leftCompare.resize(_sortOrders.size());
            _rightCompare.resize(_sortOrders.size());
        }
        
        bool operator()(const BlockPosition& left, const BlockPosition& right)
        {
            _currentBlock = left._block;
            _offset = left._offset;
            _endOffset = _blocks[_currentBlock]->_offset;
            _typeOffset = _types.begin();
            
            for(size_t n = 0, count = 0; count < _types.size(); ++count) {
                Value* val = getNextValue();
                for(const auto& order : _sortOrders) {
                    if(order._index == count) {
                        _leftCompare[n] = val;
                    }
                    ++n;
                }
                n = 0;
            }
            
            _currentBlock = right._block;
            _offset = right._offset;
            _endOffset = _blocks[_currentBlock]->_offset;
            _typeOffset = _types.begin();
            
            for(size_t n = 0, count = 0; count < _types.size(); ++count) {
                Value* val = getNextValue();
                for(const auto& order : _sortOrders) {
                    if(order._index == count) {
                        _rightCompare[n] = val;
                    }
                    ++n;
                }
                n = 0;
            }
            
            size_t n = 0;
            for(const auto& order : _sortOrders) {
                if(order._order == ASC) {
                    bool result = *_leftCompare[n] < *_rightCompare[n];
                    if(!result) {
                        result = *_rightCompare[n] < *_leftCompare[n];
                        if(result) {
                            return false;
                        }
                    } else {
                        return true;
                    }
                } else if(order._order == DESC) {
                    bool result = *_rightCompare[n] < *_leftCompare[n];
                    if(!result) {
                        result = *_leftCompare[n] < *_rightCompare[n];
                        if(result) {
                            return false;
                        }
                    } else {
                        return true;
                    }
                }
                ++n;
            }
            
            return false;
        }
        
    private:
        Value* getNextValue()
        {
            if(_offset == _endOffset) {
                CSVSQLDB_THROW(csvsqldb::Exception, "expected more values, but already at end of block");
            }
            
            // look for next block marker
            if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) == static_cast<char>(0xCC)) {
                getNextBlock();
            }
            
            // look for next value marker
            if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) != static_cast<char>(0xAA)) {
                CSVSQLDB_THROW(csvsqldb::Exception, "missing value separator");
            }
            ++_offset;
            
            Value* val = nullptr;
            eType type = *_typeOffset;
            switch(type) {
                case STRING:
                case DATE:
                case TIME:
                case TIMESTAMP:
                case BOOLEAN:
                case INT:
                case REAL:
                    val = reinterpret_cast<Value*>(&(_blocks[_currentBlock]->_store)[0] + _offset);
                    break;
                case NONE:
                    CSVSQLDB_THROW(csvsqldb::Exception, "type not allowed "  << typeToString(type));
            }
            _offset += val->size();
            ++_typeOffset;
            return val;
        }
        
        void getNextBlock()
        {
            ++_currentBlock;
            _offset = 0;
            _endOffset = _blocks[_currentBlock]->_offset;
        }
        
        const Types& _types;
        Types::const_iterator _typeOffset;
        const SortingBlockIterator::SortOrders& _sortOrders;
        const Blocks& _blocks;
        size_t _offset;
        size_t _endOffset;
        size_t _currentBlock;
        Values _leftCompare;
        Values _rightCompare;
    };
    
    SortingBlockIterator::SortingBlockIterator(const Types& types, const SortOrders& sortOrders, RowProvider& rowProvider, BlockManager& blockManager)
    : _rowProvider(rowProvider)
    , _blockManager(blockManager)
    , _types(types)
    , _currentBlock(0)
    , _offset(0)
    , _endOffset(0)
    , _initialize(true)
    , _typeOffset(_types.begin())
    , _sortOrders(sortOrders)
    {
        _row.resize(_types.size());
    }
    
    SortingBlockIterator::~SortingBlockIterator()
    {
        for(auto& block : _blocks) {
            _blockManager.release(block);
        }
    }
    
    const Values* SortingBlockIterator::getNextRow()
    {
        if(_blocks.empty()) {
            getNextBlock();
            _currentBlock = 0;
        }
        const Values* row = nullptr;
        if(_initialize) {
            do {
                row = _rowProvider.getNextRow();
                if(row) {
                    BlockPosition bp = { _currentBlock, _offset };
                    _rows.push_back(bp);
                    bool firstValue = true;
                    for(const auto& value : *row) {
                        if(!_blocks[_currentBlock]->addValue(*value)) {
                            // if the block changes with the first value, we have to adjust the currentBlock and offset in the rows collection
                            if(firstValue) {
                                _rows.back() = { _currentBlock, _offset };
                                firstValue = false;
                            }
                            _blocks[_currentBlock]->markNextBlock();
                            getNextBlock();
                            _blocks[_currentBlock]->addValue(*value);
                        }
                    }
                    _blocks[_currentBlock]->nextRow();
                } else {
                    _blocks[_currentBlock]->endBlocks();
                }
                _offset = _blocks[_currentBlock]->offset();
            } while(row);
            _initialize = false;
            // here we have to sort the thing
            std::sort(_rows.begin(), _rows.end(), SortOperation(_types, _sortOrders, _blocks));
            _rowIter = _rows.begin();
        }
        
        if(_rowIter == _rows.end()) {
            // no more rows
            return nullptr;
        }
        
        _currentBlock = _rowIter->_block;
        _endOffset = _blocks[_currentBlock]->_offset;
        _offset = _rowIter->_offset;
        _typeOffset = _types.begin();
        
        if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) == static_cast<char>(0xDD)) {
            // no more rows left
            return nullptr;
        }
        
        size_t index = 0;
        for(auto type : _types) {
            Value* val = getNextValue();
            if(!val) {
                CSVSQLDB_THROW(csvsqldb::Exception, "expected more values to fill the row (" << typeToString(type) << ")");
            }
            _row[index++] = val;
        }
        row = &_row;
        
        ++_rowIter;
        
        return row;
    }
    
    Value* SortingBlockIterator::getNextValue()
    {
        if(_offset == _endOffset) {
            CSVSQLDB_THROW(csvsqldb::Exception, "expected more values, but already at end of block");
        }
        
        // look for next block marker
        if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) == static_cast<char>(0xCC)) {
            getNextBlock();
        }
        
        // look for next value marker
        if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) != static_cast<char>(0xAA)) {
            CSVSQLDB_THROW(csvsqldb::Exception, "missing value separator");
        }
        ++_offset;
        
        Value* val = nullptr;
        eType type = *_typeOffset;
        switch(type) {
            case STRING:
            case DATE:
            case TIME:
            case TIMESTAMP:
            case BOOLEAN:
            case INT:
            case REAL:
                val = reinterpret_cast<Value*>(&(_blocks[_currentBlock]->_store)[0] + _offset);
                break;
            case NONE:
                CSVSQLDB_THROW(csvsqldb::Exception, "type not allowed "  << typeToString(type));
        }
        _offset += val->size();
        ++_typeOffset;
        return val;
    }
    
    void SortingBlockIterator::getNextBlock()
    {
        if(_initialize) {
            _blocks.push_back(_blockManager.createBlock());
            ++_currentBlock;
            _offset = 0;
        } else {
            ++_currentBlock;
            _offset = 0;
            _endOffset = _blocks[_currentBlock]->_offset;
        }
    }
    
    
    GroupingElement::GroupingElement()
    {}
    
    GroupingElement::GroupingElement(const Variants& groupingValues)
    : _groupingValues(groupingValues)
    {}
    
    void GroupingElement::disconnect()
    {
        for(auto& value : _groupingValues) {
            value.disconnect();
        }
    }
    
    size_t GroupingElement::getHash() const
    {
        size_t seed = 0;
        
        for(const auto& value : _groupingValues) {
            csvsqldb::hash_combine(seed, value);
        }
        
        return seed;
    }
    
    bool GroupingElement::operator==(const GroupingElement& rhs) const
    {
        size_t groupSize = _groupingValues.size();
        
        for(size_t n = 0; n < groupSize; ++n) {
            if(!(_groupingValues[n] == rhs._groupingValues[n])) {
                return false;
            }
        }
        
        return true;
    }
    
    
    GroupingBlockIterator::GroupingBlockIterator(const Types& types, const csvsqldb::IndexVector groupingIndices,
                                                 const csvsqldb::IndexVector outputIndices,
                                                 AggregationFunctions& aggregateFunctions, RowProvider& rowProvider, BlockManager& blockManager)
    : _rowProvider(rowProvider)
    , _blockManager(blockManager)
    , _types(types)
    , _currentBlock(0)
    , _offset(0)
    , _endOffset(0)
    , _useCache(false)
    , _groupingIndices(groupingIndices)
    , _outputIndices(outputIndices)
    , _typeOffset(_types.begin())
    , _aggregateFunctions(aggregateFunctions)
    {
        _row.resize(_types.size());
    }
    
    GroupingBlockIterator::~GroupingBlockIterator()
    {
        for(auto& block : _blocks) {
            _blockManager.release(block);
        }
        for(auto& block : _aggrFuncBlocks) {
            _blockManager.release(block);
        }
    }
    
    const Values* GroupingBlockIterator::getNextRow()
    {
        if(_blocks.empty()) {
            getNextBlock();
            _currentBlock = 0;
        }
        const Values* row = nullptr;
        if(!_useCache) {
            size_t currentAggrFuncBlock = 0;
            _aggrFuncBlocks.push_back(_blockManager.createBlock());
            do {
                row = _rowProvider.getNextRow();
                if(row) {
                    GroupingElement element;
                    // TODO LCF: should be optimized to use only pointers into grouping values
                    for(auto index : _groupingIndices) {
                        element._groupingValues.push_back(valueToVariant(*(*row)[index]));
                    }
                    
                    GroupMap::iterator iter = _groupMap.find(element);
                    if(iter == _groupMap.end()) {
                        // group currently not contained - add new group
                        AggregationFunctionPtrs groupValues;
                        size_t count = 0;
                        for(auto n : _outputIndices) {
                            auto* aggrFunc = _aggregateFunctions[count]->clone(_aggrFuncBlocks[currentAggrFuncBlock]);
                            if(!aggrFunc) {
                                _aggrFuncBlocks.push_back(_blockManager.createBlock());
                                ++currentAggrFuncBlock;
                                aggrFunc = _aggregateFunctions[count]->clone(_aggrFuncBlocks[currentAggrFuncBlock]);
                            }
                            ++count;
                            aggrFunc->init();
                            aggrFunc->step(valueToVariant(*(*row)[n]));
                            groupValues.push_back(aggrFunc);
                        }
                        
                        element.disconnect();
                        _groupMap.emplace(element, groupValues);
                    } else {
                        // has to perform the aggregation
                        size_t count = 0;
                        for(auto n : _outputIndices) {
                            auto* aggrFunc = iter->second[count++];
                            aggrFunc->step(valueToVariant(*(*row)[n]));
                        }
                    }
                }
            } while(row);
            
            // TODO LCF: build new blocks, should be optimized to build only one block at a time
            getNextBlock();
            _currentBlock = 0;
            for(auto& groupElement : _groupMap) {
                for(auto* aggrFunc : groupElement.second) {
                    if(!aggrFunc->suppress()) {
                        const auto& finalVal = aggrFunc->finalize();
                        if(!_blocks[_currentBlock]->addValue(finalVal)) {
                            _blocks[_currentBlock]->markNextBlock();
                            getNextBlock();
                            _blocks[_currentBlock]->addValue(finalVal);
                        }
                    }
                }
                _blocks[_currentBlock]->nextRow();
            }
            _blocks[_currentBlock]->endBlocks();
            
            // reset to start of blocks
            _currentBlock = 0;
            _offset = 0;
            _endOffset = _blocks[_currentBlock]->_offset;
            _useCache = true;
        }
        _typeOffset = _types.begin();
        if(_offset != 0) {
            if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) != static_cast<char>(0xBB)) {
                CSVSQLDB_THROW(csvsqldb::Exception, "should be at row delimiter");
            }
            ++_offset;
            _typeOffset = _types.begin();
        }
        if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) == static_cast<char>(0xDD)) {
            // no more rows left
            return nullptr;
        }
        
        // look for next block marker
        if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) == static_cast<char>(0xCC)) {
            getNextBlock();
        }
        
        if(_offset == _endOffset) {
            CSVSQLDB_THROW(csvsqldb::Exception, "should have found the end marker in the first place");
        }
        
        size_t index = 0;
        for(auto type : _types) {
            Value* val = getNextValue();
            if(!val) {
                CSVSQLDB_THROW(csvsqldb::Exception, "expected more values to fill the row");
            }
            if(type != val->getType()) {
                CSVSQLDB_THROW(csvsqldb::Exception, "expected " << typeToString(type) << " but got " << typeToString(val->getType()));
            }
            _row[index++] = val;
        }
        row = &_row;
        
        return row;
    }
    
    Value* GroupingBlockIterator::getNextValue()
    {
        if(_offset == _endOffset) {
            CSVSQLDB_THROW(csvsqldb::Exception, "expected more values, but already at end of block");
        }
        
        // look for next block marker
        if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) == static_cast<char>(0xCC)) {
            getNextBlock();
        }
        
        // look for next value marker
        if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) != static_cast<char>(0xAA)) {
            CSVSQLDB_THROW(csvsqldb::Exception, "missing value separator");
        }
        ++_offset;
        
        Value* val = nullptr;
        eType type = *_typeOffset;
        switch(type) {
            case STRING:
            case DATE:
            case TIME:
            case TIMESTAMP:
            case BOOLEAN:
            case INT:
            case REAL:
                val = reinterpret_cast<Value*>(&(_blocks[_currentBlock]->_store)[0] + _offset);
                break;
            case NONE:
                CSVSQLDB_THROW(csvsqldb::Exception, "type not allowed "  << typeToString(type));
        }
        _offset += val->size();
        ++_typeOffset;
        return val;
    }
    
    void GroupingBlockIterator::getNextBlock()
    {
        if(!_useCache) {
            _blocks.push_back(_blockManager.createBlock());
            ++_currentBlock;
            _offset = 0;
        } else {
            ++_currentBlock;
            _offset = 0;
            _endOffset = _blocks[_currentBlock]->_offset;
        }
    }
    
    
    HashingBlockIterator::HashingBlockIterator(const Types& types, RowProvider& rowProvider, BlockManager& blockManager, size_t hashTableKeyPosition)
    : _rowProvider(rowProvider)
    , _blockManager(blockManager)
    , _types(types)
    , _currentBlock(0)
    , _offset(0)
    , _endOffset(0)
    , _useCache(false)
    , _hashTableKeyPosition(hashTableKeyPosition)
    , _typeOffset(_types.begin())
    {
        _row.resize(_types.size());
        _context._it = _hashTable.end();
        _context._end = _hashTable.end();
    }
    
    HashingBlockIterator::~HashingBlockIterator()
    {
        for(auto& block : _blocks) {
            _blockManager.release(block);
        }
    }
    
    void HashingBlockIterator::setContextForKeyValue(const Value& keyValue)
    {
        std::pair<HashTable::const_iterator, HashTable::const_iterator> range = _hashTable.equal_range(valueToVariant(keyValue));
        _context._it = range.first;
        _context._end = range.second;
    }
    
    const Values* HashingBlockIterator::getNextKeyValueRow()
    {
        if(_hashTable.empty()) {
            // fill the cache and hash table
            while(getNextRow()) {
                // empty
            }
            _useCache = true;
        }
        if(_context._it != _context._end) {
            _currentBlock = _context._it->second._block;
            _offset = _context._it->second._offset;
            _endOffset = _blocks[_currentBlock]->_offset;
            _typeOffset = _types.begin();
            ++_context._it;
            
            size_t index = 0;
            for(auto type : _types) {
                Value* val = getNextValue();
                if(!val) {
                    CSVSQLDB_THROW(csvsqldb::Exception, "expected more values to fill the row (" << typeToString(type) << ")");
                }
                _row[index++] = val;
            }
            return &_row;
        }
        return nullptr;
    }
    
    const Values* HashingBlockIterator::getNextRow()
    {
        if(_blocks.empty()) {
            getNextBlock();
            _currentBlock = 0;
        }
        const Values* row = nullptr;
        if(!_useCache) {
            row = _rowProvider.getNextRow();
            if(row) {
                BlockPosition pos;
                pos._block = _currentBlock;
                pos._offset = _blocks[_currentBlock]->_offset;
                
                size_t n = 0;
                for(const auto& value : *row) {
                    if(_hashTableKeyPosition == n) {
                        _hashTable.emplace(std::make_pair(valueToVariant(*value), pos));
                    }
                    if(!_blocks[_currentBlock]->addValue(*value)) {
                        _blocks[_currentBlock]->markNextBlock();
                        getNextBlock();
                        _blocks[_currentBlock]->addValue(*value);
                    }
                    ++n;
                }
                _blocks[_currentBlock]->nextRow();
            } else {
                _blocks[_currentBlock]->endBlocks();
            }
        } else {
            if(_offset != 0) {
                if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) != static_cast<char>(0xBB)) {
                    CSVSQLDB_THROW(csvsqldb::Exception, "should be at row delimiter");
                }
                ++_offset;
                _typeOffset = _types.begin();
            }
            if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) == static_cast<char>(0xDD)) {
                // no more rows left
                return nullptr;
            }
            
            // look for next block marker
            if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) == static_cast<char>(0xCC)) {
                getNextBlock();
            }
            
            if(_offset == _endOffset) {
                CSVSQLDB_THROW(csvsqldb::Exception, "should have found the end marker in the first place");
            }
            
            size_t index = 0;
            for(auto type : _types) {
                Value* val = getNextValue();
                if(!val) {
                    CSVSQLDB_THROW(csvsqldb::Exception, "expected more values to fill the row");
                }
                if(type != val->getType()) {
                    CSVSQLDB_THROW(csvsqldb::Exception, "expected " << typeToString(type) << " but got " << typeToString(val->getType()));
                }
                _row[index++] = val;
            }
            row = &_row;
        }
        
        return row;
    }
    
    Value* HashingBlockIterator::getNextValue()
    {
        if(_offset == _endOffset) {
            CSVSQLDB_THROW(csvsqldb::Exception, "expected more values, but already at end of block");
        }
        
        // look for next block marker
        if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) == static_cast<char>(0xCC)) {
            getNextBlock();
        }
        
        // look for next value marker
        if(*(&(_blocks[_currentBlock]->_store)[0] + _offset) != static_cast<char>(0xAA)) {
            CSVSQLDB_THROW(csvsqldb::Exception, "missing value separator");
        }
        ++_offset;
        
        Value* val = nullptr;
        eType type = *_typeOffset;
        switch(type) {
            case STRING:
            case DATE:
            case TIME:
            case TIMESTAMP:
            case BOOLEAN:
            case INT:
            case REAL:
                val = reinterpret_cast<Value*>(&(_blocks[_currentBlock]->_store)[0] + _offset);
                break;
            case NONE:
                CSVSQLDB_THROW(csvsqldb::Exception, "type not allowed "  << typeToString(type));
        }
        _offset += val->size();
        ++_typeOffset;
        return val;
    }
    
    void HashingBlockIterator::reset()
    {
        _useCache = false;
        _currentBlock = 0;
        _offset = 0;
        _endOffset = 0;
        _context._it = _hashTable.end();
        _context._end = _hashTable.end();
        for(auto& block : _blocks) {
            _blockManager.release(block);
        }
        _hashTable.clear();
    }
    
    void HashingBlockIterator::getNextBlock()
    {
        if(!_useCache) {
            _blocks.push_back(_blockManager.createBlock());
            ++_currentBlock;
            _offset = 0;
        } else {
            ++_currentBlock;
            _offset = 0;
            _endOffset = _blocks[_currentBlock]->_offset;
        }
    }
    
}
