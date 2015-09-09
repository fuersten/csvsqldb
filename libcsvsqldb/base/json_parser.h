//
//  json_parser.h
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

#ifndef csvsqldb_json_parser_h
#define csvsqldb_json_parser_h

#include "libcsvsqldb/inc.h"

#include <iosfwd>
#include <memory>
#include <string>


namespace csvsqldb
{
    /**
     * JSON related implementations
     */
    namespace json
    {
        /**
         * Callback interface for the JSON parser.
         */
        class CSVSQLDB_EXPORT Callback
        {
        public:
            typedef std::shared_ptr<Callback> Ptr;
          
            /**
             * Called upon a left curly bracket, to introduce an object.
             */
            virtual void startObject() = 0;

            /**
             * Called upon fetching the key for an obect value.
             */
            virtual void key(const std::string& key) = 0;
            
            /**
             * Called upon a right culry bracket, closing an object.
             */
            virtual void endObject() = 0;
            
            /**
             * Called upon a left square bracket, to introduce an array.
             */
            virtual void startArray() = 0;
            
            /**
             * Called upon a right square bracket, closing an array.
             */
            virtual void endArray() = 0;
            
            /**
             * Called upon fetching a number value.
             * @param val The number value
             */
            virtual void numberValue(double val) = 0;
            
            /**
             * Called upon fetching a string value.
             * @param val The string value
             */
            virtual void stringValue(const std::string& val) = 0;
            
            /**
             * Called upon fetching a boolena value.
             * @param val The value of the boolean
             */
            virtual void booleanValue(bool val) = 0;
            
            /**
             * Called upon fetching a null value.
             */
            virtual void nullValue() = 0;
        };
        
        /**
         * A JSON parser as specified by https://tools.ietf.org/html/rfc4627
         * 
         * Does currently have the following limitations:
         * - Strings have no unicode points
         */
        class CSVSQLDB_EXPORT Parser
        {
        public:
            /**
             * Construct a parser with a string.
             * @param json The JSON string to parse
             * @param callback The callback to call for parsing events. Is allowed to be null.
             */
            Parser(const std::string& json, const Callback::Ptr& callback);

            ~Parser();
            
            /**
             * Construct a parser with a stream object.
             * @param stream The JSON stream to parse
             * @param callback The callback to call for parsing events. Is allowed to be null.
             */
            Parser(std::istream& stream, const Callback::Ptr& callback);

            /**
             * Does the actual parsing of the JSON and calls the given callback functions upon parsing events.
             * Does throw JsonException upon any error, if a callback is specified. Otherwise returns false upon an error.
             * @return true upon successful parsing, otherwise false. Depends on the given callback,
             */
            bool parse();
            
        private:
            struct Private;
            std::unique_ptr<Private> _p;
        };
    }
}

#endif
