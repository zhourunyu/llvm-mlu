#!/bin/bash

SCRIPT_PATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
mkdir -p $SCRIPT_PATH/build && cd $SCRIPT_PATH/build
cmake -DLIBCLC_TARGETS_TO_BUILD=mlisa-- -DCMAKE_LLAsm_COMPILER_FORCED=True ..
make -j$(nproc)
cp lib/clc/libspirv-mlisa--.bc ../../build/lib/clc/libspirv-mlisa--.bc
