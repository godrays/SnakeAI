#!/bin/bash

#
#  Copyright (c) 2020-Present, Arkin Terli. All rights reserved.
#

lib_name=docopt
lib_version=0.6.3
lib_url=https://github.com/docopt/docopt.cpp.git

pushd .

git clone --recurse-submodules -b v$lib_version $lib_url ./$lib_name/$lib_version
cd $lib_name/$lib_version

rm -rf build
rm -rf installed
mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../installed
cmake --build . --target install -- -j `nproc --all`

cd ..
rm -rf installed/lib/*.dylib    # Remove dynamic libs to force linker to choose static lib only.
rm -rf .git .gitattributes .github .gitignore build

popd
