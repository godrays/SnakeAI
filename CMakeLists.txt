#
#  Copyright © 2023-Present, Arkin Terli. All rights reserved.
#
#  NOTICE:  All information contained herein is, and remains the property of Arkin Terli.
#  The intellectual and technical concepts contained herein are proprietary to Arkin Terli
#  and may be covered by U.S. and Foreign Patents, patents in process, and are protected by
#  trade secret or copyright law. Dissemination of this information or reproduction of this
#  material is strictly forbidden unless prior written permission is obtained from Arkin Terli.

cmake_minimum_required(VERSION 3.24)

project(SnakeAI)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;CCov;ASan;TSan" CACHE STRING "" FORCE)

set(CMAKE_CXX_STANDARD 20)

# Set default build type as Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors -Wall -Wextra")    # Common parameters for debug and release
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_CCOV "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
set(CMAKE_CXX_FLAGS_ASAN "${CMAKE_CXX_FLAGS} -g -O1 -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_TSAN "${CMAKE_CXX_FLAGS} -g -O2 -fsanitize=thread -fPIE")

# Set external library versions
set(SFML_VERSION 2.6.x)
set(BROTLI_VERSION 1.1.0)
set(FREETYPE_VERSION 2-12-1)
set(ZLIB_VERSION 1.3.1)
set(BZIP2_VERSION master)
set(EIGEN_VERSION 3.4.0)
set(DOCOPT_VERSION 0.6.3)

# Include folders
include_directories(Externals/sfml/${SFML_VERSION}/installed/include)
include_directories(Externals/brotli/${BROTLI_VERSION}/installed/include)
include_directories(Externals/freetype/${FREETYPE_VERSION}/installed/include)
include_directories(Externals/zlib/${ZLIB_VERSION}/installed/include)
include_directories(Externals/bzip2/${BZIP2_VERSION}/installed/include)
include_directories(Externals/eigen/${EIGEN_VERSION}/installed/include/eigen3)
include_directories(Externals/docopt/${DOCOPT_VERSION}/installed/include)
include_directories(Targets/SnakeGameLib)

# Lib folders
link_directories(Externals/sfml/${SFML_VERSION}/installed/lib)
link_directories(Externals/brotli/${BROTLI_VERSION}/installed/lib)
link_directories(Externals/freetype/${FREETYPE_VERSION}/installed/lib)
link_directories(Externals/zlib/${ZLIB_VERSION}/installed/lib)
link_directories(Externals/bzip2/${BZIP2_VERSION}/installed/lib)
link_directories(Externals/docopt/${DOCOPT_VERSION}/installed/lib)

# Target folders
add_subdirectory(Targets/SnakeGameLib)
add_subdirectory(Targets/SnakeAIApp)
