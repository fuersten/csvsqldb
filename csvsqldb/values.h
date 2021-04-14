//
//  values.h
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

#include <csvsqldb/inc.h>

#include <csvsqldb/base/float_helper.h>
#include <csvsqldb/base/hash_helper.h>
#include <csvsqldb/types.h>

#include <cstring>
#include <iomanip>


namespace csvsqldb
{
  class CSVSQLDB_EXPORT Value
  {
  public:
    Value() = default;

    virtual ~Value() = default;

    Value(const Value&) = default;
    Value& operator=(const Value&) = default;
    Value(Value&&) = default;
    Value& operator=(Value&&) = default;

    virtual void toStream(std::ostream& stream) const = 0;

    virtual std::string toString() const = 0;

    virtual bool isNull() const = 0;

    virtual size_t getSize() const = 0;

    virtual eType getType() const = 0;

    virtual bool operator<(const Value& rhs) const = 0;

    virtual bool operator==(const Value& rhs) const = 0;

    virtual size_t getHash() const = 0;

  protected:
    inline static std::string sNull{"NULL"};
  };


#pragma pack(push, 1)
  class CSVSQLDB_EXPORT ValInt : public Value
  {
  public:
    ValInt()
    : _val(std::numeric_limits<int64_t>::max())
    , _isNull(true)
    {
    }

    explicit ValInt(int64_t val)
    : _val(val)
    , _isNull(false)
    {
    }

    bool isNull() const override
    {
      return _isNull;
    }

    int64_t asInt() const
    {
      return _val;
    }

    void toStream(std::ostream& stream) const override
    {
      if (_isNull) {
        stream << sNull;
      } else {
        stream << _val;
      }
    }

    std::string toString() const override
    {
      if (_isNull) {
        return sNull;
      }

      return std::to_string(_val);
    }

    static size_t getBaseSize()
    {
      return sizeof(ValInt);
    }

    size_t getSize() const override
    {
      return sizeof(ValInt);
    }

    eType getType() const override
    {
      return INT;
    }

    bool operator<(const Value& rhs) const override
    {
      if (isNull() || rhs.isNull()) {
        return false;
      }
      return _val < dynamic_cast<const ValInt&>(rhs)._val;
    }

    bool operator==(const Value& rhs) const override
    {
      if (isNull() || rhs.isNull()) {
        return false;
      }
      return _val == dynamic_cast<const ValInt&>(rhs)._val;
    }

    size_t getHash() const override
    {
      return std::hash<int64_t>()(_val);
    }

  private:
    int64_t _val;
    bool _isNull;
  };
#pragma pack(pop)


#pragma pack(push, 1)
  class CSVSQLDB_EXPORT ValDouble : public Value
  {
  public:
    ValDouble()
    : _val(std::numeric_limits<double>::max())
    , _isNull(true)
    {
    }

    explicit ValDouble(double val)
    : _val(val)
    , _isNull(false)
    {
    }

    ValDouble(const ValDouble& rhs)
    : _val(rhs._val)
    , _isNull(rhs._isNull)
    {
    }

    bool isNull() const override
    {
      return _isNull;
    }

    double asDouble() const
    {
      return _val;
    }

    void toStream(std::ostream& stream) const override
    {
      if (_isNull) {
        stream << sNull;
      } else {
        stream << std::fixed << std::showpoint << std::setprecision(6) << _val;
      }
    }

    std::string toString() const override
    {
      if (_isNull) {
        return sNull;
      }

      return std::to_string(_val);
    }

    static size_t getBaseSize()
    {
      return sizeof(ValDouble);
    }

    size_t getSize() const override
    {
      return sizeof(ValDouble);
    }

    eType getType() const override
    {
      return REAL;
    }

    bool operator<(const Value& rhs) const override
    {
      if (isNull() || rhs.isNull()) {
        return false;
      }
      return _val < dynamic_cast<const ValDouble&>(rhs)._val;
    }

    bool operator==(const Value& rhs) const override
    {
      if (isNull() || rhs.isNull()) {
        return false;
      }
      return csvsqldb::Approx(_val) == dynamic_cast<const ValDouble&>(rhs)._val;
    }

    size_t getHash() const override
    {
      return std::hash<double>()(_val);
    }

  private:
    double _val;
    bool _isNull;
  };
#pragma pack(pop)


#pragma pack(push, 1)
  class CSVSQLDB_EXPORT ValBool : public Value
  {
  public:
    ValBool()
    : _val(false)
    , _isNull(true)
    {
    }

    explicit ValBool(bool val)
    : _val(val)
    , _isNull(false)
    {
    }

