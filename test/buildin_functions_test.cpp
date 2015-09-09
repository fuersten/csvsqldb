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

#include "libcsvsqldb/base/float_helper.h"

#include "libcsvsqldb/buildin_functions.h"



class BuildinFunctionsTestCase
{
public:
    BuildinFunctionsTestCase()
    {
        initBuildInFunctions(_registry);
    }
    
    void setUp()
    {
    }
    
    void tearDown()
    {
    }
    
    void buildinDateFunctionsTest()
    {
        csvsqldb::Function::Ptr function = _registry.getFunction("CURRENT_DATE");
        MPF_TEST_ASSERT(function);
        MPF_TEST_ASSERTEQUAL("CURRENT_DATE", function->getName());
        MPF_TEST_ASSERTEQUAL(csvsqldb::DATE, function->getReturnType());
        csvsqldb::Variants parameter;
        csvsqldb::Variant result = function->call(parameter);
        MPF_TEST_ASSERTEQUAL(csvsqldb::DATE, result.getType());
        
        function = _registry.getFunction("CURRENT_TIME");
        MPF_TEST_ASSERT(function);
        MPF_TEST_ASSERTEQUAL("CURRENT_TIME", function->getName());
        MPF_TEST_ASSERTEQUAL(csvsqldb::TIME, function->getReturnType());
        result = function->call(parameter);
        MPF_TEST_ASSERTEQUAL(csvsqldb::TIME, result.getType());
        
        function = _registry.getFunction("EXTRACT");
        MPF_TEST_ASSERT(function);
        MPF_TEST_ASSERTEQUAL("EXTRACT", function->getName());
        MPF_TEST_ASSERTEQUAL(csvsqldb::INT, function->getReturnType());
        parameter.push_back(csvsqldb::Variant(5));
        parameter.push_back(csvsqldb::Timestamp(1970, csvsqldb::Date::September, 23, 8, 9, 11));
        result = function->call(parameter);
        MPF_TEST_ASSERTEQUAL(csvsqldb::INT, result.getType());
        MPF_TEST_ASSERTEQUAL(9, result.asInt());
        
        parameter[0] = csvsqldb::Variant(6);
        result = function->call(parameter);
        MPF_TEST_ASSERTEQUAL(csvsqldb::INT, result.getType());
        MPF_TEST_ASSERTEQUAL(1970, result.asInt());
        
        parameter[0] = csvsqldb::Variant(4);
        result = function->call(parameter);
        MPF_TEST_ASSERTEQUAL(csvsqldb::INT, result.getType());
        MPF_TEST_ASSERTEQUAL(23, result.asInt());
        
        parameter[0] = csvsqldb::Variant(3);
        result = function->call(parameter);
        MPF_TEST_ASSERTEQUAL(csvsqldb::INT, result.getType());
        MPF_TEST_ASSERTEQUAL(8, result.asInt());
        
        parameter[0] = csvsqldb::Variant(2);
        result = function->call(parameter);
        MPF_TEST_ASSERTEQUAL(csvsqldb::INT, result.getType());
        MPF_TEST_ASSERTEQUAL(9, result.asInt());
        
        parameter[0] = csvsqldb::Variant(1);
        result = function->call(parameter);
        MPF_TEST_ASSERTEQUAL(csvsqldb::INT, result.getType());
        MPF_TEST_ASSERTEQUAL(11, result.asInt());

        function = _registry.getFunction("DATE_FORMAT");
        MPF_TEST_ASSERT(function);
        MPF_TEST_ASSERTEQUAL("DATE_FORMAT", function->getName());
        MPF_TEST_ASSERTEQUAL(csvsqldb::STRING, function->getReturnType());
        parameter.clear();
        parameter.push_back(csvsqldb::Date(1970, csvsqldb::Date::September, 23));
        parameter.push_back("%d.%m.%Y %j %U %w");
        result = function->call(parameter);
        MPF_TEST_ASSERTEQUAL(csvsqldb::STRING, result.getType());
        MPF_TEST_ASSERTEQUAL("23.09.1970 266 39 3", result);
        
        function = _registry.getFunction("TIME_FORMAT");
        MPF_TEST_ASSERT(function);
        MPF_TEST_ASSERTEQUAL("TIME_FORMAT", function->getName());
        MPF_TEST_ASSERTEQUAL(csvsqldb::STRING, function->getReturnType());
        parameter.clear();
        parameter.push_back(csvsqldb::Time(8, 9, 11));
        parameter.push_back("%H@%M@%S");
        result = function->call(parameter);
        MPF_TEST_ASSERTEQUAL(csvsqldb::STRING, result.getType());
        MPF_TEST_ASSERTEQUAL("08@09@11", result);
        
        function = _registry.getFunction("TIMESTAMP_FORMAT");
        MPF_TEST_ASSERT(function);
        MPF_TEST_ASSERTEQUAL("TIMESTAMP_FORMAT", function->getName());
        MPF_TEST_ASSERTEQUAL(csvsqldb::STRING, function->getReturnType());
        parameter.clear();
        parameter.push_back(csvsqldb::Timestamp(1970, csvsqldb::Date::September, 23, 8, 9, 11));
        parameter.push_back("%Y@%m@%dT@%H@%M@%S");
        result = function->call(parameter);
        MPF_TEST_ASSERTEQUAL(csvsqldb::STRING, result.getType());
        MPF_TEST_ASSERTEQUAL("1970@09@23T@08@09@11", result);
    }
    
