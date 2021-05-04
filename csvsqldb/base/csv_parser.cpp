//
//  csv_parser.cpp
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

#include "csv_parser.h"

#include "exception.h"
#include "time_helper.h"

#include <fast_float/fast_float.h>


namespace csvsqldb
{
  namespace csv
  {
    CSVParser::CSVParser(CSVParserContext context, std::istream& stream, Types types, CSVParserCallback& callback)
    : _context(context)
    , _stream(stream)
    , _types(std::move(types))
    , _callback(callback)
    , _typeIterator(_types.begin())
    , _stringParser(_stringBuffer, _stringBufferSize, std::bind(&CSVParser::readNextChar, this, std::placeholders::_1))
    {
      _buffer.resize(_bufferLength);
      _stringBuffer.resize(_stringBufferSize);
      readBuffer();
      if (_context._skipFirstLine) {
        findEndOfLine();
      }
    }

    bool CSVParser::parseLine()
    {
      if (_state == LINESTART) {
        _state = FIELDSTART;
        _typeIterator = _types.begin();
      }

      while (_count > 0) {
        try {
          switch (*_typeIterator) {
            case LONG:
              parseLong();
              break;
            case DOUBLE:
              parseDouble();
              break;
            case STRING:
              parseString();
              break;
            case DATE:
              parseDate();
              break;
            case TIME:
              parseTime();
              break;
            case TIMESTAMP:
              parseTimestamp();
              break;
            case BOOLEAN:
              parseBool();
              break;
          }
          ++_typeIterator;
          if (_typeIterator == _types.end() && (_state != LINESTART && _state != END)) {
            CSVSQLDB_THROW(CSVParserException, "too many fields found in line " << _lineCount);
          } else if (_typeIterator != _types.end() && (_state == LINESTART || _state == END)) {
            CSVSQLDB_THROW(CSVParserException, "too few fields found in line " << _lineCount);
          }
        } catch (const csvsqldb::TimeException& ex) {
          return skipLineError(ex.what());
        } catch (const csvsqldb::DateException& ex) {
          return skipLineError(ex.what());
        } catch (const csvsqldb::TimestampException& ex) {
          return skipLineError(ex.what());
        } catch (const CSVParserException& ex) {
          return skipLineError(ex.what());
        }

        if (_n == static_cast<size_t>(_count) && _state == LINESTART) {
          if (!readBuffer()) {
            return false;
          }
        }

        if (_state == LINESTART && _count > 0) {
          ++_lineCount;
          return true;
        } else if (_state == LINESTART && _count == 0) {
          if (!readBuffer()) {
            return false;
          }
        } else if (_state == FIELDSTART && _count == 0) {
          if (!readBuffer()) {
            CSVSQLDB_THROW(NoMoreInputException, "no more input characters left in source in line " << _lineCount);
          }
        }
        if (_state == END) {
          return false;
        }
      }

      return false;
    }

    bool CSVParser::skipLineError(const char* error)
    {
      std::cerr << "ERROR: skipping line " << _lineCount << ": " << error << "\n";
      skipLine();
      return true;
    }

    void CSVParser::parseString()
    {
      size_t len = _stringParser.parseToBuffer();
      _callback.onString(&_stringBuffer[0], len, !_stringBuffer[0]);
    }

    void CSVParser::parseLong()
    {
      char c = readNextChar();
      _stringBuffer[0] = c;
      int64_t value = 0;
      bool neg = false;
      if (c == '-') {
        c = readNextChar();
        neg = true;
      } else if (c == '+') {
        c = readNextChar();
      }
      while (c) {
        int n = c - 48;
        if (n < 0 || n > 9) {
          CSVSQLDB_THROW(CSVParserException, "field is not a long in line " << _lineCount);
        }
        value = 10 * value + n;
        c = readNextChar();
      }
      if (_stringBuffer[0]) {
        _callback.onLong(neg ? value * (-1) : value, false);
      } else {
        _callback.onLong(std::numeric_limits<int64_t>::max(), true);
      }
    }

