//
//  exception.cpp
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

#include "exception.h"

#include "logging.h"

#include <cstring>

#if defined _MSC_VER
  #include <errno.h>
#else
  #include <sys/errno.h>
#endif


namespace csvsqldb
{
  Exception::Exception(std::error_code ec, const std::string& message) noexcept
  : std::system_error(ec, message)
  {
  }

  Exception::Exception(int ev, const std::string& message) noexcept
  : std::system_error(ev, std::generic_category(), message)
  {
  }

  Exception::Exception(std::errc ec, const std::string& message) noexcept
  : std::system_error(std::make_error_code(ec), message)
  {
  }

  Exception::Exception(const std::string& message) noexcept
  : std::system_error(std::error_code(0, std::generic_category()))
  , _message(message)
  {
  }

  Exception::Exception(const Exception& ex) noexcept
  : std::system_error(ex)
  , _message(ex._message)
  {
  }

  const char* Exception::what() const noexcept
  {
    if (code().value() == 0) {
      // a code value of 0 is seen as no code from csvsqldb
      // some systems see a code value of 0 as a success and will write that to the message, which is not what I want
      return _message.c_str();
    } else {
      return std::system_error::what();
    }
  }

  CSVSQLDB_IMPLEMENT_EXCEPTION(ConfigurationException, Exception);
  CSVSQLDB_IMPLEMENT_EXCEPTION(FilesystemException, Exception);
  CSVSQLDB_IMPLEMENT_EXCEPTION(JsonException, Exception);
  CSVSQLDB_IMPLEMENT_EXCEPTION(BadoptionException, Exception);
  CSVSQLDB_IMPLEMENT_EXCEPTION(IndexException, Exception);
  CSVSQLDB_IMPLEMENT_EXCEPTION(InvalidOperationException, Exception);
  CSVSQLDB_IMPLEMENT_EXCEPTION(LexicalAnalysisException, Exception);
  CSVSQLDB_IMPLEMENT_EXCEPTION(TimeException, Exception);
  CSVSQLDB_IMPLEMENT_EXCEPTION(TimestampException, Exception);
  CSVSQLDB_IMPLEMENT_EXCEPTION(DateException, Exception);
  CSVSQLDB_IMPLEMENT_EXCEPTION(DurationException, Exception);
  CSVSQLDB_IMPLEMENT_EXCEPTION(NoMoreInputException, Exception);
  CSVSQLDB_IMPLEMENT_EXCEPTION(ChronoException, Exception);
  CSVSQLDB_IMPLEMENT_EXCEPTION(RegExpException, Exception);


  std::string errnoText()
  {
#if defined __linux__
    char buf[256];
    char* s = strerror_r(errno, buf, sizeof(buf));
    return s ? s : buf;
#elif defined _MSC_VER
    std::system_error syserr(errno, std::generic_category(), "");
    return syserr.what();
#else
    char buf[256];
    strerror_r(errno, buf, sizeof(buf));
    return buf;
#endif
  }

  void throwSysError(const std::string& error_domain)
  {
    throw Exception(errno, error_domain);
  }

  void evaluateException()
  {
    try {
      throw;
    } catch (Exception& ex) {
      CSVSQLDB_ERRORLOG("csvsqldb exception caught: [" << ex.code().value() << "] " << ex.what());
    } catch (std::system_error& ex) {
      CSVSQLDB_ERRORLOG("std::system error caught: [" << ex.code().value() << "] " << ex.what());
    } catch (std::exception& ex) {
      CSVSQLDB_ERRORLOG("std::exception caught: " << ex.what());
    } catch (...) {
      CSVSQLDB_ERRORLOG("unknown exception caught");
    }
  }

  void evaluateExceptionAndThrow()
  {
    evaluateException();
    throw;
  }
}
