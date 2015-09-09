//
//  test_helper.h
//  csvsqldb
//
//  Copyright (c) 2015 Fürstenberg IT Systems. All rights reserved.
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
        _fdo = ::open((CSVSQLDB_TEST_PATH + std::string("/stdout.txt")).c_str(), O_CREAT | O_TRUNC | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
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
        _fdo = ::open((CSVSQLDB_TEST_PATH + std::string("/stderr.txt")).c_str(), O_CREAT | O_TRUNC | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
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
