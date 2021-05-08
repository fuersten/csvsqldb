//
//  block_producer.cpp
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

#include "block_producer.h"


namespace csvsqldb
{
  BlockProducer::BlockProducer(BlockManager& blockManager)
  : _blockManager(blockManager)
  , _block(_blockManager.createBlock())
  {
  }

  BlockProducer::~BlockProducer()
  {
    if (_readThread.joinable()) {
      _readThread.join();
    }
    while (!_blocks.empty()) {
      _blockManager.release(_blocks.front());
      _blocks.pop();
    }
  }

  void BlockProducer::addBlock()
  {
    _block->markNextBlock();
    {
      std::unique_lock lk(_queueMutex);
      _blocks.push(_block);
      _cv.notify_all();
      if (_blocks.size() > 10) {
        _cv.wait(lk, [this] { return _blocks.size() < 5; });
      }
    }
    _block = _blockManager.createBlock();
  }

  void BlockProducer::start(std::function<void(BlockProducer& producer)> reader)
  {
    _reader = reader;
    _readThread = std::thread(std::bind(&BlockProducer::readBlocks, this));
  }

  void BlockProducer::readBlocks()
  {
    try {
      _reader(*this);
      {
        std::unique_lock lk(_queueMutex);
        _block->endBlocks();
        _blocks.push(_block);
        _block = nullptr;
      }
    } catch (csvsqldb::Exception& ex) {
      _error = ex.what();
    }
    _cv.notify_all();
  }

  BlockPtr BlockProducer::getNextBlock()
  {
    std::unique_lock lk(_queueMutex);
    if (_blocks.empty() && !_block) {
      return nullptr;
    }
    _cv.wait(lk, [this] { return !_blocks.empty() || !_error.empty(); });

    if (!_error.empty()) {
      CSVSQLDB_THROW(csvsqldb::Exception, _error);
    }

    BlockPtr block = nullptr;
    if (!_blocks.empty()) {
      block = _blocks.front();
      _blocks.pop();
      _cv.notify_all();
    }

    return block;
  }

  void BlockProducer::nextRow()
  {
    _block->nextRow();
  }

  void BlockProducer::addValue(const Variant& value)
  {
    if (!_block->addValue(value)) {
      addBlock();
      _block->addValue(value);
    }
  }

  void BlockProducer::addInt(int64_t num, bool isNull)
  {
    if (!_block->addInt(num, isNull)) {
      addBlock();
      _block->addInt(num, isNull);
    }
  }
  void BlockProducer::addReal(double num, bool isNull)
  {
    if (!_block->addReal(num, isNull)) {
      addBlock();
      _block->addReal(num, isNull);
    }
  }
  void BlockProducer::addString(const char* s, size_t len, bool isNull)
  {
    if (!_block->addString(s, len, isNull)) {
      addBlock();
      _block->addString(s, len, isNull);
    }
  }
  void BlockProducer::addBool(bool b, bool isNull)
  {
    if (!_block->addBool(b, isNull)) {
      addBlock();
      _block->addBool(b, isNull);
    }
  }
  void BlockProducer::addDate(const csvsqldb::Date& date, bool isNull)
  {
    if (!_block->addDate(date, isNull)) {
      addBlock();
      _block->addDate(date, isNull);
    }
  }
  void BlockProducer::addTime(const csvsqldb::Time& time, bool isNull)
  {
    if (!_block->addTime(time, isNull)) {
      addBlock();
      _block->addTime(time, isNull);
    }
  }
  void BlockProducer::addTimestamp(const csvsqldb::Timestamp& timestamp, bool isNull)
  {
    if (!_block->addTimestamp(timestamp, isNull)) {
      addBlock();
      _block->addTimestamp(timestamp, isNull);
    }
  }
}
