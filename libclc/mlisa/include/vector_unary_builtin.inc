//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "utils.h"
#include <clcmacro.h>

#define DEFINE_BINARY_VECTORIZED(FUNCTION, BUILTIN, ARG1_TYPE, ARG2_TYPE)      \
  _CLC_DEF _CLC_OVERLOAD void FUNCTION(size_t n, ARG1_TYPE x, ARG2_TYPE y) {   \
    BUILTIN(n, x, y);                                                          \
  }                                                                            \

DEFINE_BINARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_F, float*, const float*)

#ifdef __CLC_BUILTIN_I
DEFINE_BINARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_I, int*, const int*)
#endif

#ifdef __CLC_BUILTIN_U
DEFINE_BINARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_U, unsigned int*, const unsigned int*)
#endif

#ifdef __CLC_BUILTIN_L
DEFINE_BINARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_L, long*, const long*)
#endif

#ifdef __CLC_BUILTIN_UL
DEFINE_BINARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_UL, unsigned long*, const unsigned long*)
#endif

#ifdef cl_khr_fp16
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
#ifdef __CLC_BUILTIN_H
DEFINE_BINARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_H, half*, const half*)
#endif
#endif

#undef _CLC_DEFINE_BINARY_VECTORIZED