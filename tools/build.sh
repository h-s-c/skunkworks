#!/bin/bash

export ROOT_DIR=$(pwd)/..
export ASSETS_DIR=$ROOT_DIR/assets
export BUILDS_DIR=$ROOT_DIR/builds
export TOOLCHAINS_DIR=$ROOT_DIR/toolchains
export SOURCE_DIR=$ROOT_DIR/source
export TOOLS_DIR=$ROOT_DIR/tools

mkdir -p $BUILDS_DIR/linux-host-gcc-gnustl
mkdir -p $BUILDS_DIR/linux-host-icc-gnustl
mkdir -p $BUILDS_DIR/linux-host-clang-gnustl
mkdir -p $BUILDS_DIR/linux-host-clang_analyzer-gnustl
mkdir -p $BUILDS_DIR/android-armv7a-gcc-gnustl
mkdir -p $BUILDS_DIR/blackberry-armv7a-gcc-gnustl

cd $BUILDS_DIR/linux-host-gcc-gnustl
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$(pwd) -GNinja $SOURCE_DIR
ninja

#cd $BUILDS_DIR/linux-host-icc-gnustl
#cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=icc -DCMAKE_CXX_COMPILER=icpc -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$(pwd) -GNinja $SOURCE_DIR
#ninja

cd $BUILDS_DIR/linux-host-clang-gnustl
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$(pwd) -GNinja $SOURCE_DIR
ninja

#cd $BUILDS_DIR/android-armv7a-gcc-gnustl
#unset ANDROID_NDK
#export ANDROID_STANDALONE_TOOLCHAIN=$TOOLCHAINS_DIR/android-armv7a-gcc-gnustl
#cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=$SOURCE_DIR/cmake/ToolchainANDROID.cmake -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$(pwd)/libs/armeabi-v7a  -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$(pwd)/libs/armeabi-v7a  -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=$(pwd)/libs/armeabi-v7a -GNinja $SOURCE_DIR
#ninja

#cd $BUILDS_DIR/blackberry-armv7a-gcc-gnustl
#source $TOOLCHAINS_DIR/bbndk/bbndk-env_10_3_0_698.sh
#cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=$SOURCE_DIR/cmake/ToolchainBLACKBERRY.cmake -DBLACKBERRY_USE_GCC_4_8=1 -DBLACKBERRY_ARCHITECTURE=arm -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$(pwd) -GNinja $SOURCE_DIR
#ninja

cd $TOOLS_DIR