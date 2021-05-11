//
//
//  csvsqldb test
//
//  BSD 3-Clause License
//  Copyright (c) 2015-2020 Lars-Christian Fürstenberg
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

#include <csvsqldb/base/lua_engine.h>

#include "test/test_util.h"
#include "test_helper.h"

#include <catch2/catch.hpp>

#include <fstream>
#include <set>


namespace
{
  static void logs(std::string s)
  {
    std::cout << s << std::endl;
  }

  static void logn(int n)
  {
    std::cout << n << std::endl;
  }

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
}


TEST_CASE("Lua Engine Test", "[lua]")
{
  csvsqldb::luaengine::LuaEngine lua;

  SECTION("engine")
  {
    lua.registerFunction(lua, "mul", testit);
    lua.registerFunction(lua, "doit", doit);
    lua.registerFunction(lua, "done", done);
    lua.registerFunction(lua, "logn", logn);
    lua.registerFunction(lua, "logs", logs);

    std::string text("This is cool!");
    int i = 2;
    int n = 3;

    lua.doFile(getTestPath() / std::filesystem::path("testdata") / "luaengine" / "test.lua");

    CHECK(10474 == lua.getGlobal<int>("port"));
    CHECK(std::fabs(47.11 - lua.getGlobal<double>("factor")) < 0.0001);
    CHECK("Thor" == lua.getGlobal<std::string>("hostname"));
    CHECK(lua.hasGlobal("hostname"));
    CHECK_FALSE(lua.hasGlobal("hutzli"));
    CHECK(lua.hasGlobal("server.threads"));
    CHECK(100 == lua.getGlobal<int>("server.threads"));

    CHECK("This is a test: This is cool!" == lua.callFunction<std::string>("testparams", text, i, n));

    {
      RedirectStdOut red;
      lua.callFunction<void>("dosomething");
      red.flush();
    }

    std::ifstream log((getTestPath() / std::filesystem::path("stdout.txt")));
    CHECK(log.good());
    std::string line;
    int line_count(0);
    while (std::getline(log, line).good()) {
      switch (line_count) {
        case 0: {
          CHECK(line == "14");
          break;
        }
        case 1: {
          CHECK(line == "done");
          break;
        }
        case 2: {
          CHECK(std::stoi(line) == 30);
          break;
        }
        case 3: {
          CHECK(line == "hi ho");
          break;
        }
      }
      ++line_count;
    }
    log.close();
    CHECK(4 == line_count);
  }

  SECTION("do string")
  {
    std::string script = "port = 4711";
    lua.doString(script);

    CHECK(lua.hasGlobal("port"));
  }

  SECTION("properties")
  {
    lua.doFile(getTestPath() / std::filesystem::path("testdata") / "luaengine" / "properties.lua");

    csvsqldb::StringVector properties;
    lua.getProperties("debug.level", properties);

    CHECK(4u == properties.size());
    std::set<std::string> props;
    for (const auto& property : properties) {
      props.insert(property);
    }
    CHECK(4u == props.size());
    CHECK(1u == props.count("csvsqldb::Filesystem"));
    CHECK(1u == props.count("csvsqldb::DirectoryMonitor"));
    CHECK(1u == props.count("csvsqldb::TcpServer"));
    CHECK(1u == props.count("csvsqldb::application::ApophisServer"));
    CHECK(3 == lua.getGlobal<int>("debug.level.csvsqldb::DirectoryMonitor"));
  }
}
