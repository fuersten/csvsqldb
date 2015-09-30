//  Based on MPF test framework
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
#include <sstream>

using namespace mpf::test;


class TestSuite::Private
{
public:
    Private(const std::string& name)
    : _name(name)
    {
    }

    std::string _name;
    TestSuites _testsuites;
    Testcases _testcases;
};

TestSuite::TestSuite(const std::string& name)
: _m(new Private(name))
{
}

TestSuite::~TestSuite()
{
}

void TestSuite::run(const std::string& name, const ProgressListener::Listener& progressListener)
{
    for(auto listener : progressListener) {
        listener->suiteStart(_m->_name, _m->_testcases.size());
    }

    for(auto testcase : _m->_testcases) {
        testcase->run(name, progressListener);
    }

    for(auto suite : _m->_testsuites) {
        suite->run(name, progressListener);
    }

    for(auto listener : progressListener) {
        listener->suiteEnd(_m->_name);
    }
}

void TestSuite::addTestCase(const TestcaseHolder::Ptr& testcase)
{
    _m->_testcases.push_back(testcase);
}

void TestSuite::addTestSuite(const TestSuite::Ptr& suite)
{
    _m->_testsuites.push_back(suite);
}

const std::string& TestSuite::name() const
{
    return _m->_name;
}

size_t TestSuite::count(const std::string& name) const
{
    size_t count(0);

    for(auto testcase : _m->_testcases) {
        count += testcase->count(name);
    }

    for(auto suite : _m->_testsuites) {
        count += suite->count(name);
    }

    return count;
}

TestSuite::Ptr TestSuite::suite(const std::string& name) const
{
    for(auto suite : _m->_testsuites) {
        if(suite->name() == name) {
            return suite;
        }
    }
    return TestSuite::Ptr();
}


class TestRegistry::Private
{
public:
    Private()
    : _sep('.')
    {
    }

    char _sep;
    Testsuites _suites;
    GlobalSetupFunctions _globalSetupFunctions;
    GlobalTeardownFunctions _globalTeardownFunctions;
};

TestRegistry::TestRegistry()
: _m(new Private)
{
}

TestRegistry::~TestRegistry()
{
}

TestRegistry& TestRegistry::instance()
{
    static TestRegistry g_registry;
    return g_registry;
}

void TestRegistry::fini()
{
    _m->_suites.clear();
    _m->_globalSetupFunctions.clear();
    _m->_globalTeardownFunctions.clear();
}

bool TestRegistry::run(const std::string& name, const ProgressListener::Listener& progressListener)
{
    for(auto func : _m->_globalSetupFunctions) {
        try {
            func();
        } catch(std::exception& ex) {
            std::cerr << "Global init failed: " << ex.what() << std::endl;
        } catch(...) {
            std::cerr << "Global init failed" << std::endl;
        }
    }

    ProgressListener::Listener proglistener(progressListener);
    CountingProgressListener::Ptr pgl(std::make_shared<CountingProgressListener>());
    proglistener.push_back(pgl);

    for(auto listener : proglistener) {
        listener->testStart(count(name));
    }

    for(auto suite : _m->_suites) {
        suite.second->run(name, proglistener);
    }

    for(auto listener : proglistener) {
        listener->testEnd(pgl->_successful, pgl->_failed);
    }

    for(auto func : _m->_globalTeardownFunctions) {
        try {
            func();
        } catch(std::exception& ex) {
            std::cerr << "Global teardown failed: " << ex.what() << std::endl;
        } catch(...) {
            std::cerr << "Global teardown failed" << std::endl;
        }
    }

    return pgl->_failed > 0 ? false : true;
}

size_t TestRegistry::count(const std::string& name) const
{
    size_t count(0);

    for(auto suite : _m->_suites) {
        count += suite.second->count(name);
    }
    return count;
}

bool TestRegistry::addSetupFunctions(GlobalSetupFunction setup, GlobalTeardownFunction teardown)
{
    _m->_globalSetupFunctions.push_back(setup);
    _m->_globalTeardownFunctions.push_back(teardown);
    return true;
}

bool TestRegistry::addTestcase(const std::string& suitePath, const TestcaseHolder::Ptr& testcase)
{
    TestSuite::Ptr suite;

    std::vector<std::string> tokens;
    std::stringstream ss(suitePath);
    std::string item;
    while(std::getline(ss, item, _m->_sep)) {
        tokens.push_back(item);
    }

    for(auto tok : tokens) {
        Testsuites::iterator iter = _m->_suites.find(tok);
        if(iter == _m->_suites.end()) {
            TestSuite::Ptr temp(new TestSuite(tok));

            if(suite) {
                suite->addTestSuite(temp);
            } else {
                _m->_suites.insert(std::make_pair(tok, temp));
            }

            suite = temp;
        } else {
            suite = iter->second;
        }
    }

    suite->addTestCase(testcase);

    return true;
}
