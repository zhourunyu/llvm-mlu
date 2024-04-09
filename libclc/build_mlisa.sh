mkdir -p build && cd build
cmake -DLIBCLC_TARGETS_TO_BUILD=mlisa-- -DCMAKE_LLAsm_COMPILER_FORCED=True ..
make -j$(nproc)