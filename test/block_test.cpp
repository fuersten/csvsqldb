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

#include "libcsvsqldb/sql_astdump.h"
#include "libcsvsqldb/block.h"
#include "libcsvsqldb/block_iterator.h"
#include "libcsvsqldb/sql_parser.h"
#include "libcsvsqldb/stack_machine.h"
#include "libcsvsqldb/visitor.h"

#include "libcsvsqldb/base/tribool.h"


class MyBlockProvider : public csvsqldb::BlockProvider
{
public:
    MyBlockProvider(csvsqldb::BlockPtr block, csvsqldb::BlockManager& manager)
    : _block(block)
    , _manager(manager)
    {}
    
    virtual csvsqldb::BlockPtr getNextBlock()
    {
        return _block;
    }
    
    virtual csvsqldb::BlockManager& getBlockManager()
    {
        return _manager;
    }
    
private:
    csvsqldb::BlockPtr _block;
    csvsqldb::BlockManager& _manager;
};


class BlockTestCase
{
public:
    BlockTestCase()
    {
    }
    
    void setUp()
    {
    }
    
    void tearDown()
    {
    }
    
    csvsqldb::BlockPtr setupBlock(csvsqldb::BlockManager& blockManager)
    {
        csvsqldb::BlockPtr block = blockManager.createBlock();
        block->addValue(csvsqldb::Variant(4711));
        block->addValue(csvsqldb::Variant(csvsqldb::Date(1970, csvsqldb::Date::September, 23)));
        block->addValue(csvsqldb::Variant("Lars"));
        block->addValue(csvsqldb::Variant("Fürstenberg"));
        block->addValue(csvsqldb::Variant("M"));
        block->addValue(csvsqldb::Variant(csvsqldb::Date(2012, csvsqldb::Date::February, 1)));
        block->nextRow();
        block->addValue(csvsqldb::Variant(815));
        block->addValue(csvsqldb::Variant(csvsqldb::Date(1969, csvsqldb::Date::May, 17)));
        block->addValue(csvsqldb::Variant("Mark"));
        block->addValue(csvsqldb::Variant("Fürstenberg"));
        block->addValue(csvsqldb::Variant("M"));
        block->addValue(csvsqldb::Variant(csvsqldb::Date(2003, csvsqldb::Date::April, 15)));
        block->nextRow();
        block->endBlocks();
        
        return block;
    }
    
