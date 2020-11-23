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


#include "csvsqldb/block_iterator.h"

#include <catch2/catch.hpp>


namespace
{
  class MockBlockProvider : public csvsqldb::BlockProvider
  {
  public:
    MockBlockProvider(std::function<csvsqldb::BlockPtr()> provider)
    : _provider{std::move(provider)}
    {
    }

    csvsqldb::BlockPtr getNextBlock() override
    {
      return _provider();
    }

  private:
    std::function<csvsqldb::BlockPtr()> _provider;
  };

  static csvsqldb::BlockPtr createBlock(csvsqldb::BlockManager& manager)
  {
    auto block = manager.createBlock();
    std::fill(block->getRawBuffer(), block->getRawBuffer() + csvsqldb::sDefaultBlockCapacity, 0);
    return block;
  }
}


TEST_CASE("Block Iterator Test", "[block]")
{
  csvsqldb::Types types{csvsqldb::INT, csvsqldb::BOOLEAN, csvsqldb::DATE};
  csvsqldb::BlockManager manager;

  SECTION("next row")
  {
    MockBlockProvider provider{[&manager]() {
      static csvsqldb::BlockPtr block{createBlock(manager)};
      static uint16_t count{0};

      switch (count) {
        case 0:
          block->addInt(4711, false);
          block->addBool(true, false);
          block->addDate(csvsqldb::Date{2020, csvsqldb::Date::November, 21}, false);
          block->nextRow();

          block->addInt(42, false);
          block->addBool(false, false);
          block->addDate(csvsqldb::Date{}, true);
          block->nextRow();
          block->markNextBlock();
          ++count;
          break;
        case 1:
          block = createBlock(manager);
          block->addInt(815, false);
          block->addBool(false, false);
          block->addDate(csvsqldb::Date{1970, csvsqldb::Date::May, 17}, false);
          block->nextRow();
          block->endBlocks();
          ++count;
          break;
        default:
          block = nullptr;
      }

      return block;
    }};

    csvsqldb::BlockIterator iter{types, provider, manager};

    auto* values = iter.getNextRow();
    REQUIRE(values);
    REQUIRE(3 == values->size());
    CHECK(csvsqldb::INT == values->at(0)->getType());
    CHECK(csvsqldb::BOOLEAN == values->at(1)->getType());
    CHECK(csvsqldb::DATE == values->at(2)->getType());

    values = iter.getNextRow();
    REQUIRE(values);
    REQUIRE(3 == values->size());
    CHECK(csvsqldb::INT == values->at(0)->getType());
    CHECK(csvsqldb::BOOLEAN == values->at(1)->getType());
    CHECK(csvsqldb::DATE == values->at(2)->getType());

    values = iter.getNextRow();
    REQUIRE(values);
    REQUIRE(3 == values->size());
    CHECK(csvsqldb::INT == values->at(0)->getType());
    CHECK(csvsqldb::BOOLEAN == values->at(1)->getType());
    CHECK(csvsqldb::DATE == values->at(2)->getType());

    values = iter.getNextRow();
    CHECK_FALSE(values);
  }
  SECTION("missing row delimiter")
  {
    MockBlockProvider provider{[&manager]() {
      static csvsqldb::BlockPtr block{createBlock(manager)};

      block->addInt(4711, false);
      block->addBool(true, false);
      block->addDate(csvsqldb::Date{2020, csvsqldb::Date::November, 21}, false);
      block->endBlocks();

      return block;
    }};

    csvsqldb::BlockIterator iter{types, provider, manager};

    const auto* values = iter.getNextRow();
    REQUIRE(values);
    REQUIRE(3 == values->size());
    CHECK_THROWS_WITH(iter.getNextRow(), "should be at row delimiter");
  }
  SECTION("missing end delimiter")
  {
    MockBlockProvider provider{[&manager]() {
      static csvsqldb::BlockPtr block{createBlock(manager)};

      block->addInt(4711, false);
      block->addBool(true, false);
      block->addDate(csvsqldb::Date{2020, csvsqldb::Date::November, 21}, false);
      block->nextRow();

      return block;
    }};

    csvsqldb::BlockIterator iter{types, provider, manager};

    const auto* values = iter.getNextRow();
    REQUIRE(values);
    REQUIRE(3 == values->size());
    CHECK_THROWS_WITH(iter.getNextRow(), "should have found the end marker in the first place");
  }
  SECTION("not enough values")
  {
    MockBlockProvider provider{[&manager]() {
      static csvsqldb::BlockPtr block{createBlock(manager)};

      block->addInt(4711, false);
      block->addBool(true, false);

      return block;
    }};

    csvsqldb::BlockIterator iter{types, provider, manager};

    CHECK_THROWS_WITH(iter.getNextRow(), "expected more values, but already at end of block");
  }
  SECTION("missing value separator")
  {
    MockBlockProvider provider{[&manager]() {
      static csvsqldb::BlockPtr block{createBlock(manager)};

      block->addInt(4711, false);
      auto* blockPos = block->getRawBuffer(block->offset());
      block->addBool(true, false);
      *blockPos = 0;

      return block;
    }};

    csvsqldb::BlockIterator iter{types, provider, manager};

    CHECK_THROWS_WITH(iter.getNextRow(), "missing value separator");
  }
  SECTION("wrong type")
  {
    csvsqldb::Types wrongTypes{csvsqldb::INT, csvsqldb::NONE, csvsqldb::DATE};

    MockBlockProvider provider{[&manager]() {
      static csvsqldb::BlockPtr block{createBlock(manager)};

      block->addInt(4711, false);
      block->addBool(true, false);
      block->addDate(csvsqldb::Date{2020, csvsqldb::Date::November, 21}, false);
      block->nextRow();

      return block;
    }};

    csvsqldb::BlockIterator iter{wrongTypes, provider, manager};

    CHECK_THROWS_WITH(iter.getNextRow(), "type not allowed none");
  }
  SECTION("missing next block")
  {
    MockBlockProvider provider{[&manager]() {
      static csvsqldb::BlockPtr block{createBlock(manager)};
      static bool first{true};

      if (first) {
        block->addInt(4711, false);
        block->addBool(true, false);
        block->addDate(csvsqldb::Date{2020, csvsqldb::Date::November, 21}, false);
        block->nextRow();
        *block->getRawBuffer(block->offset()) = csvsqldb::sBlockMarker;
        first = false;
      } else {
        block = nullptr;
      }

      return block;
    }};

    csvsqldb::BlockIterator iter{types, provider, manager};

    const auto* values = iter.getNextRow();
    REQUIRE(values);
    REQUIRE(3 == values->size());
    CHECK_THROWS_WITH(iter.getNextRow(), "next block is missing");
  }
}
