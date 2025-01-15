cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_COMPILER ${CMAKE_SOURCE_DIR}/../build/bin/clang)
set(CMAKE_CXX_COMPILER ${CMAKE_SOURCE_DIR}/../build/bin/clang++)
project(samples)

set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_SOURCE_DIR}/include)
add_link_options(-fsycl -fsycl-targets=mlisa-cambricon-bang)
add_compile_options(-fsycl -fsycl-targets=mlisa-cambricon-bang -O1)

include(CTest)
enable_testing()

function(add_executable_and_test target)
  add_executable(${target} ${ARGN})
  add_test(NAME ${target} COMMAND ${target})
endfunction()

add_executable(hello-sycl hello-sycl.cpp)
add_executable_and_test(usm usm.cpp)
add_executable_and_test(workitem workitem.cpp)
add_executable_and_test(local_mem local_mem.cpp)

file(GLOB TEST_SOURCES 
  "libdevice/*.cpp"
)

foreach(source_file ${TEST_SOURCES})
  get_filename_component(file_name ${source_file} NAME)
  string(REPLACE ".cpp" "" target_name ${file_name})
  add_executable_and_test(${target_name} ${source_file})
endforeach()
