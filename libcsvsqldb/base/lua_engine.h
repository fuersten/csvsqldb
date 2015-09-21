//
//  lua_engine.h
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

#ifndef csvsqldb_lua_engine_h
#define csvsqldb_lua_engine_h

#include "libcsvsqldb/inc.h"

#include "function_traits.h"
#include "string_helper.h"

#include <map>
#include <typeinfo>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}


namespace csvsqldb
{
    /**
     * Lua related implementations.
     */
    namespace luaengine
    {
        /**
         * Wraps a lua_State object. Overloaded type operator allows the usage with C/C++ lua API functions.
         */
        struct CSVSQLDB_EXPORT lua_state {
            /**
             * Constructs a luaState. Open lua base libs.
             */
            lua_state()
#if LUA_VERSION_NUM >= 502
            : _L(luaL_newstate())
#else
            : _L(lua_open())
#endif
            {
                luaopen_base(_L);
                luaL_openlibs(_L);
            }

            /**
             * Destroy the lua_State object.
             */
            ~lua_state()
            {
                lua_close(_L);
            }

            /**
             * Type operator returns the lua_State object.
             * @return The lua_State object.
             */
            operator lua_State*()
            {
                return _L;
            }

            /**
             * Type operator returns the lua_State object.
             * @return The lua_State object.
             */
            operator lua_State*() const
            {
                return _L;
            }

        private:
            lua_State* _L;
        };

        template <typename T>
        void bindType(lua_State* L, T& object);

        template <>
        void bindType<bool>(lua_State* L, bool& b)
        {
            lua_pushboolean(L, b ? 1 : 0);
        }

        template <>
        void bindType<std::string>(lua_State* L, std::string& s)
        {
            lua_pushstring(L, s.c_str());
        }

        template <>
        void bindType<float>(lua_State* L, float& f)
        {
            lua_pushnumber(L, f);
        }

        template <>
        void bindType<double>(lua_State* L, double& d)
        {
            lua_pushnumber(L, d);
        }

        template <>
        void bindType<short>(lua_State* L, short& i)
        {
            lua_pushnumber(L, i);
        }

        template <>
        void bindType<int>(lua_State* L, int& i)
        {
            lua_pushnumber(L, i);
        }

        template <>
        void bindType<long>(lua_State* L, long& l)
        {
            lua_pushnumber(L, l);
        }

        template <typename T>
        T unwrapType(lua_State* L, int index = -1);

        template <>
        void unwrapType<void>(lua_State* L, int index)
        {
        }

        template <>
        std::string unwrapType<std::string>(lua_State* L, int index)
        {
            if(!lua_isstring(L, index)) {
                throw std::runtime_error("unwrap type is not a string");
            }
            return std::string(lua_tostring(L, index));
        }

        template <>
        bool unwrapType<bool>(lua_State* L, int index)
        {
            if(!lua_isboolean(L, index)) {
                throw std::runtime_error("unwrap type is not a bool");
            }
            return static_cast<bool>(lua_toboolean(L, index));
        }

        template <>
        int unwrapType<int>(lua_State* L, int index)
        {
            if(!lua_isnumber(L, index)) {
                throw std::runtime_error("unwrap type is not a number");
            }
            return static_cast<int>(lua_tonumber(L, index));
        }

        template <>
        int64_t unwrapType<int64_t>(lua_State* L, int index)
        {
            if(!lua_isnumber(L, index)) {
                throw std::runtime_error("unwrap type is not a number");
            }
            return static_cast<int64_t>(lua_tonumber(L, index));
        }

        template <>
        float unwrapType<float>(lua_State* L, int index)
        {
            if(!lua_isnumber(L, index)) {
                throw std::runtime_error("unwrap type is not a number");
            }
            return static_cast<float>(lua_tonumber(L, index));
        }

        template <>
        double unwrapType<double>(lua_State* L, int index)
        {
            if(!lua_isnumber(L, index)) {
                throw std::runtime_error("unwrap type is not a number");
            }
            return static_cast<double>(lua_tonumber(L, index));
        }

        /**
         * Class to execute lua scripts. It is also possible to call functions from lua scripts and to register C++ functions to
         * be called
         * in lua scripts.
         */
        class CSVSQLDB_EXPORT LuaEngine
        {
        public:
            LuaEngine()
            {
            }

