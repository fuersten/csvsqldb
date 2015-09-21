//
//  signalhandler.h
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

#ifndef csvsqldb_signalhandler_h
#define csvsqldb_signalhandler_h

#include "libcsvsqldb/inc.h"

#include <list>
#include <memory>


namespace csvsqldb
{

#ifndef SIGINT
#define SIGINT 2
#endif
#ifndef SIGTERM
#define SIGTERM 15
#endif

    /**
     * Interface for all signal handling classes.
     */
    class CSVSQLDB_EXPORT SignalEventHandler
    {
    public:
        /**
         * This method has to be overridden to handle a signal. All registered signals are handled
         * by this method.
         * @param signum The signal to handle.
         * @return If the signal handling chain shall be stopped an integer < 0 has to be returned. Otherwise
         * the next handler, if one exists, will be called.
         */
        virtual int onSignal(int signum) = 0;

    protected:
        virtual ~SignalEventHandler()
        {
        }
    };


    /**
     * Signal handler facility. Manages all signal handler instances.
     */
    class CSVSQLDB_EXPORT SignalHandler
    {
    public:
        typedef std::list<SignalEventHandler*> SignalEventHandlerList;

        /**
         * Creates the signal handler object. Throws an Exception if the internal system signal handler could not be installed.
         */
        SignalHandler();

        virtual ~SignalHandler();

        /**
         * Registers a SignalEventHandler instance for the given signal.
         * If a handler is already registered for a signal, the given
         * handler is appended to the handler list.
         * @param signum The signal to register the handler for.
         * @param handler The handler instance to register.
         */
        void addHandler(int signum, SignalEventHandler* handler);

        /**
         * Removes a SignalEventHandler instance for the given signal.
         * @param signum The signal to remove the handler for.
         * @param handler The handler instance to remove.
         */
        void removeHandler(int signum, SignalEventHandler* handler);

        /**
         * Returns the SignalEventHandler instances which have been registered for this signal.
         * @param signum The signal to find handlers for.
         * @return A list of the registered SignalEventHandler instances.
         */
        SignalEventHandlerList handler(int signum) const;

        /**
         * Returns, if the signal handling is stopped or not.
         * @return If the signal handling is stopped <tt>true</tt> is returned, otherwise <tt>false</tt>.
         */
        bool isStopSignalHandling() const;

        /**
         * Stops the signal handling.
         * After calling this methods no more signals will be dispatched to the
         * SignalEventHandler instances.
         */
        void stopSignalHandling();

    private:
        struct Private;
        std::unique_ptr<Private> _p;
    };


    /**
     * This utility class registers the given event handler with the csvsqldb::SignalHandler and removes it upon destruction.
     */
    class SetUpSignalEventHandler
    {
    public:
        /**
         * Adds the given signal event handler to the signal handler.
         * @param signum The signal to add the handler for
         * @param sighandler The signal handler manager to use
         * @param handler The signal event handler to install
         */
        SetUpSignalEventHandler(int signum, SignalHandler* sighandler, SignalEventHandler* handler)
        : _signum(signum)
        , _sighandler(sighandler)
        , _handler(handler)
        {
            _sighandler->addHandler(_signum, _handler);
        }

        /**
         * Removes the previously installed signal handler.
         */
        ~SetUpSignalEventHandler()
        {
            _sighandler->removeHandler(_signum, _handler);
        }

    private:
        int _signum;
        SignalHandler* _sighandler;
        SignalEventHandler* _handler;
    };
}

#endif
