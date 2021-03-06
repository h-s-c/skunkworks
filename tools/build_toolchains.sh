#!/bin/bash

export ROOT_DIR=$(pwd)/..
export ASSETS_DIR=$ROOT_DIR/assets
export BUILDS_DIR=$ROOT_DIR/builds
export TOOLCHAINS_DIR=$ROOT_DIR/toolchains
export SOURCE_DIR=$ROOT_DIR/source
export TOOLS_DIR=$ROOT_DIR/tools

mkdir -p $TOOLCHAINS_DIR/android-armv7a-gcc-gnustl
cd $TOOLCHAINS_DIR/android-armv7a-gcc-gnustl

${ANDROID_NDK}/build/tools/make-standalone-toolchain.sh --platform=android-19  --toolchain=arm-linux-androideabi-4.9 --install-dir=. --system=linux-x86_64

mkdir prebuilt
cp -R ${ANDROID_NDK}/prebuilt/android-arm ./prebuilt

export PATH=$(pwd)/bin:$PATH
export OUTPUT_DIR=$(pwd)/sysroot/usr
export TOOLCHAIN_DIR=$(pwd)

mkdir tmp
cd tmp

git clone https://github.com/zeromq/zeromq4-x.git
cd zeromq4-x
sed -i '1iLIBS = -lgcc -lsupc++ -lstdc++' ./tools/Makefile.am
./autogen.sh
./configure --enable-static --disable-shared --host=arm-linux-androideabi --prefix=$OUTPUT_DIR LDFLAGS="-L$TOOLCHAIN_DIR/lib" CPPFLAGS="-fPIC -I$TOOLCHAIN_DIR/include" LIBS="-lgcc"
make
make install
cd ..
rm -rf zeromq4-x

git clone https://github.com/msgpack/msgpack-c.git
cd msgpack-c
./bootstrap
./configure --enable-static --disable-shared --host=arm-linux-androideabi --prefix=$OUTPUT_DIR LDFLAGS="-L$TOOLCHAIN_DIR/lib" CPPFLAGS="-fPIC -I$TOOLCHAIN_DIR/include" LIBS="-lgcc"
make
make install
cd ..
rm -rf msgpack-c

cd $TOOLS_DIR
