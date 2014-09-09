#!/bin/bash

export SRC_DIR=$(pwd)
export BIN_DIR=$(pwd)/../binaries

mkdir build 
cd build

mkdir linux-host-gcc-libstdc++
cd linux-host-gcc-libstdc++
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$BIN_DIR/linux-host-gcc-libstdc++ -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$BIN_DIR/linux-host-gcc-libstdc++ -GNinja $SRC_DIR
ninja
cd ..

mkdir linux-host-clang-libstdc++
cd linux-host-clang-libstdc++
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$BIN_DIR/linux-host-clang-libstdc++ -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$BIN_DIR/linux-host-clang-libstdc++ -GNinja $SRC_DIR
ninja
cd ..

mkdir android-armv7a-gcc-libstdc++
cd android-armv7a-gcc-libstdc++
unset ANDROID_NDK
export ANDROID_STANDALONE_TOOLCHAIN=$SRC_DIR/toolchain
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=$SRC_DIR/cmake/ToolchainANDROID.cmake -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$(pwd)/libs/armeabi-v7a  -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$(pwd)/libs/armeabi-v7a  -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=$(pwd)/libs/armeabi-v7a -GNinja $SRC_DIR
ninja
cp $SRC_DIR/AndroidManifest.xml .
android update project --name Skunkworks --path . --target "android-19"
ant debug
mkdir $BIN_DIR/android-armv7a-gcc-libstdc++
cp $(pwd)/bin/Skunkworks-debug.apk $BIN_DIR/android-armv7a-gcc-libstdc++
cd ..

cd ..
