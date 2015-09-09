//
//  default_configuration.h
//  csvsqldb
//
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

#ifndef csvsqldb_default_configuration_h
#define csvsqldb_default_configuration_h

#include "libcsvsqldb/inc.h"

#include "configuration.h"


namespace csvsqldb
{
    /**
     * Default configuratiion class used if no other configuration is available. Afterwards the configuration values can be retrieved from the configuration.
     */
	class CSVSQLDB_EXPORT DefaultConfiguration : public Configuration
    {
    public:
        /**
         * Constructs a default configuration. Will throw a ConfigurationException if an error occurs.
         */
        DefaultConfiguration();
        
        virtual ~DefaultConfiguration();

    private:
        virtual size_t doGetProperties(const std::string& path, StringVector& properties) const;
        
        virtual bool doHasProperty(const std::string& path) const;
        
        virtual bool get(const std::string& path, Typer<bool> typer) const;

        virtual int32_t get(const std::string& path, Typer<int32_t> typer) const;

        virtual int64_t get(const std::string& path, Typer<int64_t> typer) const;

        virtual float get(const std::string& path, Typer<float> typer) const;

        virtual double get(const std::string& path, Typer<double> typer) const;

        virtual std::string get(const std::string& path, Typer<std::string> typer) const;

        struct Private;
        std::unique_ptr<Private> _p;
    };
}

#endif
