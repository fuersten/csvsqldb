//
//  test.h
//  Based on MPF test framework
//
//  Created by Lars-Christian Fürstenberg on 07.04.13.
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

#ifndef MPF_TEST_H
#define MPF_TEST_H


#include "compat/regex.h"

#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <type_traits>
#include <functional>


namespace mpf
{
    namespace test
    {

        class AssertException : public std::runtime_error
        {
        public:
            explicit AssertException(const std::string& what)
            : std::runtime_error(what)
            {
            }
        };

/** \def MPF_TEST_ASSERT(Condition)
 *  A macro that throws an mpf::AssertException if the given condition is not met.<br>
 *  Example:
 *  @code
 *  MPF_TEST_ASSERT(test == false);
 *  @endcode
 */
#define MPF_TEST_ASSERT(Condition)                                                                                               \
    do {                                                                                                                         \
        if(!(Condition)) {                                                                                                       \
            std::stringstream XX_ss__;                                                                                           \
            XX_ss__ << "Condition was not met: " << #Condition << " [" << __FILE__ << " " << __LINE__ << "]";                    \
            throw mpf::test::AssertException(XX_ss__.str());                                                                     \
        }                                                                                                                        \
    } while(false)

/** \def MPF_TEST_ASSERT_MESSAGE(Message, Condition)
 *  A macro that throws an mpf::AssertException if the given condition is not met.
 *  The Message will be set as exception text for the mpf::AssertException.<br>
 *  Example:
 *  @code
 *  MPF_TEST_ASSERT_MESSAGE("TestException was expected but not detected", detected);
 *  @endcode
 */
#define MPF_TEST_ASSERT_MESSAGE(Message, Condition)                                                                              \
    do {                                                                                                                         \
        if(!(Condition)) {                                                                                                       \
            std::stringstream XX_ss__;                                                                                           \
            XX_ss__ << Message << "(" << #Condition << ") [" << __FILE__ << " " << __LINE__ << "]";                              \
            throw mpf::test::AssertException(XX_ss__.str());                                                                     \
        }                                                                                                                        \
    } while(false)

/** \def MPF_TEST_EXPECTS_START()
 *  This macro starts an exception expectation block. It has to be terminated by a MPF_TEST_EXPECTS_END macro.
 *  You dont need to open a new block between the macros, but it is recommended as it helps with automatic
 *  indentation.<br>
 *  Example:
 *  @code
 *  MPF_TEST_EXPECTS_START();
 *  {
 *      Testfunc();
 *  }
 *  MPF_TEST_EXPECTS_END(TestException);
 *  @endcode
 */
#define MPF_TEST_EXPECTS_START()                                                                                                 \
    do {                                                                                                                         \
        bool caught(false);                                                                                                      \
        try {

/** \def MPF_TEST_EXPECTS_END(Exception)
 *  This macro ends an exception expectation block. It has to be initiated by a MPF_TEST_EXPECTS_START macro.
 *  You dont need to open a new block between the macros, but it is recommended as it helps with automatic
 *  indentation. If the exception Exception is not thrown or another exception is thrown, an mpf::AssertException
 *  is thrown.<br>
 *  Example:
 *  @code
 *  MPF_TEST_EXPECTS_START();
 *  {
 *      Testfunc();
 *  }
 *  MPF_TEST_EXPECTS_END(TestException);
 *  @endcode
 */
#define MPF_TEST_EXPECTS_END(Exception)                                                                                          \
    }                                                                                                                            \
    catch(Exception&)                                                                                                            \
    {                                                                                                                            \
        caught = true;                                                                                                           \
    }                                                                                                                            \
    catch(std::exception & ex)                                                                                                   \
    {                                                                                                                            \
        std::stringstream XX_ss__;                                                                                               \
        XX_ss__ << "Expected to catch an exception of type: " << #Exception                                                      \
                << ", but caught other type with message: " << ex.what() << " [" << __FILE__ << " " << __LINE__ << "]";          \
        throw mpf::test::AssertException(XX_ss__.str());                                                                         \
    }                                                                                                                            \
    catch(...)                                                                                                                   \
    {                                                                                                                            \
        std::stringstream XX_ss__;                                                                                               \
        XX_ss__ << "Expected to catch an exception of type: " << #Exception << ", but caught an unknown exception. ["            \
                << __FILE__ << " " << __LINE__ << "]";                                                                           \
        throw mpf::test::AssertException(XX_ss__.str());                                                                         \
    }                                                                                                                            \
    MPF_TEST_ASSERT_MESSAGE(std::string("Should have caught an exception of type ") + #Exception, caught);                       \
    }                                                                                                                            \
    while(false)

/** \def MPF_TEST_EXPECTS(Code, Exception)
 *  This macro throws an mpf::AssertException if the exception Exception is not thrown or another exception is thrown
 *  by the Code given. It should be used only for simple method calls. Otherwise the exception expectation block
 *  macros MPF_TEST_EXPECTS_START should be used.<br>
 *  Example:
 *  @code
 *  MPF_TEST_EXPECTS(valdate.validate(), mpf::value::ValidatorException);
 *  @endcode
 */
#define MPF_TEST_EXPECTS(Code, Exception)                                                                                        \
    do {                                                                                                                         \
        bool caught(false);                                                                                                      \
        try {                                                                                                                    \
            (void)(Code);                                                                                                        \
        } catch(Exception&) {                                                                                                    \
            caught = true;                                                                                                       \
        } catch(std::exception & ex) {                                                                                           \
            std::stringstream XX_ss__;                                                                                           \
            XX_ss__ << "Expected to catch an exception of type: " << #Exception                                                  \
                    << ", but caught other type with message: " << ex.what() << " [" << __FILE__ << " " << __LINE__ << "]";      \
            throw mpf::test::AssertException(XX_ss__.str());                                                                     \
        } catch(...) {                                                                                                           \
            std::stringstream XX_ss__;                                                                                           \
            XX_ss__ << "Expected to catch an exception of type: " << #Exception << ", but caught an unknown exception. ["        \
                    << __FILE__ << " " << __LINE__ << "]";                                                                       \
            throw mpf::test::AssertException(XX_ss__.str());                                                                     \
        }                                                                                                                        \
        MPF_TEST_ASSERT_MESSAGE(std::string("Should have caught an exception of type ") + #Exception, caught);                   \
    } while(false)

        namespace detail
        {
            struct tag {
            };

            struct any {
                template <typename T>
                any(T const&);
            };

            tag operator<<(std::ostream&, any const&);

            typedef char yes;
            typedef char(&no)[2];

            no check(tag);

            template <typename T>
            yes check(T const&);

            template <typename T>
            struct is_output_streamable_impl {
                static typename std::remove_cv<typename std::remove_reference<T>::type>::type const& x;
                static const bool value = sizeof(check(std::cerr << x)) == sizeof(yes);
            };
        }

        template <class T>
        struct is_output_streamable : detail::is_output_streamable_impl<T> {
        };

#define MPF_TEST_ASSERTEQUAL(Expected, Actual)                                                                                   \
    do {                                                                                                                         \
        mpf::test::AssertEqual(Expected, Actual, #Expected, #Actual, __FILE__, __LINE__);                                        \
    } while(false)

        template <typename T1, typename T2, bool streamable = true>
        struct ComparisonHelper {
            static void output(const std::string& op,
                               const T1& expected,
                               const T2& actual,
                               const std::string& expectedString,
                               const std::string& actualString,
                               const std::string& file,
                               int line)
            {
                std::stringstream ss;
                ss << expectedString << " " << op << " " << actualString << " (expected: " << expected << ", actual: " << actual << ")"
                   << " [" << file << " " << line << "]";
                throw AssertException(ss.str());
            }
        };

        template <typename T1, typename T2>
        struct ComparisonHelper<T1, T2, false> {
            static void output(const std::string& op,
                               const T1& expected,
                               const T2& actual,
                               const std::string& expectedString,
                               const std::string& actualString,
                               const std::string& file,
                               int line)
            {
                std::stringstream ss;
                ss << expectedString << " " << op << " " << actualString << " [" << file << " " << line << "]";
                throw AssertException(ss.str());
            }
        };

        template <typename T1, typename T2>
        inline void
        AssertEqual(const T1& expected, const T2& actual, const std::string& expectedString, const std::string& actualString, const std::string& file, int line)
        {
            if(actual != expected) {
                ComparisonHelper<T1, T2, is_output_streamable<T1>::value && is_output_streamable<T2>::value>::output(
                "!=", expected, actual, expectedString, actualString, file, line);
            }
        }

        inline void AssertEqual(const std::string& expected,
                                const std::string& actual,
                                const std::string& expectedString,
                                const std::string& actualString,
                                const std::string& file,
                                int line)
        {
            if(actual != expected) {
                std::stringstream ss;
                ss << expectedString << " != " << actualString << " (expected: " << expected << ", actual: " << actual << ")"
                   << " [" << file << " " << line << "]";
                throw AssertException(ss.str());
            }
        }

#define MPF_TEST_ASSERTEQUALX(Expected, Actual)                                                                                  \
    do {                                                                                                                         \
        mpf::test::AssertEqualX(Expected, Actual, #Expected, #Actual, __FILE__, __LINE__);                                       \
    } while(false)

        inline std::string escapeString(const std::string& txt)
        {
            std::string result;
            char buffer[32];
            for(unsigned i = 0; i < txt.length(); ++i) {
                if(txt[i] < 32) {
                    sprintf(buffer, "\\x%02x", ((int)(unsigned char)(txt[i])));
                    result += buffer;
                } else {
                    result += txt[i];
                }
            }
            return result;
        }

        template <typename T1, typename T2>
        inline void
        AssertEqualX(const T1& expected, const T2& actual, const std::string& expectedString, const std::string& actualString, const std::string& file, int line)
        {
            if(actual != expected) {
                std::stringstream ss;
                ss << expectedString << " != " << actualString << " (expected: '" << escapeString(std::to_string(expected))
                   << "', actual: '" << escapeString(std::to_string(actual)) << "')"
                   << " [" << file << " " << line << "]";
                throw AssertException(ss.str());
            }
        }

        inline void AssertEqualX(const std::string& expected,
                                 const std::string& actual,
                                 const std::string& expectedString,
                                 const std::string& actualString,
                                 const std::string& file,
                                 int line)
        {
            if(actual != expected) {
                std::stringstream ss;
                ss << expectedString << " != " << actualString << " (expected: '" << escapeString(expected) << "', actual: '"
                   << escapeString(actual) << "')"
                   << " [" << file << " " << line << "]";
                throw AssertException(ss.str());
            }
        }

#define MPF_TEST_ASSERTNOTEQUAL(Expected, Actual)                                                                                \
    do {                                                                                                                         \
        mpf::test::AssertNotEqual(Expected, Actual, #Expected, #Actual, __FILE__, __LINE__);                                     \
    } while(false)

        template <typename T1, typename T2>
        inline void AssertNotEqual(
        const T1& expected, const T2& actual, const std::string& expectedString, const std::string& actualString, const std::string& file, int line)
        {
            if(actual == expected) {
                ComparisonHelper<T1, T2, is_output_streamable<T1>::value && is_output_streamable<T2>::value>::output(
                "==", expected, actual, expectedString, actualString, file, line);
            }
        }

        inline void AssertNotEqual(const std::string& expected,
                                   const std::string& actual,
                                   const std::string& expectedString,
                                   const std::string& actualString,
                                   const std::string& file,
                                   int line)
        {
            if(actual == expected) {
                std::stringstream ss;
                ss << expectedString << " == " << actualString << " (expected: " << expected << ", actual: " << actual << ")"
                   << " [" << file << " " << line << "]";
                throw AssertException(ss.str());
            }
        }


        class ProgressListener
        {
        public:
            typedef std::shared_ptr<ProgressListener> Ptr;
            typedef std::list<ProgressListener::Ptr> Listener;

            virtual ~ProgressListener()
            {
            }
            virtual void maxSuiteNameLength(size_t length)
            {
            }
            virtual void maxTestcaseNameLength(size_t length)
            {
            }
            virtual void testStart(size_t count)
            {
            }
            virtual void testEnd(size_t successful, size_t failed)
            {
            }
            virtual void suiteStart(const std::string& name, size_t count)
            {
            }
            virtual void suiteEnd(const std::string& name)
            {
            }
            virtual void testcaseStart(const std::string& name, size_t count)
            {
            }
            virtual void testcaseEnd(const std::string& name)
            {
            }
            virtual void fixtureStart(const std::string& name)
            {
            }
            virtual void fixtureEnd(const std::string& name, bool successful)
            {
            }
            virtual void fixtureAssert(const std::string& name, const std::string& message)
            {
            }

        protected:
            ProgressListener()
            {
            }
        };


        template <typename T>
        struct TestcaseFixture {
            typedef std::function<void(T*)> Function;
            typedef std::pair<std::string, Function> Item;

            static void run(T& testcase, Item item, const ProgressListener::Listener& progressListener)
            {
                try {
                    for(auto listener : progressListener) {
                        listener->fixtureStart(item.first);
                    }

                    testcase.setUp();

                    try {
                        item.second(&testcase);
                        for(auto listener : progressListener) {
                            listener->fixtureEnd(item.first, true);
                        }
                    } catch(AssertException& ex) {
                        for(auto listener : progressListener) {
                            listener->fixtureAssert(item.first, std::string("Assertion caught: ") + ex.what());
                            listener->fixtureEnd(item.first, false);
                        }
                    } catch(std::exception& ex) {
                        for(auto listener : progressListener) {
                            listener->fixtureAssert(item.first, std::string("Test failed: ") + ex.what());
                            listener->fixtureEnd(item.first, false);
                        }
                    } catch(...) {
                        for(auto listener : progressListener) {
                            listener->fixtureAssert(item.first, "Test failed with unknown reason");
                            listener->fixtureEnd(item.first, false);
                        }
                    }

                    try {
                        testcase.tearDown();
                    } catch(std::exception& ex) {
                        std::cerr << "tearDown failed. Test will not be performed. Reason: " << ex.what() << std::endl;
                    } catch(...) {
                        std::cerr << "tearDown failed. Test will not be performed." << std::endl;
                    }
                } catch(std::exception& ex) {
                    std::cerr << "setUp failed. Test will not be performed. Reason: " << ex.what() << std::endl;
                } catch(...) {
                    std::cerr << "setUp failed. Test will not be performed." << std::endl;
                }
            }
        };

        struct TestcaseHolder {
            typedef std::shared_ptr<TestcaseHolder> Ptr;

            virtual size_t run(const std::string& name, const ProgressListener::Listener& progressListener) = 0;
            virtual size_t count(const std::string& name) const = 0;

        protected:
            TestcaseHolder(const std::string& name)
            : _name(name)
            {
            }

            std::string _name;
        };

        template <typename T>
        class TestcaseGuard
        {
        public:
            TestcaseGuard()
            : _testcase(nullptr)
            {
            }

            ~TestcaseGuard()
            {
                try {
                    delete _testcase;
                } catch(std::exception& ex) {
                    std::cerr << "Testcase fini failed: " << ex.what() << std::endl;
                } catch(...) {
                    std::cerr << "Testcase fini failed" << std::endl;
                }
            }

            explicit operator bool() const
            {
                return _testcase ? true : false;
            }

            T* get()
            {
                return _testcase;
            }

            bool reset()
            {
                bool ret(false);

                try {
                    delete _testcase;
                } catch(std::exception& ex) {
                    std::cerr << "Testcase fini failed: " << ex.what() << std::endl;
                } catch(...) {
                    std::cerr << "Testcase fini failed" << std::endl;
                }

                try {
                    _testcase = new T;
                    ret = true;
                } catch(std::exception& ex) {
                    std::cerr << "Testcase init failed: " << ex.what() << std::endl;
                } catch(...) {
                    std::cerr << "Testcase init failed" << std::endl;
                }

                return ret;
            }

        private:
            T* _testcase;
        };

        template <typename T>
        struct Testcase : public TestcaseHolder {
            typedef std::shared_ptr<Testcase<T>> Ptr;
            typedef std::function<void(T*)> Function;
            typedef std::pair<std::string, Function> Item;
            typedef std::list<Item> TestFunctions;
            typedef typename TestFunctions::iterator iterator;

            Testcase(const std::string& name)
            : TestcaseHolder(name)
            {
            }

            virtual size_t run(const std::string& name, const ProgressListener::Listener& progressListener)
            {
                for(auto listener : progressListener) {
                    listener->testcaseStart(_name, count(name));
                }
                size_t testcount = 0;
                TestcaseGuard<T> testcase;
                boost::regex e(name);
                for(auto item : _functions) {
                    if(name.empty() || boost::regex_match(item.first, e)) {
                        if(!testcase && !testcase.reset()) {
                            break;
                        }
                        TestcaseFixture<T>::run(*testcase.get(), item, progressListener);
                        ++testcount;
                    }
                }
                for(auto listener : progressListener) {
                    listener->testcaseEnd(_name);
                }
                return testcount;
            }

            virtual size_t count(const std::string& name) const
            {
                size_t count(0);

                if(name.empty()) {
                    count = _functions.size();
                } else {
                    boost::regex e(name);
                    for(auto item : _functions) {
                        if(boost::regex_match(item.first, e)) {
                            ++count;
                        }
                    }
                }
                return count;
            }

            bool addTestFunction(const std::string& name, Function func)
            {
                _functions.push_back(std::make_pair(name, func));
                return true;
            }

        protected:
            TestFunctions _functions;
        };

        class TestSuite
        {
        public:
            typedef std::shared_ptr<TestSuite> Ptr;
            typedef std::list<Ptr> TestSuites;
            typedef std::list<TestcaseHolder::Ptr> Testcases;

            TestSuite(const std::string& name);
            ~TestSuite();

            void run(const std::string& name, const ProgressListener::Listener& progressListener);

            void addTestCase(const TestcaseHolder::Ptr& testcase);

            void addTestSuite(const TestSuite::Ptr& suite);

            const std::string& name() const;

            size_t count(const std::string& name) const;

            TestSuite::Ptr suite(const std::string& name) const;

        protected:
            class Private;
            Private* _m;
        };

        class TestRegistry
        {
        public:
            typedef std::function<void(void)> GlobalSetupFunction;
            typedef std::list<GlobalSetupFunction> GlobalSetupFunctions;
            typedef std::function<void(void)> GlobalTeardownFunction;
            typedef std::list<GlobalTeardownFunction> GlobalTeardownFunctions;

            static TestRegistry& instance();

            ~TestRegistry();

            void fini();

            bool run(const std::string& name, const ProgressListener::Listener& progressListener);

            size_t count(const std::string& name) const;

            bool addSetupFunctions(GlobalSetupFunction setup, GlobalTeardownFunction teardown);

            bool addTestcase(const std::string& suitePath, const TestcaseHolder::Ptr& testcase);

        private:
            typedef std::pair<std::string, TestSuite::Ptr> Suite;
            typedef std::map<std::string, TestSuite::Ptr> Testsuites;

            struct CountingProgressListener : ProgressListener {
                typedef std::shared_ptr<CountingProgressListener> Ptr;
                size_t _successful;
                size_t _failed;

                CountingProgressListener()
                : _successful(0)
                , _failed(0)
                {
                }
                virtual void fixtureEnd(const std::string& name, bool successful)
                {
                    if(successful) {
                        ++_successful;
                    } else {
                        ++_failed;
                    }
                }
            };

            TestRegistry();

            class Private;
            std::unique_ptr<Private> _m;
        };

        class BriefProgressListener : public mpf::test::ProgressListener
        {
        public:
            virtual void testStart(size_t count)
            {
                std::cerr << "Starting " << count << " tests:" << std::endl;
            }
            virtual void testEnd(size_t successful, size_t failed)
            {
                std::cerr << "Executed " << successful << " successful and " << failed << " failed tests" << std::endl;
            }
            virtual void suiteStart(const std::string& name, size_t count)
            {
            }
            virtual void suiteEnd(const std::string& name)
            {
                std::cerr << std::endl;
            }
            virtual void testcaseStart(const std::string& name, size_t count)
            {
                std::cerr << std::endl;
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
        };

#define MPF_REGISTER_TEST_START(MyTestSuite, MyTestcase)                                                                         \
    namespace registeredTestcase##MyTestcase                                                                                     \
    {                                                                                                                            \
        struct RegisteredTestcase : private MyTestcase {                                                                         \
            RegisteredTestcase()                                                                                                 \
            {                                                                                                                    \
                std::string suiteName = MyTestSuite;                                                                             \
            mpf::test::Testcase<MyTestcase>::Ptr tch = std::make_shared<mpf::test::Testcase<MyTestcase>>(#MyTestcase)


#define MPF_REGISTER_TEST(MyTestcaseFunc) tch->addTestFunction(#MyTestcaseFunc, std::mem_fn(&RegisteredTestcase::MyTestcaseFunc))


#define MPF_REGISTER_TEST_END()                                                                                                  \
    mpf::test::TestRegistry::instance().addTestcase(suiteName, tch);                                                             \
    }                                                                                                                            \
    }                                                                                                                            \
    ;                                                                                                                            \
    static RegisteredTestcase registeredTestcase;                                                                                \
    }


#define MPF_TEST_REGISTER_GLOBAL_SETUP(MySetup, MyTeardown)                                                                      \
    struct RegisterGlobalSetup##MySetup {                                                                                        \
        RegisterGlobalSetup##MySetup()                                                                                           \
        {                                                                                                                        \
            mpf::test::TestRegistry::instance().addSetupFunctions(MySetup, MyTeardown);                                          \
        }                                                                                                                        \
    };                                                                                                                           \
    static RegisterGlobalSetup##MySetup registerGlobalSetup_##MySetup

//----------------------------------------------------------------------

#define MPF_AUTO_TEST_SUITE_FIXTURE(MyTestSuite, MyTestCase)                                                                     \
    struct MyTestCase {                                                                                                          \
    };                                                                                                                           \
    namespace autoTestSuite##MyTestCase                                                                                          \
    {                                                                                                                            \
        static std::string suiteName = MyTestSuite;                                                                              \
    struct AutoTestClass : public MyTestCase


#define MPF_AUTO_TEST_CASE(testName)                                                                                             \
    struct testName : public AutoTestClass {                                                                                     \
        void testMethod();                                                                                                       \
    };                                                                                                                           \
    static mpf::test::Testcase<testName>::Ptr testName##_anchor = std::make_shared<mpf::test::Testcase<testName>>(#testName);    \
    static bool testName##_functionRegistered = testName##_anchor->addTestFunction(#testName, &testName::testMethod);            \
    static bool testName##_Suiteregistered = mpf::test::TestRegistry::instance().addTestcase(suiteName, testName##_anchor);      \
    void testName::testMethod()


#define MPF_AUTO_TEST_SUITE_END() }
    }
}

#endif    // MPF_TEST_H
