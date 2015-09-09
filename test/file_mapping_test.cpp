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

#include "libcsvsqldb/file_mapping.h"

#include "libcsvsqldb/base/string_helper.h"


class FileMappingTestCase
{
public:
    FileMappingTestCase()
    {
    }
    
    void setUp()
    {
    }
    
    void tearDown()
    {
    }
    
    void mappgingTest()
    {
        csvsqldb::FileMapping::Mappings mappings;
        mappings.push_back({"employees.csv->employees", ',', false});
        mappings.push_back({"salaries.csv->salaries", ';', true});
        
        csvsqldb::FileMapping mapping;
        mapping.initialize(mappings);
        MPF_TEST_ASSERTEQUAL("employees.csv", mapping.getMappingForTable("EMPLOYEES")._mapping);
        MPF_TEST_ASSERTEQUAL(',', mapping.getMappingForTable("EMPLOYEES")._delimiter);
        MPF_TEST_ASSERTEQUAL("salaries.csv", mapping.getMappingForTable("SALARIES")._mapping);
        MPF_TEST_ASSERTEQUAL(';', mapping.getMappingForTable("SALARIES")._delimiter);
        MPF_TEST_ASSERTEQUAL(true, mapping.getMappingForTable("SALARIES")._skipFirstLine);
    }
    
    void missingMappingTest()
    {
        csvsqldb::FileMapping::Mappings mappings;
        mappings.push_back({"employees.csv->employees", ',', false});
        
        csvsqldb::FileMapping mapping;
        mapping.initialize(mappings);
        MPF_TEST_ASSERTEQUAL("employees.csv", mapping.getMappingForTable("EMPLOYEES")._mapping);
        MPF_TEST_EXPECTS(mapping.getMappingForTable("salaries"), csvsqldb::MappingException);
        MPF_TEST_EXPECTS(mapping.getMappingForTable("employee"), csvsqldb::MappingException);
    }
    
    void asStringVectorTest()
    {
        csvsqldb::FileMapping::Mappings mappings;
        mappings.push_back({"employees.csv->employees", ',', false});
        mappings.push_back({"salaries.csv->salaries", ',', false});
        
        csvsqldb::FileMapping mapping;
        mapping.initialize(mappings);
        MPF_TEST_ASSERTEQUAL("employees.csv->EMPLOYEES,salaries.csv->SALARIES", csvsqldb::join(mapping.asStringVector(), ","));
    }
    
    void JSONTest()
    {
        std::string JSON;
        
        {
            csvsqldb::FileMapping::Mappings mappings;
            mappings.push_back({"employees.csv->employees", ',', false});
            mappings.push_back({"salaries.csv->salaries", ',', false});
            
            csvsqldb::FileMapping mapping;
            mapping.initialize(mappings);
            
           JSON = mapping.asJson("employees", mappings);
        }
        
        std::stringstream ss(JSON);
        csvsqldb::FileMapping mapping = csvsqldb::FileMapping::fromJson(ss);
        MPF_TEST_ASSERTEQUAL("employees.csv", mapping.getMappingForTable("EMPLOYEES")._mapping);
        MPF_TEST_EXPECTS(mapping.getMappingForTable("SALARIES"), csvsqldb::MappingException);
    }
};

MPF_REGISTER_TEST_START("MappingTestSuite", FileMappingTestCase);
MPF_REGISTER_TEST(FileMappingTestCase::mappgingTest);
MPF_REGISTER_TEST(FileMappingTestCase::missingMappingTest);
MPF_REGISTER_TEST(FileMappingTestCase::asStringVectorTest);
MPF_REGISTER_TEST(FileMappingTestCase::JSONTest);
MPF_REGISTER_TEST_END();
