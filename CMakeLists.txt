cmake_minimum_required(VERSION 3.16)
project(csvsqldb VERSION 0.2.0 LANGUAGES CXX)

if(POLICY CMP0110)
  cmake_policy(SET CMP0110 OLD)
endif()

enable_testing()

set(CSVSQLDB_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CSVSQLDB_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CSVSQLDB_VERSION_PATCH ${PROJECT_VERSION_PATCH})

set(CSVSQLDB_VERSION ${CSVSQLDB_VERSION_MAJOR}.${CSVSQLDB_VERSION_MINOR}.${CSVSQLDB_VERSION_PATCH})
set(CSVSQLDB_VERSION_SHORT ${CSVSQLDB_VERSION_MAJOR}.${CSVSQLDB_VERSION_MINOR})
set(CSVSQLDB_VERSION_STRING ${CSVSQLDB_VERSION_MAJOR}.${CSVSQLDB_VERSION_MINOR}.${CSVSQLDB_VERSION_PATCH})
set(CSVSQLDB_VERSION_HEX 0x00000200)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/archive)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

find_package(Lua REQUIRED)
find_package(Threads REQUIRED)

if(NOT LUA_FOUND)
    message(FATAL_ERROR "Could not find LUA")
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DDEBUG)
endif()

if(NOT APPLE AND UNIX)
    set(CSVSQLDB_PLATFORM_LIBS dl Threads::Threads)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Werror")

    option(USE_LIBCXX "link against libc++")
    if(USE_LIBCXX)
      set(CMAKE_C_COMPILER "clang")
      set(CMAKE_CXX_COMPILER "clang++")
      #set(CSVSQLDB_PLATFORM_LIBS ${CSVSQLDB_PLATFORM_LIBS} c++abi)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
      add_definitions(-DHAS_PUT_TIME -DHAS_GET_TIME)
    endif()
elseif(APPLE)
    set(CSVSQLDB_COMPILE_OPTIONS -Wall -Wheader-hygiene -Wcast-align -Wconversion -Wfloat-equal -Wformat=2 -Wmissing-declarations -Woverlength-strings -Wshadow -Wunreachable-code -Wextra -Wno-unused-parameter -Wpedantic -Werror)
elseif(WIN32)
    add_definitions(-DNOMINMAX -DHAS_PUT_TIME -DHAS_GET_TIME)
    set(CMAKE_CXX_FLAGS "-D_SCL_SECURE_NO_WARNINGS")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /EHsc")
else()
    message(FATAL_ERROR "Platform not supported!")
endif()

find_package(Doxygen OPTIONAL_COMPONENTS dot)

if(DOXYGEN_FOUND)
  set(DOXYGEN_PROJECT_BRIEF "A read only sql database that gets its data from supplied csv files.")
  set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc)
  set(DOXYGEN_STRIP_FROM_INC_PATH ${PROJECT_SOURCE_DIR}/csvsqldb)
  set(DOXYGEN_TAB_SIZE 2)
  set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
  set(DOXYGEN_INLINE_SIMPLE_STRUCTS YES)
  set(DOXYGEN_EXTRACT_ALL YES)
  set(DOXYGEN_EXTRACT_STATIC YES)
  set(DOXYGEN_WARN_NO_PARAMDOC YES)
  set(DOXYGEN_FILE_PATTERN "*.h *.cpp")
  set(DOXYGEN_RECURSIVE YES)
  set(DOXYGEN_EXCLUDE_PATTERNS "*/detail*")
  doxygen_add_docs(cmdoc
          ${CMAKE_SOURCE_DIR}/csvsqldb
          COMMENT "Generate HTML API documentation")
else(DOXYGEN_FOUND)
  message("Doxygen needs to be installed to generate the doxygen documentation")
endif()

add_subdirectory(apps)
add_subdirectory(csvsqldb)
add_subdirectory(test)
