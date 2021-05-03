//
//  block.cpp
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

#include "block.h"

#include <algorithm>


namespace csvsqldb
{
  BlockManager::BlockManager(size_t maxActiveBlocks, size_t blockCapacity)
  : _blockCapacity(blockCapacity)
  , _maxActiveBlocks(maxActiveBlocks)
  , _activeBlocks(0)
  , _maxCountActiveBlocks(0)
  , _totalBlocks(0)
  {
  }

  BlockManager::~BlockManager()
  {
    for (auto* block : _blocks) {
      release(block);
    }
  }

  BlockPtr BlockManager::createBlock()
  {
    auto maxCountActiveBlocks = std::max(_activeBlocks + 1, _maxCountActiveBlocks);
    if ((_activeBlocks + 1) > _maxActiveBlocks) {
      CSVSQLDB_THROW(csvsqldb::Exception, "exceeded maximum number of active blocks");
    }
    BlockPtr block = new Block(++sBlockNumber, _blockCapacity);
    _blocks.push_back(block);
    ++_activeBlocks;
    ++_totalBlocks;
    _maxCountActiveBlocks = maxCountActiveBlocks;

    return block;
  }

  BlockPtr BlockManager::getBlock(size_t blockNumber) const
  {
    Blocks::const_iterator iter =
      std::find_if(_blocks.begin(), _blocks.end(), [&](const BlockPtr& block) { return blockNumber == block->getBlockNumber(); });
    if (iter == _blocks.end()) {
      CSVSQLDB_THROW(csvsqldb::Exception, "block with number " << blockNumber << " not found");
    }
    return *iter;
  }

