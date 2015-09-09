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
    
    Value* createValue(eType type, const csvsqldb::Any& value)
    {
        switch(type) {
            case NONE:
                throw std::runtime_error("none not allowed as type here");
            case STRING:
                if(value.empty()) {
                    return new ValString;
                } else {
                    return ValueCreator<std::string>::createValue(csvsqldb::any_cast<std::string>(value));
                }
            case BOOLEAN:
                if(value.empty()) {
                    return new ValBool;
                } else {
                    return new ValBool(csvsqldb::any_cast<bool>(value));
                }
            case DATE:
                if(value.empty()) {
                    return new ValDate;
                } else {
                    return new ValDate(csvsqldb::any_cast<csvsqldb::Date>(value));
                }
            case TIME:
                if(value.empty()) {
                    return new ValTime;
                } else {
                    return new ValTime(csvsqldb::any_cast<csvsqldb::Time>(value));
                }
            case TIMESTAMP:
                if(value.empty()) {
                    return new ValTimestamp;
                } else {
                    return new ValTimestamp(csvsqldb::any_cast<csvsqldb::Timestamp>(value));
                }
            case INT:
                if(value.empty()) {
                    return new ValInt;
                } else {
                    return new ValInt(csvsqldb::any_cast<int64_t>(value));
                }
            case REAL:
                if(value.empty()) {
                    return new ValDouble;
                } else {
                    return new ValDouble(csvsqldb::any_cast<double>(value));
                }
        }
        throw std::runtime_error("just to make VC2013 happy");
    }
    
}
