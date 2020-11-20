//
//  logging.h
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

#include "global_configuration.h"
#include "types.h"

#include <mutex>
#include <sstream>
#include <string>
#include <thread>


namespace csvsqldb
{
  /**
   * Specifies a event to log.
   */
  struct CSVSQLDB_EXPORT LogEvent {
    std::chrono::system_clock::time_point _time;  //!< time point of log event
    std::string _classname;                       //!< class that triggered the event
    std::string _categorie;                       //!< categorie of the event
    long _line;                                   //!< the line in the file where the event was triggered
    std::string _file;                            //!< the file where the event was triggered
    std::string _message;                         //!< the message of the event
    std::thread::id _tid;                         //!< the thread id of the triggering thread
  };

  /**
   * Base class for log devices. A log device does the actual logging of event information to some medium (e.g. a file).
   */
  class CSVSQLDB_EXPORT LogDevice
  {
  public:
    LogDevice() = default;
    virtual ~LogDevice() = default;

    LogDevice(const LogDevice&) = delete;
    LogDevice& operator=(const LogDevice&) = delete;
    LogDevice(LogDevice&&) = delete;
    LogDevice& operator=(LogDevice&&) = delete;

    /**
     * Returns the name of this device.
     * @return The name of the device.
     */
    virtual const std::string& name() const = 0;

    /**
     * Logs the event. Calls doLog.
     * @param event The event that shall be logged
     */
    void log(const LogEvent& event);

    /**
     * Flushes the log device.
     */
    void flush();

  private:
    /**
     * Template method to do the actual logging to the specific log device.
     * @param stream The stream contains the foprmatted event information
     */
    virtual void doLog(std::ostringstream& stream) = 0;

    /**
     * Template method to flush the specific log device.
     */
    virtual void doFlush() = 0;
  };


  /**
   * This class provides the csvsqldb logging mechanism. Only the static init method should be called from user code.
   */
  class CSVSQLDB_EXPORT Logging
  {
  public:
    /**
     * Initializes the logging mechanism. Has to be called after creating and configuring the GlobalConfiguration.
     */
    static void init();

    /**
     * Calles the specific device event log method using the GlobalConfiguration.
     * @param event The event that shall be logged
     */
    static void log(const LogEvent& event);

  private:
    /**
     * As the Logging class is a singleton, the constructor is private and deleted.
     */
    Logging() = delete;
  };


/**
 * Logs events with the categorie INFO.
 * @param arg An output stream object
 */
#define CSVSQLDB_INFOLOG(arg)                                                                                                    \
  do {                                                                                                                           \
    csvsqldb::LogEvent event;                                                                                                    \
    event._time = std::chrono::system_clock::now();                                                                              \
    event._categorie = "INFO";                                                                                                   \
    event._tid = std::this_thread::get_id();                                                                                     \
    std::ostringstream ss;                                                                                                       \
    ss << arg;                                                                                                                   \
    event._message = ss.str();                                                                                                   \
    csvsqldb::Logging::log(event);                                                                                               \
  } while (0)

/**
 * Logs events with the categorie DEBUG. Debug log events can be switched on and off via the level attribute. Depending on the
 * level, the
 * event will be logged or not. Higher levels stand for more detailed information.
 * @param unitname The name of the unit, normally the class
 * @param lvl The level
 * @param arg An output stream object
 */
#define CSVSQLDB_CLASSLOG(unitname, lvl, arg)                                                                                    \
  do {                                                                                                                           \
    static std::once_flag flag;                                                                                                  \
    static bool XX_init__;                                                                                                       \
    static bool XX_test__;                                                                                                       \
    if (!XX_init__) {                                                                                                            \
      std::call_once(flag, [&]() {                                                                                               \
        int l = csvsqldb::config<csvsqldb::GlobalConfiguration>()->debug.level[csvsqldb::getTypename<unitname>()];               \
        XX_test__ = l >= lvl;                                                                                                    \
        XX_init__ = true;                                                                                                        \
      });                                                                                                                        \
    }                                                                                                                            \
    if (XX_test__ || csvsqldb::config<csvsqldb::GlobalConfiguration>()->debug.global_level >= lvl) {                             \
      static std::string XX_stripped__ = csvsqldb::getTypename<unitname>();                                                      \
      csvsqldb::LogEvent XX_event__;                                                                                             \
      XX_event__._time = std::chrono::system_clock::now();                                                                       \
      XX_event__._classname = XX_stripped__;                                                                                     \
      XX_event__._categorie = "DEBUG";                                                                                           \
      XX_event__._tid = std::this_thread::get_id();                                                                              \
      XX_event__._line = __LINE__;                                                                                               \
      XX_event__._file = __FILE__;                                                                                               \
      std::ostringstream XX_ss__;                                                                                                \
      XX_ss__ << arg;                                                                                                            \
      XX_event__._message = XX_ss__.str();                                                                                       \
      csvsqldb::Logging::log(XX_event__);                                                                                        \
    }                                                                                                                            \
  } while (0)

/**
 * Logs events with the categorie ERROR.
 * @param arg An output stream object
 */
#define CSVSQLDB_ERRORLOG(arg)                                                                                                   \
  do {                                                                                                                           \
    csvsqldb::LogEvent event;                                                                                                    \
    event._time = std::chrono::system_clock::now();                                                                              \
    event._categorie = "ERROR";                                                                                                  \
    event._tid = std::this_thread::get_id();                                                                                     \
    event._line = __LINE__;                                                                                                      \
    event._file = __FILE__;                                                                                                      \
    std::ostringstream ss;                                                                                                       \
    ss << arg;                                                                                                                   \
    event._message = ss.str();                                                                                                   \
    csvsqldb::Logging::log(event);                                                                                               \
  } while (0)
}