            /**
             * Reads in a lua file and processes the script. Afterwards lua functions can be called or globals read out.
             * Throws a std::runtime_error if an error occurs.
             * @param script Script to read in and process
             */
            void doFile(const std::string& script)
            {
                if(luaL_loadfile(_L, script.c_str())) {
                    std::string err(lua_tostring(_L, -1));
                    lua_pop(_L, 2);
                    throw std::runtime_error(err);
                }

                if(lua_pcall(_L, 0, 0, 0)) {
                    std::string err(lua_tostring(_L, -1));
                    lua_pop(_L, 2);
                    throw std::runtime_error(err);
                }
            }

            /**
             * Processes a lua script from a string. Afterwards lua functions can be called or globals read out.
             * Throws a std::runtime_error if an error occurs.
             * @param script Script to process
             */
            void doString(const std::string& script)
            {
                if(luaL_loadbuffer(_L, script.c_str(), script.length(), script.c_str())) {
                    std::string err(lua_tostring(_L, -1));
                    lua_pop(_L, 1);
                    throw std::runtime_error(err);
                }

                if(lua_pcall(_L, 0, 0, -2)) {
                    std::string err(lua_tostring(_L, -1));
                    lua_pop(_L, 1);
                    throw std::runtime_error(err);
                }
            }

            /**
             * Is a guard which pops a lua object from the lua stack upon destruction.
             */
            struct LuaStackCleaner {
                LuaStackCleaner(const lua_state& state)
                : _state(state)
                {
                }

                ~LuaStackCleaner()
                {
                    lua_pop(_state, 1);
                }

                const lua_state& _state;
            };

            /**
             * Helper for fetching a global lua value.
             * Throws a std::runtime_error if an error occurs.
             * @param name Name of the global value
             * @return The value.
             */
            template <typename T>
            T getGlobalHelper(const std::string& name) const
            {
                LuaStackCleaner guard(_L);

                lua_getglobal(_L, name.c_str());
                if(lua_isnil(_L, -1)) {
                    throw std::runtime_error("global '" + name + "' not found");
                }
                return unwrapType<T>(_L, -1);
            }

            /**
             * Fetches a global lua value.
             * Throws a std::runtime_error if an error occurs.
             * @param path Dot separated path of the global value
             * @return The value.
             */
            template <typename T>
            T getGlobal(const std::string& path) const
            {
                StringVector elems;
                if(split(path, '.', elems) == 1) {
                    return getGlobalHelper<T>(path);
                }

                LuaStackCleaner guard(_L);

                lua_getglobal(_L, elems[0].c_str());
                if(lua_isnil(_L, -1) || !lua_istable(_L, -1)) {
                    throw std::runtime_error("global '" + elems[0] + "' not found");
                }

                for(size_t n = 1; n < elems.size(); n++) {
                    lua_pushstring(_L, elems[n].c_str());
                    lua_gettable(_L, -2);
                    if(lua_isnil(_L, -1)) {
                        throw std::runtime_error("global '" + path + "' not found");
                    }
                }
                return unwrapType<T>(_L, -1);
            }

            /**
             * Return all sub-properties of the given path.
             * @param path Dot separated path to the properties
             * @param properties Will be filled with the found properties
             * @return Returns the number of found properties.
             */
            size_t getProperties(const std::string& path, StringVector& properties)
            {
                LuaStackCleaner guard(_L);

                StringVector elems;
                split(path, '.', elems);

                lua_getglobal(_L, elems[0].c_str());
                if(lua_isnil(_L, -1)) {
                    return 0;
                }

                for(size_t n = 1; n < elems.size(); n++) {
                    lua_pushstring(_L, elems[n].c_str());
                    lua_gettable(_L, -2);
                    if(lua_isnil(_L, -1)) {
                        return 0;
                    }
                }

                if(!lua_istable(_L, -1)) {
                    return 0;
                }

                lua_pushnil(_L);

                size_t count = 0;
                while(lua_next(_L, -2)) {
                    if(lua_isstring(_L, -2)) {
                        properties.push_back(lua_tostring(_L, -2));
                    }
                    lua_pop(_L, 1);
                    ++count;
                }

                return count;
            }

