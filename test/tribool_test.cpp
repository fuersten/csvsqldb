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

#include <catch2/catch.hpp>

#include <vector>


namespace
{
  static bool operatorBool(const csvsqldb::Tribool& tb)
  {
    if (tb) {
      return true;
    }

    return false;
  }
}


TEST_CASE("Tribool Test", "[utils]")
{
  SECTION("tribool")
  {
    csvsqldb::Tribool tb;
    CHECK_FALSE(!tb);
    CHECK_FALSE(operatorBool(tb));
    CHECK(tb.isIndeterminate());
    CHECK_FALSE(tb.isTrue());
    CHECK_FALSE(tb.isFalse());

    tb.set(true);
    CHECK_FALSE(!tb);
    CHECK(operatorBool(tb));
    CHECK_FALSE(tb.isIndeterminate());
    CHECK(tb.isTrue());
    CHECK_FALSE(tb.isFalse());

    tb.set(false);
    CHECK(!tb);
    CHECK_FALSE(operatorBool(tb));
    CHECK_FALSE(tb.isIndeterminate());
    CHECK_FALSE(tb.isTrue());
    CHECK(tb.isFalse());
  }

  SECTION("equality")
  {
    csvsqldb::Tribool tb1;
    csvsqldb::Tribool tb2;

    CHECK_FALSE(tb1 == tb2);

    tb1.set(true);
    CHECK_FALSE(tb1 == tb2);
    tb2.set(false);
    CHECK_FALSE(tb1 == tb2);
    tb2.set(true);
    CHECK(tb1 == tb2);
    tb1.set(false);
    tb2.set(false);
    CHECK(tb1 == tb2);
  }
}
