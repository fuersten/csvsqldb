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
  class MyBlockProvider : public csvsqldb::BlockProvider
  {
  public:
    MyBlockProvider(csvsqldb::BlockManager& manager)
    : _manager{manager}
    {
      _block = _manager.createBlock();
    }

    csvsqldb::BlockPtr getNextBlock() override
    {
      switch (_count) {
        case 0:
          _block->addInt(4711, false);
          _block->addBool(true, false);
          _block->addDate(csvsqldb::Date{2020, csvsqldb::Date::November, 21}, false);
          _block->nextRow();

          _block->addInt(42, false);
          _block->addBool(false, false);
          _block->addDate(csvsqldb::Date{}, true);
          _block->nextRow();
          _block->markNextBlock();
          ++_count;
          break;
        case 1:
          _block = _manager.createBlock();
          _block->addInt(815, false);
          _block->addBool(false, false);
          _block->addDate(csvsqldb::Date{1970, csvsqldb::Date::May, 17}, false);
          _block->nextRow();
          _block->endBlocks();
          break;
        default:
          _block = nullptr;
      }

      return _block;
    }

  private:
    uint8_t _count{0};
    csvsqldb::BlockManager& _manager;
    csvsqldb::BlockPtr _block{nullptr};
  };
}


TEST_CASE("Block Iterator Test", "[block]")
{
  csvsqldb::Types types{csvsqldb::INT, csvsqldb::BOOLEAN, csvsqldb::DATE};
  csvsqldb::BlockManager manager;
  MyBlockProvider provider{manager};

  SECTION("next row")
  {
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
}
