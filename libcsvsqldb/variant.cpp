//
//  variant.cpp
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

#include "variant.h"

#include "base/float_helper.h"


namespace csvsqldb
{
  CSVSQLDB_IMPLEMENT_EXCEPTION(VariantException, csvsqldb::Exception);


  Variant::~Variant()
  {
    if (_refCount) {
      if (_refCount->dec() == 0) {
        delete[] _storage._string;
        _storage._string = nullptr;
        delete _refCount;
      }
    }
  }

  Variant::Variant()
  : _refCount(nullptr)
  , _type(NONE)
  , _isNull(false)
  {
  }

  Variant::Variant(const Variant& rhs)
  : _refCount(rhs._refCount)
  , _type(rhs._type)
  , _isNull(rhs._isNull)
  {
    switch (_type) {
      case NONE:
        break;
      case STRING:
        if (_refCount) {
          _refCount->inc();
        }
        _storage._string = rhs._storage._string;
        break;
      case REAL:
        _storage._real = rhs._storage._real;
        break;
      case INT:
        _storage._int = rhs._storage._int;
        break;
      case BOOLEAN:
        _storage._bool = rhs._storage._bool;
        break;
      case DATE:
        _storage._date = rhs._storage._date;
        break;
      case TIME:
        _storage._time = rhs._storage._time;
        break;
      case TIMESTAMP:
        _storage._timestamp = rhs._storage._timestamp;
        break;
    }
  }

  Variant::Variant(eType type)
  : _refCount(nullptr)
  , _type(type)
  , _isNull(true)
  {
  }

  Variant::Variant(int32_t integer)
  : _refCount(nullptr)
  , _type(INT)
  , _isNull(false)
  {
    _storage._int = integer;
  }

  Variant::Variant(int64_t integer)
  : _refCount(nullptr)
  , _type(INT)
  , _isNull(false)
  {
    _storage._int = integer;
  }

  Variant::Variant(size_t integer)
  : _refCount(nullptr)
  , _type(INT)
  , _isNull(false)
  {
    _storage._int = static_cast<int64_t>(integer);
  }

  Variant::Variant(double real)
  : _refCount(nullptr)
  , _type(REAL)
  , _isNull(false)
  {
    _storage._real = real;
  }

  Variant::Variant(const csvsqldb::Date& date)
  : _refCount(nullptr)
  , _type(DATE)
  , _isNull(false)
  {
    _storage._date = date.asJulianDay();
  }

  Variant::Variant(const csvsqldb::Time& time)
  : _refCount(nullptr)
  , _type(TIME)
  , _isNull(false)
  {
    _storage._time = time.asInteger();
  }

  Variant::Variant(const csvsqldb::Timestamp& timestamp)
  : _refCount(nullptr)
  , _type(TIMESTAMP)
  , _isNull(false)
  {
    _storage._timestamp = timestamp.asInteger();
  }

  Variant::Variant(bool boolean)
  : _refCount(nullptr)
  , _type(BOOLEAN)
  , _isNull(false)
  {
    _storage._bool = boolean;
  }

  Variant::Variant(const char* string, bool owner)
  : _refCount(nullptr)
  , _type(STRING)
  , _isNull(false)
  {
    if (owner) {
      _refCount = new RefCount;
    }
    _storage._string = string;
  }

  Variant::Variant(const std::string& s)
  : _type(STRING)
  , _isNull(false)
  {
    _refCount = new RefCount;
    size_t len = s.length();
    _storage._string = new char[len + 1];
    s.copy(const_cast<char*>(_storage._string), len);
    const_cast<char*>(_storage._string)[len] = '\0';
  }

  void Variant::disconnect()
  {
    if (!_isNull && _type == STRING && !_refCount) {
      _refCount = new RefCount;
      size_t len = ::strlen(_storage._string);
      char* s = new char[len + 1];
      ::memcpy(s, _storage._string, len);
      s[len] = '\0';
      _storage._string = s;
    }
  }

