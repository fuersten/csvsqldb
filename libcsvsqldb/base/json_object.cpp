//
//  json_object.cpp
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

#include "json_object.h"

#include "exception.h"


namespace csvsqldb
{
  namespace json
  {
    const JsonObject::ObjectMap& JsonObject::getObjects() const
    {
      if (_type != Object) {
        CSVSQLDB_THROW(csvsqldb::JsonException, "not an object");
      }
      return _objects;
    }

    const JsonObject::ObjectArray& JsonObject::getArray() const
    {
      if (_type != Array) {
        CSVSQLDB_THROW(csvsqldb::JsonException, "not an array");
      }
      return _array;
    }

    double JsonObject::getAsDouble() const
    {
      return std::any_cast<double>(_value);
    }

    long JsonObject::getAsLong() const
    {
      return static_cast<long>(std::any_cast<double>(_value));
    }

    std::string JsonObject::getAsString() const
    {
      return std::any_cast<std::string>(_value);
    }

    bool JsonObject::getAsBool() const
    {
      return std::any_cast<bool>(_value);
    }

    const JsonObject& JsonObject::operator[](const std::string& name) const
    {
      if (_type != Object) {
        CSVSQLDB_THROW(csvsqldb::JsonException, "not an object");
      }
      const auto iter = _objects.find(name);
      if (iter != _objects.end()) {
        return iter->second;
      }
      CSVSQLDB_THROW(csvsqldb::JsonException, "object with name '" << name << "' not found");
    }

    JsonObjectCallback::JsonObjectCallback()
    {
    }

    const JsonObject& JsonObjectCallback::getObject() const
    {
      return _object;
    }

    void JsonObjectCallback::startObject()
    {
      JsonObject obj;
      obj._type = JsonObject::Object;
      _objectStack.push(obj);
    }

    void JsonObjectCallback::key(const std::string& key)
    {
      _keyStack.push(key);
    }

    void JsonObjectCallback::endObject()
    {
      JsonObject obj = _objectStack.top();
      _objectStack.pop();
      if (_objectStack.empty()) {
        _object = obj;
        return;
      }
      if (_objectStack.top()._type == JsonObject::Object) {
        _objectStack.top()._objects.insert(std::make_pair(_keyStack.top(), obj));
        _keyStack.pop();
      } else if (_objectStack.top()._type == JsonObject::Array) {
        _objectStack.top()._array.push_back(obj);
      }
    }

    void JsonObjectCallback::startArray()
    {
      JsonObject obj;
      obj._type = JsonObject::Array;
      _objectStack.push(obj);
    }

    void JsonObjectCallback::endArray()
    {
      JsonObject obj = _objectStack.top();
      _objectStack.pop();
      if (!_objectStack.empty()) {
        _objectStack.top()._objects.insert(std::make_pair(_keyStack.top(), obj));
        _keyStack.pop();
      }
    }

    void JsonObjectCallback::numberValue(double val)
    {
      if (_objectStack.top()._type == JsonObject::Object) {
        JsonObject obj;
        obj._value = val;
        obj._type = JsonObject::Value;
        _objectStack.top()._objects.insert(std::make_pair(_keyStack.top(), obj));
        _keyStack.pop();
      } else if (_objectStack.top()._type == JsonObject::Array) {
        JsonObject obj;
        obj._value = val;
        _objectStack.top()._array.push_back(obj);
      } else {
        _objectStack.top()._value = val;
      }
    }

    void JsonObjectCallback::stringValue(const std::string& val)
    {
      if (_objectStack.top()._type == JsonObject::Object) {
        JsonObject obj;
        obj._value = val;
        obj._type = JsonObject::Value;
        _objectStack.top()._objects.insert(std::make_pair(_keyStack.top(), obj));
        _keyStack.pop();
      } else if (_objectStack.top()._type == JsonObject::Array) {
        JsonObject obj;
        obj._value = val;
        _objectStack.top()._array.push_back(obj);
      } else {
        _objectStack.top()._value = val;
      }
    }

    void JsonObjectCallback::booleanValue(bool val)
    {
      if (_objectStack.top()._type == JsonObject::Object) {
        JsonObject obj;
        obj._value = val;
        obj._type = JsonObject::Value;
        _objectStack.top()._objects.insert(std::make_pair(_keyStack.top(), obj));
        _keyStack.pop();
      } else if (_objectStack.top()._type == JsonObject::Array) {
        JsonObject obj;
        obj._value = val;
        _objectStack.top()._array.push_back(obj);
      } else {
        _objectStack.top()._value = val;
      }
    }

    void JsonObjectCallback::nullValue()
    {
      if (_objectStack.top()._type == JsonObject::Object) {
        JsonObject obj;
        obj._type = JsonObject::Value;
        _objectStack.top()._objects.insert(std::make_pair(_keyStack.top(), obj));
        _keyStack.pop();
      } else if (_objectStack.top()._type == JsonObject::Array) {
        JsonObject obj;
        _objectStack.top()._array.push_back(obj);
      } else {
        _objectStack.top()._value = std::any();
      }
    }
  }
}
