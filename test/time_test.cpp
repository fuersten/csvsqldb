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
#include "libcsvsqldb/base/time.h"


class TimeTestCase
{
public:
    void setUp()
    {
    }
    
    void tearDown()
    {
    }

    void constructionTest()
    {
        csvsqldb::Time t1(13,45,24,115);
        MPF_TEST_ASSERTEQUAL(13, t1.hour());
        MPF_TEST_ASSERTEQUAL(45, t1.minute());
        MPF_TEST_ASSERTEQUAL(24, t1.second());
        MPF_TEST_ASSERTEQUAL(115, t1.millisecond());
        
        csvsqldb::Time t2;
        MPF_TEST_ASSERTEQUAL(0, t2.hour());
        MPF_TEST_ASSERTEQUAL(0, t2.minute());
        MPF_TEST_ASSERTEQUAL(0, t2.second());
        MPF_TEST_ASSERTEQUAL(0, t2.millisecond());
        
        struct tm y2k;
        y2k.tm_hour = 16; y2k.tm_min = 55; y2k.tm_sec = 22;
        y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;
        time_t time = mktime(&y2k);
        
        csvsqldb::Time t3(time);
        MPF_TEST_ASSERTEQUAL(y2k.tm_hour, t3.hour());
        MPF_TEST_ASSERTEQUAL(y2k.tm_min, t3.minute());
        MPF_TEST_ASSERTEQUAL(y2k.tm_sec, t3.second());
        
        csvsqldb::Time t4(0, 0, 0, 0);
        MPF_TEST_ASSERTEQUAL(0, t4.hour());
        MPF_TEST_ASSERTEQUAL(0, t4.minute());
        MPF_TEST_ASSERTEQUAL(0, t4.second());
        MPF_TEST_ASSERTEQUAL(0, t4.millisecond());
        
        csvsqldb::Time t5(23, 59, 59, 999);
        MPF_TEST_ASSERTEQUAL(23, t5.hour());
        MPF_TEST_ASSERTEQUAL(59, t5.minute());
        MPF_TEST_ASSERTEQUAL(59, t5.second());
        MPF_TEST_ASSERTEQUAL(999, t5.millisecond());
    }
    
    void copyTest()
    {
        csvsqldb::Time t1(13,45,24,115);
        
        csvsqldb::Time t2(t1);
        MPF_TEST_ASSERTEQUAL(13, t2.hour());
        MPF_TEST_ASSERTEQUAL(45, t2.minute());
        MPF_TEST_ASSERTEQUAL(24, t2.second());
        MPF_TEST_ASSERTEQUAL(115, t2.millisecond());
        
        csvsqldb::Time t3(9, 5, 8);
        MPF_TEST_ASSERTEQUAL(9, t3.hour());
        MPF_TEST_ASSERTEQUAL(5, t3.minute());
        MPF_TEST_ASSERTEQUAL(8, t3.second());
        MPF_TEST_ASSERTEQUAL(0, t3.millisecond());
        
        t3 = t2;
        MPF_TEST_ASSERTEQUAL(13, t2.hour());
        MPF_TEST_ASSERTEQUAL(45, t2.minute());
        MPF_TEST_ASSERTEQUAL(24, t2.second());
        MPF_TEST_ASSERTEQUAL(115, t2.millisecond());
    }
    
    void validateTest()
    {
        MPF_TEST_ASSERTEQUAL(false, csvsqldb::Time::isValid(25, 5, 17, 0));
        MPF_TEST_ASSERTEQUAL(true, csvsqldb::Time::isValid(23, 59, 59, 135));
        MPF_TEST_ASSERTEQUAL(false, csvsqldb::Time::isValid(23, 59, 59, 1350));
        MPF_TEST_ASSERTEQUAL(true, csvsqldb::Time::isValid(0, 0, 0, 0));
        MPF_TEST_ASSERTEQUAL(false, csvsqldb::Time::isValid(23, 60, 17, 0));
        MPF_TEST_ASSERTEQUAL(false, csvsqldb::Time::isValid(23, 5, 60, 0));
    }
    
