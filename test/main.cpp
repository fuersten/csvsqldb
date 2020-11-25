//
//  csvsqldb test
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


#define CATCH_CONFIG_RUNNER
#include <csvsqldb/base/default_configuration.h>
#include <csvsqldb/base/global_configuration.h>
#include <csvsqldb/base/logging.h>
#include <csvsqldb/typeoperations.h>

#include <catch2/catch.hpp>


int main(int argc, char* argv[])
{
  // initialize csvsqldb logging and configuration sub system
  class CSVDBGlobalConfiguration : public csvsqldb::GlobalConfiguration
  {
  public:
    virtual void doConfigure(const csvsqldb::Configuration::Ptr&)
    {
      if (logging.device == "None") {
        logging.device = "Console";
      }
    }
  };

  csvsqldb::GlobalConfiguration::create<CSVDBGlobalConfiguration>();
  csvsqldb::config<CSVDBGlobalConfiguration>()->configure(std::make_shared<csvsqldb::DefaultConfiguration>());
  csvsqldb::Logging::init();

  csvsqldb::initTypeSystem();

  Catch::Session session;

  int returnCode = session.applyCommandLine(argc, argv);
  if (returnCode != 0) {
    return returnCode;
  }

  return session.run();
}
