//
//  gloabl_configuration.h
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

#include <csvsqldb/inc.h>

#include <csvsqldb/base/configuration.h>

#include <map>


namespace csvsqldb
{
  /**
   * The global configuration is an abstraction that separates the configuration mechanism from the actual configuration.
   * All relevant configuration values are accessible through the global instance. By calling the GlobalConfiguration::configure
   * method again, the configuraiton can be changed for the running system. Currently, you have to beware of race conditions!
   */
  class CSVSQLDB_EXPORT GlobalConfiguration
  {
  public:
    using Ptr = std::shared_ptr<GlobalConfiguration>;
    using DebugLevel = std::map<std::string, int>;

    GlobalConfiguration() = default;
    virtual ~GlobalConfiguration() = default;

    GlobalConfiguration(const GlobalConfiguration&) = delete;
    GlobalConfiguration(GlobalConfiguration&&) = delete;
    GlobalConfiguration& operator=(const GlobalConfiguration&) = delete;
    GlobalConfiguration& operator=(GlobalConfiguration&&) = delete;

    /**
     * Debug logging relevant configurations.
     */
    struct Debug {
      int32_t global_level{0};
      DebugLevel level;
    };

    /**
     * Logging relevant configurations.
     */
    struct Logging {
      std::string device;
      std::string separator;
      bool escape_newline{false};
    };

    /**
     * Creates a new global configuration instance and makes it accessible to the GlobalConfiguration::instance method. The
     * type should be your inherited global configuration class. It should be called once before accessing global configuration
     * values.
     */
    template<typename T>
    static void create()
    {
      _config = std::make_shared<T>();
    }

    /**
     * Returns the only global configuration instance.
     * @return The global configuration instance.
     */
    template<typename T>
    static std::shared_ptr<T> instance()
    {
      return std::dynamic_pointer_cast<T>(_config);
    }

    /**
     * Configures the global configuration instance using the given configuration.
     * @param configuration The configuration to configure the global configuraiton with
     */
    void configure(const Configuration::Ptr& configuration);

    Debug debug;
    Logging logging;

  private:
    /**
     * Template method that can be overloaded in order to configure own configuration values.
     * Configures the global configuration instance using the given configuration.
     * @param configuration The configuration to configure the global configuration with
     */
    virtual void doConfigure(const Configuration::Ptr& configuration);

    static Ptr _config;
    Configuration::Ptr _configuration;
  };

  /**
   * Convenience template to extract the global config in a shorter way.
   * @return The global configuration instance.
   */
  template<typename T>
  static std::shared_ptr<T> config()
  {
    return GlobalConfiguration::instance<T>();
  }
}
