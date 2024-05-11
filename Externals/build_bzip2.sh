#!/bin/bash

#
#  Copyright (c) 2023-Present, Arkin Terli. All rights reserved.
#

lib_name=bzip2
lib_version=master
lib_url=https://github.com/libarchive/bzip2.git
logical_core_count=$(nproc 2>/dev/null || sysctl -n hw.logicalcpu)

pushd .

rm -rf $lib_name

git clone --recurse-submodules -b $lib_version $lib_url ./$lib_name/$lib_version
cd $lib_name/$lib_version

rm -rf build
mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_STATIC_LIB=TRUE -DENABLE_SHARED_LIB=FALSE -DCMAKE_INSTALL_PREFIX=../installed
cmake --build . --target install -- -j $logical_core_count

cd ..
rm -rf .git .gitattributes .github .gitignore build

popd
