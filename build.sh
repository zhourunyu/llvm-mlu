#!/bin/bash
/home/wzy/repos/llvm-mlu/build/bin/clang++ -fsycl -fsycl-targets=mlisa-cambricon-bang samples/wram.cpp -o samples/wram
