//
//  exception.h
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

#include <system_error>

#include <sstream>
#include <string>


namespace csvsqldb
{
  /**
   * Base class for all csvsqldb exceptions.
   */
  class CSVSQLDB_EXPORT Exception : public std::system_error
  {
  public:
    /**
     * Constructor.
     * @param ec The error code to set
     * @param message The message that shall be tied to this execption.
     */
    explicit Exception(std::error_code ec, const std::string& message) noexcept;

    /**
     * Constructor.
     * @param ev The error value to set
     * @param message The message that shall be tied to this execption.
     */
    explicit Exception(int ev, const std::string& message) noexcept;

    /**
     * Constructor.
     * @param ec The error condition to set
     * @param message The message that shall be tied to this execption.
     */
    explicit Exception(std::errc ec, const std::string& message) noexcept;

    /**
     * Constructor.
     * @param message The message that shall be tied to this execption.
     */
    explicit Exception(const std::string& message) noexcept;

    Exception(const Exception& ex) noexcept;

    /**
     * Returns the explanatory string.
     */
    const char* what() const noexcept override;

  private:
    std::string _message;
  };


/**
 * Macro for exception class definitions with a specific base class.
 */
#define CSVSQLDB_DECLARE_EXCEPTION(ex, base)                                                                                     \
  class CSVSQLDB_EXPORT ex : public base                                                                                         \
  {                                                                                                                              \
  public:                                                                                                                        \
    explicit ex(std::error_code ec, const std::string& message) noexcept;                                                        \
    explicit ex(int ev, const std::string& message) noexcept;                                                                    \
    explicit ex(std::errc ec, const std::string& message) noexcept;                                                              \
    explicit ex(const std::string& message) noexcept;                                                                            \
    ex(const base& ex) noexcept;                                                                                                 \
  }

/**
 * Macro for exception class implementations with a specific base class.
 */
#define CSVSQLDB_IMPLEMENT_EXCEPTION(ex, base)                                                                                   \
  ex::ex(std::error_code ec, const std::string& message) noexcept                                                                \
  : base(ec, message)                                                                                                            \
  {                                                                                                                              \
  }                                                                                                                              \
  ex::ex(int ev, const std::string& message) noexcept                                                                            \
  : base(ev, message)                                                                                                            \
  {                                                                                                                              \
  }                                                                                                                              \
  ex::ex(std::errc ec, const std::string& message) noexcept                                                                      \
  : base(ec, message)                                                                                                            \
  {                                                                                                                              \
  }                                                                                                                              \
  ex::ex(const std::string& message) noexcept                                                                                    \
  : base(message)                                                                                                                \
  {                                                                                                                              \
  }                                                                                                                              \
  ex::ex(const base& ex) noexcept                                                                                                \
  : base(ex)                                                                                                                     \
  {                                                                                                                              \
  }

  /**
   * Exception object for configuration exceptions.
   */
  CSVSQLDB_DECLARE_EXCEPTION(ConfigurationException, Exception);

  /**
   * Exception object for filesystem exceptions.
   */
  CSVSQLDB_DECLARE_EXCEPTION(FilesystemException, Exception);

  /**
   * Exception object for json exceptions.
   */
  CSVSQLDB_DECLARE_EXCEPTION(JsonException, Exception);

  /**
   * Exception object for bad options.
   */
  CSVSQLDB_DECLARE_EXCEPTION(BadoptionException, Exception);

  /**
   * Exception object for index errors.
   */
  CSVSQLDB_DECLARE_EXCEPTION(IndexException, Exception);

  /**
   * Exception object for invalid operations.
   */
  CSVSQLDB_DECLARE_EXCEPTION(InvalidOperationException, Exception);

  /**
   * Exception object for lexical analysis errors.
   */
  CSVSQLDB_DECLARE_EXCEPTION(LexicalAnalysisException, Exception);

  /**
   * Exception object for date errors.
   */
  CSVSQLDB_DECLARE_EXCEPTION(DateException, Exception);

  /**
   * Exception object for time errors.
   */
  CSVSQLDB_DECLARE_EXCEPTION(TimeException, Exception);

  /**
   * Exception object for timestamp errors.
   */
  CSVSQLDB_DECLARE_EXCEPTION(TimestampException, Exception);

  /**
   * Exception object for duration errors.
   */
  CSVSQLDB_DECLARE_EXCEPTION(DurationException, Exception);

  /**
   * Exception object for input errors.
   */
  CSVSQLDB_DECLARE_EXCEPTION(NoMoreInputException, Exception);

  /**
   * Exception object for chrono errors.
   */
  CSVSQLDB_DECLARE_EXCEPTION(ChronoException, Exception);

  /**
   * Exception object for regex errors.
   */
  CSVSQLDB_DECLARE_EXCEPTION(RegExpException, Exception);

  /**
   * Returns the text representation of the current errno value.
   * @return The text representation of the current errno value.
   */
  CSVSQLDB_EXPORT std::string errnoText();

  /**
   * Throws an Exception based on the current errno value.
   * @param error_domain This string is prependet to the errno strerror_r error message.
   */
  CSVSQLDB_EXPORT void throwSysError(const std::string& error_domain);

  /**
   * Internally rethrows an exception, catches it and outputs a message. It catches in the order:
   * 1. Exception
   * 2. std::exception
   * 3. ...
   */
  CSVSQLDB_EXPORT void evaluateException();

  /**
   * As evaluateException(), but rethrows the exception after outputting a message.
   */
  CSVSQLDB_EXPORT void evaluateExceptionAndThrow();

#define CSVSQLDB_THROW(ex, arg)                                                                                                  \
  do {                                                                                                                           \
    std::ostringstream XX_ss__;                                                                                                  \
    XX_ss__ << arg;                                                                                                              \
    throw ex(XX_ss__.str());                                                                                                     \
  } while (0);
}
