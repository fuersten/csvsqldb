//
//  hash_helper.h
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

#include <functional>


namespace std
{
  template<>
  struct hash<const char*> {
    using argument_type = const char*;
    using result_type = std::size_t;

    result_type operator()(argument_type const& val) const
    {
      argument_type str = val;
      result_type hash = 5381;
      int c = 0;

      while ((c = *str++)) {
        hash = ((hash << 5) + hash) ^ static_cast<result_type>(c);
      }

      return hash;
    }
  };
}

namespace csvsqldb
{
  // Extracted from the following paper:
  // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3876.pdf

  template<typename T>
  void hash_combine(std::size_t& seed, const T& val)
  {
    seed ^= std::hash<T>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  template<typename T, typename... Types>
  void hash_combine(std::size_t& seed, const T& val, const Types&... args)
  {
    hash_combine(seed, val);
    hash_combine(seed, args...);
  }

  template<typename... Types>
  std::size_t hash_val(const Types&... args)
  {
    std::size_t seed = 0;
    hash_combine(seed, args...);
    return seed;
  }
}
