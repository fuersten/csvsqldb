//
//  csv_parser.h
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

#include <csvsqldb/base/date.h>
#include <csvsqldb/base/time.h>
#include <csvsqldb/base/timestamp.h>
#include <csvsqldb/base/types.h>

#include <istream>
#include <vector>


namespace csvsqldb
{
  /**
   * CSV related implementations
   */
  namespace csv
  {
    CSVSQLDB_DECLARE_EXCEPTION(CSVParserException, Exception);

    /**
     * Context for the parametration of a CSV parser
     */
    struct CSVSQLDB_EXPORT CSVParserContext {
      CSVParserContext() = default;
      bool _skipFirstLine{false};  //!< The parser will skip the first input line (e.g. the header), default is false
      char _delimiter{','};        //!< Field delimiter, default is ','
    };


    /**
     * The type methods of the callback will be called by the corresponding parser upon encountering the specific type
     */
    class CSVSQLDB_EXPORT CSVParserCallback
    {
    public:
      virtual ~CSVParserCallback() = default;

      virtual void onLong(int64_t num, bool isNull) = 0;
      virtual void onDouble(double num, bool isNull) = 0;
      virtual void onString(const char* s, size_t len, bool isNull) = 0;
      virtual void onDate(const csvsqldb::Date& date, bool isNull) = 0;
      virtual void onTime(const csvsqldb::Time& time, bool isNull) = 0;
      virtual void onTimestamp(const csvsqldb::Timestamp& timestamp, bool isNull) = 0;
      virtual void onBoolean(bool boolean, bool isNull) = 0;
    };

    enum CsvTypes { LONG, DOUBLE, STRING, DATE, TIME, TIMESTAMP, BOOLEAN };
    using Types = std::vector<CsvTypes>;


    /**
     * A class used to parse CSV streams.
     */
    class CSVSQLDB_EXPORT CSVParser
    {
    public:
      /**
       * Constructs a CSV parser.
       * @param context The parametrising context to use
       * @param stream The input stream to parse
       * @param types The column types of the input lines in the right order
       * @param callback The callback to call type methods for
       */
      CSVParser(CSVParserContext context, std::istream& stream, Types types, CSVParserCallback& callback);

      CSVParser(const CSVParser&) = delete;
      CSVParser(CSVParser&&) = delete;
      CSVParser& operator=(const CSVParser&) = delete;
      CSVParser& operator=(CSVParser&&) = delete;

      ~CSVParser() = default;

      /**
       * Parses one line of input and calls the corresponding type method callbacks. Skips the first line of input, if
       * specified by the context.
       * @return true if there are more lines to parse, false otherwise
       */
      bool parseLine();

      /**
       * Returns the current count of lines excluding skipped lines.
       * @return The current line count starting with one.
       */
      size_t getLineCount() const;

    private:
      const char* parseString(const char* str, const char* end, const char* lineEnd) const;
      void parseLong(const char* str, const char* end) const;
      void parseDouble(const char* str, const char* end) const;
      void parseBool(const char* str, const char* end) const;
      void parseDate(const char* str, const char* end) const;
      void parseTime(const char* str, const char* end) const;
      void parseTimestamp(const char* str, const char* end) const;
      const char* parseValue(CsvTypes type, const char* start, const char* end, const char* lineEnd) const;

      CSVParserContext _context;
      std::istream& _stream;
      Types _types;
      CSVParserCallback& _callback;

      std::string _currentLine;
      size_t _lineCount{0};
      bool _continue{true};
    };
  }
}
