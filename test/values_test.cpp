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

#include "libcsvsqldb/values.h"

#include "libcsvsqldb/base/float_helper.h"

#include <array>


class ValuesTestCase
{
public:
    ValuesTestCase()
    {
    }
    
    void setUp()
    {
    }
    
    void tearDown()
    {
    }
    
    void valueCreatorTest()
    {
        csvsqldb::Value* v1 = csvsqldb::ValueCreator<std::string>::createValue(std::string("Lars"));
        csvsqldb::Value* v2 = csvsqldb::ValueCreator<std::string>::createValue(std::string("Fürstenberg"));

        MPF_TEST_ASSERTEQUAL("Lars", v1->toString());
        MPF_TEST_ASSERTEQUAL("Fürstenberg", v2->toString());
        MPF_TEST_ASSERTEQUAL(csvsqldb::STRING, v1->getType());
        
        delete v1;
        delete v2;
        
        v1 = csvsqldb::ValueCreator<int64_t>::createValue(4711);
        MPF_TEST_ASSERTEQUAL("4711", v1->toString());
        MPF_TEST_ASSERTEQUAL(csvsqldb::INT, v1->getType());
        csvsqldb::ValInt* pi = static_cast<csvsqldb::ValInt*>(v1);
        MPF_TEST_ASSERTEQUAL(4711, pi->asInt());
        delete v1;
        
        v1 = csvsqldb::ValueCreator<double>::createValue(47.11);
        MPF_TEST_ASSERTEQUAL(csvsqldb::REAL, v1->getType());
        MPF_TEST_ASSERTEQUAL("47.110000", v1->toString());
        csvsqldb::ValDouble* pd = static_cast<csvsqldb::ValDouble*>(v1);
        MPF_TEST_ASSERT(csvsqldb::compare(47.11, pd->asDouble()));
        delete v1;

        v1 = csvsqldb::ValueCreator<bool>::createValue(true);
        MPF_TEST_ASSERTEQUAL(csvsqldb::BOOLEAN, v1->getType());
        MPF_TEST_ASSERTEQUAL("1", v1->toString());
        csvsqldb::ValBool* pb = static_cast<csvsqldb::ValBool*>(v1);
        MPF_TEST_ASSERTEQUAL(true, pb->asBool());
        delete v1;

        v1 = csvsqldb::ValueCreator<csvsqldb::Date>::createValue(csvsqldb::Date(1970, csvsqldb::Date::September, 23));
        MPF_TEST_ASSERTEQUAL(csvsqldb::DATE, v1->getType());
        MPF_TEST_ASSERTEQUAL("1970-09-23", v1->toString());
        csvsqldb::ValDate* pdt = static_cast<csvsqldb::ValDate*>(v1);
        MPF_TEST_ASSERTEQUAL(csvsqldb::Date(1970, csvsqldb::Date::September, 23), pdt->asDate());
        delete v1;
        
        v1 = csvsqldb::ValueCreator<csvsqldb::Time>::createValue(csvsqldb::Time(8, 9, 11));
        MPF_TEST_ASSERTEQUAL(csvsqldb::TIME, v1->getType());
        MPF_TEST_ASSERTEQUAL("08:09:11", v1->toString());
        csvsqldb::ValTime* pt = static_cast<csvsqldb::ValTime*>(v1);
        MPF_TEST_ASSERTEQUAL(csvsqldb::Time(8, 9, 11), pt->asTime());
        delete v1;
        
        v1 = csvsqldb::ValueCreator<csvsqldb::Timestamp>::createValue(csvsqldb::Timestamp(1970, csvsqldb::Date::September, 23, 8, 9, 11));
        MPF_TEST_ASSERTEQUAL(csvsqldb::TIMESTAMP, v1->getType());
        MPF_TEST_ASSERTEQUAL("1970-09-23T08:09:11", v1->toString());
        csvsqldb::ValTimestamp* pts = static_cast<csvsqldb::ValTimestamp*>(v1);
        MPF_TEST_ASSERTEQUAL(csvsqldb::Timestamp(1970, csvsqldb::Date::September, 23, 8, 9, 11), pts->asTimestamp());
        delete v1;
    }
    
    void storeTest()
    {
        typedef std::array<char, 4096> StoreType;
        
        StoreType store;
        size_t offset = 0;
        
        {
            csvsqldb::ValInt* pi = new (&store[0]) csvsqldb::ValInt(4711l);
            offset = pi->size();
            csvsqldb::ValDouble* pd = new (&store[0] + offset) csvsqldb::ValDouble(47.11);
            offset += pd->size();
            csvsqldb::ValBool* pb = new (&store[0] + offset) csvsqldb::ValBool(true);
            offset += pb->size();
            csvsqldb::ValDate* pdt = new (&store[0] + offset) csvsqldb::ValDate(csvsqldb::Date(1970, csvsqldb::Date::September, 23));
            offset += pdt->size();
            csvsqldb::ValTime* pt = new (&store[0] + offset) csvsqldb::ValTime(csvsqldb::Time(8, 9, 11));
            offset += pt->size();
            csvsqldb::ValTimestamp* pts = new (&store[0] + offset) csvsqldb::ValTimestamp(csvsqldb::Timestamp(1970, csvsqldb::Date::September, 23, 8, 9, 11));
            offset += pts->size();
            char* psd = &store[0] + offset + sizeof(csvsqldb::ValString);
            ::strcpy(psd, "Lars Fürstenberg");
            csvsqldb::ValString* ps = new (&store[0] + offset) csvsqldb::ValString(psd);
            
            MPF_TEST_ASSERTEQUAL("4711", pi->toString());
            MPF_TEST_ASSERTEQUAL("47.110000", pd->toString());
            MPF_TEST_ASSERTEQUAL("1", pb->toString());
            MPF_TEST_ASSERTEQUAL("1970-09-23", pdt->toString());
            MPF_TEST_ASSERTEQUAL("1970-09-23T08:09:11", pts->toString());
            MPF_TEST_ASSERTEQUAL("Lars Fürstenberg", ps->toString());
            MPF_TEST_ASSERTEQUAL("08:09:11", pt->toString());
        }
        
        {
            csvsqldb::ValInt* pi = reinterpret_cast<csvsqldb::ValInt*>(&store[0]);
            MPF_TEST_ASSERTEQUAL("4711", pi->toString());
            
            csvsqldb::ValString* ps = reinterpret_cast<csvsqldb::ValString*>(&store[0] + offset);
            MPF_TEST_ASSERTEQUAL("Lars Fürstenberg", ps->toString());
        }
    }
};

MPF_REGISTER_TEST_START("ValuesTestSuite", ValuesTestCase);
MPF_REGISTER_TEST(ValuesTestCase::valueCreatorTest);
MPF_REGISTER_TEST(ValuesTestCase::storeTest);
MPF_REGISTER_TEST_END();
