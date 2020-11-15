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

#include "libcsvsqldb/base/any.h"
#include "libcsvsqldb/base/float_helper.h"
#include "libcsvsqldb/base/function_traits.h"

#include "test.h"


class AnyTestCase
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
    csvsqldb::Any any_float(47.11f);
    csvsqldb::Any any_string(std::string("test"));
    csvsqldb::Any any_integer(384765);
    csvsqldb::Any any_char(static_cast<const char*>("test"));
    csvsqldb::Any any_empty;
    std::vector<std::string> strings;
    csvsqldb::Any any_vector(strings);

    MPF_TEST_ASSERTEQUAL(true, any_empty.empty());
    MPF_TEST_ASSERTEQUAL(false, any_float.empty());
  }

  void copyTest()
  {
    csvsqldb::Any any_integer(384765);
    csvsqldb::Any any_second(any_integer);
    MPF_TEST_ASSERTEQUAL(false, any_second.empty());
    MPF_TEST_ASSERTEQUAL(384765, csvsqldb::any_cast<int32_t>(any_second));
  }

  void assignmentTest()
  {
    csvsqldb::Any any_integer(384765);
    csvsqldb::Any any_second = any_integer;
    MPF_TEST_ASSERTEQUAL(false, any_second.empty());
    MPF_TEST_ASSERTEQUAL(384765, csvsqldb::any_cast<int32_t>(any_second));

    csvsqldb::Any any_third = std::string("Test");
    MPF_TEST_ASSERTEQUAL(false, any_third.empty());
    MPF_TEST_ASSERTEQUAL("Test", csvsqldb::any_cast<std::string>(any_third));

    any_third = 47.11;
    MPF_TEST_ASSERTEQUAL(false, any_third.empty());
    MPF_TEST_ASSERT(csvsqldb::compare(47.11, csvsqldb::any_cast<double>(any_third)));
  }

  void castTest()
  {
    csvsqldb::Any any_double(47.11);
    MPF_TEST_ASSERT(csvsqldb::compare(47.11, csvsqldb::any_cast<double>(any_double)));
    MPF_TEST_EXPECTS(csvsqldb::any_cast<int32_t>(any_double), csvsqldb::BadcastException);

    MPF_TEST_ASSERT(csvsqldb::compare(47.11, *csvsqldb::any_cast<double>(&any_double)));
    MPF_TEST_ASSERT(!csvsqldb::any_cast<int32_t>(&any_double));
  }

  void constRefCastTest()
  {
    csvsqldb::Any any_integer(384765);
    MPF_TEST_ASSERTEQUAL(384765, csvsqldb::any_cast<const int&>(any_integer));
  }
};

MPF_REGISTER_TEST_START("ApplicationTestSuite", AnyTestCase);
MPF_REGISTER_TEST(AnyTestCase::constructionTest);
MPF_REGISTER_TEST(AnyTestCase::copyTest);
MPF_REGISTER_TEST(AnyTestCase::assignmentTest);
MPF_REGISTER_TEST(AnyTestCase::castTest);
MPF_REGISTER_TEST(AnyTestCase::constRefCastTest);
MPF_REGISTER_TEST_END();
