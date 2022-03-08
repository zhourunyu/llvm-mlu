
if(NOT "/home/mlx/repos/llvm-mlu/build/_deps/vc-intrinsics-subbuild/vc-intrinsics-populate-prefix/src/vc-intrinsics-populate-stamp/vc-intrinsics-populate-gitinfo.txt" IS_NEWER_THAN "/home/mlx/repos/llvm-mlu/build/_deps/vc-intrinsics-subbuild/vc-intrinsics-populate-prefix/src/vc-intrinsics-populate-stamp/vc-intrinsics-populate-gitclone-lastrun.txt")
  message(STATUS "Avoiding repeated git clone, stamp file is up to date: '/home/mlx/repos/llvm-mlu/build/_deps/vc-intrinsics-subbuild/vc-intrinsics-populate-prefix/src/vc-intrinsics-populate-stamp/vc-intrinsics-populate-gitclone-lastrun.txt'")
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E rm -rf "/home/mlx/repos/llvm-mlu/build/_deps/vc-intrinsics-src"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: '/home/mlx/repos/llvm-mlu/build/_deps/vc-intrinsics-src'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "/root/miniconda/bin/git"  clone --no-checkout "https://github.com/intel/vc-intrinsics.git" "vc-intrinsics-src"
    WORKING_DIRECTORY "/home/mlx/repos/llvm-mlu/build/_deps"
    RESULT_VARIABLE error_code
    )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once:
          ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://github.com/intel/vc-intrinsics.git'")
endif()

execute_process(
  COMMAND "/root/miniconda/bin/git"  checkout 19e6ddafc545bc96ceaacbf7c81cacbff59a39b6 --
  WORKING_DIRECTORY "/home/mlx/repos/llvm-mlu/build/_deps/vc-intrinsics-src"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: '19e6ddafc545bc96ceaacbf7c81cacbff59a39b6'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "/root/miniconda/bin/git"  submodule update --recursive --init 
    WORKING_DIRECTORY "/home/mlx/repos/llvm-mlu/build/_deps/vc-intrinsics-src"
    RESULT_VARIABLE error_code
    )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: '/home/mlx/repos/llvm-mlu/build/_deps/vc-intrinsics-src'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy
    "/home/mlx/repos/llvm-mlu/build/_deps/vc-intrinsics-subbuild/vc-intrinsics-populate-prefix/src/vc-intrinsics-populate-stamp/vc-intrinsics-populate-gitinfo.txt"
    "/home/mlx/repos/llvm-mlu/build/_deps/vc-intrinsics-subbuild/vc-intrinsics-populate-prefix/src/vc-intrinsics-populate-stamp/vc-intrinsics-populate-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: '/home/mlx/repos/llvm-mlu/build/_deps/vc-intrinsics-subbuild/vc-intrinsics-populate-prefix/src/vc-intrinsics-populate-stamp/vc-intrinsics-populate-gitclone-lastrun.txt'")
endif()

