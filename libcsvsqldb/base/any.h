//
//  any.h
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

#ifndef csvsqldb_any_h
#define csvsqldb_any_h

#include "libcsvsqldb/inc.h"

#include "exception.h"

#include "types.h"

#include <utility>
#include <type_traits>


namespace csvsqldb
{
    /**
     * This class represents any type. The assigned value can be extracted with the any_cast operator.
     */
	class CSVSQLDB_EXPORT Any
    {
    public:
        /**
         * Constructs an empty value.
         */
        Any()
        : _holder(nullptr)
        {}
        
        /**
         * Constructs an Any from the given type and value.
         * @param value The value to construct
         */
        template<typename T>
        Any(const T& value)
        : _holder(new ValueHolder<T>(value))
        {}
        
        /**
         * Copy constructs the Any from another Any.
         * @param any Any to construct with
         */
        Any(const Any& any)
        : _holder(any._holder?any._holder->clone():nullptr)
        {
        }
        
        ~Any()
        {
            delete _holder;
        }
        
        /**
         * Swaps this Any with the given one.
         * @param any Any to swap with
         */
        void swap(Any& any)
        {
            std::swap(_holder, any._holder);
        }
        
        /**
         * Assignes the given value to this Any.
         * @param value The value to assign
         * @return Returns this Any as reference.
         */
        template<typename T>
        Any& operator=(const T& value)
        {
            Any(value).swap(*this);
            return *this;
        }

        /**
         * Assignes the given Any to this Any.
         * @param any The Any to assign
         * @return Returns this Any as reference.
         */
        Any& operator=(const Any& any)
        {
            Any(any).swap(*this);
            return *this;
        }
        
        /**
         * Tests if the object has a value.
         * @return true if no value was assigned, otherwise false.
         */
        bool empty() const
        {
            return !_holder;
        }
        
    private:
        struct ValueHolderBase
        {
            virtual ~ValueHolderBase() {}
            virtual ValueHolderBase* clone() const = 0;
            virtual const std::type_info& type() const = 0;
        };
        
        template<typename T>
        struct ValueHolder : public ValueHolderBase
        {
            ValueHolder(const T& t)
            : _value(t)
            {
            }
            
            virtual ValueHolderBase* clone() const
            {
                return new ValueHolder<T>(_value);
            }
            
            virtual const std::type_info& type() const
            {
                return typeid(T);
            }
            
            T _value;
        };
        
        ValueHolderBase* _holder;
        
        template<typename T>
        friend T any_cast(const Any& any);
        
        template<typename T>
        friend T* any_cast(const Any* any);
    };

    /**
     * Extracts the value from an Any. The type T has to match the underlying Any type.
     * @param any The Any to extract the value from
     * @return A pointer to the value or nullptr, if the value is empty or the type does not match.
     */
    template<typename T>
    T* any_cast(const Any* any)
    {
        typedef typename std::remove_reference<T>::type noreftype;
        typename std::remove_const<noreftype>::type tt;

        if(!any || any->empty() || typeid(tt) != any->_holder->type()) {
            return 0;
        }
        Any::ValueHolder<T>* holder = static_cast<Any::ValueHolder<T>*>(any->_holder);
        return &holder->_value;
    }
    
    /**
     * Extracts the value from an Any. The type T has to match the underlying Any type.
     * Throws a BadcastException if the value is empty or the type does not match.
     * @param any The Any to extract the value from
     * @return The value of the any.
     */
    template<typename T>
    T any_cast(const Any& any)
    {
        typedef typename std::remove_reference<T>::type noreftype;
        
        if(any.empty()) {
            CSVSQLDB_THROW(BadcastException, "any_cast: any has no value");
        }
        noreftype* value = any_cast<noreftype>(&any);
        if(!value) {
            CSVSQLDB_THROW(BadcastException, "any_cast: cannot cast to given type '" << getTypename<T>() << "'");
        }
        return static_cast<T>(*value);
    }
    
}

#endif
