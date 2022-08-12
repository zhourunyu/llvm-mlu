#!/bin/bash
/home/mlx/repos/llvm-mlu/build/bin/clang++ -fsycl -fsycl-targets=mlisa-cambricon-bang samples/wram.cpp -o samples/wram
