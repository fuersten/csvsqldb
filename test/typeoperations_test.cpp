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

#include "libcsvsqldb/typeoperations.h"

#include "libcsvsqldb/base/float_helper.h"


class TypeoperationsTestCase
{
public:
    TypeoperationsTestCase()
    {
        csvsqldb::initTypeSystem();
    }
    
    void setUp()
    {
    }
    
    void tearDown()
    {
    }
    
    void operationsTest()
    {
        csvsqldb::Variant lhs(4711);
        csvsqldb::Variant rhs(815);
        
        csvsqldb::Variant result = binaryOperation(csvsqldb::OP_ADD, lhs, rhs);
        MPF_TEST_ASSERTEQUAL(csvsqldb::INT, result.getType());
        MPF_TEST_ASSERTEQUAL(5526, result.asInt());
        
        csvsqldb::Variant lhs1(4711.0);
        csvsqldb::Variant rhs1(815.0);
        
        csvsqldb::Variant result1 = binaryOperation(csvsqldb::OP_ADD, lhs1, rhs1);
        MPF_TEST_ASSERTEQUAL(csvsqldb::REAL, result1.getType());
        MPF_TEST_ASSERT(csvsqldb::compare(5526.0, result1.asDouble()));

        result1 = binaryOperation(csvsqldb::OP_ADD, lhs1, rhs);
        MPF_TEST_ASSERTEQUAL(csvsqldb::REAL, result1.getType());
        MPF_TEST_ASSERT(csvsqldb::compare(5526.0, result1.asDouble()));
        
        result1 = binaryOperation(csvsqldb::OP_ADD, lhs, rhs1);
        MPF_TEST_ASSERTEQUAL(csvsqldb::REAL, result1.getType());
        MPF_TEST_ASSERT(csvsqldb::compare(5526.0, result1.asDouble()));
    }
    
    void valueConcatTest()
    {
        csvsqldb::Variant i(4711);
        csvsqldb::Variant d(47.11);
        
        csvsqldb::Variant ret = binaryOperation(csvsqldb::OP_ADD, i, d);
        MPF_TEST_ASSERTEQUAL("4758.110000", ret.toString());
        
        csvsqldb::Variant s1(std::string("Lars "));
        csvsqldb::Variant s2(std::string("Fürstenberg"));
        
        csvsqldb::Variant result = binaryOperation(csvsqldb::OP_CONCAT, s1, s2);
        MPF_TEST_ASSERTEQUAL("Lars Fürstenberg", result.toString());
        
        csvsqldb::Variant ret2 = binaryOperation(csvsqldb::OP_CONCAT, d, result);
        MPF_TEST_ASSERTEQUAL("Lars Fürstenberg47.110000", ret2.toString());
        
        csvsqldb::Variant t(csvsqldb::Time(8, 9, 11));
        s1 = "The time is ";
        result = binaryOperation(csvsqldb::OP_CONCAT, s1, t);
        MPF_TEST_ASSERTEQUAL("The time is 08:09:11", result.toString());
        
        csvsqldb::Variant dt(csvsqldb::Date(2015, csvsqldb::Date::June, 29));
        s1 = "The date is ";
        result = binaryOperation(csvsqldb::OP_CONCAT, s1, dt);
        MPF_TEST_ASSERTEQUAL("The date is 2015-06-29", result.toString());

        csvsqldb::Variant ts(csvsqldb::Timestamp(2015, csvsqldb::Date::June, 29, 8, 9, 11));
        s1 = "The timestamp is ";
        result = binaryOperation(csvsqldb::OP_CONCAT, s1, ts);
        MPF_TEST_ASSERTEQUAL("The timestamp is 2015-06-29T08:09:11", result.toString());
    }
    
    void nullOperationsTest()
    {
        csvsqldb::Variant i(4711);
        csvsqldb::Variant b_true(true);
        csvsqldb::Variant b_false(false);
        csvsqldb::Variant null(csvsqldb::INT);
        
        csvsqldb::Variant result = binaryOperation(csvsqldb::OP_ADD, i, null);
        MPF_TEST_ASSERT(result.isNull());
        
        result = binaryOperation(csvsqldb::OP_AND, b_true, null);
        MPF_TEST_ASSERT(result.isNull());
        result = binaryOperation(csvsqldb::OP_AND, null, b_false);
        MPF_TEST_ASSERT(!result.isNull());
        MPF_TEST_ASSERT(!result.asBool());
        
        result = binaryOperation(csvsqldb::OP_OR, b_true, null);
        MPF_TEST_ASSERT(!result.isNull());
        MPF_TEST_ASSERT(result.asBool());
        result = binaryOperation(csvsqldb::OP_OR, b_false, null);
        MPF_TEST_ASSERT(result.isNull());
        result = binaryOperation(csvsqldb::OP_OR, null, b_true);
        MPF_TEST_ASSERT(!result.isNull());
        MPF_TEST_ASSERT(result.asBool());
    }
    
    void castOperationsTest()
    {
        csvsqldb::Variant lhs(4711);
        
        csvsqldb::Variant result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::REAL, lhs);
        MPF_TEST_ASSERTEQUAL(csvsqldb::REAL, result.getType());
        MPF_TEST_ASSERT(csvsqldb::compare(4711.0, result.asDouble()));
        