            /**
             * Checks a global lua value.
             * @param path Dot separated path of the global value
             * @return true if the configuration has the entry, false otherwise.
             */
            bool hasGlobal(const std::string& path) const
            {
                LuaStackCleaner guard(_L);

                StringVector elems;
                split(path, '.', elems);

                lua_getglobal(_L, elems[0].c_str());
                if(lua_isnil(_L, -1)) {
                    return false;
                }

                for(size_t n = 1; n < elems.size(); n++) {
                    lua_pushstring(_L, elems[n].c_str());
                    lua_gettable(_L, -2);
                    if(lua_isnil(_L, -1)) {
                        return false;
                    }
                }

                return true;
            }

            /**
             * Calls a lua function with no parameter.
             * Throws a std::runtime_error if an error occurs.
             * @param name Name of the lua function to call
             * @return If the value is non void, returns the return value of the lua function.
             */
            template <typename R>
            R callFunction(const std::string& name)
            {
                LuaStackCleaner guard(_L);

                lua_getglobal(_L, name.c_str());
                if(lua_isnil(_L, -1)) {
                    throw std::runtime_error("function object is nil");
                }
                if(!lua_isfunction(_L, -1)) {
                    throw std::runtime_error(name + " is not a function object");
                }

                lua_call(_L, 0, 1);

                return unwrapType<R>(_L);
            }

            /**
             * Calls a lua function with one parameter.
             * Throws a std::runtime_error if an error occurs.
             * @param name Name of the lua function to call
             * @param t0 First parameter to be passed to the lua function
             * @return If the value is non void, returns the return value of the lua function.
             */
            template <typename R, typename T>
            R callFunction(const std::string& name, T& t0)
            {
                LuaStackCleaner guard(_L);

                lua_getglobal(_L, name.c_str());
                if(lua_isnil(_L, -1)) {
                    throw std::runtime_error("function object is nil");
                }
                if(!lua_isfunction(_L, -1)) {
                    throw std::runtime_error(name + " is not a function object");
                }

                bindType(_L, t0);

                lua_call(_L, 1, 1);

                return unwrapType<R>(_L);
            }

            /**
             * Calls a lua function with two parameters.
             * Throws a std::runtime_error if an error occurs.
             * @param name Name of the lua function to call
             * @param t0 First parameter to be passed to the lua function
             * @param t1 Second parameter to be passed to the lua function
             * @return If the value is non void, returns the return value of the lua function.
             */
            template <typename R, typename T0, typename T1>
            R callFunction(const std::string& name, T0& t0, T1& t1)
            {
                LuaStackCleaner guard(_L);

                lua_getglobal(_L, name.c_str());
                if(lua_isnil(_L, -1)) {
                    throw std::runtime_error("function object is nil");
                }
                if(!lua_isfunction(_L, -1)) {
                    throw std::runtime_error(name + " is not a function object");
                }

                bindType(_L, t0);
                bindType(_L, t1);

                lua_call(_L, 2, 1);

                return unwrapType<R>(_L);
            }

            /**
             * Calls a lua function with three parameters.
             * Throws a std::runtime_error if an error occurs.
             * @param name Name of the lua function to call
             * @param t0 First parameter to be passed to the lua function
             * @param t1 Second parameter to be passed to the lua function
             * @param t2 Third parameter to be passed to the lua function
             * @return If the value is non void, returns the return value of the lua function.
             */
            template <typename R, typename T0, typename T1, typename T2>
            R callFunction(const std::string& name, T0& t0, T1& t1, T2& t2)
            {
                LuaStackCleaner guard(_L);

                lua_getglobal(_L, name.c_str());
                if(lua_isnil(_L, -1)) {
                    throw std::runtime_error("function object is nil");
                }
                if(!lua_isfunction(_L, -1)) {
                    throw std::runtime_error(name + " is not a function object");
                }

                bindType(_L, t0);
                bindType(_L, t1);
                bindType(_L, t2);

                lua_call(_L, 3, 1);

                return unwrapType<R>(_L);
            }

