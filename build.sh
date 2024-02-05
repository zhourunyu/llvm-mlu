#!/bin/bash
./build/bin/clang++ -fsycl -fsycl-targets=mlisa-cambricon-bang samples/tensor.cpp -o samples/tensor
