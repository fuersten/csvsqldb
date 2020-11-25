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


#include <csvsqldb/base/exception.h>

#include "test_helper.h"

#include <catch2/catch.hpp>

#include <cstring>
#include <iostream>


TEST_CASE("Exception Test", "[exception]")
{
  SECTION("construction")
  {
    auto ex = csvsqldb::Exception(ENOENT, "Filesystem");
    CHECK(ex.code().value() == ENOENT);
    CHECK(std::string("Filesystem: No such file or directory") == ex.what());

    auto ex1 = csvsqldb::Exception(std::errc::invalid_argument, "Parameter");
    CHECK(ex1.code().value() == EINVAL);
    CHECK(std::string("Parameter: Invalid argument") == ex1.what());

    auto ex2 = csvsqldb::Exception("check");
    CHECK(ex2.code().value() == 0);
    CHECK(std::string("check") == ex2.what());

    auto ex3 = csvsqldb::Exception(std::make_error_code(std::errc::invalid_argument), "Parameter");
    CHECK(ex3.code().value() == EINVAL);
    CHECK(std::string("Parameter: Invalid argument") == ex3.what());
  }

  SECTION("copy")
  {
    auto ex = csvsqldb::Exception(ENOENT, "Filesystem");
    auto ex1{ex};

    CHECK(ex1.code().value() == ENOENT);
    CHECK(std::string("Filesystem: No such file or directory") == ex1.what());
  }

  SECTION("exception")
  {
    try {
      throw csvsqldb::FilesystemException(ENOENT, "Filesystem");
    } catch (csvsqldb::Exception& ex) {
      CHECK(ENOENT == ex.code().value());
    }
  }

  SECTION("errno")
  {
    errno = EWOULDBLOCK;
    std::string txt = csvsqldb::errnoText();
    CHECK(strerror(EWOULDBLOCK) == txt);
  }

  SECTION("sys exception")
  {
    errno = EWOULDBLOCK;
    try {
      csvsqldb::throwSysError("mydomain");
    } catch (csvsqldb::Exception& ex) {
      CHECK(EWOULDBLOCK == ex.code().value());
      errno = EWOULDBLOCK;
      std::string expected = csvsqldb::errnoText();
      CHECK("mydomain" + std::string(": ") + expected == ex.what());
    }
  }

  SECTION("evaluate exception")
  {
    RedirectStdErr red;

    try {
      CSVSQLDB_THROW(csvsqldb::Exception, "check");
    } catch (...) {
      csvsqldb::evaluateException();
    }

    try {
      throw std::system_error(std::make_error_code(std::io_errc::stream));
    } catch (...) {
      csvsqldb::evaluateException();
    }

    try {
      throw std::runtime_error{"test"};
    } catch (...) {
      csvsqldb::evaluateException();
    }

    try {
      throw std::string{"test"};
    } catch (...) {
      csvsqldb::evaluateException();
    }
  }

  SECTION("evaluate and throw exception")
  {
    RedirectStdErr red;

    try {
      CSVSQLDB_THROW(csvsqldb::Exception, "check");
    } catch (...) {
      CHECK_THROWS_AS(csvsqldb::evaluateExceptionAndThrow(), csvsqldb::Exception);
    }

    try {
      throw std::system_error(std::make_error_code(std::io_errc::stream));
    } catch (...) {
      CHECK_THROWS_AS(csvsqldb::evaluateExceptionAndThrow(), std::system_error);
    }

    try {
      throw std::runtime_error{"test"};
    } catch (...) {
      CHECK_THROWS_AS(csvsqldb::evaluateExceptionAndThrow(), std::runtime_error);
    }

    try {
      throw std::string{"test"};
    } catch (...) {
      CHECK_THROWS_AS(csvsqldb::evaluateExceptionAndThrow(), std::string);
    }
  }
}
