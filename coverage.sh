#!/bin/sh
mkdir build_coverage
cd build_coverage
find . -name "*.gcda" -exec rm \{} \;
rm -rf html || true
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -DCOVERAGE=ON ..
cmake --build . --config Debug
ctest

files=`find . -name "*.gcda" | xargs`
/Library/Developer/CommandLineTools/usr/bin/llvm-cov gcov -f -b $files

lcov --directory . --base-directory . --gcov-tool ../llvm-gcov.sh --capture -o cov.info
lcov --remove cov.info '*/usr/include/*' '*test*' '*thirdparty*' -o coverage.info
cat coverage.info | c++filt >coverage_filtered.info
genhtml coverage_filtered.info --output-directory html

