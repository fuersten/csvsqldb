# Building and installation
## Mac OS X 10.7 and newer
- You will need the following software packages
  - lua-5.1 or higher
  - cmake-2.8 or higher

- Create a build directory in the source directory of csvsqldb and change to it
- Call `cmake -DCMAKE_BUILD_TYPE=Release ..`
- Call `make`

## Linux
- You will need the following software packages
  - lua-5.1 or higher
  - cmake-2.8 or higher
  - clang++ 3.4 or higher compiler or g++ 4.8 or higher
  - libc++ 3.4 or higher

- Create a build directory in the source directory of csvsqldb and change to it
- Call `cmake -DCMAKE_BUILD_TYPE=Release ..`
- Call `make`

## Windows
- You will need the following software packages
  - lua-5.1 or higher
  - cmake-2.8 or higher
  - Visual Studio 2013 or newer
  - The Express version will also work

- Precompiled Lua binaries can be obtained from  [http://sourceforge.net/projects/luabinaries/files/?source=navbar](http://sourceforge.net/projects/luabinaries/files/?source=navbar)
- CMake can be downloaded from [http://www.cmake.org/download](http://www.cmake.org/download)
- Create a build directory in the source directory of csvsqldb and change to it
- Now you have to tell cmake, where the lua libraries are
- `SET LUA_DIR=<path to the Lua root directory>`
- Call `cmake -DCMAKE_BUILD_TYPE=Release ..`
- Call `make`
