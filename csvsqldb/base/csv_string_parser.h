//
//  csv_string_parser.h
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

#include <csvsqldb/base/types.h>

#include <functional>
#include <istream>
#include <vector>

#ifndef __has_attribute  // if we don't have __has_attribute, ignore it
  #define __has_attribute(x) 0
#endif

#if __has_attribute(always_inline)
  #define ALWAYS_INLINE __attribute__((always_inline))
#else
  #define ALWAYS_INLINE
#endif

namespace csvsqldb
{
  namespace csv
  {
    /**
     * Parser for csv strings
     */
    class CSVSQLDB_EXPORT CSVStringParser
    {
    public:
      using BufferType = std::vector<char>;
      using ReadFunction = std::function<char(bool)>;

      /**
       * Constructs a CSV string parser.
       * @param buffer The buffer to put the parsed string into. Will be resized upon need.
       * @param bufferSize The size to increase the buffer with
       * @param readFunction A function to read a character from the input stream. Has the prototype:
       ~~~~~~~~~~~~~{.cpp}
       std::function<char(bool)> ReadFunction
       ~~~~~~~~~~~~~
       */
      CSVStringParser(BufferType& buffer, size_t bufferSize, ReadFunction readFunction);

      CSVStringParser(const CSVStringParser&) = delete;
      CSVStringParser(CSVStringParser&&) = delete;
      CSVStringParser& operator=(const CSVStringParser&) = delete;
      CSVStringParser& operator=(CSVStringParser&&) = delete;

      ~CSVStringParser() = default;

      /**
       * Parses the next characters returned by the readFunction as a CSV string.
       * @return Returns the number of characters read into the buffer
       */
      size_t parseToBuffer();

    private:
      /// The states of the internal state machine for parsing CSV strings
      enum eState {
        START = 0,
        DOUBLE_QUOTE_STRING = 1,
        DOUBLE_QUOTE_STRING_END = 2,
        SINGLE_QUOTE_STRING = 3,
        NO_QUOTE_STRING = 4,
        ERROR = 5,
        SINGLE_QUOTE_STRING_END = 6
      };

      /// The actual state object of each state
      struct State {
        eState _state;  //!< The state
        bool _copy;     //!< If the next character shall be copied into the buffer or not (e.g we dont copy ' or " if they
        //! delimit the string)
        bool _final;            //!< If this is a final state
        bool _ignoreDelimiter;  //!< If the current CSV field delimiter shall be ignored (e.g. the ',' if it is in a
                                //! delimited string)
      };

      enum eCharCategory { SINGLE_QUOTE = 0, DOUBLE_QUOTE = 1, OTHER = 2 };

      static inline ALWAYS_INLINE eCharCategory charCategory(char c)
      {
        switch (c) {
          case '\'':
            return SINGLE_QUOTE;
          case '\"':
            return DOUBLE_QUOTE;
          default:
            return OTHER;
        }
      }

      void initializeTransitionTable();

      BufferType& _buffer;
      const size_t _bufferSize;
      ReadFunction _readFunction;
      size_t _currentBufferSize;
      eState _currentState{START};
      std::vector<std::vector<State>> _transitionTable;
    };
  }
}
