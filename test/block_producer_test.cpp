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

#include <csvsqldb/block_iterator.h>
#include <csvsqldb/block_producer.h>

#include "test/test_util.h"

#include <catch2/catch.hpp>


TEST_CASE("Block Producer Test", "[block]")
{
  csvsqldb::BlockManager blockManager;
  csvsqldb::BlockProducer dataProducer{blockManager};

  SECTION("Produce")
  {
    constexpr size_t numFields = 1000;
    constexpr size_t rows = 2000;

    csvsqldb::Types types;
    for (auto m = 0u; m < numFields; ++m) {
      types.emplace_back(csvsqldb::INT);
    }

    dataProducer.start([&](csvsqldb::BlockProducer& producer) {
      for (auto n = 0u; n < rows; ++n) {
        for (auto m = 0u; m < numFields; ++m) {
          producer.addBool(true, false);
        }
        producer.nextRow();
      }
    });

    auto iterator = csvsqldb::BlockIterator(types, dataProducer, blockManager);
    bool hasRows{true};
    for (auto n = 0u; n < rows; ++n) {
      hasRows &= !!iterator.getNextRow();
    }
    CHECK(hasRows);
    CHECK_FALSE(iterator.getNextRow());
  }
  SECTION("Produce no consume")
  {
    dataProducer.start([](csvsqldb::BlockProducer& producer) { producer.addBool(true, false); });
  }
  SECTION("Produce exception")
  {
    constexpr size_t numFields = 10;

    csvsqldb::Types types;
    for (auto m = 0u; m < numFields; ++m) {
      types.emplace_back(csvsqldb::INT);
    }

    dataProducer.start([&](csvsqldb::BlockProducer& producer) {
      for (auto m = 0u; m < numFields; ++m) {
        producer.addBool(true, false);
        throw csvsqldb::Exception("exception");
      }
    });

    auto iterator = csvsqldb::BlockIterator(types, dataProducer, blockManager);
    CHECK_THROWS_WITH(iterator.getNextRow(), "exception");
  }
}