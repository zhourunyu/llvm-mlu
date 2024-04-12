#!/bin/bash
./build/bin/clang++ -fsycl -fsycl-targets=mlisa-cambricon-bang -fsycl-libspirv-path=libclc/build/lib/clc/builtins.opt.libspirv-mlisa--.bc samples/tensor.cpp -o samples/tensor.out
