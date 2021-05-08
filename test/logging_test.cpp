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


#include <csvsqldb/base/configuration.h>
#include <csvsqldb/base/log_devices.h>
#include <csvsqldb/base/logging.h>

#include "test_helper.h"

#include <catch2/catch.hpp>

#include <fstream>


namespace
{
  class StaticConfiguration : public csvsqldb::Configuration
  {
  public:
    StaticConfiguration()
    {
    }

  private:
    size_t doGetProperties(const std::string&, csvsqldb::StringVector& properties) const override
    {
      properties.push_back("TestClass1");
      properties.push_back("TestClass2");
      return 2;
    }

    bool doHasProperty(const std::string& path) const override
    {
      if (path == "debug.level") {
        return true;
      }
      throw csvsqldb::ConfigurationException("not implemented");
    }

    bool get(const std::string&, csvsqldb::Typer<bool>) const override
    {
      throw csvsqldb::ConfigurationException("not implemented");
    }

    int32_t get(const std::string& path, csvsqldb::Typer<int32_t>) const override
    {
      if (path == "debug.level.TestClass1") {
        return 1;
      } else if (path == "debug.level.TestClass2") {
        return 2;
      }

      throw csvsqldb::ConfigurationException("not implemented");
    }

    int64_t get(const std::string&, csvsqldb::Typer<int64_t>) const override
    {
      throw csvsqldb::ConfigurationException("not implemented");
    }

    float get(const std::string&, csvsqldb::Typer<float>) const override
    {
      throw csvsqldb::ConfigurationException("not implemented");
    }

    double get(const std::string&, csvsqldb::Typer<double>) const override
    {
      throw csvsqldb::ConfigurationException("not implemented");
    }

    std::string get(const std::string& path, csvsqldb::Typer<std::string>) const override
    {
      if (path == "logging.device") {
        return "Console";
      }

      throw csvsqldb::ConfigurationException("not implemented");
    }
  };
}

class TestClass1
{
};
class TestClass2
{
};


TEST_CASE("Logging Test", "[logging]")
{
  SECTION("class logging")
  {
    csvsqldb::GlobalConfiguration::create<csvsqldb::GlobalConfiguration>();
    csvsqldb::config<csvsqldb::GlobalConfiguration>()->configure(std::make_shared<StaticConfiguration>());
    csvsqldb::Logging::init();

    RedirectStdErr red;

    CSVSQLDB_CLASSLOG(TestClass1, 2, "This will not be printed");
    CSVSQLDB_CLASSLOG(TestClass1, 1, "This shows up");
    CSVSQLDB_CLASSLOG(TestClass2, 2, "This shows up also");
    CSVSQLDB_INFOLOG("Cannot be hidden");
    CSVSQLDB_ERRORLOG("Errors are always printed");

    std::ifstream log(getTestPath() / "stderr.txt");
    CHECK(log.good());
    std::string line;
    int line_count(0);
    while (std::getline(log, line).good()) {
      switch (line_count) {
        case 0: {
          CHECK(line.find("TestClass1") != std::string::npos);
          CHECK(line.find("This shows up") != std::string::npos);
          CHECK(line.find("DEBUG") != std::string::npos);
          break;
        }
        case 1: {
          CHECK(line.find("TestClass2") != std::string::npos);
          CHECK(line.find("This shows up also") != std::string::npos);
          CHECK(line.find("DEBUG") != std::string::npos);
          break;
        }
        case 2: {
          CHECK(line.find("Cannot be hidden") != std::string::npos);
          CHECK(line.find("INFO") != std::string::npos);
          break;
        }
        case 3: {
          CHECK(line.find("Errors are always printed") != std::string::npos);
          CHECK(line.find("ERROR") != std::string::npos);
          break;
        }
      }
      ++line_count;
    }
    log.close();
  }
  SECTION("multiline logging")
  {
    csvsqldb::GlobalConfiguration::create<csvsqldb::GlobalConfiguration>();
    csvsqldb::config<csvsqldb::GlobalConfiguration>()->configure(std::make_shared<StaticConfiguration>());
    csvsqldb::config<csvsqldb::GlobalConfiguration>()->logging.escape_newline = true;
    csvsqldb::Logging::init();

    RedirectStdErr red;

    CSVSQLDB_INFOLOG("This is a multiline\ninfo log.");

    std::ifstream log(getTestPath() / "stderr.txt");
    CHECK(log.good());
    std::string line;
    CHECK(std::getline(log, line).good());
    CHECK(line.find("This is a multiline\\ninfo log.") != std::string::npos);
  }
  SECTION("none logger")
  {
    csvsqldb::GlobalConfiguration::create<csvsqldb::GlobalConfiguration>();
    csvsqldb::config<csvsqldb::GlobalConfiguration>()->configure(std::make_shared<StaticConfiguration>());
    csvsqldb::config<csvsqldb::GlobalConfiguration>()->logging.device = "None";
    csvsqldb::Logging::init();

    CSVSQLDB_INFOLOG("Cannot be hidden");
  }
  SECTION("class logging error")
  {
    csvsqldb::GlobalConfiguration::create<csvsqldb::GlobalConfiguration>();
    csvsqldb::config<csvsqldb::GlobalConfiguration>()->configure(std::make_shared<StaticConfiguration>());
    csvsqldb::config<csvsqldb::GlobalConfiguration>()->logging.device = "Unknown device";
    CHECK_THROWS_AS(csvsqldb::Logging::init(), csvsqldb::Exception);
  }
}

TEST_CASE("LogDevice Test", "[logging]")
{
  SECTION("ConsoleLogDevice")
  {
    csvsqldb::ConsoleLogDevice dev;
    CHECK("ConsoleLogDevice" == dev.name());
  }
}
