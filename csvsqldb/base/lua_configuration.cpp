//
//  lua_configuration.cpp
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

#include "lua_configuration.h"

#include "exception.h"
#include "lua_engine.h"

#include <filesystem>


namespace csvsqldb
{
  namespace fs = std::filesystem;

  struct LuaConfiguration::Private {
    Private(const fs::path& configFile)
    {
      if (fs::exists(configFile)) {
        try {
          _lua.doFile(configFile);
        } catch (std::exception& ex) {
          CSVSQLDB_THROW(ConfigurationException, "Could not process config file '" << configFile << "' error: " << ex.what());
        }
      } else {
        CSVSQLDB_THROW(FilesystemException, "Lua config script '" << configFile << "' does not exist");
      }
    }

    luaengine::LuaEngine _lua;
  };

  LuaConfiguration::LuaConfiguration(const std::filesystem::path& configFile)
  : _p(new Private(configFile))
  {
  }

  LuaConfiguration::~LuaConfiguration()
  {
    // Needs to be specified due to the unique_ptr impl.
  }

  size_t LuaConfiguration::doGetProperties(const std::string& path, StringVector& properties) const
  {
    return _p->_lua.getProperties(path, properties);
  }

  bool LuaConfiguration::doHasProperty(const std::string& path) const
  {
    return _p->_lua.hasGlobal(path);
  }

  bool LuaConfiguration::get(const std::string& path, Typer<bool> typer) const
  {
    try {
      return _p->_lua.getGlobal<bool>(path);
    } catch (std::exception& ex) {
      throw ConfigurationException("path '" + path + "' not found (" + ex.what() + ")");
    }
  }

  int32_t LuaConfiguration::get(const std::string& path, Typer<int32_t> typer) const
  {
    try {
      return _p->_lua.getGlobal<int32_t>(path);
    } catch (std::exception& ex) {
      throw ConfigurationException("path '" + path + "' not found (" + ex.what() + ")");
    }
  }

  int64_t LuaConfiguration::get(const std::string& path, Typer<int64_t> typer) const
  {
    try {
      return _p->_lua.getGlobal<int64_t>(path);
    } catch (std::exception& ex) {
      throw ConfigurationException("path '" + path + "' not found (" + ex.what() + ")");
    }
  }

  float LuaConfiguration::get(const std::string& path, Typer<float> typer) const
  {
    try {
      return _p->_lua.getGlobal<float>(path);
    } catch (std::exception& ex) {
      throw ConfigurationException("path '" + path + "' not found (" + ex.what() + ")");
    }
  }

  double LuaConfiguration::get(const std::string& path, Typer<double> typer) const
  {
    try {
      return _p->_lua.getGlobal<double>(path);
    } catch (std::exception& ex) {
      throw ConfigurationException("path '" + path + "' not found (" + ex.what() + ")");
    }
  }

  std::string LuaConfiguration::get(const std::string& path, Typer<std::string> typer) const
  {
    try {
      return _p->_lua.getGlobal<std::string>(path);
    } catch (std::exception& ex) {
      throw ConfigurationException("path '" + path + "' not found (" + ex.what() + ")");
    }
  }
}