    void buildinStringFunctionsTest()
    {
        csvsqldb::Function::Ptr function = _registry.getFunction("UPPER");
        MPF_TEST_ASSERT(function);
        MPF_TEST_ASSERTEQUAL("UPPER", function->getName());
        MPF_TEST_ASSERTEQUAL(csvsqldb::STRING, function->getReturnType());
        csvsqldb::Variants parameter;
        parameter.push_back(csvsqldb::Variant("Lars"));
        csvsqldb::Variant result = function->call(parameter);
        MPF_TEST_ASSERTEQUAL(csvsqldb::STRING, result.getType());
        MPF_TEST_ASSERTEQUAL("LARS", result);
        
        function = _registry.getFunction("LOWER");
        MPF_TEST_ASSERT(function);
        MPF_TEST_ASSERTEQUAL("LOWER", function->getName());
        MPF_TEST_ASSERTEQUAL(csvsqldb::STRING, function->getReturnType());
        result = function->call(parameter);
        MPF_TEST_ASSERTEQUAL(csvsqldb::STRING, result.getType());
        MPF_TEST_ASSERTEQUAL("lars", result);
        
        function = _registry.getFunction("CHARACTER_LENGTH");
        MPF_TEST_ASSERT(function);
        MPF_TEST_ASSERTEQUAL("CHARACTER_LENGTH", function->getName());
        MPF_TEST_ASSERTEQUAL(csvsqldb::INT, function->getReturnType());
        result = function->call(parameter);
        MPF_TEST_ASSERTEQUAL(csvsqldb::INT, result.getType());
        MPF_TEST_ASSERTEQUAL(4, result);

        function = _registry.getFunction("CHAR_LENGTH");
        MPF_TEST_ASSERT(function);
        MPF_TEST_ASSERTEQUAL("CHAR_LENGTH", function->getName());
        MPF_TEST_ASSERTEQUAL(csvsqldb::INT, function->getReturnType());
        result = function->call(parameter);
        MPF_TEST_ASSERTEQUAL(csvsqldb::INT, result.getType());
        MPF_TEST_ASSERTEQUAL(4, result);
    }
    
    void buildinMathFunctionsTest()
    {
        csvsqldb::Function::Ptr function = _registry.getFunction("POW");
        MPF_TEST_ASSERT(function);
        MPF_TEST_ASSERTEQUAL("POW", function->getName());
        MPF_TEST_ASSERTEQUAL(csvsqldb::REAL, function->getReturnType());
        csvsqldb::Variants parameter;
        parameter.push_back(csvsqldb::Variant(10.0));
        parameter.push_back(csvsqldb::Variant(2.0));
        csvsqldb::Variant result = function->call(parameter);
        MPF_TEST_ASSERTEQUAL(csvsqldb::REAL, result.getType());
        MPF_TEST_ASSERT(csvsqldb::compare(100.0, result.asDouble()));
    }
    
    csvsqldb::FunctionRegistry _registry;
};

MPF_REGISTER_TEST_START("FunctionTestSuite", BuildinFunctionsTestCase);
MPF_REGISTER_TEST(BuildinFunctionsTestCase::buildinDateFunctionsTest);
MPF_REGISTER_TEST(BuildinFunctionsTestCase::buildinStringFunctionsTest);
MPF_REGISTER_TEST(BuildinFunctionsTestCase::buildinMathFunctionsTest);
MPF_REGISTER_TEST_END();
