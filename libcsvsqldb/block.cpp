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

#include "block.h"

#include <algorithm>


namespace csvsqldb
{
    
    size_t BlockManager::sBlockNumber = 0;
    
    BlockManager::BlockManager(size_t maxActiveBlocks, size_t blockCapacity)
    : _blockCapacity(blockCapacity)
    , _maxActiveBlocks(maxActiveBlocks)
    , _activeBlocks(0)
    , _maxCountActiveBlocks(0)
    , _totalBlocks(0)
    {
    }
    
    BlockPtr BlockManager::createBlock()
    {
        ++_activeBlocks;
        ++_totalBlocks;
        _maxCountActiveBlocks = std::max(_activeBlocks, _maxCountActiveBlocks);
        if(_activeBlocks > _maxActiveBlocks) {
            CSVSQLDB_THROW(csvsqldb::Exception, "exceeded maximum number of active blocks");
        }
        BlockPtr block = new Block(++sBlockNumber, _blockCapacity);
        _blocks.push_back(block);
        
        return block;
    }
    
    BlockPtr BlockManager::getBlock(size_t blockNumber) const
    {
        Blocks::const_iterator iter = std::find_if(_blocks.begin(), _blocks.end(), [&](const BlockPtr block){ return blockNumber == block->getBlockNumber(); });
        if(iter == _blocks.end()) {
            CSVSQLDB_THROW(csvsqldb::Exception, "block with number " << blockNumber << " not found");
        }
        return *iter;
    }
    
    void BlockManager::release(BlockPtr& block)
    {
        if(block) {
            --_activeBlocks;
            if(_blocks.size()) {
                csvsqldb::remove(_blocks, block);
            }
            
            delete block;
            block = nullptr;
        }
    }
    
    void BlockManager::cache(const BlockPtr block)
    {
        if(block) {
            // TODO LCF: here we should mark the block as cachable and cache it later
            //           in the file system, if resources are low
            //           if the block is retrieved again, the cachable flag has to be removed
        }
    }
    
    size_t BlockManager::getActiveBlocks() const
    {
        return _activeBlocks;
    }
    
    size_t BlockManager::getMaxActiveBlocks() const
    {
        return _maxActiveBlocks;
    }
    
    size_t BlockManager::getMaxUsedBlocks() const
    {
        return _maxCountActiveBlocks;
    }
    
    size_t BlockManager::getBlockCapacity() const
    {
        return _blockCapacity;
    }
    
    size_t BlockManager::getTotalBlocks() const
    {
        return _totalBlocks;
    }
    
    
    Block::Block(size_t blockNumber, size_t capacity)
    : _capacity(capacity)
    , _offset(0)
    , _blockNumber(blockNumber)
    {
        _store = new char[capacity];
    }
    
    Block::~Block()
    {
        delete [] _store;
    }
    
    bool Block::hasSizeFor(size_t size) const
    {
        return _offset + size + 2 < _capacity;
    }
    
    Value* Block::addInt(int64_t num, bool isNull)
    {
        Value* val = nullptr;
        
        if(hasSizeFor(ValInt::baseSize())) {
            markValue();
            if(!isNull) {
                val = new (&_store[0] + _offset) ValInt(num);
            } else {
                val = new (&_store[0] + _offset) ValInt();
            }
            _offset += val->size();
        }
        return val;
    }
    
    Value* Block::addReal(double num, bool isNull)
    {
        Value* val = nullptr;
        
        if(hasSizeFor(ValDouble::baseSize())) {
            markValue();
            if(!isNull) {
                val = new (&_store[0] + _offset) ValDouble(num);
            } else {
                val = new (&_store[0] + _offset) ValDouble();
            }
            _offset += val->size();
        }
        return val;
    }
    
    Value* Block::addString(const char* s, size_t len, bool isNull)
    {
        Value* val = nullptr;
        
        if(hasSizeFor(ValString::baseSize()+len+1)) {
            markValue();
            if(!isNull) {
                char* psd = &_store[0] + _offset + sizeof(ValString);
                ::memcpy(psd, s, len+1);
                val = new (&_store[0] + _offset) ValString(psd, len);
            } else {
                val = new (&_store[0] + _offset) ValString();
            }
            _offset += val->size();
        }
        return val;
    }
    
    Value* Block::addBool(bool b, bool isNull)
    {
        Value* val = nullptr;
        
        if(hasSizeFor(ValBool::baseSize())) {
            markValue();
            if(!isNull) {
                val = new (&_store[0] + _offset) ValBool(b);
            } else {
                val = new (&_store[0] + _offset) ValBool();
            }
            _offset += val->size();
        }
        return val;
    }
    
    Value* Block::addDate(const csvsqldb::Date& date, bool isNull)
    {
        Value* val = nullptr;
        
        if(hasSizeFor(ValDate::baseSize())) {
            markValue();
            if(!isNull) {
                val = new (&_store[0] + _offset) ValDate(date);
            } else {
                val = new (&_store[0] + _offset) ValDate();
            }
            _offset += val->size();
        }
        return val;
    }
    
    Value* Block::addTime(const csvsqldb::Time& time, bool isNull)
    {
        Value* val = nullptr;
        
        if(hasSizeFor(ValTime::baseSize())) {
            markValue();
            if(!isNull) {
                val = new (&_store[0] + _offset) ValTime(time);
            } else {
                val = new (&_store[0] + _offset) ValTime();
            }
            _offset += val->size();
        }
        return val;
    }
    
