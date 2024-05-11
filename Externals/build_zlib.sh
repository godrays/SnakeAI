#!/bin/bash

#
#  Copyright (c) 2023-Present, Arkin Terli. All rights reserved.
#

lib_name=zlib
lib_version=1.3.1
lib_url=https://github.com/madler/zlib.git
logical_core_count=$(nproc 2>/dev/null || sysctl -n hw.logicalcpu)

pushd .

rm -rf $lib_name

git clone --recurse-submodules -b v$lib_version $lib_url ./$lib_name/$lib_version
cd $lib_name/$lib_version

rm -rf build
mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=FALSE -DCMAKE_INSTALL_PREFIX=../installed
cmake --build . --target install -- -j $logical_core_count

cd ..
rm -rf installed/lib/*.dylib        # Remove dynamic libs to force linker to choose static lib only.
rm -rf .git .gitattributes .github .gitignore build

popd