  void BlockManager::release(BlockPtr& block)
  {
    if (block) {
      --_activeBlocks;
      if (!_blocks.empty()) {
        _blocks.erase(std::remove_if(_blocks.begin(), _blocks.end(),
                                     [&block](const auto& b) { return block->getBlockNumber() == b->getBlockNumber(); }),
                      _blocks.end());
      }

      delete block;
      block = nullptr;
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
  , _store{new StoreType[capacity]}
  {
  }

  Value* Block::addInt(int64_t num, bool isNull)
  {
    Value* val = nullptr;

    if (hasSizeFor(ValInt::getBaseSize())) {
      markValue();
      if (!isNull) {
        val = new (&_store[0] + _offset) ValInt(num);
      } else {
        val = new (&_store[0] + _offset) ValInt();
      }
      _offset += ValInt::getBaseSize();
    }
    return val;
  }

  Value* Block::addReal(double num, bool isNull)
  {
    Value* val = nullptr;

    if (hasSizeFor(ValDouble::getBaseSize())) {
      markValue();
      if (!isNull) {
        val = new (&_store[0] + _offset) ValDouble(num);
      } else {
        val = new (&_store[0] + _offset) ValDouble();
      }
      _offset += ValDouble::getBaseSize();
    }
    return val;
  }

  Value* Block::addString(const char* s, size_t len, bool isNull)
  {
    Value* val = nullptr;

    if (hasSizeFor(ValString::getBaseSize() + len + 1)) {
      markValue();
      if (!isNull) {
        char* psd = &_store[0] + _offset + sizeof(ValString);
        ::memcpy(psd, s, len + 1);
        psd[len] = 0;
        val = new (&_store[0] + _offset) ValString(psd);
      } else {
        val = new (&_store[0] + _offset) ValString();
      }
      _offset += val->getSize();
    }
    return val;
  }

  Value* Block::addBool(bool b, bool isNull)
  {
    Value* val = nullptr;

    if (hasSizeFor(ValBool::getBaseSize())) {
      markValue();
      if (!isNull) {
        val = new (&_store[0] + _offset) ValBool(b);
      } else {
        val = new (&_store[0] + _offset) ValBool();
      }
      _offset += ValBool::getBaseSize();
    }
    return val;
  }

  Value* Block::addDate(const csvsqldb::Date& date, bool isNull)
  {
    Value* val = nullptr;

    if (hasSizeFor(ValDate::getBaseSize())) {
      markValue();
      if (!isNull) {
        val = new (&_store[0] + _offset) ValDate(date);
      } else {
        val = new (&_store[0] + _offset) ValDate();
      }
      _offset += ValDate::getBaseSize();
    }
    return val;
  }

  Value* Block::addTime(const csvsqldb::Time& time, bool isNull)
  {
    Value* val = nullptr;

    if (hasSizeFor(ValTime::getBaseSize())) {
      markValue();
      if (!isNull) {
        val = new (&_store[0] + _offset) ValTime(time);
      } else {
        val = new (&_store[0] + _offset) ValTime();
      }
      _offset += ValTime::getBaseSize();
    }
    return val;
  }

  Value* Block::addTimestamp(const csvsqldb::Timestamp& timestamp, bool isNull)
  {
    Value* val = nullptr;

    if (hasSizeFor(ValTimestamp::getBaseSize())) {
      markValue();
      if (!isNull) {
        val = new (&_store[0] + _offset) ValTimestamp(timestamp);
      } else {
        val = new (&_store[0] + _offset) ValTimestamp();
      }
      _offset += ValTimestamp::getBaseSize();
    }
    return val;
  }

  Value* Block::addValue(const Variant& value)
  {
    Value* val = nullptr;

    switch (value.getType()) {
      case INT:
        if (hasSizeFor(ValInt::getBaseSize())) {
          markValue();
          if (!value.isNull()) {
            val = new (&_store[0] + _offset) ValInt(value.asInt());
          } else {
            val = new (&_store[0] + _offset) ValInt();
          }
          _offset += ValInt::getBaseSize();
        }
        break;
      case REAL:
        if (hasSizeFor(ValDouble::getBaseSize())) {
          markValue();
          if (!value.isNull()) {
            val = new (&_store[0] + _offset) ValDouble(value.asDouble());
          } else {
            val = new (&_store[0] + _offset) ValDouble();
          }
          _offset += ValDouble::getBaseSize();
        }
        break;
      case BOOLEAN:
        if (hasSizeFor(ValBool::getBaseSize())) {
          markValue();
          if (!value.isNull()) {
            val = new (&_store[0] + _offset) ValBool(value.asBool());
          } else {
            val = new (&_store[0] + _offset) ValBool();
          }
          _offset += ValBool::getBaseSize();
        }
        break;
      case DATE:
        if (hasSizeFor(ValDate::getBaseSize())) {
          markValue();
          if (!value.isNull()) {
            val = new (&_store[0] + _offset) ValDate(value.asDate());
          } else {
            val = new (&_store[0] + _offset) ValDate();
          }
          _offset += ValDate::getBaseSize();
        }
        break;
      case TIME:
        if (hasSizeFor(ValTime::getBaseSize())) {
          markValue();
          if (!value.isNull()) {
            val = new (&_store[0] + _offset) ValTime(value.asTime());
          } else {
            val = new (&_store[0] + _offset) ValTime();
          }
          _offset += ValTime::getBaseSize();
        }
        break;
      case TIMESTAMP:
        if (hasSizeFor(ValTimestamp::getBaseSize())) {
          markValue();
          if (!value.isNull()) {
            val = new (&_store[0] + _offset) ValTimestamp(value.asTimestamp());
          } else {
            val = new (&_store[0] + _offset) ValTimestamp();
          }
          _offset += ValTimestamp::getBaseSize();
        }
        break;
      case STRING: {
        size_t len = value.isNull() ? 0 : ::strlen(value.asString());
        if (hasSizeFor(ValString::getBaseSize() + len + 1)) {
          markValue();
          if (!value.isNull()) {
            char* psd = &_store[0] + _offset + sizeof(ValString);
            ::memcpy(psd, value.asString(), len + 1);
            val = new (&_store[0] + _offset) ValString(psd);
          } else {
            val = new (&_store[0] + _offset) ValString();
          }
          _offset += val->getSize();
        }
        break;
      }
      case NONE:
        CSVSQLDB_THROW(csvsqldb::Exception, "type not allowed " << typeToString(value.getType()));
    }
    return val;
  }

  Value* Block::addValue(const Value& value)
  {
    Value* val = nullptr;

    switch (value.getType()) {
      case INT:
        if (hasSizeFor(ValInt::getBaseSize())) {
          markValue();
          val = new (&_store[0] + _offset) ValInt(static_cast<const ValInt&>(value));
          _offset += ValInt::getBaseSize();
        }
        break;
      case REAL:
        if (hasSizeFor(ValDouble::getBaseSize())) {
          markValue();
          val = new (&_store[0] + _offset) ValDouble(static_cast<const ValDouble&>(value));
          _offset += ValDouble::getBaseSize();
        }
        break;
      case BOOLEAN:
        if (hasSizeFor(ValBool::getBaseSize())) {
          markValue();
          val = new (&_store[0] + _offset) ValBool(static_cast<const ValBool&>(value));
          _offset += ValBool::getBaseSize();
        }
        break;
      case DATE:
        if (hasSizeFor(ValDate::getBaseSize())) {
          markValue();
          val = new (&_store[0] + _offset) ValDate(static_cast<const ValDate&>(value));
          _offset += ValDate::getBaseSize();
        }
        break;
      case TIME:
        if (hasSizeFor(ValTime::getBaseSize())) {
          markValue();
          val = new (&_store[0] + _offset) ValTime(static_cast<const ValTime&>(value));
          _offset += ValTime::getBaseSize();
        }
        break;
      case TIMESTAMP:
        if (hasSizeFor(ValTimestamp::getBaseSize())) {
          markValue();
          val = new (&_store[0] + _offset) ValTimestamp(static_cast<const ValTimestamp&>(value));
          _offset += ValTimestamp::getBaseSize();
        }
        break;
      case STRING: {
        if (hasSizeFor(value.getSize())) {
          markValue();
          if (!value.isNull()) {
            char* psd = &_store[0] + _offset + sizeof(ValString);
            size_t len = static_cast<const ValString&>(value).getLength();
            ::memcpy(psd, static_cast<const ValString&>(value).asString(), len + 1);
            val = new (&_store[0] + _offset) ValString(psd);
          } else {
            val = new (&_store[0] + _offset) ValString();
          }
          _offset += val->getSize();
        }
        break;
      }
      case NONE:
        CSVSQLDB_THROW(csvsqldb::Exception, "type not allowed " << typeToString(value.getType()));
    }
    return val;
  }

  void Block::nextRow()
  {
    *(&_store[0] + _offset) = sRowMarker;
    ++_offset;
  }

  void Block::markValue()
  {
    *(&_store[0] + _offset) = sValueMarker;
    ++_offset;
  }

  void Block::markNextBlock()
  {
    *(&_store[0] + _offset) = sBlockMarker;
    ++_offset;
  }

  void Block::endBlocks()
  {
    *(&_store[0] + _offset) = sEndMarker;
    ++_offset;
  }
}
