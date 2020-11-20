//
//  variant.h
//  csvsqldb
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

#pragma once

#include "csvsqldb/inc.h"

#include "base/exception.h"
#include "base/hash_helper.h"
#include "types.h"
#include "values.h"

namespace csvsqldb
{
  CSVSQLDB_DECLARE_EXCEPTION(VariantException, csvsqldb::Exception);

  class CSVSQLDB_EXPORT RefCount
  {
  public:
    RefCount()
    : _refCount(1)
    {
    }

    uint16_t inc()
    {
      ++_refCount;
      return _refCount;
    }

    uint16_t dec()
    {
      if (_refCount == 0) {
        CSVSQLDB_THROW(VariantException, "refCount is already 0");
      }
      --_refCount;
      return _refCount;
    }

    uint16_t count() const
    {
      return _refCount;
    }

  private:
    uint16_t _refCount;
  };


  class CSVSQLDB_EXPORT Variant
  {
  public:
    union Storage {
      int64_t _int;
      double _real;
      uint32_t _date;
      int32_t _time;
      int64_t _timestamp;
      bool _bool;
      const char* _string;
    };

    Variant() = default;

    ~Variant();

    Variant(const Variant& rhs);

    explicit Variant(eType type);

    Variant(int32_t integer);

    Variant(int64_t integer);

    Variant(size_t integer);

    Variant(double real);

    Variant(const csvsqldb::Date& date);

    Variant(const csvsqldb::Time& time);

    Variant(const csvsqldb::Timestamp& timestamp);

    Variant(bool boolean);

    Variant(const char* string, bool owner = false);

    Variant(const std::string& s);

    bool isDisconnected() const;

    void disconnect();

    Variant& operator=(const Variant& rhs);

    bool operator==(const Variant& rhs) const;

    bool operator!=(const Variant& rhs) const;

    bool operator<(const Variant& rhs) const;

    Variant& operator+=(const Variant& rhs);

    Variant& operator+=(int32_t rhs);

    Variant& operator+=(int64_t rhs);

    // TODO LCF: not sure if it would be better to actually change the type to
    // REAL if adding a double
    Variant& operator+=(double rhs);

    Variant& operator/=(const Variant& rhs);

    eType getType() const;

    bool isNull() const;

    int64_t asInt() const;

    double asDouble() const;

    csvsqldb::Date asDate() const;

    csvsqldb::Time asTime() const;

    csvsqldb::Timestamp asTimestamp() const;

    bool asBool() const;

    const char* asString() const;

    std::string toString() const;

    size_t getHash() const;

  private:
    Storage _storage;
    RefCount* _refCount{nullptr};
    eType _type{NONE};
    bool _isNull{false};
  } __attribute__((__packed__));


  using Variants = std::vector<Variant>;

  CSVSQLDB_EXPORT Variant typedValueToVariant(const TypedValue& value);
  CSVSQLDB_EXPORT Variant valueToVariant(const Value& value);

  template<typename T>
  struct ValueGetter {
    static T getValue(const Variant& value);
  };

  template<>
  struct ValueGetter<NoneType> {
    static NoneType getValue(const Variant& value)
    {
      CSVSQLDB_THROW(VariantException, "Cannot get a value from a null type");
    }
  };

  template<>
  struct ValueGetter<bool> {
    static bool getValue(const Variant& value)
    {
      return value.asBool();
    }
  };

  template<>
  struct ValueGetter<int64_t> {
    static int64_t getValue(const Variant& value)
    {
      return value.asInt();
    }
  };

  template<>
  struct ValueGetter<double> {
    static double getValue(const Variant& value)
    {
      return value.asDouble();
    }
  };

  template<>
  struct ValueGetter<csvsqldb::Date> {
    static csvsqldb::Date getValue(const Variant& value)
    {
      return value.asDate();
    }
  };

  template<>
  struct ValueGetter<csvsqldb::Time> {
    static csvsqldb::Time getValue(const Variant& value)
    {
      return value.asTime();
    }
  };

  template<>
  struct ValueGetter<csvsqldb::Timestamp> {
    static csvsqldb::Timestamp getValue(const Variant& value)
    {
      return value.asTimestamp();
    }
  };

  template<>
  struct ValueGetter<StringType> {
    static StringType getValue(const Variant& value)
    {
      return const_cast<StringType>(value.asString());
    }
  };

  template<>
  struct ValueGetter<std::string> {
    static const char* getValue(const Variant& value)
    {
      return value.asString();
    }
  };
}

namespace std
{
  template<>
  struct hash<csvsqldb::Variant> {
    typedef csvsqldb::Variant argument_type;
    typedef std::size_t result_type;

    result_type operator()(argument_type const& val) const
    {
      return val.getHash();
    }
  };
}
