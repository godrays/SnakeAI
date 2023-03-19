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

set(CMAKE_CONFIGURATION_TYPES "CCov;ASan;TSan" CACHE STRING "" FORCE)

set(CMAKE_CXX_STANDARD 20)

# Set default build type as Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic-errors -Wall -Wextra")    # Common parameters for debug and release
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_CCOV "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
set(CMAKE_CXX_FLAGS_ASAN "${CMAKE_CXX_FLAGS} -g -O1 -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_TSAN "${CMAKE_CXX_FLAGS} -g -O2 -fsanitize=thread -fPIE")

# Include folders
include_directories(Externals/sfml/2.6.x/installed/include)
include_directories(Externals/freetype/2-12-1/installed/include)
include_directories(Externals/zlib/1.2.13/installed/include)
include_directories(Externals/bzip2/master/installed/include)
include_directories(Externals/eigen/3.4.0/installed/include/eigen3)
include_directories(Externals/docopt/0.6.3/installed/include)
include_directories(Targets/SnakeGameLib)

# Lib folders
link_directories(Externals/sfml/2.6.x/installed/lib)
link_directories(Externals/freetype/2-12-1/installed/lib)
link_directories(Externals/zlib/1.2.13/installed/lib)
link_directories(Externals/bzip2/master/installed/lib)
link_directories(Externals/docopt/0.6.3/installed/lib)


# Target folders
add_subdirectory(Targets/SnakeGameLib)
add_subdirectory(Targets/SnakeAIApp)
