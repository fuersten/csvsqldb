//
//  csv_parser.cpp
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

#include "csv_parser.h"

#include "exception.h"
#include "time_helper.h"


namespace csvsqldb
{
    namespace csv
    {

        CSVParser::CSVParser(CSVParserContext context, std::istream& stream, Types types, CSVParserCallback& callback)
        : _context(context)
        , _stream(stream)
        , _types(types)
        , _callback(callback)
        , _state(INIT)
        , _typeIterator(_types.begin())
        , _lineCount(1)
        , _stringBufferSize(256)
        , _n(0)
        , _count(0)
        {
            _buffer.resize(_bufferLength);
            _stringBuffer.resize(_stringBufferSize);
            readBuffer();
            if(_context._skipFirstLine) {
                findEndOfHeaderline();
                ++_lineCount;
            }
        }

        bool CSVParser::parseLine()
        {
            if(_state == LINESTART) {
                _state = FIELDSTART;
                _typeIterator = _types.begin();
            }

            while(_count > 0) {
                switch(*_typeIterator) {
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
                if(_typeIterator == _types.end() && (_state != LINESTART && _state != END)) {
                    CSVSQLDB_THROW(csvsqldb::Exception, "too many fields found in line " << _lineCount);
                } else if(_typeIterator != _types.end() && (_state == LINESTART || _state == END)) {
                    CSVSQLDB_THROW(csvsqldb::Exception, "too few fields found in line " << _lineCount);
                }

                if(_n == static_cast<size_t>(_count) && _state == LINESTART) {
                    if(!readBuffer()) {
                        return false;
                    }
                }

                if(_state == LINESTART && _count > 0) {
                    ++_lineCount;
                    return true;
                } else if(_state == LINESTART && _count == 0) {
                    if(!readBuffer()) {
                        return false;
                    }
                } else if(_state == FIELDSTART && _count == 0) {
                    if(!readBuffer()) {
                        CSVSQLDB_THROW(NoMoreInputException, "no more input characters left in source in line " << _lineCount);
                    }
                }
                if(_state == END) {
                    return false;
                }
            }

            return true;
        }

        void CSVParser::parseString()
        {
            size_t n = 0;
            size_t start = 0;
            size_t end = 0;
            _stringBuffer[n] = readNextChar();

            bool expectQuotationMarkAtEnd = false;
            bool expectSingleQuotationMarkAtEnd = false;

            if(_stringBuffer[0] == '"') {
                expectQuotationMarkAtEnd = true;
                ++start;
            } else if(_stringBuffer[0] == '\'') {
                expectSingleQuotationMarkAtEnd = true;
                ++start;
            }

            bool ignoreDelimiter = expectQuotationMarkAtEnd | expectSingleQuotationMarkAtEnd;

            while(_stringBuffer[n]) {
                if(++n == _stringBufferSize - 1) {
                    _stringBufferSize += _stringBufferSize;
                    _stringBuffer.resize(_stringBufferSize);
                }
                _stringBuffer[n] = readNextChar(ignoreDelimiter);
                if(expectQuotationMarkAtEnd && _stringBuffer[n] == '"') {
                    ignoreDelimiter = false;
                } else if(expectSingleQuotationMarkAtEnd && _stringBuffer[n] == '\'') {
                    ignoreDelimiter = false;
                }
            }
            end = n;
            if(_stringBuffer[0] && (expectQuotationMarkAtEnd || expectSingleQuotationMarkAtEnd)) {
                if(expectQuotationMarkAtEnd && _stringBuffer[n - 1] != '"') {
                    CSVSQLDB_THROW(csvsqldb::Exception, "expected a '\"' at the end of the string in line " << _lineCount);
                } else if(expectSingleQuotationMarkAtEnd && _stringBuffer[n - 1] != '\'') {
                    CSVSQLDB_THROW(csvsqldb::Exception, "expected a '\'' at the end of the string in line " << _lineCount);
                }
                end = n - 1;
                _stringBuffer[n - 1] = '\0';
            }
            if(_stringBuffer[0] && !(expectQuotationMarkAtEnd || expectSingleQuotationMarkAtEnd)) {
                if(_stringBuffer[n - 1] == '"' || _stringBuffer[n - 1] == '\'') {
                    CSVSQLDB_THROW(csvsqldb::Exception, "didn't expected a '\"' at the end of the string in line " << _lineCount);
                }
            }
            size_t len = static_cast<size_t>(&_stringBuffer[end] - &_stringBuffer[start]);
            _callback.onString(&_stringBuffer[start], len, !_stringBuffer[0]);
        }

        void CSVParser::parseLong()
        {
            char c = readNextChar();
            _stringBuffer[0] = c;
            int64_t value = 0;
            int n = 0;
            while(c) {
                n = c - 48;
                if(n < 0 || n > 9) {
                    CSVSQLDB_THROW(csvsqldb::Exception, "field is not a long in line " << _lineCount);
                }
                value = 10 * value + n;
                c = readNextChar();
            }
            if(_stringBuffer[0]) {
                _callback.onLong(value, false);
            } else {
                _callback.onLong(std::numeric_limits<int64_t>::max(), true);
            }
        }

        void CSVParser::parseDouble()
        {
            size_t n = 0;
            _stringBuffer[n] = readNextChar();
            while(_stringBuffer[n]) {
                _stringBuffer[++n] = readNextChar();
            }
            if(_stringBuffer[0]) {
                _callback.onDouble(::atof(&_stringBuffer[0]), false);
            } else {
                _callback.onDouble(std::numeric_limits<double>::max(), true);
            }
        }

        void CSVParser::parseBool()
        {
            char c = readNextChar();
            if(c) {
                if((c - 48) < 0 || (c - 48) > 9) {
                    CSVSQLDB_THROW(csvsqldb::Exception, "field is not a bool in line " << _lineCount);
                }
                _callback.onBoolean((c - 48) != 0, false);
                c = readNextChar();
                if(c) {
                    CSVSQLDB_THROW(csvsqldb::Exception, "field is not a bool in line " << _lineCount);
                }
            } else {
                _callback.onBoolean(false, true);
            }
        }

        void CSVParser::parseDate()
        {
            size_t n = 0;
            _stringBuffer[n] = readNextChar();
            while(_stringBuffer[n]) {
                _stringBuffer[++n] = readNextChar();
            }
            if(_stringBuffer[0]) {
                // TODO LCF: check for digits
                if(_stringBuffer[4] != '-' || _stringBuffer[7] != '-') {
                    CSVSQLDB_THROW(csvsqldb::Exception, "expected a date field (YYYY-mm-dd) in line " << _lineCount);
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
            while(_stringBuffer[n]) {
                _stringBuffer[++n] = readNextChar();
            }
            if(_stringBuffer[0]) {
                // TODO LCF: check for digits
                if(_stringBuffer[2] != ':' || _stringBuffer[5] != ':') {
                    CSVSQLDB_THROW(csvsqldb::Exception, "expected a time field (HH:MM:SS) in line " << _lineCount);
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
            while(_stringBuffer[n]) {
                _stringBuffer[++n] = readNextChar();
            }
            if(_stringBuffer[0]) {
                // TODO LCF: check for digits
                if(_stringBuffer[4] != '-' || _stringBuffer[7] != '-' || (_stringBuffer[10] != 'T' && _stringBuffer[10] != ' ')
                   || _stringBuffer[13] != ':'
                   || _stringBuffer[16] != ':') {
                    CSVSQLDB_THROW(csvsqldb::Exception,
                                   "expected a timestamp field (YYYY-mm-ddTHH:MM:SS) in line " << _lineCount << ", but got '"
                                                                                               << &_stringBuffer[0]
                                                                                               << "'");
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

        void CSVParser::findEndOfHeaderline()
        {
            readNextChar();
            while(_state != LINESTART) {
                readNextChar();
            }
        }

        char CSVParser::readNextChar(bool ignoreDelimiter)
        {
            if(!checkBuffer()) {
                _state = END;
                return '\0';
            }
            if(!ignoreDelimiter && _buffer[_n] == _context._delimiter) {
                _state = FIELDSTART;
                ++_n;
                while(_buffer[_n] == ' ') {
                    ++_n;
                    if(!checkBuffer()) {
                        _state = END;
                        return '\0';
                    }
                }
                return '\0';
            }
            if(_buffer[_n] == '\n' || _buffer[_n] == '\r') {
                _state = LINESTART;
                ++_n;
                if(checkBuffer()) {
                    if(_buffer[_n] == '\n') {
                        ++_n;
                    }
                }
                return '\0';
            }
            return _buffer[_n++];
        }

        bool CSVParser::checkBuffer()
        {
            if(_n >= static_cast<size_t>(_count)) {
                if(!readBuffer()) {
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