    void CSVParser::parseDouble()
    {
      size_t n = 0;
      _stringBuffer[n] = readNextChar();
      while (_stringBuffer[n]) {
        _stringBuffer[++n] = readNextChar();
      }
      if (_stringBuffer[0]) {
        double result{0.0};
        auto answer = fast_float::from_chars(_stringBuffer.data(), _stringBuffer.data() + n, result);
        if (answer.ec != std::errc()) {
          CSVSQLDB_THROW(CSVParserException, "not a float '" << _stringBuffer.data() << "'");
        }
        _callback.onDouble(result, false);
      } else {
        _callback.onDouble(std::numeric_limits<double>::max(), true);
      }
    }

    void CSVParser::parseBool()
    {
      char c = readNextChar();
      if (c) {
        if ((c - 48) < 0 || (c - 48) > 9) {
          CSVSQLDB_THROW(CSVParserException, "field is not a bool in line " << _lineCount);
        }
        _callback.onBoolean((c - 48) != 0, false);
        c = readNextChar();
        if (c) {
          CSVSQLDB_THROW(CSVParserException, "field is not a bool in line " << _lineCount);
        }
      } else {
        _callback.onBoolean(false, true);
      }
    }

    void CSVParser::parseDate()
    {
      size_t n = 0;
      _stringBuffer[n] = readNextChar();
      while (_stringBuffer[n]) {
        _stringBuffer[++n] = readNextChar();
      }
      if (_stringBuffer[0]) {
        // TODO LCF: check for digits
        if (_stringBuffer[4] != '-' || _stringBuffer[7] != '-') {
          CSVSQLDB_THROW(CSVParserException, "expected a date field (YYYY-mm-dd) in line " << _lineCount);
        }

        uint16_t year = static_cast<uint16_t>(_stringBuffer[0] - 48) * 1000;
        year += static_cast<uint16_t>(_stringBuffer[1] - 48) * 100;
        year += static_cast<uint16_t>(_stringBuffer[2] - 48) * 10;
        year += static_cast<uint16_t>(_stringBuffer[3] - 48);

        uint16_t mo = static_cast<uint16_t>(_stringBuffer[5] - 48) * 10;
        mo += static_cast<uint16_t>(_stringBuffer[6] - 48);
        csvsqldb::Date::eMonth month = static_cast<csvsqldb::Date::eMonth>(mo);

        uint16_t day = static_cast<uint16_t>(_stringBuffer[8] - 48) * 10;
        day += static_cast<uint16_t>(_stringBuffer[9] - 48);

        csvsqldb::Date date(year, month, day);
        _callback.onDate(date, false);
      } else {
        _callback.onDate(csvsqldb::Date(), true);
      }
    }

    void CSVParser::parseTime()
    {
      size_t n = 0;
      _stringBuffer[n] = readNextChar();
      while (_stringBuffer[n]) {
        _stringBuffer[++n] = readNextChar();
      }
      if (_stringBuffer[0]) {
        // TODO LCF: check for digits
        if (_stringBuffer[2] != ':' || _stringBuffer[5] != ':') {
          CSVSQLDB_THROW(CSVParserException, "expected a time field (HH:MM:SS) in line " << _lineCount);
        }

        uint16_t hour = static_cast<uint16_t>(_stringBuffer[0] - 48) * 10;
        hour += static_cast<uint16_t>(_stringBuffer[1] - 48);

        uint16_t minute = static_cast<uint16_t>(_stringBuffer[3] - 48) * 10;
        minute += static_cast<uint16_t>(_stringBuffer[4] - 48);

        uint16_t second = static_cast<uint16_t>(_stringBuffer[6] - 48) * 10;
        second += static_cast<uint16_t>(_stringBuffer[7] - 48);

        csvsqldb::Time time(hour, minute, second);
        _callback.onTime(time, false);
      } else {
        _callback.onTime(csvsqldb::Time(), true);
      }
    }

