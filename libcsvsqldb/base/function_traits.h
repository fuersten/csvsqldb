//
//  function_traits.h
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

#ifndef csvsqldb_function_traits_h
#define csvsqldb_function_traits_h

#include <type_traits>

#include <utility>


namespace csvsqldb
{
  /**
   * With the function_traits template, a C++ function can be decomposed into its components. The components consist of the
   * return type,
   * the number of arguments and the type of the arguments.
   */
  template<class F>
  struct function_traits;

  /**
   * With the function_traits template, a C++ function can be decomposed into its components. The components consist of the
   * return type,
   * the number of arguments and the type of the arguments.
   */
  template<class R, class... Args>
  struct function_traits<R (*)(Args...)> : public function_traits<R(Args...)> {
  };

  /**
   * With the function_traits template, a C++ function can be decomposed into its components. The components consist of the
   * return type,
   * the number of arguments and the type of the arguments.
   */
  template<class R, class... Args>
  struct function_traits<R(Args...)> {
    using return_type = R;  //!< The return type of the function
#if defined _MSC_VER
    static const std::size_t arity = sizeof...(Args);  //!< The number of arguments of the function
#else
    static constexpr std::size_t arity = sizeof...(Args);  //!< The number of arguments of the function
#endif

    /**
     * Inner helper template for the extraction of argument types of functions.
     *
     */
    template<std::size_t N>
    struct argument {
      static_assert(N < arity, "error: invalid parameter index.");
      using type = typename std::tuple_element<N, std::tuple<Args...>>::type;  //!< The type of the Nth argument of the function
    };
  };
}

#endif
