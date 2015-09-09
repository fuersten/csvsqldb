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

#include "libcsvsqldb/sql_ast.h"


class TypesTestCase
{
public:
    TypesTestCase()
    {
    }
    
    void setUp()
    {
    }
    
    void tearDown()
    {
    }
    
    void identifiersTest()
    {
        csvsqldb::IdentifierSet identifiers;
        
        csvsqldb::SymbolTablePtr symbolTable = csvsqldb::SymbolTable::createSymbolTable();
        csvsqldb::SymbolInfoPtr info = std::make_shared<csvsqldb::SymbolInfo>();
        info->_name = "BIRTHDATE";
        
        csvsqldb::ASTIdentifier bd(symbolTable, info, "", "BIRTHDATE", false);
        
        identifiers.insert(bd);
        MPF_TEST_ASSERT(identifiers.size() == 1);
        
        csvsqldb::ASTIdentifier id(symbolTable, info, "", "ID", false);
        identifiers.insert(id);
        MPF_TEST_ASSERT(identifiers.size() == 2);
        identifiers.insert(id);
        MPF_TEST_ASSERT(identifiers.size() == 2);
        
        csvsqldb::ASTIdentifier prefix_bd(symbolTable, info, "EMPLOYEES", "BIRTHDATE", false);
        identifiers.insert(prefix_bd);
        MPF_TEST_ASSERT(identifiers.size() == 3);
    }
    
    void stringToBoolTest()
    {
        MPF_TEST_ASSERTEQUAL(true, csvsqldb::stringToBool("true"));
        MPF_TEST_ASSERTEQUAL(false, csvsqldb::stringToBool("false"));
        MPF_TEST_ASSERTEQUAL(true, csvsqldb::stringToBool("TRUE"));
        MPF_TEST_ASSERTEQUAL(false, csvsqldb::stringToBool("FaLsE"));
        MPF_TEST_ASSERTEQUAL(true, csvsqldb::stringToBool("1"));
        MPF_TEST_ASSERTEQUAL(false, csvsqldb::stringToBool("0"));
    }
};

MPF_REGISTER_TEST_START("TypesSuite", TypesTestCase);
MPF_REGISTER_TEST(TypesTestCase::identifiersTest);
MPF_REGISTER_TEST(TypesTestCase::stringToBoolTest);
MPF_REGISTER_TEST_END();