    void setTimePartTest()
    {
        csvsqldb::Time t1(13,45,24, 115);
        
        t1.hour(10);
        t1.minute(33);
        t1.second(41);
        t1.millisecond(93);
        
        MPF_TEST_ASSERTEQUAL(10, t1.hour());
        MPF_TEST_ASSERTEQUAL(33, t1.minute());
        MPF_TEST_ASSERTEQUAL(41, t1.second());
        MPF_TEST_ASSERTEQUAL(93, t1.millisecond());

        t1.hmsm(9, 18, 56, 115);

        MPF_TEST_ASSERTEQUAL(9, t1.hour());
        MPF_TEST_ASSERTEQUAL(18, t1.minute());
        MPF_TEST_ASSERTEQUAL(56, t1.second());
        MPF_TEST_ASSERTEQUAL(115, t1.millisecond());
    }
    
    void addTest()
    {
        csvsqldb::Time t(18,50,45);
        int32_t days = t.addHours(6);
        MPF_TEST_ASSERTEQUAL(1, days);
        MPF_TEST_ASSERTEQUAL(0, t.hour());
        MPF_TEST_ASSERTEQUAL(50, t.minute());
        MPF_TEST_ASSERTEQUAL(45, t.second());

        csvsqldb::Time t1(23,50,45);
        days = t1.addMinutes(10);
        MPF_TEST_ASSERTEQUAL(1, days);
        MPF_TEST_ASSERTEQUAL(0, t1.hour());
        MPF_TEST_ASSERTEQUAL(0, t1.minute());
        MPF_TEST_ASSERTEQUAL(45, t1.second());
        
        csvsqldb::Time t2(23,59,45);
        days = t2.addSeconds(20);
        MPF_TEST_ASSERTEQUAL(1, days);
        MPF_TEST_ASSERTEQUAL(0, t2.hour());
        MPF_TEST_ASSERTEQUAL(0, t2.minute());
        MPF_TEST_ASSERTEQUAL(5, t2.second());
    }

    void compareTest()
    {
        csvsqldb::Time t1(13,45,24);
        csvsqldb::Time t2(18,50,45);

        MPF_TEST_ASSERT(t1 != t2);
        MPF_TEST_ASSERT(!(t1 == t2));
        MPF_TEST_ASSERT(t1 < t2);
        MPF_TEST_ASSERT(t2 > t1);
        MPF_TEST_ASSERT(t1 <= t2);
        MPF_TEST_ASSERT(t2 >= t1);

        csvsqldb::Time t3(t2);
        MPF_TEST_ASSERT(t2 == t3);
        MPF_TEST_ASSERT(!(t2 != t3));
        MPF_TEST_ASSERT(t2 >= t3);
        MPF_TEST_ASSERT(t2 <= t3);
    }
    
    void formatTest()
    {
        csvsqldb::Time t1(13,45,24);
        csvsqldb::Time t2(22,50,45,834);

        MPF_TEST_ASSERTEQUAL("13:45:24", t1.format("%H:%M:%S"));
        MPF_TEST_ASSERTEQUAL("13:45:24", t1.format("%X"));
        MPF_TEST_ASSERTEQUAL("22:50:45.834", t2.format("%H:%M:%S.%s"));
        
        MPF_TEST_ASSERTEQUAL("01:45:24 pm", t1.format("%I:%M:%S %p"));
        MPF_TEST_ASSERTEQUAL("10:50:45 pm", t2.format("%I:%M:%S %p"));
    }
    
    void timeArithmeticTest()
    {
        csvsqldb::Time t1(13,45,24);
        csvsqldb::Time t2(22,50,45,834);
        
        MPF_TEST_ASSERTEQUAL(32721834, t2 - t1);
        MPF_TEST_ASSERTEQUAL(-32721834, t1 - t2);
    }
};

MPF_REGISTER_TEST_START("BaseValuesTestSuite", TimeTestCase);
MPF_REGISTER_TEST(TimeTestCase::constructionTest);
MPF_REGISTER_TEST(TimeTestCase::copyTest);
MPF_REGISTER_TEST(TimeTestCase::validateTest);
MPF_REGISTER_TEST(TimeTestCase::setTimePartTest);
MPF_REGISTER_TEST(TimeTestCase::addTest);
MPF_REGISTER_TEST(TimeTestCase::compareTest);
MPF_REGISTER_TEST(TimeTestCase::formatTest);
MPF_REGISTER_TEST(TimeTestCase::timeArithmeticTest);
MPF_REGISTER_TEST_END();