            /**
             * Calls a lua function with four parameters.
             * Throws a std::runtime_error if an error occurs.
             * @param name Name of the lua function to call
             * @param t0 First parameter to be passed to the lua function
             * @param t1 Second parameter to be passed to the lua function
             * @param t2 Third parameter to be passed to the lua function
             * @param t3 Fourth parameter to be passed to the lua function
             * @return If the value is non void, returns the return value of the lua function.
             */
            template <typename R, typename T0, typename T1, typename T2, typename T3>
            R callFunction(const std::string& name, T0& t0, T1& t1, T2& t2, T3& t3)
            {
                LuaStackCleaner guard(_L);

                lua_getglobal(_L, name.c_str());
                if(lua_isnil(_L, -1)) {
                    throw std::runtime_error("function object is nil");
                }
                if(!lua_isfunction(_L, -1)) {
                    throw std::runtime_error(name + " is not a function object");
                }

                bindType(_L, t0);
                bindType(_L, t1);
                bindType(_L, t2);
                bindType(_L, t3);

                lua_call(_L, 4, 1);

                return unwrapType<R>(_L);
            }

            /**
             * Dumps the stack auf a lua handle.
             * @param _L The handle to dump the stack for
             */
            static void stackDump(lua_State* _L)
            {
                int i;
                int top = lua_gettop(_L);
                std::cout << "Stack start:" << std::endl;
                for(i = top; i >= 1; --i) {
                    int t = lua_type(_L, i);
                    switch(t) {
                        case LUA_TSTRING:
                            std::cout << lua_tostring(_L, i) << std::endl;
                            break;

                        case LUA_TBOOLEAN:
                            std::cout << (lua_toboolean(_L, i) ? "true" : "false") << std::endl;
                            break;

                        case LUA_TNUMBER:
                            std::cout << lua_tonumber(_L, i) << std::endl;
                            break;

                        case LUA_TTABLE:
                            std::cout << lua_typename(_L, t) << std::endl;
                            lua_pushvalue(_L, i);
                            for(lua_pushnil(_L); lua_next(_L, -2); lua_pop(_L, 1)) {
                                if(lua_type(_L, -2) == LUA_TSTRING) {
                                    std::cout << "\t" << lua_tostring(_L, -2) << " " << lua_typename(_L, lua_type(_L, -1)) << std::endl;
                                } else {
                                    std::cout << "\t" << lua_typename(_L, lua_type(_L, -2)) << " "
                                              << lua_typename(_L, lua_type(_L, -1)) << std::endl;
                                }
                            }
                            lua_pop(_L, 1);
                            break;

                        default:
                            std::cout << lua_typename(_L, t) << std::endl;
                            break;
                    }
                }
                std::cout << "Stack end" << std::endl;
            }

            /**
             * Returns the internal lua state.
             * @return The lua state.
             */
            operator lua_State*()
            {
                return _L;
            }

        private:
            mutable lua_state _L;
        };

        /**
         * Base class for C++ function objects that can be registered and called from lua.
         */
        class FunctionObject
        {
        public:
            virtual ~FunctionObject()
            {
            }

        protected:
            FunctionObject()
            {
            }
        };

        /**
         * Function object for C++ function without parameters.
         */
        template <typename F, typename R>
        class FunctionObjectImpl0 : public FunctionObject
        {
        public:
            FunctionObjectImpl0(F f)
            : _f(f)
            {
            }

            /**
             * Method called for functions returning void. Calls the actual C++ function.
             * @param L The lua state object
             */
            template <typename RET>
            typename std::enable_if<std::is_void<RET>::value, RET>::type call(lua_State* L) const
            {
                _f();
            }

            /**
             * Method called for functions returning non-void values. Calls the actual C++ function.
             * @param L The lua state object
             */
            template <typename RET>
            typename std::enable_if<!std::is_void<RET>::value, RET>::type call(lua_State* L) const
            {
                RET r = _f();
                bindType<RET>(L, r);
                return r;
            }

            /**
             * Static function entry point that gets actually registered with lua.
             * @param L The lua state object
             */
            static int entry(lua_State* L)
            {
                typedef FunctionObjectImpl0<F, R> F0;
                F0 const* impl = (F0 const*)lua_touserdata(L, lua_upvalueindex(1));
                impl->call<R>(L);

                return type2return<R>();
            }

            F _f;
        };

