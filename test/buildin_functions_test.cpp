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


#include "libcsvsqldb/base/float_helper.h"
#include "libcsvsqldb/buildin_functions.h"

#include <catch2/catch.hpp>


TEST_CASE("Buildin Functions Test", "[engine]")
{
  csvsqldb::FunctionRegistry _registry;
  initBuildInFunctions(_registry);

  SECTION("buildin date functions")
  {
    csvsqldb::Function::Ptr function = _registry.getFunction("CURRENT_DATE");
    CHECK(function);
    CHECK("CURRENT_DATE" == function->getName());
    CHECK(csvsqldb::DATE == function->getReturnType());
    csvsqldb::Variants parameter;
    csvsqldb::Variant result = function->call(parameter);
    CHECK(csvsqldb::DATE == result.getType());

    function = _registry.getFunction("CURRENT_TIME");
    CHECK(function);
    CHECK("CURRENT_TIME" == function->getName());
    CHECK(csvsqldb::TIME == function->getReturnType());
    result = function->call(parameter);
    CHECK(csvsqldb::TIME == result.getType());

    function = _registry.getFunction("EXTRACT");
    CHECK(function);
    CHECK("EXTRACT" == function->getName());
    CHECK(csvsqldb::INT == function->getReturnType());
    parameter.push_back(csvsqldb::Variant(5));
    parameter.push_back(csvsqldb::Timestamp(1970, csvsqldb::Date::September, 23, 8, 9, 11));
    result = function->call(parameter);
    CHECK(csvsqldb::INT == result.getType());
    CHECK(9 == result.asInt());

    parameter[0] = csvsqldb::Variant(6);
    result = function->call(parameter);
    CHECK(csvsqldb::INT == result.getType());
    CHECK(1970 == result.asInt());

    parameter[0] = csvsqldb::Variant(4);
    result = function->call(parameter);
    CHECK(csvsqldb::INT == result.getType());
    CHECK(23 == result.asInt());

    parameter[0] = csvsqldb::Variant(3);
    result = function->call(parameter);
    CHECK(csvsqldb::INT == result.getType());
    CHECK(8 == result.asInt());

    parameter[0] = csvsqldb::Variant(2);
    result = function->call(parameter);
    CHECK(csvsqldb::INT == result.getType());
    CHECK(9 == result.asInt());

    parameter[0] = csvsqldb::Variant(1);
    result = function->call(parameter);
    CHECK(csvsqldb::INT == result.getType());
    CHECK(11 == result.asInt());

    function = _registry.getFunction("DATE_FORMAT");
    CHECK(function);
    CHECK("DATE_FORMAT" == function->getName());
    CHECK(csvsqldb::STRING == function->getReturnType());
    parameter.clear();
    parameter.push_back(csvsqldb::Date(1970, csvsqldb::Date::September, 23));
    parameter.push_back("%d.%m.%Y %j %U %w");
    result = function->call(parameter);
    CHECK(csvsqldb::STRING == result.getType());
    CHECK("23.09.1970 266 39 3" == result.toString());

    function = _registry.getFunction("TIME_FORMAT");
    CHECK(function);
    CHECK("TIME_FORMAT" == function->getName());
    CHECK(csvsqldb::STRING == function->getReturnType());
    parameter.clear();
    parameter.push_back(csvsqldb::Time(8, 9, 11));
    parameter.push_back("%H@%M@%S");
    result = function->call(parameter);
    CHECK(csvsqldb::STRING == result.getType());
    CHECK("08@09@11" == result.toString());

    function = _registry.getFunction("TIMESTAMP_FORMAT");
    CHECK(function);
    CHECK("TIMESTAMP_FORMAT" == function->getName());
    CHECK(csvsqldb::STRING == function->getReturnType());
    parameter.clear();
    parameter.push_back(csvsqldb::Timestamp(1970, csvsqldb::Date::September, 23, 8, 9, 11));
    parameter.push_back("%Y@%m@%dT@%H@%M@%S");
    result = function->call(parameter);
    CHECK(csvsqldb::STRING == result.getType());
    CHECK("1970@09@23T@08@09@11" == result.toString());
  }

  SECTION("buildin string functions")
  {
    csvsqldb::Function::Ptr function = _registry.getFunction("UPPER");
    CHECK(function);
    CHECK("UPPER" == function->getName());
    CHECK(csvsqldb::STRING == function->getReturnType());
    csvsqldb::Variants parameter;
    parameter.push_back(csvsqldb::Variant("Lars"));
    csvsqldb::Variant result = function->call(parameter);
    CHECK(csvsqldb::STRING == result.getType());
    CHECK("LARS" == result.toString());

    function = _registry.getFunction("LOWER");
    CHECK(function);
    CHECK("LOWER" == function->getName());
    CHECK(csvsqldb::STRING == function->getReturnType());
    result = function->call(parameter);
    CHECK(csvsqldb::STRING == result.getType());
    CHECK("lars" == result.toString());

    function = _registry.getFunction("CHARACTER_LENGTH");
    CHECK(function);
    CHECK("CHARACTER_LENGTH" == function->getName());
    CHECK(csvsqldb::INT == function->getReturnType());
    result = function->call(parameter);
    CHECK(csvsqldb::INT == result.getType());
    CHECK(4 == result.asInt());

    function = _registry.getFunction("CHAR_LENGTH");
    CHECK(function);
    CHECK("CHAR_LENGTH" == function->getName());
    CHECK(csvsqldb::INT == function->getReturnType());
    result = function->call(parameter);
    CHECK(csvsqldb::INT == result.getType());
    CHECK(4 == result.asInt());
  }

  SECTION("buildin math functions")
  {
    csvsqldb::Function::Ptr function = _registry.getFunction("POW");
    CHECK(function);
    CHECK("POW" == function->getName());
    CHECK(csvsqldb::REAL == function->getReturnType());
    csvsqldb::Variants parameter;
    parameter.push_back(csvsqldb::Variant(10.0));
    parameter.push_back(csvsqldb::Variant(2.0));
    csvsqldb::Variant result = function->call(parameter);
    CHECK(csvsqldb::REAL == result.getType());
    CHECK(csvsqldb::compare(100.0, result.asDouble()));
  }
}
