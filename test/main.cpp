//
//  main.cpp
//  csvsqldb test runner
//
//  Created by Lars-Christian Fürstenberg on 28.07.13.
//  BSD 3-Clause License
//  Copyright (c) 2015, Lars-Christian Fürstenberg
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

#include "libcsvsqldb/base/default_configuration.h"
#include "libcsvsqldb/base/global_configuration.h"
#include "libcsvsqldb/base/logging.h"

#include "test.h"

#include <memory>
#include <chrono>
#include <sstream>
#include <iomanip>


class DetailedProgressListener : public mpf::test::ProgressListener
{
public:
    DetailedProgressListener(size_t nameWidth = 20)
    : _width(nameWidth)
    {}

    template<typename D>
    std::string formattedDuration(const D& duration) const
    {
        std::ostringstream result;
        if (std::chrono::duration_cast<std::chrono::duration<double,std::micro>>(duration).count() < 100) {
            result << std::fixed << std::setprecision(1) << std::chrono::duration_cast<std::chrono::duration<double,std::micro>>(duration).count() << "us";
        } else if (std::chrono::duration_cast<std::chrono::duration<double,std::milli>>(duration).count() < 100) {
            result << std::fixed << std::setprecision(1) << std::chrono::duration_cast<std::chrono::duration<double,std::milli>>(duration).count() << "ms";
        } else if (std::chrono::duration_cast<std::chrono::duration<double>>(duration).count() < 100) {
            result << std::fixed << std::setprecision(1) << std::chrono::duration_cast<std::chrono::duration<double>>(duration).count() << "s";
        } else {
            result << std::fixed << std::setprecision(1) << std::chrono::duration_cast<std::chrono::duration<double, std::ratio<60>>>(duration).count() << "min";
        }
        return result.str();
    }

    virtual void testStart(size_t count)
    {
        _testStartTime = std::chrono::high_resolution_clock::now();
        std::cerr << "Starting " << count << " tests:" << std::endl;
    }
    virtual void testEnd(size_t successful, size_t failed)
    {
        std::cerr << "Executed " << successful << " successful and " << failed << " failed tests in "
        << formattedDuration(std::chrono::high_resolution_clock::now() - _testStartTime) << "." << std::endl;
    }
    virtual void suiteStart(const std::string& name, size_t count)
    {
        _suiteStartTime = std::chrono::high_resolution_clock::now();
        std::cerr << std::setw(static_cast<int>(_width)) << std::left << (name + ":");
    }
    virtual void suiteEnd(const std::string& name)
    {
        std::cerr << " (" << formattedDuration(std::chrono::high_resolution_clock::now() - _suiteStartTime) << ")" << std::endl;
    }
    virtual void testcaseStart(const std::string& name, size_t count)
    {
    }
    virtual void fixtureEnd(const std::string& name, bool successful)
    {
        if(successful) {
            std::cerr << ".";
        } else {
            std::cerr << "E";
        }
    }
    virtual void fixtureAssert(const std::string& name, const std::string& message)
    {
        std::cerr << std::endl << "Fixture " << name << " caught an assertion: " << message << std::endl;
    }
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> _testStartTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> _suiteStartTime;
    size_t _width;
};

int main(int argc, char** argv)
{
    // initialize csvsqldb logging and configuration sub system
    class CSVDBGlobalConfiguration : public csvsqldb::GlobalConfiguration
    {
    public:
        virtual void doConfigure(const csvsqldb::Configuration::Ptr&)
        {
            if(logging.device == "None") {
                logging.device = "Console";
            }
        }
    };
    
    csvsqldb::GlobalConfiguration::create<CSVDBGlobalConfiguration>();
    csvsqldb::config<CSVDBGlobalConfiguration>()->configure(std::make_shared<csvsqldb::DefaultConfiguration>());
    csvsqldb::Logging::init();
    
    mpf::test::ProgressListener::Listener listener;
    listener.push_back(std::make_shared<DetailedProgressListener>(16));

    int ret = mpf::test::TestRegistry::instance().run((argc == 1 ? "" : argv[1]), listener) ? 0 : 1;
    mpf::test::TestRegistry::instance().fini();
    return ret;
}
