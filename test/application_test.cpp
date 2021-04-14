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


#include <csvsqldb/base/application.h>

#include "test_helper.h"

#include <catch2/catch.hpp>

#include <fstream>

class MyApplication : public csvsqldb::Application
{
public:
  MyApplication(int argc, char** argv, bool setUp, bool throwException = false)
  : csvsqldb::Application(argc, argv)
  , _setUp(setUp)
  , _throwException{throwException}
  {
  }

  bool _didSetup{false};
  bool _didRun{false};
  bool _didTearDown{false};

private:
  bool setUp(int argc, char** argv) override
  {
    _didSetup = true;
    return _setUp;
  }

  int doRun() override
  {
    _didRun = true;

    if (_throwException) {
      throw std::runtime_error{"throw in doRun"};
    }

    return 0;
  }

  void tearDown() override
  {
    _didTearDown = true;
  }

  bool _setUp{false};
  bool _throwException{false};
};


TEST_CASE("Application Test", "[app]")
{
  SECTION("run")
  {
    int argc = 2;
    const char* args[] = {"MyApp", "Arg1"};

    MyApplication myapp(argc, const_cast<char**>(args), true);
    int ret = myapp.run();

    CHECK(0 == ret);
    CHECK(true == myapp._didSetup);
    CHECK(true == myapp._didRun);
    CHECK(true == myapp._didTearDown);
  }
  SECTION("dont run")
  {
    int argc = 2;
    const char* args[] = {"MyApp", "Arg1"};

    MyApplication myapp(argc, const_cast<char**>(args), false);
    int ret = myapp.run();

    CHECK(-1 == ret);
    CHECK(true == myapp._didSetup);
    CHECK(false == myapp._didRun);
    CHECK(false == myapp._didTearDown);
  }
  SECTION("throw in run")
  {
    int argc = 2;
    const char* args[] = {"MyApp", "Arg1"};

    RedirectStdErr red;

    MyApplication myapp(argc, const_cast<char**>(args), true, true);
    int ret = myapp.run();

    CHECK(-1 == ret);
    CHECK(true == myapp._didSetup);
    CHECK(true == myapp._didRun);
    CHECK(false == myapp._didTearDown);

    std::ifstream log(getTestPath() / "stderr.txt");
    CHECK(log.good());
    std::string line;

    CHECK(std::getline(log, line).good());
    CHECK(line.find("throw in doRun") != std::string::npos);
  }
}
