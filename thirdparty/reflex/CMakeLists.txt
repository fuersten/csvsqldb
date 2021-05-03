cmake_minimum_required(VERSION 3.16)
project(libreflex LANGUAGES CXX)


add_library(reflex STATIC 
  ${reflex_SOURCE_DIR}/lib/convert.cpp
  ${reflex_SOURCE_DIR}/lib/debug.cpp
  ${reflex_SOURCE_DIR}/lib/error.cpp
  ${reflex_SOURCE_DIR}/lib/input.cpp
  ${reflex_SOURCE_DIR}/lib/matcher.cpp
  ${reflex_SOURCE_DIR}/lib/pattern.cpp
  ${reflex_SOURCE_DIR}/lib/posix.cpp
  ${reflex_SOURCE_DIR}/lib/unicode.cpp
  ${reflex_SOURCE_DIR}/lib/utf8.cpp

  ${reflex_SOURCE_DIR}/unicode/block_scripts.cpp	
  ${reflex_SOURCE_DIR}/unicode/language_scripts.cpp
  ${reflex_SOURCE_DIR}/unicode/letter_scripts.cpp
)

target_include_directories(reflex SYSTEM PRIVATE ${reflex_SOURCE_DIR}/include)

if(NOT WIN32)
  set_property(TARGET reflex PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()

