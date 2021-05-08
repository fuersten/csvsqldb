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
    {
      if (_context._skipFirstLine) {
        if (std::getline(_stream, _currentLine)) {
          ++_lineCount;
        }
      }
      _continue = !!std::getline(_stream, _currentLine);
    }

    size_t CSVParser::getLineCount() const
    {
      return _lineCount;
    }

    bool CSVParser::parseLine()
    {
      if (_continue) {
        ++_lineCount;
        if (!_currentLine.empty()) {
          const auto* start{_currentLine.data()};
          const auto* end{_currentLine.data() + _currentLine.length()};
          auto typesIter = _types.begin();

          try {
            for (const auto* scanPos = start; scanPos < end; ++scanPos) {
              if (*scanPos == _context._delimiter) {
                scanPos = parseValue(*typesIter, start, scanPos, end);
                start = scanPos + 1;
                ++typesIter;
                if (typesIter == _types.end() && start < end) {
                  CSVSQLDB_THROW(CSVParserException, "too many fields found");
                }
              }
            }
            if (start < end) {
              parseValue(*typesIter, start, end, end);
              if (++typesIter != _types.end()) {
                start = end - (start < end ? 1 : 0);
                CSVSQLDB_THROW(CSVParserException, "too few fields found");
              }
            }
          } catch (const csvsqldb::TimeException& ex) {
            skipLineError(typesIter, start, ex.what());
          } catch (const csvsqldb::DateException& ex) {
            skipLineError(typesIter, start, ex.what());
          } catch (const csvsqldb::TimestampException& ex) {
            skipLineError(typesIter, start, ex.what());
          } catch (const CSVParserException& ex) {
            skipLineError(typesIter, start, ex.what());
          }
        }
      } else {
        return false;
      }
      _continue = !!std::getline(_stream, _currentLine);
      return _continue && !_currentLine.empty();
    }

    void CSVParser::skipLineError(Types::const_iterator typesIter, const char* fieldStart, const char* error) const
    {
      auto field = static_cast<size_t>(typesIter - _types.begin()) + 1;
      auto column = static_cast<size_t>(fieldStart - _currentLine.data()) + 1;
      std::cerr << _context._filename << ":" << _lineCount << ":" << column << ":" << field << ": ERROR: " << error << " - skipping line\n";
      std::cerr << "ERROR: skipping line " << _lineCount << ": " << error << "\n";
    }

    const char* CSVParser::parseString(const char* str, const char* end, const char* lineEnd) const
    {
      if (str < lineEnd && (*str == '"' || *str == '\'')) {
        char quote = *str++;
        const auto* src = str;
        for (; src < lineEnd; ++src) {
          if (*src == quote) {
            ++src;
            if (src == lineEnd || *src != quote) {
              break;
            }
          }
        }
        if (src < lineEnd && *src != _context._delimiter) {
          CSVSQLDB_THROW(CSVParserException, "wrong delimiters in STRING");
        } else {
          auto val = std::string(str, src - 1);
          char dequote[2] = {quote, quote};
          size_t pos = 0;
          while (true) {
            pos = val.find(&dequote[0], pos, 2);
            if (pos == std::string::npos) {
              break;
            }
            val.erase(++pos, 1);
          }
          _callback.onString(val.c_str(), val.size(), false);
          return src;
        }
      } else {
        _callback.onString(str, static_cast<size_t>(end - str), str == end);
        return end;
      }
    }

    void CSVParser::parseLong(const char* str, const char* end) const
    {
      if (str != end) {
        bool neg = *str == '-';
        if (neg || *str == '+') {
          ++str;
        }
        int64_t val = 0;
        unsigned d;
        while ((d = static_cast<unsigned>(*str++ - '0')) <= 9) {
          val = val * 10 + d;
        }
        if (--str != end) {
          CSVSQLDB_THROW(CSVParserException, "field is not a valid INTEGER");
        }
        _callback.onLong(neg ? -val : val, false);
      } else {
        _callback.onLong(std::numeric_limits<int64_t>::max(), true);
      }
    }

    void CSVParser::parseDouble(const char* str, const char* end) const
    {
      if (str != end) {
        double val{0.0};
        auto answer = fast_float::from_chars(str, end, val);
        if (answer.ec != std::errc()) {
          CSVSQLDB_THROW(CSVParserException, "'" << std::string(str, end) << "' is not a valid REAL");
        }
        _callback.onDouble(val, false);
      } else {
        _callback.onDouble(std::numeric_limits<double>::max(), true);
      }
    }

    void CSVParser::parseBool(const char* str, const char* end) const
    {
      if (end - str == 1) {
        if ((*str - 48) < 0 || (*str - 48) > 9) {
          CSVSQLDB_THROW(CSVParserException, "field is not a valid BOOLEAN");
        }
        _callback.onBoolean((*str - 48) != 0, false);
      } else {
        CSVSQLDB_THROW(CSVParserException, "field is not a valid BOOLEAN");
      }
    }

    void CSVParser::parseDate(const char* str, const char* end) const
    {
      if (str != end) {
        // TODO LCF: check for digits
        if (str[4] != '-' || str[7] != '-') {
          CSVSQLDB_THROW(CSVParserException, "expected a DATE field (YYYY-mm-dd)");
        }

        uint16_t year = static_cast<uint16_t>(str[0] - 48) * 1000;
        year += static_cast<uint16_t>(str[1] - 48) * 100;
        year += static_cast<uint16_t>(str[2] - 48) * 10;
        year += static_cast<uint16_t>(str[3] - 48);

        uint16_t mo = static_cast<uint16_t>(str[5] - 48) * 10;
        mo += static_cast<uint16_t>(str[6] - 48);
        auto month = static_cast<csvsqldb::Date::eMonth>(mo);

        uint16_t day = static_cast<uint16_t>(str[8] - 48) * 10;
        day += static_cast<uint16_t>(str[9] - 48);

        _callback.onDate(csvsqldb::Date(year, month, day), false);
      } else {
        _callback.onDate(csvsqldb::Date(), true);
      }
    }

    void CSVParser::parseTime(const char* str, const char* end) const
    {
      if (str != end) {
        // TODO LCF: check for digits
        if (str[2] != ':' || str[5] != ':') {
          CSVSQLDB_THROW(CSVParserException, "expected a TIME field (HH:MM:SS)");
        }

        uint16_t hour = static_cast<uint16_t>(str[0] - 48) * 10;
        hour += static_cast<uint16_t>(str[1] - 48);

        uint16_t minute = static_cast<uint16_t>(str[3] - 48) * 10;
        minute += static_cast<uint16_t>(str[4] - 48);

        uint16_t second = static_cast<uint16_t>(str[6] - 48) * 10;
        second += static_cast<uint16_t>(str[7] - 48);

        _callback.onTime(csvsqldb::Time(hour, minute, second), false);
      } else {
        _callback.onTime(csvsqldb::Time(), true);
      }
    }

    void CSVParser::parseTimestamp(const char* str, const char* end) const
    {
      if (str != end) {
        if (end - str < 19) {
          CSVSQLDB_THROW(CSVParserException, "expected a TIMESTAMP field (YYYY-mm-ddTHH:MM:SS)");
        } else {
          // TODO LCF: check for digits
          if (str[4] != '-' || str[7] != '-' || (str[10] != 'T' && str[10] != ' ') || str[13] != ':' || str[16] != ':') {
            CSVSQLDB_THROW(CSVParserException, "expected a TIMESTAMP field (YYYY-mm-ddTHH:MM:SS)");
          }

          uint16_t year = static_cast<uint16_t>(str[0] - 48) * 1000;
          year += static_cast<uint16_t>(str[1] - 48) * 100;
          year += static_cast<uint16_t>(str[2] - 48) * 10;
          year += static_cast<uint16_t>(str[3] - 48);

          uint16_t mo = static_cast<uint16_t>(str[5] - 48) * 10;
          mo += static_cast<uint16_t>(str[6] - 48);
          auto month = static_cast<csvsqldb::Date::eMonth>(mo);

          uint16_t day = static_cast<uint16_t>(str[8] - 48) * 10;
          day += static_cast<uint16_t>(str[9] - 48);

          uint16_t hour = static_cast<uint16_t>(str[11] - 48) * 10;
          hour += static_cast<uint16_t>(str[12] - 48);

          uint16_t minute = static_cast<uint16_t>(str[14] - 48) * 10;
          minute += static_cast<uint16_t>(str[15] - 48);

          uint16_t second = static_cast<uint16_t>(str[17] - 48) * 10;
          second += static_cast<uint16_t>(str[18] - 48);

          uint16_t milliseconds = 0;
          if (end - str == 23 && str[19] == '.') {
            milliseconds = static_cast<uint16_t>(str[20] - 48) * 100;
            milliseconds += static_cast<uint16_t>(str[21] - 48) * 10;
            milliseconds += static_cast<uint16_t>(str[22] - 48);
          }
          _callback.onTimestamp(csvsqldb::Timestamp(year, month, day, hour, minute, second, milliseconds), false);
        }
      } else {
        _callback.onTimestamp(csvsqldb::Timestamp(), true);
      }
    }

    const char* CSVParser::parseValue(CsvTypes type, const char* start, const char* end, const char* lineEnd) const
    {
      switch (type) {
        case LONG:
          parseLong(start, end);
          break;
        case DOUBLE:
          parseDouble(start, end);
          break;
        case STRING:
          return parseString(start, end, lineEnd);
        case DATE:
          parseDate(start, end);
          break;
        case TIME:
          parseTime(start, end);
          break;
        case TIMESTAMP:
          parseTimestamp(start, end);
          break;
        case BOOLEAN:
          parseBool(start, end);
          break;
      }
      return end;
    }
  }
}
