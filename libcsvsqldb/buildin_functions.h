//
//  buildin_functions.h
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

#ifndef csvsqldb_buildin_functions_h
#define csvsqldb_buildin_functions_h

#include "libcsvsqldb/inc.h"

#include "function_registry.h"


namespace csvsqldb
{

    CSVSQLDB_EXPORT void initBuildInFunctions(FunctionRegistry& registry);


    class CSVSQLDB_EXPORT CurrentDateFunction : public Function
    {
    public:
        CurrentDateFunction();

    private:
        virtual const Variant doCall(const Variants& parameter) const;
    };


    class CSVSQLDB_EXPORT CurrentTimeFunction : public Function
    {
    public:
        CurrentTimeFunction();

    private:
        virtual const Variant doCall(const Variants& parameter) const;
    };


    class CSVSQLDB_EXPORT CurrentTimestampFunction : public Function
    {
    public:
        CurrentTimestampFunction();

    private:
        virtual const Variant doCall(const Variants& parameter) const;
    };


    class CSVSQLDB_EXPORT ExtractFunction : public Function
    {
    public:
        ExtractFunction();

    private:
        virtual const Variant doCall(const Variants& parameter) const;
    };


    class CSVSQLDB_EXPORT DateFormatFunction : public Function
    {
    public:
        DateFormatFunction();

    private:
        virtual const Variant doCall(const Variants& parameter) const;
    };


    class CSVSQLDB_EXPORT TimeFormatFunction : public Function
    {
    public:
        TimeFormatFunction();

    private:
        virtual const Variant doCall(const Variants& parameter) const;
    };


    class CSVSQLDB_EXPORT TimestampFormatFunction : public Function
    {
    public:
        TimestampFormatFunction();

    private:
        virtual const Variant doCall(const Variants& parameter) const;
    };


    class CSVSQLDB_EXPORT PowerFunction : public Function
    {
    public:
        PowerFunction();

    private:
        virtual const Variant doCall(const Variants& parameter) const;
    };


    class CSVSQLDB_EXPORT UpperFunction : public Function
    {
    public:
        UpperFunction();

    private:
        virtual const Variant doCall(const Variants& parameter) const;
    };


    class CSVSQLDB_EXPORT LowerFunction : public Function
    {
    public:
        LowerFunction();

    private:
        virtual const Variant doCall(const Variants& parameter) const;
    };

    class CSVSQLDB_EXPORT CharLengthFunction : public Function
    {
    public:
        CharLengthFunction(const std::string& name);

    private:
        virtual const Variant doCall(const Variants& parameter) const;
    };

    class CSVSQLDB_EXPORT VersionFunction : public Function
    {
    public:
        VersionFunction();

    private:
        virtual const Variant doCall(const Variants& parameter) const;
    };
}

#endif
