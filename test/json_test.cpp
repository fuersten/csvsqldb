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


#include "libcsvsqldb/base/exception.h"
#include "libcsvsqldb/base/json_object.h"
#include "libcsvsqldb/base/json_parser.h"

#include <catch2/catch.hpp>

#include <iostream>


namespace
{
  class JsonCallback : public csvsqldb::json::Callback
  {
  public:
    JsonCallback(std::ostringstream& ss)
    : _ss(ss)
    {
    }

    virtual void startObject()
    {
      _ss << "{" << std::endl;
    }
    virtual void key(const std::string& key)
    {
      _ss << "\"" << key << "\" : ";
    }
    virtual void endObject()
    {
      _ss << "}" << std::endl;
    }
    virtual void startArray()
    {
      _ss << "[" << std::endl;
    }
    virtual void endArray()
    {
      _ss << "]" << std::endl;
    }
    virtual void numberValue(double val)
    {
      _ss << val << std::endl;
    }
    virtual void stringValue(const std::string& val)
    {
      _ss << "\"" << val << "\"" << std::endl;
    }
    virtual void booleanValue(bool val)
    {
      _ss << (val ? "true" : "false") << std::endl;
    }
    virtual void nullValue()
    {
      _ss << "null" << std::endl;
    }

  private:
    std::ostringstream& _ss;
  };

  static std::string json = "                        "
                            "{"
                            "    \"Image\": { "
                            "       \"Width\":  800, "
                            "       \"Height\": 600, "
                            "       \"Title\":  \"View from \t15th Floor\", "
                            "       \"Thumbnail\": { "
                            "            \"Url\":    \"http://www.example.com/image/481989943\", "
                            "            \"Height\": 125, "
                            "            \"Width\":  \"100\" "
                            "        }, "
                            "        \"IDs\": [116.47, 943, 234, -38793, null, false], "
                            "        \"Cool\": true "
                            "    } "
                            "} ";

  static std::string json2 = "["
                             " {"
                             " \"precision\": \"zip\","
                             " \"Latitude\":  37.7668,"
                             " \"Longitude\": -122.3959,"
                             " \"Address\":   \"\","
                             " \"City\":      \"SAN FRANCISCO\","
                             " \"State\":     \"CA\","
                             " \"Zip\":       \"94107\","
                             " \"Country\":   \"US\""
                             " },"
                             " {"
                             " \"precision\": \"zip\","
                             " \"Latitude\":  37.371991,"
                             " \"Longitude\": -122.026020,"
                             " \"Address\":   \"\","
                             " \"City\":      \"SUNNYVALE\","
                             " \"State\":     \"CA\","
                             " \"Zip\":       \"94085\","
                             " \"Country\":   \"US\""
                             " }"
                             " ]";

  static std::string jsonBad = "                        "
                               "{"
                               "    \"Image\": { "
                               "       \"Width\":  800, "
                               "       \"Height\": 600, "
                               "       \"Title\":  \"View from \t15th Floor\", "
                               "       \"Thumbnail\": { "
                               "            \"Url\":    \"http://www.example.com/image/481989943\", "
                               "            \"Height\": 125, "
                               "            \"100\" "  // <== the key is missing here
                               "        }, "
                               "        \"IDs\": [116.47, 943, 234, -38793, null, false], "
                               "        \"Cool\": true "
                               "    } "
                               "} ";

  static std::string result = "{\n"
                              "\"Image\" : {\n"
                              "\"Width\" : 800\n"
                              "\"Height\" : 600\n"
                              "\"Title\" : \"View from \t15th Floor\"\n"
                              "\"Thumbnail\" : {\n"
                              "\"Url\" : \"http://www.example.com/image/481989943\"\n"
                              "\"Height\" : 125\n"
                              "\"Width\" : \"100\"\n"
                              "}\n"
                              "\"IDs\" : [\n"
                              "116.47\n"
                              "943\n"
                              "234\n"
                              "-38793\n"
                              "null\n"
                              "false\n"
                              "]\n"
                              "\"Cool\" : true\n}\n"
                              "}\n";

  static std::string result2 = "[\n"
                               "{\n"
                               "\"precision\" : \"zip\"\n"
                               "\"Latitude\" : 37.7668\n"
                               "\"Longitude\" : -122.396\n"
                               "\"Address\" : \"\"\n"
                               "\"City\" : \"SAN FRANCISCO\"\n"
                               "\"State\" : \"CA\"\n"
                               "\"Zip\" : \"94107\"\n"
                               "\"Country\" : \"US\"\n"
                               "}\n"
                               "{\n"
                               "\"precision\" : \"zip\"\n"
                               "\"Latitude\" : 37.372\n"
                               "\"Longitude\" : -122.026\n"
                               "\"Address\" : \"\"\n"
                               "\"City\" : \"SUNNYVALE\"\n"
                               "\"State\" : \"CA\"\n"
                               "\"Zip\" : \"94085\"\n"
                               "\"Country\" : \"US\"\n"
                               "}\n"
                               "]\n";

