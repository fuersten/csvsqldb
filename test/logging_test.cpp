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


#include "libcsvsqldb/base/configuration.h"
#include "libcsvsqldb/base/logging.h"

#include "test.h"
#include "test_helper.h"

#include <fstream>


class StaticConfiguration : public csvsqldb::Configuration
{
public:
  StaticConfiguration()
  {
  }

private:
  virtual size_t doGetProperties(const std::string& path, csvsqldb::StringVector& properties) const
  {
    properties.push_back("TestClass1");
    properties.push_back("TestClass2");
    return 2;
  }

  virtual bool doHasProperty(const std::string& path) const
  {
    if (path == "debug.level") {
      return true;
    }
    throw csvsqldb::ConfigurationException("not implemented");
  }

  virtual bool get(const std::string& path, csvsqldb::Typer<bool> typer) const
  {
    throw csvsqldb::ConfigurationException("not implemented");
  }

  virtual int32_t get(const std::string& path, csvsqldb::Typer<int32_t> typer) const
  {
    if (path == "debug.level.TestClass1") {
      return 1;
    } else if (path == "debug.level.TestClass2") {
      return 2;
    }

    throw csvsqldb::ConfigurationException("not implemented");
  }

  virtual int64_t get(const std::string& path, csvsqldb::Typer<int64_t> typer) const
  {
    throw csvsqldb::ConfigurationException("not implemented");
  }

  virtual float get(const std::string& path, csvsqldb::Typer<float> typer) const
  {
    throw csvsqldb::ConfigurationException("not implemented");
  }

  virtual double get(const std::string& path, csvsqldb::Typer<double> typer) const
  {
    throw csvsqldb::ConfigurationException("not implemented");
  }

  virtual std::string get(const std::string& path, csvsqldb::Typer<std::string> typer) const
  {
    if (path == "logging.device") {
      return "Console";
    }

    throw csvsqldb::ConfigurationException("not implemented");
  }
};

class TestClass1
{
};
class TestClass2
{
};

class LoggingTestCase
{
public:
  void setUp()
  {
  }

  void tearDown()
  {
  }

  void logIni()
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

    std::ifstream log((CSVSQLDB_TEST_PATH + std::string("/stderr.txt")));
    MPF_TEST_ASSERTEQUAL(true, log.good());
    std::string line;
    int line_count(0);
    while (std::getline(log, line).good()) {
      switch (line_count) {
        case 0: {
          MPF_TEST_ASSERT(line.find("TestClass1") != std::string::npos);
          MPF_TEST_ASSERT(line.find("This shows up") != std::string::npos);
          MPF_TEST_ASSERT(line.find("DEBUG") != std::string::npos);
          break;
        }
        case 1: {
          MPF_TEST_ASSERT(line.find("TestClass2") != std::string::npos);
          MPF_TEST_ASSERT(line.find("This shows up also") != std::string::npos);
          MPF_TEST_ASSERT(line.find("DEBUG") != std::string::npos);
          break;
        }
        case 2: {
          MPF_TEST_ASSERT(line.find("Cannot be hidden") != std::string::npos);
          MPF_TEST_ASSERT(line.find("INFO") != std::string::npos);
          break;
        }
        case 3: {
          MPF_TEST_ASSERT(line.find("Errors are always printed") != std::string::npos);
          MPF_TEST_ASSERT(line.find("ERROR") != std::string::npos);
          break;
        }
      }
      ++line_count;
    }
    log.close();
  }
};

MPF_REGISTER_TEST_START("ApplicationTestSuite", LoggingTestCase);
MPF_REGISTER_TEST(LoggingTestCase::logIni);
MPF_REGISTER_TEST_END();
