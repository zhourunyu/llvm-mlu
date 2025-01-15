message(STATUS "Including the PI API CNRT backend.")

 # cannot rely on cmake support for CNRT; it assumes runtime API is being used.
 # we only require the CNRT driver API to be used
 # CNRT_CNRT_LIBRARY variable defines the path to libcnrt.so, the CNRT Driver API library.

# find_package(CNRT 10.1 REQUIRED)

# Make imported library global to use it within the project.
add_library(cnrtdrv SHARED IMPORTED GLOBAL)

if (DEFINED ENV{NEUWARE_HOME})
  set(NEUWARE_HOME $ENV{NEUWARE_HOME})
else()
  set(NEUWARE_HOME "/usr/local/neuware")
endif()

set_target_properties(
  cnrtdrv PROPERTIES
    IMPORTED_LOCATION             ${NEUWARE_HOME}/lib64/libcndrv.so
    INTERFACE_INCLUDE_DIRECTORIES ${NEUWARE_HOME}/include
)

add_library(pi_cnrt SHARED
  "${sycl_inc_dir}/CL/sycl/detail/pi.h"
  "${sycl_inc_dir}/CL/sycl/detail/pi.hpp"
  "pi_cnrt.hpp"
  "pi_cnrt.cpp"
)

add_dependencies(sycl-toolchain pi_cnrt)

set_target_properties(pi_cnrt PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(pi_cnrt
  PRIVATE
    ${sycl_inc_dir}
)

target_link_libraries(pi_cnrt
  PRIVATE
    OpenCL-Headers
    cnrtdrv
)

add_common_options(pi_cnrt)

install(TARGETS pi_cnrt
  LIBRARY DESTINATION "lib${LLVM_LIBDIR_SUFFIX}" COMPONENT pi_cnrt
  RUNTIME DESTINATION "bin" COMPONENT pi_cnrt
)
