//
//  values.cpp
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

#include "values.h"


namespace csvsqldb
{
  Value* createValue(eType type, const std::any& value)
  {
    switch (type) {
      case NONE:
        throw std::runtime_error("none not allowed as type here");
      case STRING:
        if (value.has_value()) {
          return ValueCreator<std::string>::createValue(std::any_cast<std::string>(value));
        } else {
          return new ValString;
        }
      case BOOLEAN:
        if (value.has_value()) {
          return new ValBool(std::any_cast<bool>(value));
        } else {
          return new ValBool;
        }
      case DATE:
        if (value.has_value()) {
          return new ValDate(std::any_cast<csvsqldb::Date>(value));
        } else {
          return new ValDate;
        }
      case TIME:
        if (value.has_value()) {
          return new ValTime(std::any_cast<csvsqldb::Time>(value));
        } else {
          return new ValTime;
        }
      case TIMESTAMP:
        if (value.has_value()) {
          return new ValTimestamp(std::any_cast<csvsqldb::Timestamp>(value));
        } else {
          return new ValTimestamp;
        }
      case INT:
        if (value.has_value()) {
          return new ValInt(std::any_cast<int64_t>(value));
        } else {
          return new ValInt;
        }
      case REAL:
        if (value.has_value()) {
          return new ValDouble(std::any_cast<double>(value));
        } else {
          return new ValDouble;
        }
    }
    throw std::runtime_error("just to make VC2013 happy");
  }
}