  static std::string badJson1 = "{ \"Test\" : 0123 }";
  static std::string badJson2 = "{ \"Te\vst\" : 0123 }";
  static std::string badJson3 = "{ \"Test : 123 }";
  static std::string badJson4 = "{ \"Test : 12  ";
  static std::string badJson5 = "{ \"Test\" : 123 \"Test2\" : 236 }";
  static std::string badJson6 = "{ \"Test\" : 123, \"Test2\" : [ 127, 27  }";
  static std::string badJson7 = "{ \"Test\" : 123, \"Test2\" : [ 127, 27 23 ] }";
  static std::string badJson8 = "{ \"Test\" : 123. }";
  static std::string badJson9 = "{ \"Test\" }";
  static std::string badJson10 = "{ \"Test\" : [ \"Test\" : 123 ] }";

  static std::string goodJson1 = "{ \"Test\" : [  ] }";
  static std::string goodJson2 = "{  }";
  static std::string goodJson3 = "[ 123.12e-34 ]";
  static std::string goodJson4 = "{ \"length\" : 0 }";
}


TEST_CASE("Json Test", "[json]")
{
  SECTION("parse string")
  {
    std::ostringstream oss;
    csvsqldb::json::Parser parser(json, std::make_shared<JsonCallback>(oss));
    CHECK(parser.parse());
    CHECK(result == oss.str());

    csvsqldb::json::Parser parser2(jsonBad, std::make_shared<JsonCallback>(oss));
    CHECK_THROWS_AS(parser2.parse(), csvsqldb::JsonException);
  }

  SECTION("parse stream")
  {
    std::ostringstream oss;
    std::istringstream ss(json);
    csvsqldb::json::Parser parser(ss, std::make_shared<JsonCallback>(oss));
    CHECK(parser.parse());
    CHECK(result == oss.str());
  }

  SECTION("parse complex")
  {
    std::ostringstream oss;
    csvsqldb::json::Parser parser(json2, std::make_shared<JsonCallback>(oss));
    CHECK(parser.parse());
    CHECK(result2 == oss.str());
  }

  SECTION("parse only")
  {
    csvsqldb::json::Parser parser(json, JsonCallback::Ptr());
    CHECK(parser.parse());

    csvsqldb::json::Parser parser2(jsonBad, JsonCallback::Ptr());
    CHECK_FALSE(parser2.parse());
  }

  SECTION("parse good")
  {
    csvsqldb::json::Parser parser(goodJson1, JsonCallback::Ptr());
    CHECK(parser.parse());

    csvsqldb::json::Parser parser2(goodJson2, JsonCallback::Ptr());
    CHECK(parser2.parse());

    csvsqldb::json::Parser parser3(goodJson3, JsonCallback::Ptr());
    CHECK(parser3.parse());

    csvsqldb::json::Parser parser4(goodJson4, JsonCallback::Ptr());
    CHECK(parser4.parse());
  }

  SECTION("parse error")
  {
    csvsqldb::json::Parser parser(badJson1, JsonCallback::Ptr());
    CHECK_FALSE(parser.parse());

    csvsqldb::json::Parser parser2(badJson2, JsonCallback::Ptr());
    CHECK_FALSE(parser2.parse());

    csvsqldb::json::Parser parser3(badJson3, JsonCallback::Ptr());
    CHECK_FALSE(parser3.parse());

    csvsqldb::json::Parser parser4(badJson4, JsonCallback::Ptr());
    CHECK_FALSE(parser4.parse());

    csvsqldb::json::Parser parser5(badJson5, JsonCallback::Ptr());
    CHECK_FALSE(parser5.parse());

    csvsqldb::json::Parser parser6(badJson6, JsonCallback::Ptr());
    CHECK_FALSE(parser6.parse());

    csvsqldb::json::Parser parser7(badJson7, JsonCallback::Ptr());
    CHECK_FALSE(parser7.parse());

    csvsqldb::json::Parser parser8(badJson8, JsonCallback::Ptr());
    CHECK_FALSE(parser8.parse());

    csvsqldb::json::Parser parser9(badJson9, JsonCallback::Ptr());
    CHECK_FALSE(parser9.parse());

    csvsqldb::json::Parser parser10(badJson10, JsonCallback::Ptr());
    CHECK_FALSE(parser10.parse());
  }

  SECTION("json object")
  {
    std::shared_ptr<csvsqldb::json::JsonObjectCallback> callback = std::make_shared<csvsqldb::json::JsonObjectCallback>();
    csvsqldb::json::Parser parser(json, callback);
    CHECK(parser.parse());
    const csvsqldb::json::JsonObject& obj = callback->getObject();
    CHECK(obj["Image"]["Width"].getAsLong() == 800);
    CHECK(obj["Image"]["Title"].getAsString() == "View from \t15th Floor");
    CHECK(obj["Image"]["IDs"].getArray()[0].getAsDouble() - 116.47 <= 0.001);
    CHECK(obj["Image"]["IDs"].getArray()[3].getAsLong() == -38793);
    CHECK_FALSE(obj["Image"]["IDs"].getArray()[5].getAsBool());
  }
}