    Value* Block::addTimestamp(const csvsqldb::Timestamp& timestamp, bool isNull)
    {
        Value* val = nullptr;
        
        if(hasSizeFor(ValTimestamp::baseSize())) {
            markValue();
            if(!isNull) {
                val = new (&_store[0] + _offset) ValTimestamp(timestamp);
            } else {
                val = new (&_store[0] + _offset) ValTimestamp();
            }
            _offset += val->size();
        }
        return val;
    }
    
    Value* Block::addValue(const Variant& value)
    {
        Value* val = nullptr;
        
        switch(value.getType()) {
            case INT:
                if(hasSizeFor(ValInt::baseSize())) {
                    markValue();
                    if(!value.isNull()) {
                        val = new (&_store[0] + _offset) ValInt(value.asInt());
                    } else {
                        val = new (&_store[0] + _offset) ValInt();
                    }
                }
                break;
            case REAL:
                if(hasSizeFor(ValDouble::baseSize())) {
                    markValue();
                    if(!value.isNull()) {
                        val = new (&_store[0] + _offset) ValDouble(value.asDouble());
                    } else {
                        val = new (&_store[0] + _offset) ValDouble();
                    }
                }
                break;
            case BOOLEAN:
                if(hasSizeFor(ValBool::baseSize())) {
                    markValue();
                    if(!value.isNull()) {
                        val = new (&_store[0] + _offset) ValBool(value.asBool());
                    } else {
                        val = new (&_store[0] + _offset) ValBool();
                    }
                }
                break;
            case DATE:
                if(hasSizeFor(ValDate::baseSize())) {
                    markValue();
                    if(!value.isNull()) {
                        val = new (&_store[0] + _offset) ValDate(value.asDate());
                    } else {
                        val = new (&_store[0] + _offset) ValDate();
                    }
                }
                break;
            case TIME:
                if(hasSizeFor(ValTime::baseSize())) {
                    markValue();
                    if(!value.isNull()) {
                        val = new (&_store[0] + _offset) ValTime(value.asTime());
                    } else {
                        val = new (&_store[0] + _offset) ValTime();
                    }
                }
                break;
            case TIMESTAMP:
                if(hasSizeFor(ValTimestamp::baseSize())) {
                    markValue();
                    if(!value.isNull()) {
                        val = new (&_store[0] + _offset) ValTimestamp(value.asTimestamp());
                    } else {
                        val = new (&_store[0] + _offset) ValTimestamp();
                    }
                }
                break;
            case STRING:
            {
                size_t len = ::strlen(value.asString());
                if(hasSizeFor(ValString::baseSize()+len+1)) {
                    markValue();
                    if(!value.isNull()) {
                        char* psd = &_store[0] + _offset + sizeof(ValString);
                        ::memcpy(psd, value.asString(), len+1);
                        val = new (&_store[0] + _offset) ValString(psd);
                    } else {
                        val = new (&_store[0] + _offset) ValString();
                    }
                }
                break;
            }
            case NONE:
                CSVSQLDB_THROW(csvsqldb::Exception, "type not allowed "  << typeToString(value.getType()));
        }
        if(val) {
            _offset += val->size();
        }
        return val;
    }
    
    Value* Block::addValue(const Value& value)
    {
        Value* val = nullptr;
        
        switch(value.getType()) {
            case INT:
                if(hasSizeFor(value.size())) {
                    markValue();
                    val = new (&_store[0] + _offset) ValInt(static_cast<const ValInt&>(value));
                }
                break;
            case REAL:
                if(hasSizeFor(value.size())) {
                    markValue();
                    val = new (&_store[0] + _offset) ValDouble(static_cast<const ValDouble&>(value));
                }
                break;
            case BOOLEAN:
                if(hasSizeFor(value.size())) {
                    markValue();
                    val = new (&_store[0] + _offset) ValBool(static_cast<const ValBool&>(value));
                }
                break;
            case DATE:
                if(hasSizeFor(value.size())) {
                    markValue();
                    val = new (&_store[0] + _offset) ValDate(static_cast<const ValDate&>(value));
                }
                break;
            case TIME:
                if(hasSizeFor(value.size())) {
                    markValue();
                    val = new (&_store[0] + _offset) ValTime(static_cast<const ValTime&>(value));
                }
                break;
            case TIMESTAMP:
                if(hasSizeFor(value.size())) {
                    markValue();
                    val = new (&_store[0] + _offset) ValTimestamp(static_cast<const ValTimestamp&>(value));
                }
                break;
            case STRING:
            {
                if(hasSizeFor(value.size())) {
                    markValue();
                    if(!value.isNull()) {
                        char* psd = &_store[0] + _offset + sizeof(ValString);
                        size_t len = static_cast<const ValString&>(value).length();
                        ::memcpy(psd, static_cast<const ValString&>(value).asString(), len+1);
                        val = new (&_store[0] + _offset) ValString(psd);
                    } else {
                        val = new (&_store[0] + _offset) ValString();
                    }
                }
                break;
            }
            case NONE:
                CSVSQLDB_THROW(csvsqldb::Exception, "type not allowed "  << typeToString(value.getType()));
        }
        if(val) {
            _offset += val->size();
        }
        return val;
    }
    
    void Block::nextRow()
    {
        *(&_store[0] + _offset) = static_cast<char>(0xBB);
        ++_offset;
    }
    
    void Block::markValue()
    {
        *(&_store[0] + _offset) = static_cast<char>(0xAA);
        ++_offset;
    }
    
    void Block::markNextBlock()
    {
        *(&_store[0] + _offset) = static_cast<char>(0xCC);
        ++_offset;
    }
    
    void Block::endBlocks()
    {
        *(&_store[0] + _offset) = static_cast<char>(0xDD);
        ++_offset;
    }
    
}
