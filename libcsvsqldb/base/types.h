//
//  types.h
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

#ifndef csvsqldb_base_types_h
#define csvsqldb_base_types_h

#include "libcsvsqldb/inc.h"

#include "detail/types_detail.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <vector>


namespace csvsqldb
{
  /**
   * Timestamp
   */
  typedef std::chrono::system_clock::time_point Timepoint;

  typedef std::vector<std::string> StringVector;
  typedef std::set<std::string> StringSet;
  typedef std::vector<size_t> IndexVector;

  /**
   * Converts the given integer into a string representation. The string will be null terminated.
   * @param i The integer to convert
   * @param buffer The buffer to convert the integer into. It has to be big enough (including the terminating null).
   * @return A pointer to the start of the buffer
   */
  char* itoa(int i, char* buffer);

  /**
   * Converts the given string into a boolean value. The boolean will be true if the string is 'true' or not equal to 0. The
   * string will be converted
   * to upper case for the test, so 'TRUE' will also yield true.
   * @param buffer The string to convert
   * @return true if the string is 'true' or not equal to 0, otherwise false
   */
  bool stringToBool(const std::string& buffer);

  /**
   * Templating helper for the removal of elements in a vector.
   */
  template<typename T>
  inline void remove(std::vector<T>& v, const T& item)
  {
    v.erase(std::remove(v.begin(), v.end(), item), v.end());
  }

  /**
   * Returns a cleaned up platform independent name for an RTTI type name.
   * @param classname An RTTI type name
   * @return The cleaned type name.
   */
  CSVSQLDB_EXPORT std::string stripTypeName(const std::string& classname);

  /**
   * Templating helper to convert an integer into a type.
   */
  template<int v>
  struct int2type {
    enum { value = v };
  };

  /**
   * Templating helper to enforce a specific type for an argument.
   */
  template<typename T>
  struct Typer {
    typedef T type;
  };

  /**
   * Returns the name of a type. The default is to get the RTTI name.
   * @return The RTTI name of the type.
   */
  template<typename T>
  inline std::string getTypename()
  {
    return stripTypeName(typeid(T).name());
  }

  /**
   * Returns the name of the *int* type.
   * @return The string 'int'.
   */
  template<>
  inline std::string getTypename<int>()
  {
    return "int";
  }

  /**
   * Returns the name of the *int* type.
   * @return The string 'int'.
   */
  template<>
  inline std::string getTypename<int64_t>()
  {
    return "int64";
  }

  /**
   * Returns the name of the *double* type.
   * @return The string 'double'.
   */
  template<>
  inline std::string getTypename<double>()
  {
    return "double";
  }

  /**
   * Returns the name of the *float* type.
   * @return The string 'float'.
   */
  template<>
  inline std::string getTypename<float>()
  {
    return "float";
  }

  /**
   * Returns the name of the *bool* type.
   * @return The string 'bool'.
   */
  template<>
  inline std::string getTypename<bool>()
  {
    return "bool";
  }

  /**
   * Returns the name of the *char* type.
   * @return The string 'char'.
   */
  template<>
  inline std::string getTypename<char>()
  {
    return "char";
  }

  /**
   * Returns the name of the *std::string* type.
   * @return The string 'std::string'.
   */
  template<>
  inline std::string getTypename<const std::string&>()
  {
    return "std::string";
  }

  /**
   * Returns 1 if the type is non-void.
   * @return 1
   */
  template<typename T>
  inline int type2return()
  {
    return 1;
  }

  /**
   * Returns 0 if the type is void.
   * @return 0
   */
  template<>
  inline int type2return<void>()
  {
    return 0;
  }

  /**
   * Derives, if the given type is output streamable to a std::ostream. Used for template meta programming. Provides a static
   * bool value,
   * which will be true, if the type is streamable and false if not.
   */
  template<class T>
  struct is_output_streamable : detail::is_output_streamable_impl<T> {
  };
}

#endif
