# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.18.4)

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(ocl-headers-populate NONE)

include(ExternalProject)
ExternalProject_Add(ocl-headers-populate
                     "UPDATE_DISCONNECTED" "False" "GIT_REPOSITORY" "https://github.com/KhronosGroup/OpenCL-Headers.git" "GIT_TAG" "23710f1b99186065c1768fc3098ba681adc0f253"
                    SOURCE_DIR          "/home/mlx/repos/llvm-mlu/build/_deps/ocl-headers-src"
                    BINARY_DIR          "/home/mlx/repos/llvm-mlu/build/_deps/ocl-headers-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
)