        /**
         * Function object for C++ function with one parameter.
         */
        template <typename F, typename R, typename T0>
        class FunctionObjectImpl1 : public FunctionObject
        {
        public:
            FunctionObjectImpl1(F f)
            : _f(f)
            {
            }

            /**
             * Method called for functions returning void. Calls the actual C++ function.
             * @param L The lua state object
             * @param t0 First function parameter
             */
            template <typename RET>
            typename std::enable_if<std::is_void<RET>::value, RET>::type call(lua_State* L, T0 t0) const
            {
                _f(t0);
            }

            /**
             * Method called for functions returning non-void values. Calls the actual C++ function.
             * @param L The lua state object
             * @param t0 First function parameter
             */
            template <typename RET>
            typename std::enable_if<!std::is_void<RET>::value, RET>::type call(lua_State* L, T0 t0) const
            {
                RET r = _f(t0);
                bindType<RET>(L, r);
                return r;
            }

            /**
             * Static function entry point that gets actually registered with lua.
             * @param L The lua state object
             */
            static int entry(lua_State* L)
            {
                typedef FunctionObjectImpl1<F, R, T0> F1;
                F1 const* impl = (F1 const*)lua_touserdata(L, lua_upvalueindex(1));
                T0 t0 = unwrapType<T0>(L, -1);
                impl->call<R>(L, t0);

                return type2return<R>();
            }

            F _f;
        };

        /**
         * Function object for C++ function with two parameters.
         */
        template <typename F, typename R, typename T0, typename T1>
        class FunctionObjectImpl2 : public FunctionObject
        {
        public:
            FunctionObjectImpl2(F f)
            : _f(f)
            {
            }

            /**
             * Method called for functions returning void. Calls the actual C++ function.
             * @param L The lua state object
             * @param t0 First function parameter
             * @param t1 Second function parameter
             */
            template <typename RET>
            typename std::enable_if<std::is_void<RET>::value, RET>::type call(lua_State* L, T0 t0, T1 t1) const
            {
                _f(t0, t1);
            }

            /**
             * Method called for functions returning non-void values. Calls the actual C++ function.
             * @param L The lua state object
             * @param t0 First function parameter
             * @param t1 Second function parameter
             */
            template <typename RET>
            typename std::enable_if<!std::is_void<RET>::value, RET>::type call(lua_State* L, T0 t0, T1 t1) const
            {
                RET r = _f(t0, t1);
                bindType<RET>(L, r);
                return r;
            }

            /**
             * Static function entry point that gets actually registered with lua.
             * @param L The lua state object
             */
            static int entry(lua_State* L)
            {
                typedef FunctionObjectImpl2<F, R, T0, T1> F2;
                F2 const* impl = (F2 const*)lua_touserdata(L, lua_upvalueindex(1));
                T0 t0 = unwrapType<T0>(L, -1);
                T1 t1 = unwrapType<T1>(L, -2);
                impl->call<R>(L, t0, t1);

                return type2return<R>();
            }

            F _f;
        };

        /**
         * Function object for C++ function with three parameters.
         */
        template <typename F, typename R, typename T0, typename T1, typename T2>
        class FunctionObjectImpl3 : public FunctionObject
        {
        public:
            FunctionObjectImpl3(F f)
            : _f(f)
            {
            }

            /**
             * Method called for functions returning void. Calls the actual C++ function.
             * @param L The lua state object
             * @param t0 First function parameter
             * @param t1 Second function parameter
             * @param t2 Third function parameter
             */
            template <typename RET>
            typename std::enable_if<std::is_void<RET>::value, RET>::type call(lua_State* L, T0 t0, T1 t1, T2 t2) const
            {
                _f(t0, t1, t2);
            }

            /**
             * Method called for functions returning non-void values. Calls the actual C++ function.
             * @param L The lua state object
             * @param t0 First function parameter
             * @param t1 Second function parameter
             * @param t2 Third function parameter
             */
            template <typename RET>
            typename std::enable_if<!std::is_void<RET>::value, RET>::type call(lua_State* L, T0 t0, T1 t1, T2 t2) const
            {
                RET r = _f(t0, t1, t2);
                bindType<RET>(L, r);
                return r;
            }

