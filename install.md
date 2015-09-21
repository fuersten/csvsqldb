# Building and installation
## Mac OS X 10.7 and newer
- You will need the following software packages
  - boost-1.53 or higher
  - lua-5.1 or higher
  - readline-6.0 or higher (only for the command line interface)
  - cmake-2.8 or higher

- The easiest way to install these packages is using homebrew
- Create a build directory in the source directory of csvsqldb and change to it
- You have to tell cmake where to find the brew readline version, as Mac OS X has
- an older version `export CMAKE_PREFIX_PATH=/usr/local/Cellar/readline/6.3.8`
- Call `cmake -DCMAKE_BUILD_TYPE=Release ..`
- Call `make`

## Linux
- You will need the following software packages
  - boost-1.53 or higher
  - lua-5.1 or higher
  - readline-6.0 or higher (only for the command line interface)
  - cmake-2.8 or higher
  - clang++ 3.4 or higher compiler or g++ 4.8 or higher
  - libc++ 3.4 or higher

- If you use the clang compier you cannot use the standard boost package of your distro, as boost has to be build with the clang compiler and the next steps are only necessary if you want to use the clang compiler
  - So download boost from [http://sourceforge.net/projects/boost/files/boost/](http://sourceforge.net/projects/boost/files/boost/)
  - Unpack the archive and change into the directory
  - Call `./bootstrap.sh --with-toolset=clang --with-libraries=filesystem,program_options,system`
  - You can add more libraries if you like
  - Call `./b2 cxxflags="-stdlib=libc++" linkflags="-stdlib=libc++"`
  - I don't recommend to install the boost libraries, as you will probably have trouble to build other software packages with these libraries
- Create a build directory in the source directory of csvsqldb and change to it
- Now you have to tell cmake, where the recently build boost libraries are. The previous boost
- build step will output the right directories for you.
- `export BOOST_ROOT=<path to the boost root directory>`
- `export BOOST_LIBRARYDIR=<path to the boost library directory>`
- Call `cmake -DCMAKE_BUILD_TYPE=Release ..`
- Call `make`

## Windows
- You will need the following software packages
  - boost-1.53 or higher
  - lua-5.1 or higher
  - readline-5.0 or higher (only for the command line interface)
  - cmake-2.8 or higher
  - Visual Studio 2013 or newer
  - The Express version will also work

- Precompiled boost libraries can be obtained from  [http://sourceforge.net/projects/boost/files/boost-binaries](http://sourceforge.net/projects/boost/files/boost-binaries)
- Precompiled Lua binaries can be obtained from  [http://sourceforge.net/projects/luabinaries/files/?source=navbar](http://sourceforge.net/projects/luabinaries/files/?source=navbar)
- Precompiled Windows binaries for a readline port can be obtained from [http://gnuwin32.sourceforge.net/packages/readline.htm](http://gnuwin32.sourceforge.net/packages/readline.htm)
- CMake can be downloaded from [http://www.cmake.org/download](http://www.cmake.org/download)
- Create a build directory in the source directory of csvsqldb and change to it
- Now you have to tell cmake, where the boost, lua and readline libraries are
- `SET BOOST_ROOT=<path to the boost root directory>`
- `SET BOOST_LIBRARYDIR=<path to the boost library directory>`
- `SET LUA_DIR=<path to the Lua root directory>`
- `SET Readline_ROOT_DIR=<path to the readline root directory>`
- Call `cmake -DCMAKE_BUILD_TYPE=Release ..`
- Call `make`
