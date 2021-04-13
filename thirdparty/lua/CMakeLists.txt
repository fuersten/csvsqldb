cmake_minimum_required(VERSION 3.16)
project(liblua)

if(NOT APPLE AND UNIX)
  set(CMAKE_C_FLAGS "-DLUA_USE_LINUX")
elseif(APPLE)
  set(CMAKE_C_FLAGS "-DLUA_USE_MACOSX")
elseif(WIN32)
endif()

add_library(lua STATIC 
 ${lua_SOURCE_DIR}/lapi.c
 ${lua_SOURCE_DIR}/lauxlib.c
 ${lua_SOURCE_DIR}/lbaselib.c
 ${lua_SOURCE_DIR}/lcode.c
 ${lua_SOURCE_DIR}/lcorolib.c
 ${lua_SOURCE_DIR}/lctype.c
 ${lua_SOURCE_DIR}/ldblib.c
 ${lua_SOURCE_DIR}/ldebug.c
 ${lua_SOURCE_DIR}/ldo.c
 ${lua_SOURCE_DIR}/ldump.c
 ${lua_SOURCE_DIR}/lfunc.c
 ${lua_SOURCE_DIR}/lgc.c
 ${lua_SOURCE_DIR}/linit.c
 ${lua_SOURCE_DIR}/liolib.c
 ${lua_SOURCE_DIR}/llex.c
 ${lua_SOURCE_DIR}/lmathlib.c
 ${lua_SOURCE_DIR}/lmem.c
 ${lua_SOURCE_DIR}/loadlib.c
 ${lua_SOURCE_DIR}/lobject.c
 ${lua_SOURCE_DIR}/lopcodes.c
 ${lua_SOURCE_DIR}/loslib.c
 ${lua_SOURCE_DIR}/lparser.c
 ${lua_SOURCE_DIR}/lstate.c
 ${lua_SOURCE_DIR}/lstring.c
 ${lua_SOURCE_DIR}/lstrlib.c
 ${lua_SOURCE_DIR}/ltable.c
 ${lua_SOURCE_DIR}/ltablib.c
 ${lua_SOURCE_DIR}/ltests.c
 ${lua_SOURCE_DIR}/ltm.c
 ${lua_SOURCE_DIR}/lundump.c
 ${lua_SOURCE_DIR}/lutf8lib.c
 ${lua_SOURCE_DIR}/lvm.c
 ${lua_SOURCE_DIR}/lzio.c
)

set_property(TARGET lua PROPERTY C_STANDARD 99)

target_include_directories(lua SYSTEM PRIVATE ${lua_SOURCE_DIR})
if(NOT WIN32)
  set_property(TARGET lua PROPERTY POSITION_INDEPENDENT_CODE ON)
  target_link_libraries(lua m)
endif()