    void CSVParser::parseTimestamp()
    {
      size_t n = 0;
      _stringBuffer[n] = readNextChar();
      while (_stringBuffer[n]) {
        _stringBuffer[++n] = readNextChar();
      }
      if (_stringBuffer[0]) {
        // TODO LCF: check for digits
        if (_stringBuffer[4] != '-' || _stringBuffer[7] != '-' || (_stringBuffer[10] != 'T' && _stringBuffer[10] != ' ') ||
            _stringBuffer[13] != ':' || _stringBuffer[16] != ':') {
          CSVSQLDB_THROW(CSVParserException, "expected a timestamp field (YYYY-mm-ddTHH:MM:SS) in line "
                                               << _lineCount << ", but got '" << &_stringBuffer[0] << "'");
        }

        uint16_t year = static_cast<uint16_t>(_stringBuffer[0] - 48) * 1000;
        year += static_cast<uint16_t>(_stringBuffer[1] - 48) * 100;
        year += static_cast<uint16_t>(_stringBuffer[2] - 48) * 10;
        year += static_cast<uint16_t>(_stringBuffer[3] - 48);

        uint16_t mo = static_cast<uint16_t>(_stringBuffer[5] - 48) * 10;
        mo += static_cast<uint16_t>(_stringBuffer[6] - 48);
        csvsqldb::Date::eMonth month = static_cast<csvsqldb::Date::eMonth>(mo);

        uint16_t day = static_cast<uint16_t>(_stringBuffer[8] - 48) * 10;
        day += static_cast<uint16_t>(_stringBuffer[9] - 48);

        uint16_t hour = static_cast<uint16_t>(_stringBuffer[11] - 48) * 10;
        hour += static_cast<uint16_t>(_stringBuffer[12] - 48);

        uint16_t minute = static_cast<uint16_t>(_stringBuffer[14] - 48) * 10;
        minute += static_cast<uint16_t>(_stringBuffer[15] - 48);

        uint16_t second = static_cast<uint16_t>(_stringBuffer[17] - 48) * 10;
        second += static_cast<uint16_t>(_stringBuffer[18] - 48);

        csvsqldb::Timestamp ts(year, month, day, hour, minute, second, 0);
        _callback.onTimestamp(ts, false);
      } else {
        _callback.onTimestamp(csvsqldb::Timestamp(), true);
      }
    }

    void CSVParser::skipLine()
    {
      while (_state != LINESTART && _state != END) {
        readNextChar();
      }
      ++_lineCount;
    }

    void CSVParser::findEndOfLine()
    {
      readNextChar();
      skipLine();
    }

    char CSVParser::readNextChar(bool ignoreDelimiter)
    {
      if (!checkBuffer()) {
        _state = END;
        return '\0';
      }
      if (!ignoreDelimiter && _buffer[_n] == _context._delimiter) {
        _state = FIELDSTART;
        ++_n;
        if (!checkBuffer()) {
          _state = END;
          return '\0';
        }
        while (_buffer[_n] == ' ') {
          ++_n;
          if (!checkBuffer()) {
            _state = END;
            return '\0';
          }
        }
        return '\0';
      }
      if (_buffer[_n] == '\n' || _buffer[_n] == '\r') {
        _state = LINESTART;
        ++_n;
        if (checkBuffer()) {
          if (_buffer[_n] == '\n') {
            ++_n;
          }
        }
        return '\0';
      }
      return _buffer[_n++];
    }

    bool CSVParser::checkBuffer()
    {
      if (_n >= static_cast<size_t>(_count)) {
        if (!readBuffer()) {
          return false;
        }
      }
      return true;
    }

    bool CSVParser::readBuffer()
    {
      _stream.read(&_buffer[0], _bufferLength);
      _count = _stream.gcount();
      _n = 0;
      return _count > 0;
    }
  }
}