            /**
             * Static function entry point that gets actually registered with lua.
             * @param L The lua state object
             */
            static int entry(lua_State* L)
            {
                typedef FunctionObjectImpl3<F, R, T0, T1, T2> F3;
                F3 const* impl = (F3 const*)lua_touserdata(L, lua_upvalueindex(1));
                T0 t0 = unwrapType<T0>(L, -1);
                T1 t1 = unwrapType<T1>(L, -2);
                T2 t2 = unwrapType<T2>(L, -3);
                impl->call<R>(L, t0, t1, t2);

                return type2return<R>();
            }

            F _f;
        };

        typedef std::map<std::string, FunctionObject*> FunctionMap;
        static FunctionMap sFunctions;

        /**
         * Helper function to register functions without parameters.
         * @param L The lua state object
         * @param name The name of the function in the lua context
         * @param f The actual C++ function
         */
        template <typename F>
        void registerFunctionN(lua_State* L, const std::string& name, F f, int2type<0>)
        {
            typedef decltype(f) function_type;

            using R = typename function_traits<function_type>::return_type;
            typedef FunctionObjectImpl0<F, R> F0;

            FunctionObject* func = new F0(f);
            sFunctions[name] = func;
            lua_pushlightuserdata(L, func);
            lua_pushcclosure(L, F0::entry, 1);
            lua_setglobal(L, name.c_str());
        }

        /**
         * Helper function to register functions with one parameter.
         * @param L The lua state object
         * @param name The name of the function in the lua context
         * @param f The actual C++ function
         */
        template <typename F>
        void registerFunctionN(lua_State* L, const std::string& name, F f, int2type<1>)
        {
            typedef decltype(f) function_type;

            using R = typename function_traits<function_type>::return_type;
            using T1 = typename function_traits<function_type>::template argument<0>::type;
            typedef FunctionObjectImpl1<F, R, T1> F1;

            FunctionObject* func = new F1(f);
            sFunctions[name] = func;
            lua_pushlightuserdata(L, func);
            lua_pushcclosure(L, F1::entry, 1);
            lua_setglobal(L, name.c_str());
        }

        /**
         * Helper function to register functions with two parameters.
         * @param L The lua state object
         * @param name The name of the function in the lua context
         * @param f The actual C++ function
         */
        template <typename F>
        void registerFunctionN(lua_State* L, const std::string& name, F f, int2type<2>)
        {
            typedef decltype(f) function_type;

            using R = typename function_traits<function_type>::return_type;
            using T1 = typename function_traits<function_type>::template argument<0>::type;
            using T2 = typename function_traits<function_type>::template argument<1>::type;
            typedef FunctionObjectImpl2<F, R, T1, T2> F2;

            FunctionObject* func = new F2(f);
            sFunctions[name] = func;
            lua_pushlightuserdata(L, func);
            lua_pushcclosure(L, F2::entry, 1);
            lua_setglobal(L, name.c_str());
        }

        /**
         * Helper function to register functions with three parameters.
         * @param L The lua state object
         * @param name The name of the function in the lua context
         * @param f The actual C++ function
         */
        template <typename F>
        void registerFunctionN(lua_State* L, const std::string& name, F f, int2type<3>)
        {
            typedef decltype(f) function_type;

            using R = typename function_traits<function_type>::return_type;
            using T1 = typename function_traits<function_type>::template argument<0>::type;
            using T2 = typename function_traits<function_type>::template argument<1>::type;
            using T3 = typename function_traits<function_type>::template argument<2>::type;
            typedef FunctionObjectImpl3<F, R, T1, T2, T3> F3;

            FunctionObject* func = new F3(f);
            sFunctions[name] = func;
            lua_pushlightuserdata(L, func);
            lua_pushcclosure(L, F3::entry, 1);
            lua_setglobal(L, name.c_str());
        }

        /**
         * Function to register C++ functions that shall be called from lua scripts.
         * @param L The lua state object
         * @param name The name of the function in the lua context
         * @param f The actual C++ function
         */
        template <typename F>
        void registerFunction(lua_State* L, const std::string& name, F f)
        {
            registerFunctionN(L, name, f, int2type<function_traits<decltype(f)>::arity>());
        }
    }
}

#endif
