//
//  lexer.h
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
#include <regex>
#include <string>
#include <vector>


namespace csvsqldb
{
  /**
   * Lexical analysis related implementations
   */
  namespace lexer
  {
    /**
     * Default tokens mostly for internal usage.
     */
    enum eToken {
      UNDEFINED = -1,   //!< No token set yet
      WHITESPACE = -2,  //!< Identifies a whitespace (but not newline) token
      NEWLINE = -3,     //!< Identifies a newline token
      EOI = -4          //!< Marks the end of input
    };

    /**
     * For each found token an instance of this class will be created.
     */
    struct CSVSQLDB_EXPORT Token {
      /**
       * Constructs an empty token. The token identifier will be set to INDEFINED.
       */
      Token() = default;

      std::string _name;          //!< Name/group of the token
      int32_t _token{UNDEFINED};  //!< The token identifier, either an eToken or a user defined value
      std::string _value;         //!< The corresponding string value of the token
      uint32_t _lineCount{0};     //!< The starting line of the token
      uint16_t _charCount{0};     //!< The starting column position of the token
    };

    /**
     * A generic lexer. By adding lexical definitions a string can be separated generically into its tokens. The added
     * definitions will be tested against the current processing position exactly in the order as they where specified. If no
     * definition matches, a LexicalAnalysisException is thrown. Two definitions will be added as internal definitions:
     *
     *    - addDefinition("newline", R"(\r|\n)", NEWLINE);
     *    - addDefinition("whitespace", R"([ \t\f]+)", WHITESPACE);
     *
     * The lexer will not return WHITESPACE or NEWLINE tokens, but try to find the next non WHITESPACE or NEWLINE token.
     */
    class CSVSQLDB_EXPORT Lexer
    {
    public:
      using InspectTokenCallback = std::function<void(Token&)>;
      /**
       * Constructs a Lexer
       */
      Lexer(InspectTokenCallback callback = InspectTokenCallback());

      Lexer(const Lexer&) = delete;
      Lexer& operator=(const Lexer&) = delete;
      Lexer(Lexer&&) = delete;
      Lexer& operator=(Lexer&&) = delete;

      ~Lexer() = default;

      /**
       * Adds a lexical definition to the lexer. Example:
       ~~~~~~~~~~~~~{.cpp}
       addDefinition("identifier", R"([a-zA-Z][_a-zA-Z0-9]*)", IDENTIFIER);
       ~~~~~~~~~~~~~
       * It might be necessary to wrap keywords for languages in word boundaries:
       ~~~~~~~~~~~~~{.cpp}
       addDefinition("select", R"(\b[sS][eE][lL][eE][cC][tT]\b)", SELECT);
       ~~~~~~~~~~~~~
       * @param name Name or group of the definition. This is just a description. Does not need to be unique.
       * @param r The regular for this token.
       * @param token The unique identifier for this token.
       */
      void addDefinition(std::string name, std::string r, int32_t token);

      /**
       * Sets a new input for the lexer. The state of the lexer will be resetted by this operation. The added definitions
       * will stay the same.
       * @param input The input string to analyse.
       */
      void setInput(std::string input);

      /**
       * Scans the next available Token and returns it. Will throw a LexicalAnalysisException if no definition matches.
       * @return The next available Token. If the end of the input is reached, Token::EOI will be set as token identifier.
       */
      Token next();

      /**
       * Returns the current line count.
       * @return The current line count starting with 1.
       */
      uint32_t lineCount() const
      {
        return _lineCount;
      }

    private:
      struct TokenDefinition {
        TokenDefinition() = default;

        TokenDefinition(std::string name, std::regex r, int32_t token)
        : _name(std::move(name))
        , _rx(std::move(r))
        , _token(token)
        {
        }

        std::string _name;
        std::regex _rx{""};
        int32_t _token{UNDEFINED};
      };
      using TokenDefinitions = std::vector<TokenDefinition>;

      TokenDefinitions _tokenDefinitions;
      InspectTokenCallback _callback;
      std::string _input;
      std::string::const_iterator _pos;
      std::string::const_iterator _end;
      uint32_t _lineCount{1};
      std::string::const_iterator _lineStart;
    };
  }
}
