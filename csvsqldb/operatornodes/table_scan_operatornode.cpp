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
  CSVBlockReader::CSVBlockReader(BlockManager& blockManager)
  : _producer{blockManager}
  {
  }

  void CSVBlockReader::initialize(std::unique_ptr<csvsqldb::csv::CSVParser> csvparser)
  {
    _csvparser.swap(csvparser);
    _producer.start([&](BlockProducer& producer) {
      while (_csvparser->parseLine()) {
        producer.nextRow();
      }
      producer.nextRow();
    });
  }

  BlockPtr CSVBlockReader::getNextBlock()
  {
    return _producer.getNextBlock();
  }

  void CSVBlockReader::onLong(int64_t num, bool isNull)
  {
    _producer.addInt(num, isNull);
  }

  void CSVBlockReader::onDouble(double num, bool isNull)
  {
    _producer.addReal(num, isNull);
  }

  void CSVBlockReader::onString(const char* s, size_t len, bool isNull)
  {
    _producer.addString(s, len, isNull);
  }

  void CSVBlockReader::onDate(const csvsqldb::Date& date, bool isNull)
  {
    _producer.addDate(date, isNull);
  }

  void CSVBlockReader::onTime(const csvsqldb::Time& time, bool isNull)
  {
    _producer.addTime(time, isNull);
  }

  void CSVBlockReader::onTimestamp(const csvsqldb::Timestamp& timestamp, bool isNull)
  {
    _producer.addTimestamp(timestamp, isNull);
  }

  void CSVBlockReader::onBoolean(bool boolean, bool isNull)
  {
    _producer.addBool(boolean, isNull);
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
