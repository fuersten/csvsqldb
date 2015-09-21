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
#include "test_helper.h"
#include "test/test_util.h"

#include "libcsvsqldb/base/lua_engine.h"

#include <set>
#include <fstream>


static void doit(int n, double d)
{
    std::cout << n * d << std::endl;
}

static int testit(int n)
{
    return 2 * n;
}

static void done()
{
    std::cout << "done" << std::endl;
}


class LuaEngineTestCase
{
public:
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void engineTest()
    {
        csvsqldb::luaengine::LuaEngine lua;

        registerFunction(lua, "mul", testit);
        registerFunction(lua, "doit", doit);
        registerFunction(lua, "done", done);

        std::string text("This is cool!");
        int i = 2;
        int n = 3;

        lua.doFile(CSVSQLDB_TEST_PATH + std::string("/testdata/luaengine/test.lua"));

        MPF_TEST_ASSERTEQUAL(10474, lua.getGlobal<int>("port"));
        MPF_TEST_ASSERT(std::fabs(47.11 - lua.getGlobal<double>("factor")) < 0.0001);
        MPF_TEST_ASSERTEQUAL("Thor", lua.getGlobal<std::string>("hostname"));
        MPF_TEST_ASSERTEQUAL(true, lua.hasGlobal("hostname"));
        MPF_TEST_ASSERTEQUAL(false, lua.hasGlobal("hutzli"));
        MPF_TEST_ASSERTEQUAL(true, lua.hasGlobal("server.threads"));
        MPF_TEST_ASSERTEQUAL(100, lua.getGlobal<int>("server.threads"));

        MPF_TEST_ASSERTEQUAL("This is a test: This is cool!", lua.callFunction<std::string>("testparams", text, i, n));

        RedirectStdOut red;
        lua.callFunction<void>("dosomething");
        red.flush();

        std::ifstream log((CSVSQLDB_TEST_PATH + std::string("/stdout.txt")));
        MPF_TEST_ASSERTEQUAL(true, log.good());
        std::string line;
        int line_count(0);
        while(std::getline(log, line).good()) {
            switch(line_count) {
                case 0: {
                    MPF_TEST_ASSERT(line == "14");
                    break;
                }
                case 1: {
                    MPF_TEST_ASSERT(line == "done");
                    break;
                }
                case 2: {
                    MPF_TEST_ASSERT(line == "30");
                    break;
                }
                case 3: {
                    MPF_TEST_ASSERT(line == "hi ho");
                    break;
                }
            }
            ++line_count;
        }
        log.close();
        MPF_TEST_ASSERTEQUAL(4, line_count);

        // TODO: this is not good
        // remove header static object from luaengine and replace with better implementation!!!
        csvsqldb::luaengine::sFunctions.clear();
    }

    void doStringTest()
    {
        csvsqldb::luaengine::LuaEngine lua;

        std::string script = "port = 4711";
        lua.doString(script);

        MPF_TEST_ASSERTEQUAL(true, lua.hasGlobal("port"));
    }

    void propertiesTest()
    {
        csvsqldb::luaengine::LuaEngine lua;

        lua.doFile(CSVSQLDB_TEST_PATH + std::string("/testdata/luaengine/properties.lua"));

        csvsqldb::StringVector properties;
        lua.getProperties("debug.level", properties);

        MPF_TEST_ASSERTEQUAL(4u, properties.size());
        std::set<std::string> props;
        for(auto& property : properties) {
            props.insert(property);
        }
        MPF_TEST_ASSERTEQUAL(4u, props.size());
        MPF_TEST_ASSERTEQUAL(1u, props.count("csvsqldb::Filesystem"));
        MPF_TEST_ASSERTEQUAL(1u, props.count("csvsqldb::DirectoryMonitor"));
        MPF_TEST_ASSERTEQUAL(1u, props.count("csvsqldb::TcpServer"));
        MPF_TEST_ASSERTEQUAL(1u, props.count("csvsqldb::application::ApophisServer"));
        MPF_TEST_ASSERTEQUAL(3, lua.getGlobal<int>("debug.level.csvsqldb::DirectoryMonitor"));
    }
};

MPF_REGISTER_TEST_START("ApplicationTestSuite", LuaEngineTestCase);
MPF_REGISTER_TEST(LuaEngineTestCase::engineTest);
MPF_REGISTER_TEST(LuaEngineTestCase::doStringTest);
MPF_REGISTER_TEST(LuaEngineTestCase::propertiesTest);
MPF_REGISTER_TEST_END();
