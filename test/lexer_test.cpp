//
//  csvsqldb test
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


#include "libcsvsqldb/base/lexer.h"
#include "libcsvsqldb/base/string_helper.h"

#include "test/test_util.h"
#include "test_helper.h"

#include <catch2/catch.hpp>

#include <map>


namespace
{
  class TestSQLLexer
  {
  public:
    enum eToken {
      IDENTIFIER,
      FLOAT,
      INTEGER,
      EQUAL,
      COMMA,
      LEFT_PAREN,
      RIGHT_PAREN,
      STAR,
      STRING,
      COMMENT,
      SELECT,
      FROM,
      WHERE,
      BY,
      GROUP,
      LIMIT,
      SUM,
      COUNT,
      LIKE,
      AND
    };

    TestSQLLexer(const std::string& text)
    : _lexer(std::bind(&TestSQLLexer::inspectToken, this, std::placeholders::_1))
    {
      _lexer.addDefinition("identifier", R"([_a-zA-Z][_a-zA-Z0-9]*)", IDENTIFIER);
      _lexer.addDefinition("float", R"([+-]?(?:0|[1-9]\d*)\.\d+)", FLOAT);
      _lexer.addDefinition("integer", R"(0|[+-]?[1-9]\d*)", INTEGER);
      _lexer.addDefinition("equal", R"(=)", EQUAL);
      _lexer.addDefinition("comma", R"(,)", COMMA);
      _lexer.addDefinition("left_paren", R"(\()", LEFT_PAREN);
      _lexer.addDefinition("right_paren", R"(\))", RIGHT_PAREN);
      _lexer.addDefinition("star", R"(\*)", STAR);
      _lexer.addDefinition("string", R"('([^']*)')", STRING);
      _lexer.addDefinition("comment", R"(//([^\r\n]*))", COMMENT);
      _lexer.addDefinition("comment", R"(/\*((.|[\r\n])*?)\*/)", COMMENT);

      _keywords["SELECT"] = eToken(SELECT);
      _keywords["FROM"] = eToken(FROM);
      _keywords["WHERE"] = eToken(WHERE);
      _keywords["SUM"] = eToken(SUM);
      _keywords["COUNT"] = eToken(COUNT);
      _keywords["LIKE"] = eToken(LIKE);
      _keywords["AND"] = eToken(AND);
      _keywords["GROUP"] = eToken(GROUP);
      _keywords["BY"] = eToken(BY);

      _lexer.setInput(text);
    }

    csvsqldb::lexer::Token next()
    {
      return _lexer.next();
    }

    uint32_t lineCount() const
    {
      return _lexer.lineCount();
    }

  private:
    typedef std::map<std::string, eToken> Keywords;

    void inspectToken(csvsqldb::lexer::Token& token)
    {
      if (token._token == IDENTIFIER) {
        Keywords::const_iterator iter = _keywords.find(csvsqldb::toupper_copy(token._value));
        if (iter != _keywords.end()) {
          token._token = iter->second;
        }
      }
    }

    csvsqldb::lexer::Lexer _lexer;
    Keywords _keywords;
  };
}


TEST_CASE("Lexer Test", "[lexer]")
{
  SECTION("sql lexer")
  {
    std::string text = R"(/** and now a c style comment
with nested new lines
and more new lines **/
// this is a test
SELECT a,sum(b),count(*),c_d from test where a like 'test' and b= -70.6
group by a,c_d)";

    TestSQLLexer lexer(text);
    csvsqldb::lexer::Token token = lexer.next();
    std::vector<csvsqldb::lexer::Token> tokens;
    int n = 0;
    while (token._token != csvsqldb::lexer::EOI) {
      tokens.push_back(token);
      ++n;
      token = lexer.next();
    }
    CHECK(6u == lexer.lineCount());
    CHECK(TestSQLLexer::COMMENT == tokens[0]._token);
    CHECK(TestSQLLexer::SELECT == tokens[2]._token);
    CHECK(tokens[2]._value == "SELECT");
    CHECK(TestSQLLexer::SUM == tokens[5]._token);
    CHECK(tokens[5]._value == "sum");
    CHECK(TestSQLLexer::LEFT_PAREN == tokens[6]._token);
    CHECK(TestSQLLexer::STAR == tokens[12]._token);
    CHECK(TestSQLLexer::FROM == tokens[16]._token);
    CHECK(tokens[16]._value == "from");
    CHECK(TestSQLLexer::STRING == tokens[21]._token);
    CHECK(tokens[21]._value == "test");
    CHECK(TestSQLLexer::FLOAT == tokens[25]._token);
    CHECK(tokens[25]._value == "-70.6");
  }

  SECTION("exception")
  {
    std::string text("select 1.1.1 from test");
    TestSQLLexer lexer(text);
    csvsqldb::lexer::Token token = lexer.next();
    token = lexer.next();
    CHECK_THROWS_AS(lexer.next(), csvsqldb::LexicalAnalysisException);
  }
}
