#!/bin/bash

mkdir build 
cd build

mkdir linux-gcc-libstdc++
cd linux-gcc-libstdc++
export TOOLCHAIN=$HOME/Toolchains/linux_x86-64/
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=../../cmake/ToolchainLINUX.cmake -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=../../../binaries/linux-gcc-libstdc++ -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=../../../../../binaries/linux-gcc-libstdc++ -GNinja ../..
ninja
cd ..

cd ..
