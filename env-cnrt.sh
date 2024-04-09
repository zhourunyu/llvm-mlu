SCRIPT_PATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
export SYCL_DEVICE_FILTER=cnrt
export DPCPP_HOME=$SCRIPT_PATH
export PATH=$DPCPP_HOME/build/bin:$PATH
export LD_LIBRARY_PATH=$DPCPP_HOME/build/lib:$LD_LIBRARY_PATH

export NEUWARE_HOME=/usr/local/neuware
export LD_LIBRARY_PATH=${NEUWARE_HOME}/lib64:${LD_LIBRARY_PATH}
