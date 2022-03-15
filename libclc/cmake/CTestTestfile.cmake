# CMake generated Testfile for 
# Source directory: /home/mlx/repos/llvm-mlu/libclc
# Build directory: /home/mlx/repos/llvm-mlu/libclc/cmake
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(external-calls-libspirv-mlisa--.bc "./check_external_calls.sh" "/home/mlx/repos/llvm-mlu/libclc/cmake/./lib/clc/libspirv-mlisa--.bc")
set_tests_properties(external-calls-libspirv-mlisa--.bc PROPERTIES  ENVIRONMENT "LLVM_CONFIG=/home/mlx/repos/llvm-mlu/build/lib/cmake/llvm/LLVMConfig.cmake" WORKING_DIRECTORY "/home/mlx/repos/llvm-mlu/libclc/cmake/./lib/clc" _BACKTRACE_TRIPLES "/home/mlx/repos/llvm-mlu/libclc/cmake/modules/AddLibclc.cmake;112;add_test;/home/mlx/repos/llvm-mlu/libclc/CMakeLists.txt;350;add_libclc_builtin_set;/home/mlx/repos/llvm-mlu/libclc/CMakeLists.txt;0;")
add_test(external-calls-clc-mlisa--.bc "./check_external_calls.sh" "/home/mlx/repos/llvm-mlu/libclc/cmake/./lib/clc/clc-mlisa--.bc")
set_tests_properties(external-calls-clc-mlisa--.bc PROPERTIES  ENVIRONMENT "LLVM_CONFIG=/home/mlx/repos/llvm-mlu/build/lib/cmake/llvm/LLVMConfig.cmake" WORKING_DIRECTORY "/home/mlx/repos/llvm-mlu/libclc/cmake/./lib/clc" _BACKTRACE_TRIPLES "/home/mlx/repos/llvm-mlu/libclc/cmake/modules/AddLibclc.cmake;112;add_test;/home/mlx/repos/llvm-mlu/libclc/CMakeLists.txt;360;add_libclc_builtin_set;/home/mlx/repos/llvm-mlu/libclc/CMakeLists.txt;0;")
subdirs("utils")
subdirs("test")
