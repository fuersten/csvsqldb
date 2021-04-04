//
//  lua_configuration.h
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

#include <filesystem>


namespace csvsqldb
{
  /**
   * Configuratiion class for lua based configurations. A lua script file with the configuration is specified upon construction
   * and processed. Afterwards the configuration values can be retrieved from the configuration.
   */
  class CSVSQLDB_EXPORT LuaConfiguration : public Configuration
  {
  public:
    /**
     * Construct a lua configuration with the given lua script file. Will throw a ConfigurationException if an error occurs,
     * i.e the a lua error occurs while compiling the script. Throws a FilesystemException if the file does not exist.
     * @param configFile The lua script file to use as configuration
     */
    LuaConfiguration(const std::filesystem::path& configFile);

    ~LuaConfiguration() override;

  private:
    size_t doGetProperties(const std::string& path, StringVector& properties) const override;

    bool doHasProperty(const std::string& path) const override;

    bool get(const std::string& path, Typer<bool> typer) const override;

    int32_t get(const std::string& path, Typer<int32_t> typer) const override;

    int64_t get(const std::string& path, Typer<int64_t> typer) const override;

    float get(const std::string& path, Typer<float> typer) const override;

    double get(const std::string& path, Typer<double> typer) const override;

    std::string get(const std::string& path, Typer<std::string> typer) const override;

    struct Private;
    std::unique_ptr<Private> _p;
  };
}
