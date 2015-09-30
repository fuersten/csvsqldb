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


#ifndef csvsqldb_test_helper_h
#define csvsqldb_test_helper_h

#include "base/exception.h"
#include "test/test_util.h"

#include <iostream>

#include <fcntl.h>
#include <unistd.h>


class RedirectStdOut
{
public:
    RedirectStdOut()
    {
        _oldstdout = ::dup(::fileno(stdout));
        if(_oldstdout == -1) {
            csvsqldb::throwSysError("RedirectStdOut");
        }
        _fdo = ::open((CSVSQLDB_TEST_PATH + std::string("/stdout.txt")).c_str(),
                      O_CREAT | O_TRUNC | O_APPEND | O_WRONLY,
                      S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(_fdo == -1) {
            csvsqldb::throwSysError("RedirectStdOut");
        }
        _newstdout = ::dup2(_fdo, ::fileno(stdout));
        if(_newstdout == -1) {
            csvsqldb::throwSysError("RedirectStdOut");
        }
    }

    void flush()
    {
        ::fflush(stdout);
    }

    ~RedirectStdOut()
    {
        ::dup2(_oldstdout, _newstdout);
        ::close(_fdo);
    }

private:
    int _oldstdout;
    int _fdo;
    int _newstdout;
};

class RedirectStdErr
{
public:
    RedirectStdErr()
    {
        _oldstderr = ::dup(::fileno(stderr));
        if(_oldstderr == -1) {
            csvsqldb::throwSysError("RedirectStdErr");
        }
        _fdo = ::open((CSVSQLDB_TEST_PATH + std::string("/stderr.txt")).c_str(),
                      O_CREAT | O_TRUNC | O_APPEND | O_WRONLY,
                      S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(_fdo == -1) {
            csvsqldb::throwSysError("RedirectStdErr");
        }
        _newstderr = ::dup2(_fdo, ::fileno(stderr));
        if(_newstderr == -1) {
            csvsqldb::throwSysError("RedirectStdErr");
        }
    }

    void flush()
    {
        ::fflush(stderr);
    }

    ~RedirectStdErr()
    {
        ::dup2(_oldstderr, _newstderr);
        ::close(_fdo);
    }

private:
    int _oldstderr;
    int _fdo;
    int _newstderr;
};

#endif
