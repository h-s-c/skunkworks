#!/bin/bash

export SOURCE_DIR=$(pwd)

mkdir build 
cd build

mkdir linux-gcc-libstdc++
cd linux-gcc-libstdc++
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$SOURCE_DIR/../binaries/linux-gcc-libstdc++ -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$SOURCE_DIR/../binaries/linux-gcc-libstdc++ -GNinja $SOURCE_DIR
ninja
cd ..

mkdir linux-clang-libstdc++
cd linux-clang-libstdc++
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$SOURCE_DIR/../binaries/linux-clang-libstdc++ -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$SOURCE_DIR/../binaries/linux-clang-libstdc++ -GNinja $SOURCE_DIR
ninja
cd ..

mkdir android-gcc-libstdc++
cd android-gcc-libstdc++
unset ANDROID_NDK
export ANDROID_STANDALONE_TOOLCHAIN=$SOURCE_DIR/toolchain
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=$SOURCE_DIR/cmake/ToolchainANDROID.cmake -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$SOURCE_DIR/../binaries/android-gcc-libstdc++ -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$SOURCE_DIR/../binaries/android-gcc-libstdc++ -GNinja $SOURCE_DIR
ninja
cd ..

cd ..