        lhs = csvsqldb::Variant("4711.0");
        result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::REAL, lhs);
        MPF_TEST_ASSERTEQUAL(csvsqldb::REAL, result.getType());
        MPF_TEST_ASSERT(csvsqldb::compare(4711.0, result.asDouble()));
        
        result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::INT, lhs);
        MPF_TEST_ASSERTEQUAL(csvsqldb::INT, result.getType());
        MPF_TEST_ASSERTEQUAL(4711, result.asInt());
        
        result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::BOOLEAN, lhs);
        MPF_TEST_ASSERTEQUAL(csvsqldb::BOOLEAN, result.getType());
        MPF_TEST_ASSERT(result.asBool());
        
        lhs = csvsqldb::Variant("08:09:11");
        result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::TIME, lhs);
        MPF_TEST_ASSERTEQUAL(csvsqldb::TIME, result.getType());
        
        lhs = csvsqldb::Variant("2015-06-29");
        result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::DATE, lhs);
        MPF_TEST_ASSERTEQUAL(csvsqldb::DATE, result.getType());
        
        lhs = csvsqldb::Variant("2015-06-29T08:09:11");
        result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::TIMESTAMP, lhs);
        MPF_TEST_ASSERTEQUAL(csvsqldb::TIMESTAMP, result.getType());
    }
    
    void nullCastOperationsTest()
    {
        csvsqldb::Variant result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::TIMESTAMP, csvsqldb::Variant(csvsqldb::NONE));
        MPF_TEST_ASSERTEQUAL(csvsqldb::TIMESTAMP, result.getType());
        MPF_TEST_ASSERT(result.isNull());
        result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::TIME, csvsqldb::Variant(csvsqldb::NONE));
        MPF_TEST_ASSERTEQUAL(csvsqldb::TIME, result.getType());
        MPF_TEST_ASSERT(result.isNull());
        result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::DATE, csvsqldb::Variant(csvsqldb::NONE));
        MPF_TEST_ASSERTEQUAL(csvsqldb::DATE, result.getType());
        MPF_TEST_ASSERT(result.isNull());
        result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::INT, csvsqldb::Variant(csvsqldb::NONE));
        MPF_TEST_ASSERTEQUAL(csvsqldb::INT, result.getType());
        MPF_TEST_ASSERT(result.isNull());
        result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::REAL, csvsqldb::Variant(csvsqldb::NONE));
        MPF_TEST_ASSERTEQUAL(csvsqldb::REAL, result.getType());
        MPF_TEST_ASSERT(result.isNull());
        result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::STRING, csvsqldb::Variant(csvsqldb::NONE));
        MPF_TEST_ASSERTEQUAL(csvsqldb::STRING, result.getType());
        MPF_TEST_ASSERT(result.isNull());
        result = unaryOperation(csvsqldb::OP_CAST, csvsqldb::BOOLEAN, csvsqldb::Variant(csvsqldb::NONE));
        MPF_TEST_ASSERTEQUAL(csvsqldb::BOOLEAN, result.getType());
        MPF_TEST_ASSERT(result.isNull());
    }
    
    void compareOperationsTest()
    {
        csvsqldb::Variant lhs(csvsqldb::Time(8, 9, 11));
        csvsqldb::Variant lhs2("08:09:11");
        csvsqldb::Variant rhs(csvsqldb::Time(12, 0, 0));
        csvsqldb::Variant rhs2("12:00:00");
        
        csvsqldb::Variant result = binaryOperation(csvsqldb::OP_GT, lhs, rhs);
        MPF_TEST_ASSERTEQUAL(false, result.asBool());
        result = binaryOperation(csvsqldb::OP_GT, rhs, lhs);
        MPF_TEST_ASSERTEQUAL(true, result.asBool());
        result = binaryOperation(csvsqldb::OP_GT, rhs2, lhs);
        MPF_TEST_ASSERTEQUAL(true, result.asBool());
        result = binaryOperation(csvsqldb::OP_LT, lhs, rhs);
        MPF_TEST_ASSERTEQUAL(true, result.asBool());
        result = binaryOperation(csvsqldb::OP_EQ, lhs, lhs);
        MPF_TEST_ASSERTEQUAL(true, result.asBool());
        result = binaryOperation(csvsqldb::OP_EQ, lhs, lhs2);
        MPF_TEST_ASSERTEQUAL(true, result.asBool());
    }
};

MPF_REGISTER_TEST_START("TypesTestSuite", TypeoperationsTestCase);
MPF_REGISTER_TEST(TypeoperationsTestCase::operationsTest);
MPF_REGISTER_TEST(TypeoperationsTestCase::valueConcatTest);
MPF_REGISTER_TEST(TypeoperationsTestCase::nullOperationsTest);
MPF_REGISTER_TEST(TypeoperationsTestCase::castOperationsTest);
MPF_REGISTER_TEST(TypeoperationsTestCase::nullCastOperationsTest);
MPF_REGISTER_TEST(TypeoperationsTestCase::compareOperationsTest);
MPF_REGISTER_TEST_END();
