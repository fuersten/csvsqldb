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


#include <csvsqldb/base/default_configuration.h>
#include <csvsqldb/base/exception.h>
#include <csvsqldb/base/global_configuration.h>
#include <csvsqldb/base/lua_configuration.h>

#include "test/test_util.h"

#include <catch2/catch.hpp>

#include <set>


class MyGlobalConfiguration : public csvsqldb::GlobalConfiguration
{
public:
  struct Test {
    int32_t wtf;
  };

  virtual void doConfigure(const csvsqldb::Configuration::Ptr& configuration)
  {
    test.wtf = configuration->get("test.wtf", 4711);
  }

  Test test;
};

TEST_CASE("Configuration Test", "[config]")
{
  SECTION("config")
  {
    csvsqldb::Configuration::Ptr config(
      std::make_shared<csvsqldb::LuaConfiguration>(CSVSQLDB_TEST_PATH + std::string("/testdata/luaengine/test.lua")));

    CHECK(10474 == config->get("port", 9999));
    CHECK(47.11 == Approx(config->get("factor", 8.15)));
    CHECK("Thor" == config->get("hostname", "horst"));
    CHECK(100 == config->get("server.threads", 10));
    CHECK("LUA" == config->get("server.api.code", "php"));
    CHECK(47.11 == Approx(config->get<double>("factor")));
    CHECK(config->get<bool>("daemonize"));
    CHECK(2 == config->get<int32_t>("debug.level.tcp_server"));
  }
  SECTION("non existent config")
  {
    CHECK_THROWS_AS(csvsqldb::LuaConfiguration("not existent configuration"), csvsqldb::FilesystemException);
  }
  SECTION("config check")
  {
    csvsqldb::Configuration::Ptr config(
      std::make_shared<csvsqldb::LuaConfiguration>(CSVSQLDB_TEST_PATH + std::string("/testdata/luaengine/test.lua")));

    CHECK(config->hasProperty("daemonize"));
    CHECK(config->hasProperty("server.api.code"));
    CHECK_FALSE(config->hasProperty("not_available_entry"));

    CHECK(1 == config->get<int32_t>("debug.global_level"));
    CHECK_THROWS_AS(config->get<int32_t>("non.existent.value"), csvsqldb::ConfigurationException);
    CHECK(1 == config->get("debug.global_level", static_cast<int32_t>(5)));
    CHECK(5 == config->get("non.existent.value", static_cast<int32_t>(5)));

    CHECK(1 == config->get<int64_t>("debug.global_level"));
    CHECK_THROWS_AS(config->get<int64_t>("non.existent.value"), csvsqldb::ConfigurationException);
    CHECK(1 == config->get("debug.global_level", static_cast<int64_t>(5)));
    CHECK(5 == config->get("non.existent.value", static_cast<int64_t>(5)));

    CHECK(47.11 == Approx(config->get<double>("factor")));
    CHECK_THROWS_AS(config->get<double>("non.existent.value"), csvsqldb::ConfigurationException);
    CHECK(47.11 == Approx(config->get("factor", 8.15)));
    CHECK(8.15 == Approx(config->get("non.existent.value", 8.15)));

    CHECK(47.11f == Approx(config->get<float>("factor")));
    CHECK_THROWS_AS(config->get<float>("non.existent.value"), csvsqldb::ConfigurationException);
    CHECK(47.11f == Approx(config->get("factor", 8.15f)));
    CHECK(8.15f == Approx(config->get("non.existent.value", 8.15f)));

    CHECK(config->get<bool>("daemonize"));
    CHECK_THROWS_AS(config->get<bool>("non.existent.value"), csvsqldb::ConfigurationException);
    CHECK(config->get("daemonize", false));
    CHECK_FALSE(config->get("non.existent.value", false));

    CHECK("LUA" == config->get<std::string>("server.api.code"));
    CHECK_THROWS_AS(config->get<std::string>("non.existent.value"), csvsqldb::ConfigurationException);
    CHECK("LUA" == config->get("server.api.code", std::string("PHP")));
    CHECK("PHP" == config->get("non.existent.value", std::string("PHP")));
    CHECK("LUA" == config->get("server.api.code", "PHP"));
    CHECK("PHP" == config->get("non.existent.value", "PHP"));

    csvsqldb::StringVector properties;
    CHECK(3u == config->getProperties("debug.level", properties));

    std::set<std::string> props(properties.begin(), properties.end());
    CHECK(props.find("tcp_server") != props.end());
    CHECK(props.find("connection") != props.end());
    CHECK(props.find("filesystem") != props.end());
  }
  SECTION("default config")
  {
    csvsqldb::Configuration::Ptr config(std::make_shared<csvsqldb::DefaultConfiguration>());

    CHECK_FALSE(config->hasProperty("application.daemonize"));
    CHECK_FALSE(config->hasProperty("not_available_entry"));
    CHECK_FALSE(config->hasProperty("logging.device"));
    CHECK_FALSE(config->hasProperty("debug.global_level"));
    csvsqldb::StringVector properties;
    CHECK(0u == config->getProperties("debug.level", properties));
    CHECK_THROWS_AS(config->get<int32_t>("debug.global_level"), csvsqldb::ConfigurationException);
    CHECK_THROWS_AS(config->get<int64_t>("non.existent.value"), csvsqldb::ConfigurationException);
    CHECK_THROWS_AS(config->get<float>("non.existent.value"), csvsqldb::ConfigurationException);
    CHECK_THROWS_AS(config->get<double>("non.existent.value"), csvsqldb::ConfigurationException);
  }
  SECTION("global config")
  {
    csvsqldb::Configuration::Ptr config(
      std::make_shared<csvsqldb::LuaConfiguration>(CSVSQLDB_TEST_PATH + std::string("/testdata/luaengine/test.lua")));
    csvsqldb::GlobalConfiguration::create<csvsqldb::GlobalConfiguration>();
    csvsqldb::GlobalConfiguration::instance<csvsqldb::GlobalConfiguration>()->configure(config);

    CHECK(1 == csvsqldb::GlobalConfiguration::instance<csvsqldb::GlobalConfiguration>()->debug.global_level);
    CHECK(3 == csvsqldb::GlobalConfiguration::instance<csvsqldb::GlobalConfiguration>()->debug.level["connection"]);
  }
  SECTION("global config default values")
  {
    csvsqldb::Configuration::Ptr config(std::make_shared<csvsqldb::DefaultConfiguration>());
    csvsqldb::GlobalConfiguration::create<csvsqldb::GlobalConfiguration>();
    csvsqldb::GlobalConfiguration::instance<csvsqldb::GlobalConfiguration>()->configure(config);

    CHECK("Console" == csvsqldb::GlobalConfiguration::instance<csvsqldb::GlobalConfiguration>()->logging.device);
    CHECK(" | " == csvsqldb::GlobalConfiguration::instance<csvsqldb::GlobalConfiguration>()->logging.separator);
    CHECK_FALSE(csvsqldb::GlobalConfiguration::instance<csvsqldb::GlobalConfiguration>()->logging.escape_newline);
    CHECK(csvsqldb::GlobalConfiguration::instance<csvsqldb::GlobalConfiguration>()->debug.level.empty());
    CHECK(0 == csvsqldb::GlobalConfiguration::instance<csvsqldb::GlobalConfiguration>()->debug.global_level);
  }
  SECTION("custom global config")
  {
    csvsqldb::Configuration::Ptr config(
      std::make_shared<csvsqldb::LuaConfiguration>(CSVSQLDB_TEST_PATH + std::string("/testdata/luaengine/test.lua")));
    csvsqldb::GlobalConfiguration::create<MyGlobalConfiguration>();
    csvsqldb::GlobalConfiguration::instance<MyGlobalConfiguration>()->configure(config);

    CHECK(1 == csvsqldb::GlobalConfiguration::instance<MyGlobalConfiguration>()->debug.global_level);
    CHECK(3 == csvsqldb::GlobalConfiguration::instance<MyGlobalConfiguration>()->debug.level["connection"]);
    CHECK(815 == csvsqldb::config<MyGlobalConfiguration>()->test.wtf);
  }
  SECTION("custom global config default values")
  {
    csvsqldb::Configuration::Ptr config(std::make_shared<csvsqldb::DefaultConfiguration>());
    csvsqldb::GlobalConfiguration::create<MyGlobalConfiguration>();
    csvsqldb::GlobalConfiguration::instance<MyGlobalConfiguration>()->configure(config);

    CHECK(0 == csvsqldb::GlobalConfiguration::instance<MyGlobalConfiguration>()->debug.global_level);
    CHECK(4711 == csvsqldb::config<MyGlobalConfiguration>()->test.wtf);
  }
}
