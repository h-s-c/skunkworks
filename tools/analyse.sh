#!/bin/bash

export ROOT_DIR=$(pwd)/..
export ASSETS_DIR=$ROOT_DIR/assets
export BUILD_DIR=$ROOT_DIR/build
export SOURCE_DIR=$ROOT_DIR/source
export TOOLS_DIR=$ROOT_DIR/tools

mkdir -p $BUILD_DIR/linux-host-clang_analyzer-libstdc++

cd $BUILD_DIR/linux-host-clang_analyzer-libstdc++
export CCC_CC=clang
export CCC_CXX=clang++
scan-build cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$(pwd) -GNinja $SOURCE_DIR
scan-build ninja

cd $TOOLS_DIR