    void rowTest()
    {
        csvsqldb::BlockManager blockManager;
        
        csvsqldb::Types types;
        types.push_back(csvsqldb::INT);
        types.push_back(csvsqldb::DATE);
        types.push_back(csvsqldb::STRING);
        types.push_back(csvsqldb::STRING);
        types.push_back(csvsqldb::STRING);
        types.push_back(csvsqldb::DATE);
        
        {
            csvsqldb::FunctionRegistry functions;
            csvsqldb::SQLParser parser(functions);
            csvsqldb::VariableStore store;
            
            csvsqldb::ASTExprNodePtr exp = parser.parseExpression("hire_date > DATE'2012-01-01'");
            csvsqldb::StackMachine::VariableMapping mapping;
            csvsqldb::StackMachine sm;
            csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
            exp->accept(visitor);

            csvsqldb::BlockPtr block = setupBlock(blockManager);
            MyBlockProvider blockProvider(block, blockManager);
            csvsqldb::BlockIteratorPtr iterator = std::make_shared<csvsqldb::BlockIterator>(types, blockProvider, blockManager);
            
            {
                const csvsqldb::Values& rowValues = *(iterator->getNextRow());
                
                const csvsqldb::Value* val = rowValues[0];
                MPF_TEST_ASSERTEQUAL("4711", val->toString());
                val = rowValues[1];
                MPF_TEST_ASSERTEQUAL("1970-09-23", val->toString());
                val = rowValues[2];
                MPF_TEST_ASSERTEQUAL("Lars", val->toString());
                val = rowValues[3];
                MPF_TEST_ASSERTEQUAL("Fürstenberg", val->toString());
                val = rowValues[4];
                MPF_TEST_ASSERTEQUAL("M", val->toString());
                val = rowValues[5];
                MPF_TEST_ASSERTEQUAL("2012-02-01", val->toString());
                
                store.addVariable(0, csvsqldb::Variant(dynamic_cast<const csvsqldb::ValDate*>(val)->asDate()));
                bool matches = sm.evaluate(store, functions).asBool();
                MPF_TEST_ASSERT(matches);
            }
            
            {
                const csvsqldb::Values& rowValues = *(iterator->getNextRow());
                
                const csvsqldb::Value* val = rowValues[0];
                MPF_TEST_ASSERTEQUAL("815", val->toString());
                val = rowValues[1];
                MPF_TEST_ASSERTEQUAL("1969-05-17", val->toString());
                val = rowValues[2];
                MPF_TEST_ASSERTEQUAL("Mark", val->toString());
                val = rowValues[3];
                MPF_TEST_ASSERTEQUAL("Fürstenberg", val->toString());
                val = rowValues[4];
                MPF_TEST_ASSERTEQUAL("M", val->toString());
                val = rowValues[5];
                MPF_TEST_ASSERTEQUAL("2003-04-15", val->toString());
                
                store.addVariable(0, csvsqldb::Variant(dynamic_cast<const csvsqldb::ValDate*>(val)->asDate()));
                bool matches = sm.evaluate(store, functions).asBool();
                MPF_TEST_ASSERT(!matches);
            }
        }
        
        {
            csvsqldb::FunctionRegistry functions;
            csvsqldb::SQLParser parser(functions);
            csvsqldb::VariableStore store;
            
            csvsqldb::ASTExprNodePtr exp = parser.parseExpression("emp.last_name = 'Fürstenberg'");
            csvsqldb::StackMachine::VariableMapping mapping;
            csvsqldb::StackMachine sm;
            csvsqldb::ASTInstructionStackVisitor visitor(sm, mapping);
            exp->accept(visitor);
            
            csvsqldb::BlockPtr block = setupBlock(blockManager);
            MyBlockProvider blockProvider(block, blockManager);
            csvsqldb::BlockIteratorPtr iterator = std::make_shared<csvsqldb::BlockIterator>(types, blockProvider, blockManager);

            const csvsqldb::Values* row = nullptr;
            int count = 0;
            while( (row = iterator->getNextRow()) ) {
                if(count == 0) {
                    MPF_TEST_ASSERTEQUAL("4711", (*row)[0]->toString());
                    MPF_TEST_ASSERTEQUAL("1970-09-23", (*row)[1]->toString());
                    MPF_TEST_ASSERTEQUAL("Lars", (*row)[2]->toString());
                    MPF_TEST_ASSERTEQUAL("Fürstenberg", (*row)[3]->toString());
                    MPF_TEST_ASSERTEQUAL("M", (*row)[4]->toString());
                    MPF_TEST_ASSERTEQUAL("2012-02-01", (*row)[5]->toString());
                    
                    store.addVariable(0, csvsqldb::Variant(dynamic_cast<const csvsqldb::ValString*>((*row)[3])->asString()));
                    bool matches = sm.evaluate(store, functions).asBool();
                    MPF_TEST_ASSERT(matches);
                } else if(count == 1) {
                    MPF_TEST_ASSERTEQUAL("815", (*row)[0]->toString());
                    MPF_TEST_ASSERTEQUAL("1969-05-17", (*row)[1]->toString());
                    MPF_TEST_ASSERTEQUAL("Mark", (*row)[2]->toString());
                    MPF_TEST_ASSERTEQUAL("Fürstenberg", (*row)[3]->toString());
                    MPF_TEST_ASSERTEQUAL("M", (*row)[4]->toString());
                    MPF_TEST_ASSERTEQUAL("2003-04-15", (*row)[5]->toString());
                    
                    store.addVariable(0, csvsqldb::Variant(dynamic_cast<const csvsqldb::ValString*>((*row)[3])->asString()));
                    bool matches = sm.evaluate(store, functions).asBool();
                    MPF_TEST_ASSERT(matches);
                }
                ++count;
            }
        }
    }
};

MPF_REGISTER_TEST_START("BlockTestSuite", BlockTestCase);
MPF_REGISTER_TEST(BlockTestCase::rowTest);
MPF_REGISTER_TEST_END();
