//
//  application.h
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

#ifndef csvsqldb_application_h
#define csvsqldb_application_h

#include "libcsvsqldb/inc.h"


/**
 * The general csvsqldb namespace.
 */
namespace csvsqldb
{
    /**
     * The main application class used for all csvsqldb applications.
     */
    class CSVSQLDB_EXPORT Application
    {
    public:
        /**
         * Constructs the application object.
         * Normally the parameters are the same as to the main function.
         * @param argc Number of arguments supplied by argv
         * @param argv Arguments to the application
         */
        Application(int argc, char** argv);

        /**
         * Starts the execution of the application code.
         * Internally calls in sequence
         * 1. setUp()
         * 2. doRun()
         * 3. tearDown()
         * @return Returns the applications exit code directly to the operating system.
         */
        int run();

    protected:
        /**
         * Used to configure the application.
         * @return Should return true, if the application should go on calling doRun or false in order to terminate the
         * application.
         */
        virtual bool setUp(int argc, char** argv) = 0;

        /**
         * This template method should contain the actual application code. It gets called after the setUp method.
         * @return Returns the applications exit code directly to the operating system.
         */
        virtual int doRun() = 0;

        /**
         * Used to deinitialize the application.
         */
        virtual void tearDown(){};

    private:
        int _argc;
        char** _argv;
    };
}

#endif
