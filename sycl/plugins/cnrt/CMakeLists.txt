message(STATUS "Including the PI API CNRT backend.")

 # cannot rely on cmake support for CNRT; it assumes runtime API is being used.
 # we only require the CNRT driver API to be used
 # CNRT_CNRT_LIBRARY variable defines the path to libcnrt.so, the CNRT Driver API library.

# find_package(CNRT 10.1 REQUIRED)

# Make imported library global to use it within the project.
add_library(cnrtdrv SHARED IMPORTED GLOBAL)

# set(CNRT_CNRT_LIBRARY 
#   )
# set(CNRT_INCLUDE_DIRS "/usr/local/neuware/include")

# set_target_properties(
#   cnrtdrv PROPERTIES
#     IMPORTED_LOCATION             ${CNRT_CNRT_LIBRARY}
#     INTERFACE_INCLUDE_DIRECTORIES ${CNRT_INCLUDE_DIRS}
# )

if (NOT DEFINED ENV{NEUWARE_HOME})
  message(SEND_ERROR "Environment Variable \"NEUWARE_HOME\" is not defined")
else()
endif()

add_library(pi_cnrt SHARED
  "${sycl_inc_dir}/CL/sycl/detail/pi.h"
  "${sycl_inc_dir}/CL/sycl/detail/pi.hpp"
  "pi_cnrt.hpp"
  "pi_cnrt.cpp"
  "$ENV{NEUWARE_HOME}/include/cnrt.h"
  "$ENV{NEUWARE_HOME}/include/cndrv_api.h"
  "$ENV{NEUWARE_HOME}/include/cn_api.h"
)

add_dependencies(sycl-toolchain pi_cnrt)

set_target_properties(pi_cnrt PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(pi_cnrt
  PRIVATE
    ${sycl_inc_dir}
    "$ENV{NEUWARE_HOME}/include"
    .
)

target_link_libraries(pi_cnrt
  PRIVATE
    OpenCL-Headers
    # cnrtdrv
    "$ENV{NEUWARE_HOME}/lib64/libcnrt.so"
    "/usr/local/neuware/lib64/libcndrv.so"
)

add_common_options(pi_cnrt)

install(TARGETS pi_cnrt
  LIBRARY DESTINATION "lib${LLVM_LIBDIR_SUFFIX}" COMPONENT pi_cnrt
  RUNTIME DESTINATION "bin" COMPONENT pi_cnrt
)
