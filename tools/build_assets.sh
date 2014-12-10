#!/bin/bash

export ROOT_DIR=$(pwd)/..
export ASSETS_DIR=$ROOT_DIR/assets
export BUILD_DIR=$ROOT_DIR/builds
export SOURCE_DIR=$ROOT_DIR/source
export TOOLS_DIR=$ROOT_DIR/tools

mkdir -p $BUILD_DIR/assets/players/darksaber/sprite
cp $ASSETS_DIR/players/darksaber/sprite/idle.png $BUILD_DIR/assets/players/darksaber/sprite
cp $ASSETS_DIR/players/darksaber/sprite/idle.json $BUILD_DIR/assets/players/darksaber/sprite
cp $ASSETS_DIR/players/darksaber/sprite/walk.png $BUILD_DIR/assets/players/darksaber/sprite
cp $ASSETS_DIR/players/darksaber/sprite/walk.json $BUILD_DIR/assets/players/darksaber/sprite

cd $BUILD_DIR
zip -r Skunkworks.pak assets

mkdir -p $BUILD_DIR/linux-host-gcc-gnustl
mkdir -p $BUILD_DIR/linux-host-icc-gnustl
mkdir -p $BUILD_DIR/linux-host-clang-gnustl
mkdir -p $BUILD_DIR/android-armv7a-gcc-gnustl
cp $BUILD_DIR/Skunkworks.pak $BUILD_DIR/linux-host-gcc-gnustl
cp $BUILD_DIR/Skunkworks.pak $BUILD_DIR/linux-host-icc-gnustl
cp $BUILD_DIR/Skunkworks.pak $BUILD_DIR/linux-host-clang-gnustl
cp $SOURCE_DIR/AndroidManifest.xml $BUILD_DIR/android-armv7a-gcc-gnustl
rsync -av --progress $BUILD_DIR/assets $BUILD_DIR/android-armv7a-gcc-gnustl

cd $BUILD_DIR/android-armv7a-gcc-gnustl
android update project --name Skunkworks --path . --target "android-19"
ant debug

cd $TOOLS_DIR