  Variant& Variant::operator=(const Variant& rhs)
  {
    if (_refCount) {
      if (_refCount->dec() == 0) {
        delete[] _storage._string;
        delete _refCount;
      }
    }

    _type = rhs._type;
    _refCount = rhs._refCount;
    _isNull = rhs._isNull;
    switch (_type) {
      case NONE:
        CSVSQLDB_THROW(VariantException, "NONE not allowed as type");
      case STRING:
        if (_refCount) {
          _refCount->inc();
        }
        _storage._string = rhs._storage._string;
        break;
      case REAL:
        _storage._real = rhs._storage._real;
        break;
      case INT:
        _storage._int = rhs._storage._int;
        break;
      case BOOLEAN:
        _storage._bool = rhs._storage._bool;
        break;
      case DATE:
        _storage._date = rhs._storage._date;
        break;
      case TIME:
        _storage._time = rhs._storage._time;
        break;
      case TIMESTAMP:
        _storage._timestamp = rhs._storage._timestamp;
        break;
    }

    return *this;
  }

  bool Variant::operator==(const Variant& rhs) const
  {
    if (_type != rhs._type) {
      CSVSQLDB_THROW(VariantException,
                     "comparing Variants with different types (" << typeToString(_type) << ":" << typeToString(rhs._type));
    }
    if (_isNull || rhs._isNull) {
      return false;
    }
    switch (_type) {
      case NONE:
        CSVSQLDB_THROW(VariantException, "cannot compare Variant with no type");
      case STRING:
        return ::strcoll(_storage._string, rhs._storage._string) == 0;
        break;
      case REAL:
        return csvsqldb::compare(_storage._real, rhs._storage._real);
        break;
      case INT:
        return _storage._int == rhs._storage._int;
        break;
      case BOOLEAN:
        return _storage._bool == rhs._storage._bool;
        break;
      case DATE:
        return _storage._date == rhs._storage._date;
        break;
      case TIME:
        return _storage._time == rhs._storage._time;
        break;
      case TIMESTAMP:
        return _storage._timestamp == rhs._storage._timestamp;
        break;
    }
    throw std::runtime_error("just to make VC2013 happy");
  }

  bool Variant::operator!=(const Variant& rhs) const
  {
    if (_type != rhs._type) {
      CSVSQLDB_THROW(VariantException,
                     "comparing Variants with different types (" << typeToString(_type) << ":" << typeToString(rhs._type));
    }
    if (_isNull || rhs._isNull) {
      return false;
    }
    switch (_type) {
      case NONE:
        CSVSQLDB_THROW(VariantException, "cannot compare Variant with no type");
      case STRING:
        return ::strcoll(_storage._string, rhs._storage._string) != 0;
        break;
      case REAL:
        return !csvsqldb::compare(_storage._real, rhs._storage._real);
        break;
      case INT:
        return _storage._int != rhs._storage._int;
        break;
      case BOOLEAN:
        return _storage._bool != rhs._storage._bool;
        break;
      case DATE:
        return _storage._date != rhs._storage._date;
        break;
      case TIME:
        return _storage._time != rhs._storage._time;
        break;
      case TIMESTAMP:
        return _storage._timestamp != rhs._storage._timestamp;
        break;
    }
    throw std::runtime_error("just to make VC2013 happy");
  }

  bool Variant::operator<(const Variant& rhs) const
  {
    if (_type != rhs._type) {
      CSVSQLDB_THROW(VariantException,
                     "comparing Variants with different types (" << typeToString(_type) << ":" << typeToString(rhs._type));
    }
    if (_isNull || rhs._isNull) {
      return false;
    }
    switch (_type) {
      case NONE:
        CSVSQLDB_THROW(VariantException, "cannot compare Variant with no type");
      case STRING:
        return ::strcoll(_storage._string, rhs._storage._string) < 0;
        break;
      case REAL:
        return _storage._real < rhs._storage._real;
        break;
      case INT:
        return _storage._int < rhs._storage._int;
        break;
      case BOOLEAN:
        return _storage._bool < rhs._storage._bool;
        break;
      case DATE:
        return _storage._date < rhs._storage._date;
        break;
      case TIME:
        return _storage._time < rhs._storage._time;
        break;
      case TIMESTAMP:
        return _storage._timestamp < rhs._storage._timestamp;
        break;
    }
    throw std::runtime_error("just to make VC2013 happy");
  }

  Variant& Variant::operator+=(const Variant& rhs)
  {
    if (_type != rhs._type) {
      CSVSQLDB_THROW(VariantException,
                     "adding Variants with different types (" << typeToString(_type) << ":" << typeToString(rhs._type));
    }
    if (_type != INT && _type != REAL) {
      CSVSQLDB_THROW(VariantException, "cannot add to non numeric types");
    }
    if (_isNull || rhs._isNull) {
      CSVSQLDB_THROW(VariantException, "cannot add to null");
    }
    if (_type == INT) {
      _storage._int += rhs._storage._int;
    } else if (_type == REAL) {
      _storage._real += rhs._storage._real;
    }
    return *this;
  }

