//
//  csvsqldb test
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


#include "test.h"

#include "libcsvsqldb/block.h"


class BlockManagerTestCase
{
public:
    BlockManagerTestCase()
    {
    }

    void setUp()
    {
    }

    void tearDown()
    {
    }

    void constructionTest()
    {
        {
            csvsqldb::BlockManager blockManager;
            MPF_TEST_ASSERTEQUAL(1u * 1024 * 1024, blockManager.getBlockCapacity());
            MPF_TEST_ASSERTEQUAL(100u, blockManager.getMaxActiveBlocks());
            MPF_TEST_ASSERTEQUAL(0u, blockManager.getActiveBlocks());
            MPF_TEST_ASSERTEQUAL(0u, blockManager.getMaxUsedBlocks());
            MPF_TEST_ASSERTEQUAL(0u, blockManager.getTotalBlocks());
        }

        {
            csvsqldb::BlockManager blockManager(1000, 1024 * 1024);
            MPF_TEST_ASSERTEQUAL(1024u * 1024, blockManager.getBlockCapacity());
            MPF_TEST_ASSERTEQUAL(1000u, blockManager.getMaxActiveBlocks());
            MPF_TEST_ASSERTEQUAL(0u, blockManager.getActiveBlocks());
            MPF_TEST_ASSERTEQUAL(0u, blockManager.getMaxUsedBlocks());
            MPF_TEST_ASSERTEQUAL(0u, blockManager.getTotalBlocks());
        }
    }

    void createBlocks()
    {
        csvsqldb::Types types;
        types.push_back(csvsqldb::INT);
        types.push_back(csvsqldb::DATE);
        types.push_back(csvsqldb::STRING);

        csvsqldb::BlockManager blockManager;

        csvsqldb::BlockPtr block = blockManager.createBlock();
        MPF_TEST_ASSERTEQUAL(1u, blockManager.getMaxUsedBlocks());
        MPF_TEST_ASSERTEQUAL(1u, blockManager.getTotalBlocks());
        MPF_TEST_ASSERTEQUAL(1u, blockManager.getActiveBlocks());

        csvsqldb::BlockPtr block2 = blockManager.createBlock();
        MPF_TEST_ASSERTEQUAL(2u, blockManager.getActiveBlocks());
        blockManager.release(block);
        MPF_TEST_ASSERTEQUAL(2u, blockManager.getMaxUsedBlocks());
        MPF_TEST_ASSERTEQUAL(2u, blockManager.getTotalBlocks());
        MPF_TEST_ASSERTEQUAL(1u, blockManager.getActiveBlocks());
        blockManager.release(block2);
        MPF_TEST_ASSERTEQUAL(2u, blockManager.getMaxUsedBlocks());
        MPF_TEST_ASSERTEQUAL(2u, blockManager.getTotalBlocks());
        MPF_TEST_ASSERTEQUAL(0u, blockManager.getActiveBlocks());
    }

    void getBlockTest()
    {
        csvsqldb::Types types;
        types.push_back(csvsqldb::INT);
        types.push_back(csvsqldb::DATE);
        types.push_back(csvsqldb::STRING);

        csvsqldb::BlockManager blockManager;

        csvsqldb::BlockPtr block = blockManager.createBlock();
        csvsqldb::BlockPtr block2 = blockManager.createBlock();

        csvsqldb::BlockPtr block3 = blockManager.getBlock(block->getBlockNumber());
        MPF_TEST_ASSERT(block3);
        MPF_TEST_ASSERTEQUAL(block->getBlockNumber(), block3->getBlockNumber());

        block3 = blockManager.getBlock(block2->getBlockNumber());
        MPF_TEST_ASSERT(block3);
        MPF_TEST_ASSERTEQUAL(block2->getBlockNumber(), block3->getBlockNumber());

        size_t blockNumber = block2->getBlockNumber();
        blockManager.release(block2);
        MPF_TEST_EXPECTS(block3 = blockManager.getBlock(blockNumber), csvsqldb::Exception);

        blockManager.release(block);
    }
};

MPF_REGISTER_TEST_START("BlockTestSuite", BlockManagerTestCase);
MPF_REGISTER_TEST(BlockManagerTestCase::constructionTest);
MPF_REGISTER_TEST(BlockManagerTestCase::createBlocks);
MPF_REGISTER_TEST_END();
