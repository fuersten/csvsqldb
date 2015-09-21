//
//  function_registry.h
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

#ifndef csvsqldb_function_registry_h
#define csvsqldb_function_registry_h

#include "libcsvsqldb/inc.h"

#include "types.h"
#include "variant.h"

#include <map>
#include <memory>


namespace csvsqldb
{

    class CSVSQLDB_EXPORT Function
    {
    public:
        typedef std::shared_ptr<Function> Ptr;

        Function(std::string name, eType retType, const Types parameterTypes)
        : _name(name)
        , _retType(retType)
        , _parameterTypes(parameterTypes)
        {
        }

        const Variant call(const Variants& parameter) const
        {
            return doCall(parameter);
        }

        const std::string& getName() const
        {
            return _name;
        }
        eType getReturnType() const
        {
            return _retType;
        }
        const Types& getParameterTypes() const
        {
            return _parameterTypes;
        }

    private:
        virtual const Variant doCall(const Variants& parameter) const = 0;

        std::string _name;
        eType _retType;
        const Types _parameterTypes;
    };


    class CSVSQLDB_EXPORT FunctionRegistry
    {
    public:
        typedef std::vector<Function::Ptr> FunctionVector;

        FunctionRegistry()
        {
        }

        void registerFunction(const Function::Ptr& function);

        Function::Ptr getFunction(const std::string& funcname) const;

        void getFunctions(FunctionVector& functions) const
        {
            for(const auto& entry : _functions) {
                functions.push_back(entry.second);
            }
        }

    private:
        typedef std::map<std::string, Function::Ptr> Functions;

        Functions _functions;
    };
}

#endif