  Variant& Variant::operator+=(int32_t rhs)
  {
    if (_type != INT && _type != REAL) {
      CSVSQLDB_THROW(VariantException, "cannot add to non numeric types");
    }
    if (_isNull) {
      CSVSQLDB_THROW(VariantException, "cannot add to null");
    }
    if (_type == INT) {
      _storage._int += rhs;
    } else if (_type == REAL) {
      _storage._real += rhs;
    }
    return *this;
  }

  Variant& Variant::operator+=(int64_t rhs)
  {
    if (_isNull) {
      CSVSQLDB_THROW(VariantException, "cannot add to null");
    }
    if (_type != INT && _type != REAL) {
      CSVSQLDB_THROW(VariantException, "cannot add to non numeric types");
    }
    if (_type == INT) {
      _storage._int += rhs;
    } else if (_type == REAL) {
      _storage._real += rhs;
    }
    return *this;
  }

  Variant& Variant::operator+=(double rhs)
  {
    if (_isNull) {
      CSVSQLDB_THROW(VariantException, "cannot add to null");
    }
    if (_type != INT && _type != REAL) {
      CSVSQLDB_THROW(VariantException, "cannot add to non numeric types");
    }
    if (_type == INT) {
      _storage._int += static_cast<int64_t>(rhs);
    } else if (_type == REAL) {
      _storage._real += rhs;
    }
    return *this;
  }

  Variant& Variant::operator/=(const Variant& rhs)
  {
    if (_type != INT && _type != REAL) {
      CSVSQLDB_THROW(VariantException, "cannot divide non numeric types");
    }
    if (_isNull || rhs._isNull) {
      CSVSQLDB_THROW(VariantException, "cannot devide with null");
    }
    // TODO LCF: test for devide through 0
    if (_type == INT) {
      if (rhs._type == INT) {
        _storage._int /= rhs._storage._int;
      } else {
        _storage._int /= static_cast<int64_t>(rhs._storage._real);
      }
    } else if (_type == REAL) {
      if (rhs._type == INT) {
        _storage._real /= rhs._storage._int;
      } else {
        _storage._real /= rhs._storage._real;
      }
    }
    return *this;
  }

  eType Variant::getType() const
  {
    return _type;
  }

  bool Variant::isNull() const
  {
    return _isNull;
  }

  int64_t Variant::asInt() const
  {
    if (_isNull) {
      CSVSQLDB_THROW(VariantException, "variant is null");
    }
    if (_type != INT) {
      CSVSQLDB_THROW(VariantException, "expected type " << typeToString(INT));
    }
    return _storage._int;
  }

  double Variant::asDouble() const
  {
    if (_isNull) {
      CSVSQLDB_THROW(VariantException, "variant is null");
    }
    if (_type != REAL) {
      CSVSQLDB_THROW(VariantException, "expected type " << typeToString(REAL));
    }
    return _storage._real;
  }

  csvsqldb::Date Variant::asDate() const
  {
    if (_isNull) {
      CSVSQLDB_THROW(VariantException, "variant is null");
    }
    if (_type != DATE) {
      CSVSQLDB_THROW(VariantException, "expected type " << typeToString(DATE));
    }
    return csvsqldb::Date(_storage._date);
  }

  csvsqldb::Time Variant::asTime() const
  {
    if (_isNull) {
      CSVSQLDB_THROW(VariantException, "variant is null");
    }
    if (_type != TIME) {
      CSVSQLDB_THROW(VariantException, "expected type " << typeToString(TIME));
    }
    return csvsqldb::Time(_storage._time);
  }

  csvsqldb::Timestamp Variant::asTimestamp() const
  {
    if (_isNull) {
      CSVSQLDB_THROW(VariantException, "variant is null");
    }
    if (_type != TIMESTAMP) {
      CSVSQLDB_THROW(VariantException, "expected type " << typeToString(TIMESTAMP));
    }
    return csvsqldb::Timestamp(_storage._timestamp);
  }

