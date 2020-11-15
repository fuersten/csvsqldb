#!/bin/bash

clang-format -i test/*.cpp
clang-format -i test/*.h
clang-format -i csvsqldb/*.cpp
clang-format -i csvsqldb/*.h
clang-format -i libcsvsqldb/*.cpp
clang-format -i libcsvsqldb/*.h
clang-format -i libcsvsqldb/base/*.cpp
clang-format -i libcsvsqldb/base/*.h
clang-format -i libcsvsqldb/base/detail/*.h
clang-format -i libcsvsqldb/base/detail/posix/*.cpp
clang-format -i libcsvsqldb/base/detail/windows/*.cpp

