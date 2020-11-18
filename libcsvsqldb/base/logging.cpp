//
//  logging.cpp
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

#include "logging.h"

#include "global_configuration.h"
#include "log_devices.h"
#include "string_helper.h"

#include <memory>
#include <regex>
#include <vector>


namespace csvsqldb
{
  typedef std::shared_ptr<LogDevice> LogDevicePtr;

  void LogDevice::log(const LogEvent& event)
  {
    static std::string separator = config<GlobalConfiguration>()->logging.separator;

    std::ostringstream os;
    os << event._time << separator;
    os << event._categorie << separator;
    os << event._tid << separator;
    if (!event._classname.empty()) {
      os << event._classname << separator;
    }
    if (config<GlobalConfiguration>()->logging.escape_newline) {
      std::regex exp("\n|\r");
      os << regex_replace(event._message, exp, std::string("\\n"));
    } else {
      os << event._message;
    }
    if (!event._file.empty()) {
      os << separator << event._file << ":" << event._line;
    }
    os << std::endl;

    doLog(os);
  }

  struct LogDeviceFactory {
    LogDevicePtr create(const std::string& device) const
    {
      if (device == "Console") {
        return std::make_shared<ConsoleLogDevice>();
      } else if (device == "None") {
        return LogDevicePtr();
      } else {
        throw Exception("unknown log device " + device);
      }
    }
  };

  static LogDevicePtr s_logDevice;

  void LogDevice::flush()
  {
    doFlush();
  }

  void Logging::init()
  {
    s_logDevice = LogDeviceFactory().create(config<GlobalConfiguration>()->logging.device);
  }

  void Logging::log(const LogEvent& event)
  {
    static std::mutex s_logMutex;

    if (s_logDevice) {
      std::unique_lock guard(s_logMutex);
      s_logDevice->log(event);
      s_logDevice->flush();
    }
  }
}