  bool Variant::asBool() const
  {
    if (_isNull) {
      CSVSQLDB_THROW(VariantException, "variant is null");
    }

    switch (_type) {
      case BOOLEAN:
        return _storage._bool;
      case INT:
        return _storage._int != 0;
      case REAL:
        return csvsqldb::compare(_storage._real, 0.0);
      case STRING:
        return _storage._string;
      case DATE:
        return !csvsqldb::Date(_storage._date).isInfinite();
      case TIME:
        return _storage._time;
      case TIMESTAMP:
        return _storage._timestamp;
      case NONE:
        CSVSQLDB_THROW(VariantException, "expected type " << typeToString(BOOLEAN));
    }
    return _storage._bool;
  }

  const char* Variant::asString() const
  {
    if (_isNull) {
      CSVSQLDB_THROW(VariantException, "variant is null");
    }
    if (_type != STRING) {
      CSVSQLDB_THROW(VariantException, "expected type " << typeToString(STRING));
    }
    return _storage._string;
  }

  std::string Variant::toString() const
  {
    if (_isNull) {
      return "NULL";
    }
    switch (_type) {
      case NONE:
        return "NONE";
      case INT:
        return std::to_string(_storage._int);
      case STRING:
        return _storage._string;
      case BOOLEAN:
        return std::to_string(_storage._bool);
      case DATE:
        return asDate().format("%Y-%m-%d");
      case TIME:
        return asTime().format("%H:%M:%S");
      case TIMESTAMP:
        return asTimestamp().format("%Y-%m-%dT%H:%M:%S");
      case REAL:
        return std::to_string(_storage._real);
    }
    throw std::runtime_error("just to make VC2013 happy");
  }

  size_t Variant::getHash() const
  {
    if (_isNull) {
      return 0;
    }
    switch (_type) {
      case NONE:
        CSVSQLDB_THROW(VariantException, "cannot hash a variant with no type");
      case INT:
        return std::hash<int64_t>()(_storage._int);
      case REAL:
        return std::hash<double>()(_storage._real);
      case BOOLEAN:
        return std::hash<bool>()(_storage._bool);
      case DATE:
        return std::hash<uint32_t>()(_storage._date);
      case TIME:
        return std::hash<int32_t>()(_storage._time);
      case TIMESTAMP:
        return std::hash<int64_t>()(_storage._timestamp);
      case STRING:
        return std::hash<const char*>()(_storage._string);
    }
    throw std::runtime_error("just to make VC2013 happy");
  }

  Variant typedValueToVariant(const TypedValue& value)
  {
    if (value._value.has_value()) {
      switch (value._type) {
        case BOOLEAN:
          return Variant(std::any_cast<bool>(value._value));
          break;
        case DATE:
          return Variant(std::any_cast<csvsqldb::Date>(value._value));
          break;
        case TIME:
          return Variant(std::any_cast<csvsqldb::Time>(value._value));
          break;
        case TIMESTAMP:
          return Variant(std::any_cast<csvsqldb::Timestamp>(value._value));
          break;
        case INT:
          return Variant(std::any_cast<int64_t>(value._value));
          break;
        case REAL:
          return Variant(std::any_cast<double>(value._value));
          break;
        case STRING:
          return Variant(std::any_cast<std::string>(value._value));
          break;
        case NONE:
          return Variant(NONE);
          break;
      }
    } else {
      return Variant(value._type);
    }
    throw std::runtime_error("just to make VC2013 happy");
  }

  Variant valueToVariant(const Value& value)
  {
    if (!value.isNull()) {
      switch (value.getType()) {
        case BOOLEAN:
          return Variant(static_cast<const ValBool&>(value).asBool());
          break;
        case DATE:
          return Variant(static_cast<const ValDate&>(value).asDate());
          break;
        case TIME:
          return Variant(static_cast<const ValTime&>(value).asTime());
          break;
        case TIMESTAMP:
          return Variant(static_cast<const ValTimestamp&>(value).asTimestamp());
          break;
        case INT:
          return Variant(static_cast<const ValInt&>(value).asInt());
          break;
        case REAL:
          return Variant(static_cast<const ValDouble&>(value).asDouble());
          break;
        case STRING:
          return Variant(static_cast<const ValString&>(value).asString());
          break;
        case NONE:
          return Variant(NONE);
          break;
      }
    } else {
      return Variant(value.getType());
    }
    throw std::runtime_error("just to make VC2013 happy");
  }
}
