//
//  csvsqldb test
//
//  Created by Lars-Christian Fürstenberg on 28.07.13.
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

#include "libcsvsqldb/base/exception.h"

#include <iostream>


class ExceptionTestCase
{
public:
    void setUp()
    {
    }
    
    void tearDown()
    {
    }
    
    void exceptionTest()
    {
        try {
            throw csvsqldb::FilesystemException(ENOENT, "Filesystem");
        } catch (csvsqldb::Exception& ex) {
            MPF_TEST_ASSERTEQUAL(ENOENT, ex.code().value());
            //csvsqldb::evaluateException();
        }
    }
    
    void errnoTest()
    {
        errno = EWOULDBLOCK;
        std::string txt = csvsqldb::errnoText();
        MPF_TEST_ASSERTEQUAL(strerror(EWOULDBLOCK), txt);
    }
    
    void sysException()
    {
        errno = EWOULDBLOCK;
        try {
            csvsqldb::throwSysError("mydomain");
        } catch(csvsqldb::Exception& ex) {
            MPF_TEST_ASSERTEQUAL(EWOULDBLOCK, ex.code().value());
            errno = EWOULDBLOCK;
            std::string expected = csvsqldb::errnoText();
            MPF_TEST_ASSERTEQUAL("mydomain" + std::string(": ") + expected, ex.what());
        }
    }
};

MPF_REGISTER_TEST_START("ApplicationTestSuite", ExceptionTestCase);
MPF_REGISTER_TEST(ExceptionTestCase::exceptionTest);
MPF_REGISTER_TEST(ExceptionTestCase::errnoTest);
MPF_REGISTER_TEST(ExceptionTestCase::sysException);
MPF_REGISTER_TEST_END();
