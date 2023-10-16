# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.18.4)

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(ocl-icd-populate NONE)

include(ExternalProject)
ExternalProject_Add(ocl-icd-populate
                     "UPDATE_DISCONNECTED" "False" "GIT_REPOSITORY" "https://github.com/KhronosGroup/OpenCL-ICD-Loader.git" "GIT_TAG" "5f8249691ec8c25775789498951f8e9eb62c201d"
                    SOURCE_DIR          "/home/wzy/repos/llvm-mlu/build/_deps/ocl-icd-src"
                    BINARY_DIR          "/home/wzy/repos/llvm-mlu/build/_deps/ocl-icd-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
)


