//
//  global_configuration.cpp
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

#include "global_configuration.h"


namespace csvsqldb
{
  GlobalConfiguration::Ptr GlobalConfiguration::_config;

  void GlobalConfiguration::configure(const Configuration::Ptr& configuration)
  {
    _configuration = configuration;

    logging.device = _configuration->get("logging.device", "Console");
    logging.separator = " " + _configuration->get("logging.separator", "|") + " ";
    logging.escape_newline = _configuration->get("logging.escape_newline", false);

    debug.global_level = _configuration->get("debug.global_level", 0);
    if (_configuration->hasProperty("debug.level")) {
      StringVector properties;
      _configuration->getProperties("debug.level", properties);
      for (auto& property : properties) {
        debug.level.insert(std::make_pair(property, _configuration->get<int32_t>("debug.level." + property)));
      }
    }

    doConfigure(_configuration);
  }

  void GlobalConfiguration::doConfigure(const Configuration::Ptr&)
  {
  }
}