    ValBool(const ValBool& rhs)
    : _val(rhs._val)
    , _isNull(rhs._isNull)
    {
    }

    bool isNull() const override
    {
      return _isNull;
    }

    bool asBool() const
    {
      return _val;
    }

    void toStream(std::ostream& stream) const override
    {
      if (_isNull) {
        stream << sNull;
      } else {
        stream << _val;
      }
    }

    std::string toString() const override
    {
      if (_isNull) {
        return sNull;
      }

      return std::to_string(_val);
    }

    static size_t getBaseSize()
    {
      return sizeof(ValBool);
    }

    size_t getSize() const override
    {
      return sizeof(ValBool);
    }

    eType getType() const override
    {
      return BOOLEAN;
    }

    bool operator<(const Value& rhs) const override
    {
      if (isNull() || rhs.isNull()) {
        return false;
      }
      return _val < dynamic_cast<const ValBool&>(rhs)._val;
    }

    bool operator==(const Value& rhs) const override
    {
      if (isNull() || rhs.isNull()) {
        return false;
      }
      return _val == dynamic_cast<const ValBool&>(rhs)._val;
    }

    size_t getHash() const override
    {
      return std::hash<bool>()(_val);
    }

  private:
    bool _val;
    bool _isNull;
  };
#pragma pack(pop)


#pragma pack(push, 1)
  class CSVSQLDB_EXPORT ValDate : public Value
  {
  public:
    ValDate()
    {
    }

    explicit ValDate(const csvsqldb::Date& val)
    : _val(val)
    {
    }

    bool isNull() const override
    {
      return _val.isInfinite();
    }

    const csvsqldb::Date& asDate() const
    {
      return _val;
    }

    void toStream(std::ostream& stream) const override
    {
      stream << toString();
    }

    std::string toString() const override
    {
      if (isNull()) {
        return sNull;
      }

      return _val.format("%F");
    }

    static size_t getBaseSize()
    {
      return sizeof(ValDate);
    }

    size_t getSize() const override
    {
      return sizeof(ValDate);
    }

    eType getType() const override
    {
      return DATE;
    }

    bool operator<(const Value& rhs) const override
    {
      if (isNull() || rhs.isNull()) {
        return false;
      }
      return _val < dynamic_cast<const ValDate&>(rhs)._val;
    }

    bool operator==(const Value& rhs) const override
    {
      if (isNull() || rhs.isNull()) {
        return false;
      }
      return _val == dynamic_cast<const ValDate&>(rhs)._val;
    }

    size_t getHash() const override
    {
      return std::hash<int64_t>()(_val.asJulianDay());
    }

  private:
    csvsqldb::Date _val;
  };
#pragma pack(pop)


#pragma pack(push, 1)
  class CSVSQLDB_EXPORT ValTime : public Value
  {
  public:
    ValTime()
    : _isNull(true)
    {
    }

    explicit ValTime(const csvsqldb::Time& val)
    : _val(val)
    , _isNull(false)
    {
    }

    bool isNull() const override
    {
      return _isNull;
    }

    const csvsqldb::Time& asTime() const
    {
      return _val;
    }

    void toStream(std::ostream& stream) const override
    {
      stream << toString();
    }

    std::string toString() const override
    {
      if (_isNull) {
        return sNull;
      }

      return _val.format("%H:%M:%S");
    }

    static size_t getBaseSize()
    {
      return sizeof(ValTime);
    }

    size_t getSize() const override
    {
      return sizeof(ValTime);
    }

    eType getType() const override
    {
      return TIME;
    }

    bool operator<(const Value& rhs) const override
    {
      if (isNull() || rhs.isNull()) {
        return false;
      }
      return _val < dynamic_cast<const ValTime&>(rhs)._val;
    }

    bool operator==(const Value& rhs) const override
    {
      if (isNull() || rhs.isNull()) {
        return false;
      }
      return _val == dynamic_cast<const ValTime&>(rhs)._val;
    }

    size_t getHash() const override
    {
      return std::hash<int32_t>()(_val.asInteger());
    }

  private:
    csvsqldb::Time _val;
    bool _isNull;
  };
#pragma pack(pop)


#pragma pack(push, 1)
  class CSVSQLDB_EXPORT ValTimestamp : public Value
  {
  public:
    ValTimestamp()
    : _isNull(true)
    {
    }

    explicit ValTimestamp(const csvsqldb::Timestamp& val)
    : _val(val)
    , _isNull(false)
    {
    }

    bool isNull() const override
    {
      return _isNull;
    }

    const csvsqldb::Timestamp& asTimestamp() const
    {
      return _val;
    }

