//
//  table_scan_operatornode.cpp
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

#include "table_scan_operatornode.h"

#include <fstream>
#include <regex>


namespace csvsqldb
{
  BlockReader::BlockReader(BlockManager& blockManager)
  : _blockManager(blockManager)
  , _block(_blockManager.createBlock())
  {
  }

  BlockReader::~BlockReader()
  {
    _continue = false;
    if (_readThread.joinable()) {
      _readThread.join();
    }
    while (!_blocks.empty()) {
      _blockManager.release(_blocks.front());
      _blocks.pop();
    }
  }

  void BlockReader::initialize(std::unique_ptr<csvsqldb::csv::CSVParser> csvparser)
  {
    _csvparser.swap(csvparser);
    _readThread = std::thread(std::bind(&BlockReader::readBlocks, this));
  }

  BlockPtr BlockReader::getNextBlock()
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

  void BlockReader::addBlock()
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

  void BlockReader::readBlocks()
  {
    try {
      bool moreLines = _csvparser->parseLine();
      _block->nextRow();

      while (_continue && moreLines) {
        moreLines = _csvparser->parseLine();
        _block->nextRow();
      }
      {
        std::unique_lock lk(_queueMutex);
        _block->endBlocks();
        _blocks.push(_block);
        _block = nullptr;
      }
    } catch (csvsqldb::Exception& ex) {
      _continue = false;
      _error = ex.what();
    }
    _cv.notify_all();
  }

  void BlockReader::onLong(int64_t num, bool isNull)
  {
    if (!_block->addInt(num, isNull)) {
      addBlock();
      _block->addInt(num, isNull);
    }
  }

  void BlockReader::onDouble(double num, bool isNull)
  {
    if (!_block->addReal(num, isNull)) {
      addBlock();
      _block->addReal(num, isNull);
    }
  }

  void BlockReader::onString(const char* s, size_t len, bool isNull)
  {
    if (!_block->addString(s, len, isNull)) {
      addBlock();
      _block->addString(s, len, isNull);
    }
  }

  void BlockReader::onDate(const csvsqldb::Date& date, bool isNull)
  {
    if (!_block->addDate(date, isNull)) {
      addBlock();
      _block->addDate(date, isNull);
    }
  }

  void BlockReader::onTime(const csvsqldb::Time& time, bool isNull)
  {
    if (!_block->addTime(time, isNull)) {
      addBlock();
      _block->addTime(time, isNull);
    }
  }

  void BlockReader::onTimestamp(const csvsqldb::Timestamp& timestamp, bool isNull)
  {
    if (!_block->addTimestamp(timestamp, isNull)) {
      addBlock();
      _block->addTimestamp(timestamp, isNull);
    }
  }

  void BlockReader::onBoolean(bool boolean, bool isNull)
  {
    if (!_block->addBool(boolean, isNull)) {
      addBlock();
      _block->addBool(boolean, isNull);
    }
  }


  TableScanOperatorNode::TableScanOperatorNode(const OperatorContext& context, const SymbolTablePtr& symbolTable,
                                               const SymbolInfo& tableInfo)
  : ScanOperatorNode(context, symbolTable, tableInfo)
  , _blockReader(_context._blockManager)
  {
  }

  const Values* TableScanOperatorNode::getNextRow()
  {
    if (!_blockReader.valid()) {
      initializeBlockReader();
    }

    return _iterator->getNextRow();
  }

  BlockPtr TableScanOperatorNode::getNextBlock()
  {
    return _blockReader.getNextBlock();
  }

  void TableScanOperatorNode::initializeBlockReader()
  {
    _iterator = std::make_shared<BlockIterator>(_types, *this, getBlockManager());

    csvsqldb::csv::Types types;
    for (const auto& type : _types) {
      switch (type) {
        case INT:
          types.push_back(csvsqldb::csv::LONG);
          break;
        case REAL:
          types.push_back(csvsqldb::csv::DOUBLE);
          break;
        case DATE:
          types.push_back(csvsqldb::csv::DATE);
          break;
        case TIME:
          types.push_back(csvsqldb::csv::TIME);
          break;
        case TIMESTAMP:
          types.push_back(csvsqldb::csv::TIMESTAMP);
          break;
        case STRING:
          types.push_back(csvsqldb::csv::STRING);
          break;
        case BOOLEAN:
          types.push_back(csvsqldb::csv::BOOLEAN);
          break;
        default:
          CSVSQLDB_THROW(csvsqldb::Exception, "type not implemented yet");
      }
    }

    Mapping mapping = _context._database.getMappingForTable(_tableInfo._identifier);
    std::string filePattern = mapping._mapping;
    filePattern = R"(.*)" + filePattern;
    std::regex r(filePattern);

    fs::path pathToCsvFile("");

    for (const auto& file : _context._files) {
      std::smatch match;
      if (regex_match(file, match, r)) {
        pathToCsvFile = file;
        break;
      }
    }
    if (pathToCsvFile.string().empty()) {
      CSVSQLDB_THROW(MappingException, "no file found for mapping '" << filePattern << "'");
    }

    _stream = std::make_unique<std::fstream>(pathToCsvFile);
    if (!_stream || _stream->fail()) {
      CSVSQLDB_THROW(csvsqldb::FilesystemException,
                     "could not open file '" << pathToCsvFile << "' (" << csvsqldb::errnoText() << ")");
    }

    _csvContext._filename = pathToCsvFile.string();
    _csvContext._skipFirstLine = true;
    _csvContext._delimiter = mapping._delimiter;
    _blockReader.initialize(std::make_unique<csvsqldb::csv::CSVParser>(_csvContext, *_stream, types, _blockReader));
  }

  void TableScanOperatorNode::dump(std::ostream& stream) const
  {
    stream << "TableScanOperator (" << _tableInfo._identifier << ")\n";
  }
}
