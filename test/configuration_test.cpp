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
#include "test/test_util.h"

#include "libcsvsqldb/base/exception.h"
#include "libcsvsqldb/base/default_configuration.h"
#include "libcsvsqldb/base/lua_configuration.h"
#include "libcsvsqldb/base/global_configuration.h"

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

class ConfigurationTestCase
{
public:
    void setUp()
    {
    }

    void tearDown()
    {
    }

    void configTest()
    {
        csvsqldb::Configuration::Ptr config(
        std::make_shared<csvsqldb::LuaConfiguration>(CSVSQLDB_TEST_PATH + std::string("/testdata/luaengine/test.lua")));

        MPF_TEST_ASSERTEQUAL(10474, config->get("port", 9999));
        MPF_TEST_ASSERT(std::fabs(47.11 - config->get("factor", 8.15)) < 0.0001);
        MPF_TEST_ASSERTEQUAL("Thor", config->get("hostname", "horst"));
        MPF_TEST_ASSERTEQUAL(100, config->get("server.threads", 10));
        MPF_TEST_ASSERTEQUAL("LUA", config->get("server.api.code", "php"));
        MPF_TEST_ASSERT(std::fabs(47.11 - config->get<double>("factor")) < 0.0001);
        MPF_TEST_ASSERTEQUAL(true, config->get<bool>("daemonize"));
        MPF_TEST_ASSERTEQUAL(2, config->get<int32_t>("debug.level.tcp_server"));

        MPF_TEST_EXPECTS(csvsqldb::LuaConfiguration("not existent configuration"), csvsqldb::FilesystemException);
    }

    void configCheck()
    {
        csvsqldb::Configuration::Ptr config(
        std::make_shared<csvsqldb::LuaConfiguration>(CSVSQLDB_TEST_PATH + std::string("/testdata/luaengine/test.lua")));

        MPF_TEST_ASSERTEQUAL(true, config->hasProperty("daemonize"));
        MPF_TEST_ASSERTEQUAL(true, config->hasProperty("server.api.code"));
        MPF_TEST_ASSERTEQUAL(false, config->hasProperty("not_available_entry"));

        MPF_TEST_ASSERTEQUAL(1, config->get<int32_t>("debug.global_level"));

        csvsqldb::StringVector properties;
        MPF_TEST_ASSERTEQUAL(3u, config->getProperties("debug.level", properties));

        std::set<std::string> props(properties.begin(), properties.end());
        MPF_TEST_ASSERT(props.find("tcp_server") != props.end());
        MPF_TEST_ASSERT(props.find("connection") != props.end());
        MPF_TEST_ASSERT(props.find("filesystem") != props.end());
    }

    void defaultConfigTest()
    {
        csvsqldb::Configuration::Ptr config(std::make_shared<csvsqldb::DefaultConfiguration>());

        MPF_TEST_ASSERTEQUAL(false, config->hasProperty("application.daemonize"));
        MPF_TEST_ASSERTEQUAL(false, config->hasProperty("not_available_entry"));
        MPF_TEST_ASSERTEQUAL(false, config->hasProperty("logging.device"));
        MPF_TEST_ASSERTEQUAL(false, config->hasProperty("debug.global_level"));
        csvsqldb::StringVector properties;
        MPF_TEST_ASSERTEQUAL(0u, config->getProperties("debug.level", properties));
        MPF_TEST_EXPECTS(config->get<int32_t>("debug.global_level"), csvsqldb::ConfigurationException);
    }

    void globalConfigTest()
    {
        csvsqldb::Configuration::Ptr config(
        std::make_shared<csvsqldb::LuaConfiguration>(CSVSQLDB_TEST_PATH + std::string("/testdata/luaengine/test.lua")));
        csvsqldb::GlobalConfiguration::create<MyGlobalConfiguration>();
        csvsqldb::GlobalConfiguration::instance<MyGlobalConfiguration>()->configure(config);

        MPF_TEST_ASSERTEQUAL(1, csvsqldb::GlobalConfiguration::instance<MyGlobalConfiguration>()->debug.global_level);
        MPF_TEST_ASSERTEQUAL(3, csvsqldb::GlobalConfiguration::instance<MyGlobalConfiguration>()->debug.level["connection"]);
        MPF_TEST_ASSERTEQUAL(815, csvsqldb::config<MyGlobalConfiguration>()->test.wtf);
    }
};

MPF_REGISTER_TEST_START("ApplicationTestSuite", ConfigurationTestCase);
MPF_REGISTER_TEST(ConfigurationTestCase::configTest);
MPF_REGISTER_TEST(ConfigurationTestCase::configCheck);
MPF_REGISTER_TEST(ConfigurationTestCase::defaultConfigTest);
MPF_REGISTER_TEST(ConfigurationTestCase::globalConfigTest);
MPF_REGISTER_TEST_END();
