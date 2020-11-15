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


#include "libcsvsqldb/base/tribool.h"

#include "test.h"

#include <vector>


static bool operatorBool(const csvsqldb::Tribool& tb)
{
  if (tb) {
    return true;
  }

  return false;
}

class TriboolTestCase
{
public:
  void setUp()
  {
  }

  void tearDown()
  {
  }

  void triboolTest()
  {
    csvsqldb::Tribool tb;
    MPF_TEST_ASSERTEQUAL(false, !tb);
    MPF_TEST_ASSERTEQUAL(false, operatorBool(tb));
    MPF_TEST_ASSERTEQUAL(true, tb.isIndeterminate());
    MPF_TEST_ASSERTEQUAL(false, tb.isTrue());
    MPF_TEST_ASSERTEQUAL(false, tb.isFalse());

    tb.set(true);
    MPF_TEST_ASSERTEQUAL(false, !tb);
    MPF_TEST_ASSERTEQUAL(true, operatorBool(tb));
    MPF_TEST_ASSERTEQUAL(false, tb.isIndeterminate());
    MPF_TEST_ASSERTEQUAL(true, tb.isTrue());
    MPF_TEST_ASSERTEQUAL(false, tb.isFalse());

    tb.set(false);
    MPF_TEST_ASSERTEQUAL(true, !tb);
    MPF_TEST_ASSERTEQUAL(false, operatorBool(tb));
    MPF_TEST_ASSERTEQUAL(false, tb.isIndeterminate());
    MPF_TEST_ASSERTEQUAL(false, tb.isTrue());
    MPF_TEST_ASSERTEQUAL(true, tb.isFalse());
  }

  void equalityTest()
  {
    csvsqldb::Tribool tb1;
    csvsqldb::Tribool tb2;

    MPF_TEST_ASSERTEQUAL(false, tb1 == tb2);

    tb1.set(true);
    MPF_TEST_ASSERTEQUAL(false, tb1 == tb2);
    tb2.set(false);
    MPF_TEST_ASSERTEQUAL(false, tb1 == tb2);
    tb2.set(true);
    MPF_TEST_ASSERTEQUAL(true, tb1 == tb2);
    tb1.set(false);
    tb2.set(false);
    MPF_TEST_ASSERTEQUAL(true, tb1 == tb2);
  }
};

MPF_REGISTER_TEST_START("ApplicationTestSuite", TriboolTestCase);
MPF_REGISTER_TEST(TriboolTestCase::triboolTest);
MPF_REGISTER_TEST(TriboolTestCase::equalityTest);
MPF_REGISTER_TEST_END();
