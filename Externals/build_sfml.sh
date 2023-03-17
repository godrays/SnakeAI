#!/bin/bash

#
#  Copyright (c) 2023-Present, Arkin Terli. All rights reserved.
#

lib_name=sfml
lib_version=2.6.x
lib_url=https://github.com/SFML/SFML.git

pushd .

rm -rf $lib_name

git clone --recurse-submodules -b $lib_version $lib_url ./$lib_name/$lib_version
cd $lib_name/$lib_version

rm -rf build
mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release -DSFML_BUILD_FRAMEWORKS=FALSE -DBUILD_SHARED_LIBS=FALSE -DCMAKE_INSTALL_PREFIX=../installed
cmake --build . --target install -- -j `nproc --all`

cd ..
rm -rf .git .gitattributes .github .gitignore build

popd
