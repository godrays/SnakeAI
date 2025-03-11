#
#  Copyright © 2024-Present, Arkin Terli. All rights reserved.
#
#  NOTICE:  All information contained herein is, and remains the property of Arkin Terli.
#  The intellectual and technical concepts contained herein are proprietary to Arkin Terli
#  and may be covered by U.S. and Foreign Patents, patents in process, and are protected by
#  trade secret or copyright law. Dissemination of this information or reproduction of this
#  material is strictly forbidden unless prior written permission is obtained from Arkin Terli.

include(ExternalProject)

# ---------------------------------------------------------------------------------
# HELPER FUNCTIONS
# ---------------------------------------------------------------------------------

# Builds and installs external git projects.
function(add_external_git_project lib_name git_repository git_tag cmake_project_args external_bin_dir build_type)
    message(STATUS "Configuring External Project: ${lib_name}")
    ExternalProject_Add(
            ${lib_name}
            GIT_REPOSITORY ${git_repository}
            GIT_TAG        ${git_tag}
            PREFIX        "${external_bin_dir}/${lib_name}/prefix"
            SOURCE_DIR    "${external_bin_dir}/${lib_name}/src"
            STAMP_DIR     "${external_bin_dir}/${lib_name}/stamp"
            BINARY_DIR    "${external_bin_dir}/${lib_name}/build"
            INSTALL_DIR   "${external_bin_dir}/${lib_name}/install"
            DOWNLOAD_DIR  "${external_bin_dir}/${lib_name}/download"
            LOG_DIR       "${external_bin_dir}/${lib_name}/log"
            CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${build_type}
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            ${cmake_project_args}       # Project Build Options
            LOG_CONFIGURE ON
            LOG_BUILD ON
            LOG_INSTALL ON
            LOG_UPDATE ON
            LOG_PATCH ON
            LOG_TEST ON
            LOG_MERGED_STDOUTERR ON
            LOG_OUTPUT_ON_FAILURE ON
            GIT_SUBMODULES_RECURSE ON
            GIT_PROGRESS OFF
            BUILD_ALWAYS YES
    )
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${external_bin_dir}/${lib_name}")
    include_directories(${external_bin_dir}/${lib_name}/install/include)
    link_directories(${external_bin_dir}/${lib_name}/install/lib)
endfunction()

# ---------------------------------------------------------------------------------
# COMMON SETTINGS
# ---------------------------------------------------------------------------------

# Common cmake project settings for the external projects.
set(EXTERNAL_COMMON_CMAKE_ARGS
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
        -DCMAKE_POSITION_INDEPENDENT_CODE=${CMAKE_POSITION_INDEPENDENT_CODE}
        -DCMAKE_BUILD_WITH_INSTALL_RPATH=${CMAKE_BUILD_WITH_INSTALL_RPATH}
        -DCMAKE_INSTALL_RPATH=${CMAKE_INSTALL_RPATH}
)

# Externals build and install folder.
set(EXTERNALS_BINARY_DIR "${CMAKE_BINARY_DIR}/Externals")

# ---------------------------------------------------------------------------------
# DOCOPT CPP
# ---------------------------------------------------------------------------------
set(EXTERNAL_DOCOPT_CMAKE_ARGS
        ${EXTERNAL_COMMON_CMAKE_ARGS}
        # Project specific cmake args
        -DBUILD_SHARED_LIBS=OFF
)

add_external_git_project(
        "docopt_cpp"
        "https://github.com/docopt/docopt.cpp.git"
        "${EXTERNAL_DOCOPT_VERSION}"
        "${EXTERNAL_DOCOPT_CMAKE_ARGS}"
        "${EXTERNALS_BINARY_DIR}"
        "Release"
)

# ---------------------------------------------------------------------------------
# EIGEN CPP
# ---------------------------------------------------------------------------------
add_external_git_project(
        "eigen_cpp"
        "https://github.com/live-clones/eigen.git"
        "${EXTERNAL_EIGEN_VERSION}"
        "${EXTERNAL_COMMON_CMAKE_ARGS}"
        "${EXTERNALS_BINARY_DIR}"
        "Release"
)

# ---------------------------------------------------------------------------------
# FREETYPE CPP
# ---------------------------------------------------------------------------------
set(EXTERNAL_FREETYPE_CMAKE_ARGS
        ${EXTERNAL_COMMON_CMAKE_ARGS}
        # Project specific cmake args
        -DBUILD_SHARED_LIBS=FALSE
        -DFT_DISABLE_ZLIB=TRUE
        -DFT_DISABLE_BZIP2=TRUE
        -DFT_DISABLE_PNG=TRUE
        -DFT_DISABLE_HARFBUZZ=TRUE
        -DFT_DISABLE_BROTLI=TRUE
)

add_external_git_project(
        "freetype_cpp"
        "https://github.com/freetype/freetype.git"
        "${EXTERNAL_FREETYPE_VERSION}"
        "${EXTERNAL_FREETYPE_CMAKE_ARGS}"
        "${EXTERNALS_BINARY_DIR}"
        "Release"
)

# ---------------------------------------------------------------------------------
# SFML CPP
# ---------------------------------------------------------------------------------
set(EXTERNAL_SFML_CMAKE_ARGS
        ${EXTERNAL_COMMON_CMAKE_ARGS}
        # Project specific cmake args
        -DSFML_BUILD_FRAMEWORKS=FALSE
        -DBUILD_SHARED_LIBS=FALSE
        -DSFML_USE_STATIC_STD_LIBS=TRUE
)

add_external_git_project(
        "sfml_cpp"
        "https://github.com/SFML/SFML.git"
        "${EXTERNAL_SFML_VERSION}"
        "${EXTERNAL_SFML_CMAKE_ARGS}"
        "${EXTERNALS_BINARY_DIR}"
        "Release"
)
