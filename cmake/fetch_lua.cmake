FetchContent_Declare(
  lua
  URL https://github.com/lua/lua/archive/refs/tags/v5.4.3.tar.gz
)

FetchContent_GetProperties(lua)
if(NOT lua_POPULATED)
  FetchContent_Populate(lua)
endif()

