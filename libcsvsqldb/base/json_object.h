//
//  json_object.h
//  csvsqldb
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

#ifndef csvsqldb_json_object_h
#define csvsqldb_json_object_h

#include "libcsvsqldb/inc.h"

#include "json_parser.h"

#include <any>
#include <map>
#include <stack>
#include <vector>


namespace csvsqldb
{
  namespace json
  {
    /**
     * Represents a JSON object. Can be constructed using the JsonObjectCallback callback with a json::Parser.
     */
    class CSVSQLDB_EXPORT JsonObject
    {
    public:
      enum eType { Array, Object, Value };

      typedef std::map<std::string, JsonObject> ObjectMap;
      typedef std::vector<JsonObject> ObjectArray;

      /**
       * Returns an JsonObject map if this instance is an object. Will throw a JsonException if this instance is not an
       * object.
       * @return The JsonObject map from this object.
       */
      const ObjectMap& getObjects() const;

      /**
       * Retrieves the JsonObject matching the given name. Will throw a JsonException if this instance is not an object or
       * if there is no
       * JsonObject for the name.
       * @param name The name to lookup
       * @return The JsonObject associated to the name.
       */
      const JsonObject& operator[](const std::string& name) const;

      /**
       * Returns an JsonObject array if this instance is an array. Will throw a JsonException if this instance is not an
       * array.
       * @return The JsonObject array from this object.
       */
      const ObjectArray& getArray() const;

      /**
       * If this instance is a value, tries to convert the internal value to a double. Will throw a BadcastException if the
       * value is not a double.
       * @return A double value.
       */
      double getAsDouble() const;

      /**
       * If this instance is a value, tries to convert the internal value to a long. Will throw a BadcastException if the
       * value is not a long.
       * @return A long value.
       */
      long getAsLong() const;

      /**
       * If this instance is a value, tries to convert the internal value to a string. Will throw a BadcastException if the
       * value is not a string.
       * @return A string value.
       */
      std::string getAsString() const;

      /**
       * If this instance is a bool, tries to convert the internal value to a bool. Will throw a BadcastException if the
       * value is not a bool.
       * @return A bool value.
       */
      bool getAsBool() const;


      ObjectMap _objects;
      ObjectArray _array;
      std::any _value;
      eType _type;
    };

    /**
     * JSON callback to construct JsonObject instances.
     */
    class CSVSQLDB_EXPORT JsonObjectCallback : public csvsqldb::json::Callback
    {
    public:
      JsonObjectCallback();

      /**
       * Returns the constructed JsonObject.
       * @return The constructed JsonObject
       */
      const JsonObject& getObject() const;

      void startObject();

      void key(const std::string& key);

      void endObject();

      void startArray();

      void endArray();

      void numberValue(double val);

      void stringValue(const std::string& val);

      void booleanValue(bool val);

      void nullValue();

    private:
      std::stack<JsonObject> _objectStack;
      std::stack<std::string> _keyStack;
      JsonObject _object;
    };
  }
}

#endif
