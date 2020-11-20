//
//  configuration.cpp
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

#include "configuration.h"

#include "exception.h"


namespace csvsqldb
{
  size_t Configuration::getProperties(const std::string& path, StringVector& properties) const
  {
    return doGetProperties(path, properties);
  }

  bool Configuration::hasProperty(const std::string& path) const
  {
    return doHasProperty(path);
  }

  bool Configuration::get(const std::string& path, bool defaultValue) const
  {
    try {
      return get<bool>(path);
    } catch (ConfigurationException&) {
      return defaultValue;
    }
  }

  int32_t Configuration::get(const std::string& path, int32_t defaultValue) const
  {
    try {
      return get<int32_t>(path);
    } catch (ConfigurationException&) {
      return defaultValue;
    }
  }

  int64_t Configuration::get(const std::string& path, int64_t defaultValue) const
  {
    try {
      return get<int64_t>(path);
    } catch (ConfigurationException&) {
      return defaultValue;
    }
  }

  float Configuration::get(const std::string& path, float defaultValue) const
  {
    try {
      return get<float>(path);
    } catch (ConfigurationException&) {
      return defaultValue;
    }
  }

  double Configuration::get(const std::string& path, double defaultValue) const
  {
    try {
      return get<double>(path);
    } catch (ConfigurationException&) {
      return defaultValue;
    }
  }

  std::string Configuration::get(const std::string& path, const char* defaultValue) const
  {
    try {
      return get<std::string>(path);
    } catch (ConfigurationException&) {
      return defaultValue;
    }
  }

  std::string Configuration::get(const std::string& path, const std::string& defaultValue) const
  {
    try {
      return get<std::string>(path);
    } catch (ConfigurationException&) {
      return defaultValue;
    }
  }
}
