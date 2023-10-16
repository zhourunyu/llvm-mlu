# CMake generated Testfile for 
# Source directory: /home/wzy/repos/llvm-mlu/libclc
# Build directory: /home/wzy/repos/llvm-mlu/libclc/cmake
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(external-calls-libspirv-mlisa--.bc "./check_external_calls.sh" "/home/wzy/repos/llvm-mlu/libclc/cmake/./lib/clc/libspirv-mlisa--.bc")
set_tests_properties(external-calls-libspirv-mlisa--.bc PROPERTIES  ENVIRONMENT "LLVM_CONFIG=/home/wzy/repos/llvm-mlu/build/lib/cmake/llvm/LLVMConfig.cmake" WORKING_DIRECTORY "/home/wzy/repos/llvm-mlu/libclc/cmake/./lib/clc" _BACKTRACE_TRIPLES "/home/wzy/repos/llvm-mlu/libclc/cmake/modules/AddLibclc.cmake;112;add_test;/home/wzy/repos/llvm-mlu/libclc/CMakeLists.txt;350;add_libclc_builtin_set;/home/wzy/repos/llvm-mlu/libclc/CMakeLists.txt;0;")
add_test(external-calls-clc-mlisa--.bc "./check_external_calls.sh" "/home/wzy/repos/llvm-mlu/libclc/cmake/./lib/clc/clc-mlisa--.bc")
set_tests_properties(external-calls-clc-mlisa--.bc PROPERTIES  ENVIRONMENT "LLVM_CONFIG=/home/wzy/repos/llvm-mlu/build/lib/cmake/llvm/LLVMConfig.cmake" WORKING_DIRECTORY "/home/wzy/repos/llvm-mlu/libclc/cmake/./lib/clc" _BACKTRACE_TRIPLES "/home/wzy/repos/llvm-mlu/libclc/cmake/modules/AddLibclc.cmake;112;add_test;/home/wzy/repos/llvm-mlu/libclc/CMakeLists.txt;360;add_libclc_builtin_set;/home/wzy/repos/llvm-mlu/libclc/CMakeLists.txt;0;")
subdirs("utils")
subdirs("test")
