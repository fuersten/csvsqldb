cmake_minimum_required(VERSION 3.16)
project(csvsqldb VERSION 0.2.0 LANGUAGES CXX)

set(CSVSQLDB_VERSION_MAJOR 0)
set(CSVSQLDB_VERSION_MINOR 2)
set(CSVSQLDB_VERSION_PATCH 0)

set(CSVSQLDB_VERSION ${CSVSQLDB_VERSION_MAJOR}.${CSVSQLDB_VERSION_MINOR}.${CSVSQLDB_VERSION_PATCH})
set(CSVSQLDB_VERSION_SHORT ${CSVSQLDB_VERSION_MAJOR}.${CSVSQLDB_VERSION_MINOR})
set(CSVSQLDB_VERSION_STRING ${CSVSQLDB_VERSION_MAJOR}.${CSVSQLDB_VERSION_MINOR}.${CSVSQLDB_VERSION_PATCH})
set(CSVSQLDB_VERSION_HEX 0x00000200)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    message("Setting up csvsqldb ${CSVSQLDB_VERSION_STRING} for a debug build")
elseif(CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    message("Setting up csvsqldb ${CSVSQLDB_VERSION_STRING} for a release with debug info build")
elseif(CMAKE_BUILD_TYPE MATCHES "Release")
    message("Setting up csvsqldb ${CSVSQLDB_VERSION_STRING} for a release build")
else()
    message(FATAL_ERROR "Unknown build type ${CMAKE_BUILD_TYPE} specified")
endif()

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/thirdparty)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

find_package(Lua51 REQUIRED)
find_package(Doxygen)
find_package(Boost 1.55 COMPONENTS filesystem program_options system regex REQUIRED)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find boost")
endif()

if(NOT LUA51_FOUND)
    message(FATAL_ERROR "Could not find LUA")
endif()

if(NOT WIN32)
    include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/libcsvsqldb ${CMAKE_CURRENT_BINARY_DIR})
    include_directories(SYSTEM ${LUA_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
    link_directories(/usr/local/lib ${Boost_LIBRARY_DIRS})
else()
    include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
    include_directories(SYSTEM ${LUA_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
    link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${LUA_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DDEBUG)
endif()

if(NOT APPLE AND UNIX)
    set(CSVSQLDB_PLATFORM_LIBS dl pthread)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Werror")

    option(USE_LIBCXX "link against libc++")
    if(USE_LIBCXX)
      set(CMAKE_C_COMPILER "clang")
      set(CMAKE_CXX_COMPILER "clang++")
      set(CSVSQLDB_PLATFORM_LIBS ${CSVSQLDB_PLATFORM_LIBS} c++abi)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem ${Boost_INCLUDE_DIR}")
      add_definitions(-DHAS_PUT_TIME -DHAS_GET_TIME)
    endif()

elseif(APPLE AND CMAKE_GENERATOR STREQUAL Xcode)
    message("Setting up Xcode for C++11 with libc++.")
    set(CSVSQLDB_PLATFORM_LIBS )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wheader-hygiene -Wcast-align -Wconversion -Wfloat-equal -Wformat=2
                         -Wmissing-declarations -Wmissing-prototypes -Woverlength-strings
                         -Wshadow -Wunreachable-code -Wextra
                         -Wno-unused-parameter -Wpedantic -Werror")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem ${Boost_INCLUDE_DIR}")
elseif(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wheader-hygiene -Wcast-align -Wconversion -Wfloat-equal -Wformat=2
                         -Wmissing-declarations -Wmissing-prototypes -Woverlength-strings
                         -Wshadow -Wunreachable-code -Wextra
                         -Wno-unused-parameter -Wpedantic -Werror")
                         SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem ${Boost_INCLUDE_DIR}")
elseif(WIN32)
    add_definitions(-DNOMINMAX)
    set(Boost_USE_STATIC_LIBS ON)
    set(CMAKE_CXX_FLAGS "-D_SCL_SECURE_NO_WARNINGS")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /EHsc")
    set(CSVSQLDB_PLATFORM_LIBS )
else()
    set(CSVSQLDB_PLATFORM_LIBS )
endif()

if(DOXYGEN_FOUND)
    add_custom_target(
        docs
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/csvsqldb.cfg
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif()

add_subdirectory(csvsqldb)
add_subdirectory(libcsvsqldb)
add_subdirectory(test)
