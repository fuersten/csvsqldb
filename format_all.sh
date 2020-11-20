#!/bin/bash

clang-format -i test/*.cpp
clang-format -i test/*.h
clang-format -i csvsqldb/*.cpp
clang-format -i csvsqldb/*.h
clang-format -i csvsqldb/*.cpp
clang-format -i csvsqldb/*.h
clang-format -i csvsqldb/base/*.cpp
clang-format -i csvsqldb/base/*.h
clang-format -i csvsqldb/base/detail/*.h
clang-format -i csvsqldb/base/detail/posix/*.cpp
clang-format -i csvsqldb/base/detail/windows/*.cpp

