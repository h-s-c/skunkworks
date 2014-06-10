#!/bin/bash

mkdir build 
cd build

mkdir linux-gcc-libstdc++
cd linux-gcc-libstdc++
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=../../../binaries/linux-gcc-libstdc++ -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=../../../../../binaries/linux-gcc-libstdc++ -GNinja ../..
ninja
cd ..

mkdir linux-clang-libstdc++
cd linux-clang-libstdc++
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=../../../binaries/linux-clang-libstdc++ -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=../../../../../binaries/linux-clang-libstdc++ -GNinja ../..
ninja
cd ..

mkdir android-gcc-libstdc++
cd android-gcc-libstdc++
export ANDROID_NATIVE_API_LEVEL=android-9
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=../../cmake/ToolchainANDROID.cmake -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=../../../binaries/android-gcc-libstdc++ -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=../../../../../binaries/android-gcc-libstdc++ -GNinja ../..
ninja
cd ..

cd ..
