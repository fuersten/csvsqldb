//
//  string_helper.cpp
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

#include "string_helper.h"

#include <algorithm>
#include <sstream>


namespace csvsqldb
{
    
    std::string& toupper(std::string& s)
    {
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        return s;
    }

    std::string toupper_copy(const std::string& s)
    {
        std::string str(s);
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        return str;
    }

    std::string& tolower(std::string& s)
    {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }
    
    std::string tolower_copy(const std::string& s)
    {
        std::string str(s);
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }
    
    int stricmp(const char* str1, const char* str2)
    {
#if defined _MSC_VER
        return ::stricmp( str1, str2 );
#else
        return ::strcasecmp( str1, str2 );
#endif
    }
    
    int strnicmp(const char* str1, const char* str2, size_t count)
    {
#if defined _MSC_VER
        return ::strnicmp( str1, str2, count );
#else
        return ::strncasecmp( str1, str2, count );
#endif
    }

    std::string trim_right(const std::string& s) {
        size_t endpos = s.find_last_not_of(" \t");
        if( std::string::npos != endpos )
        {
            return s.substr( 0, endpos+1 );
        }
        return s;
    }
    
    std::string trim_left(const std::string& s) {
        size_t startpos = s.find_first_not_of(" \t");
        if( std::string::npos != startpos )
        {
            return s.substr( startpos );
        }
        return s;
    }

    bool decode(const std::string& in, std::string& out)
    {
        out.clear();
        out.reserve(in.size());
        for(std::size_t i = 0; i < in.size(); ++i)
        {
            if (in[i] == '%')
            {
                if (i + 3 <= in.size())
                {
                    int value = 0;
                    std::istringstream is(in.substr(i + 1, 2));
                    if (is >> std::hex >> value)
                    {
                        out += static_cast<char>(value);
                        i += 2;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            else if (in[i] == '+')
            {
                out += ' ';
            }
            else
            {
                out += in[i];
            }
        }
        return true;
    }

}
