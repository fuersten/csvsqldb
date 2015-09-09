//
//  test_helper.h
//  csvsqldb
//
//  Copyright (c) 2015 Fürstenberg IT Systems. All rights reserved.
//

#ifndef csvsqldb_test_helper_h
#define csvsqldb_test_helper_h

#include "libcsvsqldb/base/exception.h"
#include "test/test_util.h"

#include <iostream>


// TODO LCF: implement this for windows

class RedirectStdOut
{
public:
    RedirectStdOut()
    {
    }
    
    void flush()
    {
    }
    
    ~RedirectStdOut()
    {
    }
};

class RedirectStdErr
{
public:
    RedirectStdErr()
    {
    }
    
    void flush()
    {
    }
    
    ~RedirectStdErr()
    {
    }
};

#endif
