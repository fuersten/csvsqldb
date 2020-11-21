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


#include "csvsqldb/block.h"

#include <catch2/catch.hpp>


TEST_CASE("BlockManager Test", "[block]")
{
  SECTION("construction")
  {
    {
      csvsqldb::BlockManager blockManager;
      CHECK(csvsqldb::sDefaultBlockCapacity == blockManager.getBlockCapacity());
      CHECK(csvsqldb::sDefaultMaxActiveBlocks == blockManager.getMaxActiveBlocks());
      CHECK(0u == blockManager.getActiveBlocks());
      CHECK(0u == blockManager.getMaxUsedBlocks());
      CHECK(0u == blockManager.getTotalBlocks());
    }

    {
      csvsqldb::BlockManager blockManager(1000, 1024 * 1024 * 1024);
      CHECK(1024u * 1024 * 1024 == blockManager.getBlockCapacity());
      CHECK(1000u == blockManager.getMaxActiveBlocks());
      CHECK(0u == blockManager.getActiveBlocks());
      CHECK(0u == blockManager.getMaxUsedBlocks());
      CHECK(0u == blockManager.getTotalBlocks());
    }
  }
  SECTION("create blocks")
  {
    csvsqldb::Types types;
    types.push_back(csvsqldb::INT);
    types.push_back(csvsqldb::DATE);
    types.push_back(csvsqldb::STRING);

    csvsqldb::BlockManager blockManager;

    csvsqldb::BlockPtr block = blockManager.createBlock();
    CHECK(1u == blockManager.getMaxUsedBlocks());
    CHECK(1u == blockManager.getTotalBlocks());
    CHECK(1u == blockManager.getActiveBlocks());

    csvsqldb::BlockPtr block2 = blockManager.createBlock();
    CHECK(2u == blockManager.getActiveBlocks());
    blockManager.release(block);
    CHECK(2u == blockManager.getMaxUsedBlocks());
    CHECK(2u == blockManager.getTotalBlocks());
    CHECK(1u == blockManager.getActiveBlocks());
    blockManager.release(block2);
    CHECK(2u == blockManager.getMaxUsedBlocks());
    CHECK(2u == blockManager.getTotalBlocks());
    CHECK(0u == blockManager.getActiveBlocks());
  }
  SECTION("create too many blocks")
  {
    csvsqldb::BlockManager blockManager{2};
    CHECK(2 == blockManager.getMaxActiveBlocks());
    blockManager.createBlock();
    blockManager.createBlock();
    CHECK(2 == blockManager.getActiveBlocks());
    CHECK(2 == blockManager.getTotalBlocks());
    CHECK(2 == blockManager.getMaxUsedBlocks());
    CHECK_THROWS_AS(blockManager.createBlock(), csvsqldb::Exception);
    CHECK(2 == blockManager.getActiveBlocks());
    CHECK(2 == blockManager.getTotalBlocks());
    CHECK(2 == blockManager.getMaxUsedBlocks());
  }
  SECTION("get block")
  {
    csvsqldb::BlockManager blockManager;

    csvsqldb::BlockPtr block = blockManager.createBlock();
    csvsqldb::BlockPtr block2 = blockManager.createBlock();

    csvsqldb::BlockPtr block3 = blockManager.getBlock(block->getBlockNumber());
    CHECK(block3);
    CHECK(2 == blockManager.getActiveBlocks());
    CHECK(block->getBlockNumber() == block3->getBlockNumber());

    block3 = blockManager.getBlock(block2->getBlockNumber());
    CHECK(block3);
    CHECK(block2->getBlockNumber() == block3->getBlockNumber());

    size_t blockNumber = block2->getBlockNumber();
    blockManager.release(block2);
    CHECK(1 == blockManager.getActiveBlocks());
    CHECK_THROWS_AS(block3 = blockManager.getBlock(blockNumber), csvsqldb::Exception);

    blockManager.release(block);
    CHECK(0 == blockManager.getActiveBlocks());
    CHECK(2 == blockManager.getTotalBlocks());
    CHECK(2 == blockManager.getMaxUsedBlocks());
  }
}