    void toStream(std::ostream& stream) const override
    {
      stream << toString();
    }

    std::string toString() const override
    {
      if (_isNull) {
        return sNull;
      }

      return _val.format("%Y-%m-%dT%H:%M:%S");
    }

    static size_t getBaseSize()
    {
      return sizeof(ValTimestamp);
    }

    size_t getSize() const override
    {
      return sizeof(ValTimestamp);
    }

    eType getType() const override
    {
      return TIMESTAMP;
    }

    bool operator<(const Value& rhs) const override
    {
      if (isNull() || rhs.isNull()) {
        return false;
      }
      return _val < dynamic_cast<const ValTimestamp&>(rhs)._val;
    }

    bool operator==(const Value& rhs) const override
    {
      if (isNull() || rhs.isNull()) {
        return false;
      }
      return _val == dynamic_cast<const ValTimestamp&>(rhs)._val;
    }

    size_t getHash() const override
    {
      return std::hash<int64_t>()(_val.asInteger());
    }

  private:
    csvsqldb::Timestamp _val;
    bool _isNull;
  };
#pragma pack(pop)


#pragma pack(push, 1)
  class CSVSQLDB_EXPORT ValString : public Value
  {
  public:
    ValString() = default;

    explicit ValString(const char* val)
    : _length(val ? ::strlen(val) : 0)
    , _val(val)
    {
    }

    bool isNull() const override
    {
      return _val == nullptr;
    }

    const char* asString() const
    {
      return _val;
    }

    void toStream(std::ostream& stream) const override
    {
      if (!_val) {
        stream << sNull;
      } else {
        stream << _val;
      }
    }

    std::string toString() const override
    {
      if (!_val) {
        return sNull;
      }

      return _val;
    }

    static size_t getBaseSize()
    {
      return sizeof(ValString);
    }

    size_t getSize() const override
    {
      return sizeof(ValString) + _length + 1;
    }

    eType getType() const override
    {
      return STRING;
    }

    size_t getLength() const
    {
      return _length;
    }

    bool operator<(const Value& rhs) const override
    {
      if (isNull() || rhs.isNull()) {
        return false;
      }
      return ::strcoll(_val, dynamic_cast<const ValString&>(rhs)._val) < 0;
    }

    bool operator==(const Value& rhs) const override
    {
      if (isNull() || rhs.isNull()) {
        return false;
      }
      return ::strcoll(_val, dynamic_cast<const ValString&>(rhs)._val) == 0;
    }

    size_t getHash() const override
    {
      if (!_val) {
        return 0;
      }
      return std::hash<const char*>()(_val);
    }

  private:
    size_t _length{0};
    const char* _val{nullptr};
  };
#pragma pack(pop)


  CSVSQLDB_EXPORT Value* createValue(eType type, const std::any& value);

  typedef std::vector<const Value*> Values;

  template<typename T>
  struct ValueCreator {
    static Value* createValue(const T& val);
  };

  template<>
  struct ValueCreator<bool> {
    static ValBool* createValue(const bool& val)
    {
      return new ValBool(val);
    }
  };

  template<>
  struct ValueCreator<int64_t> {
    static ValInt* createValue(const int64_t& val)
    {
      return new ValInt(val);
    }
  };

  template<>
  struct ValueCreator<double> {
    static ValDouble* createValue(const double& val)
    {
      return new ValDouble(val);
    }
  };

  template<>
  struct ValueCreator<csvsqldb::Date> {
    static ValDate* createValue(const csvsqldb::Date& val)
    {
      return new ValDate(val);
    }
  };

  template<>
  struct ValueCreator<csvsqldb::Time> {
    static ValTime* createValue(const csvsqldb::Time& val)
    {
      return new ValTime(val);
    }
  };

  template<>
  struct ValueCreator<csvsqldb::Timestamp> {
    static ValTimestamp* createValue(const csvsqldb::Timestamp& val)
    {
      return new ValTimestamp(val);
    }
  };

  template<>
  struct ValueCreator<std::string> {
    static ValString* createValue(const std::string& val)
    {
      char* c = new char[val.length() + 1];
      std::size_t length = val.copy(&c[0], val.length());
      c[length] = '\0';
      return new ValString(c);
    }
  };

  template<>
  struct ValueCreator<StringType> {
    static ValString* createValue(const StringType& val)
    {
      return new ValString(val);
    }
  };
}

namespace std
{
  template<>
  struct hash<csvsqldb::Value> {
    typedef csvsqldb::Value argument_type;
    typedef std::size_t result_type;

    result_type operator()(argument_type const& val) const
    {
      return val.getHash();
    }
  };
}
