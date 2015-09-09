//
//  types.cpp
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

#include "types.h"
#include "string_helper.h"

#if defined(__GNUC__)
#include <cxxabi.h>
#include <stdlib.h>
#include <strings.h>
#endif


namespace csvsqldb
{
    char* itoa(int i, char* buffer)
    {
        if(i == 0) {
            buffer[0] = '0';
            buffer[1] = '\0';
            return &buffer[0];
        }
        
        size_t n = 0;
        if(i < 0) {
            buffer[n] = '-';
            i *= -1;
        }
        
        int tmp = i / 10;
        int factor = 1;
        while(tmp) {
            factor = factor * 10;
            tmp = tmp / 10;
        }
        tmp = i;
        while(factor) {
            int part = tmp / factor;
            buffer[n++] = static_cast<char>(part + 48);
            tmp = tmp - (part * factor);
            factor = factor / 10;
        }
        buffer[n] = '\0';
        return &buffer[0];
    }
    
    bool stringToBool(const std::string& value)
    {
        if(toupper_copy(value) == "TRUE") {
            return true;
        } else if(toupper_copy(value) == "FALSE") {
            return false;
        } else if(std::stoi(value) != 0) {
            return true;
        }
        return false;
    }
    
#if defined(__GNUC__)
    
    static std::string demangle(const std::string&  s)
    {
        int status(0);
        std::string result;
        
        char* ss = abi::__cxa_demangle(s.c_str(), 0, 0, &status);
        
        if(status == -1)
        {
            throw std::bad_alloc();
        }
        else if(status == -2)
        {
            result = s;
        }
        else if(status == -3)
        {
            throw std::runtime_error("__cxa_demangle returned -3");
        }
        else
        {
            result = ss;
        }
        
        if(ss)
        {
            ::free(ss);
        }
        
        if(result[result.length()-1] == '*')
        {
            result.erase(result.length()-1);
        }
        std::string::size_type pos = 0;
        while((pos = result.find(", ", pos)) != std::string::npos)
        {
            result.erase(pos+1, 1);
        }
        
        return result;
    }
    
#endif
    
    std::string stripTypeName(const std::string& classname)
    {
#if defined _MSC_VER
        std::string::size_type pos1 = classname.find_first_of(" ");
        std::string::size_type pos3 = classname.find_last_of(">");
        if(pos3 != std::string::npos)
        {
            return classname.substr(pos1+1, (pos3 - pos1));
        }
        std::string::size_type pos2 = classname.find_last_of(" ");
        return classname.substr(pos1+1, (pos2 - pos1)-1);
#else
        return demangle(classname);
#endif
    }
}
