#!/bin/bash

#
#  Copyright (c) 2023-Present, Arkin Terli. All rights reserved.
#

lib_name=freetype
lib_version=2-12-1
lib_url=https://github.com/freetype/freetype.git

pushd .

rm -rf $lib_name

git clone --recurse-submodules -b VER-$lib_version $lib_url ./$lib_name/$lib_version
cd $lib_name/$lib_version

rm -rf build
mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=FALSE -DCMAKE_INSTALL_PREFIX=../installed
cmake --build . --target install -- -j `nproc --all`

cd ..
rm -rf .git .gitattributes .github .gitignore build

popd
