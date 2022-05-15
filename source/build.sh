#!/bin/bash

set -e

if [ "$#" -eq 0 ]; then
    echo "Usage: build.sh pc|mcu [debug|release]"
    exit 1
fi

ARCH=$1
BUILD=$2
CMAKE_ARGS=""
BUILD_DIR="build"

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

if [ "$ARCH" == "pc" ];
then
	echo "Building PC configuration"
	BUILD_DIR="${BUILD_DIR}-pc"
elif [ "$ARCH" == "stm" ];
then
	echo "Building STM32 configuration"
	CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=$SCRIPT_DIR/stm32f407.cmake"
	BUILD_DIR="${BUILD_DIR}-stm32"
else
	echo "Invalid architecture: $ARCH"
	exit 1
fi

if [ -z "$BUILD" ]
then
    BUILD="debug"
fi

if [ "$BUILD" == "debug" ];
then
	CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_BUILD_TYPE=Debug"
	BUILD_DIR="${BUILD_DIR}-debug"
elif [ "$BUILD" == "release" ];
then
	CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release"
	BUILD_DIR="${BUILD_DIR}-release"
else
	echo "Invalid build type: $BUILD"
	exit 1
fi

mkdir -p $BUILD_DIR
cd $BUILD_DIR
echo cmake "$CMAKE_ARGS" ..
cmake $CMAKE_ARGS ..
make -j`nproc --ignore=2`
