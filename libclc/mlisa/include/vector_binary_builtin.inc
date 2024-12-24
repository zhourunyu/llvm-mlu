//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "utils.h"
#include <clcmacro.h>

#define DEFINE_TERNARY_VECTORIZED(FUNCTION, BUILTIN, ARG1_TYPE, ARG2_TYPE,     \
                                  ARG3_TYPE)                                   \
  _CLC_DEF _CLC_OVERLOAD void FUNCTION(size_t n, ARG1_TYPE x, ARG2_TYPE y,     \
                                       ARG3_TYPE z) {                          \
    BUILTIN(n, x, y, z);                                                       \
  }                                                                            \

DEFINE_TERNARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_F, float*, const float*, const float*)
#ifdef __CLC_BUILTIN_SCALAR_F
#ifndef __CLC_BUILTIN_SCALAR2_F
#define __CLC_BUILTIN_SCALAR2_F(n, a, b, c) __CLC_BUILTIN_SCALAR_F(n, a, c, b)
#endif
DEFINE_TERNARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_SCALAR_F, float*, const float*, float)
DEFINE_TERNARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_SCALAR2_F, float*, float, const float*)
#endif

#ifdef __CLC_BUILTIN_I
DEFINE_TERNARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_I, int*, const int*, const int*)
#ifdef __CLC_BUILTIN_SCALAR_I
#ifndef __CLC_BUILTIN_SCALAR2_I
#define __CLC_BUILTIN_SCALAR2_I(n, a, b, c) __CLC_BUILTIN_SCALAR_I(n, a, c, b)
#endif
DEFINE_TERNARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_SCALAR_I, int*, const int*, int)
DEFINE_TERNARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_SCALAR2_I, int*, int, const int*)
#endif
#endif

#ifdef __CLC_BUILTIN_U
DEFINE_TERNARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_U, unsigned int*, const unsigned int*, const unsigned int*)
#ifdef __CLC_BUILTIN_SCALAR_U
#ifndef __CLC_BUILTIN_SCALAR2_U
#define __CLC_BUILTIN_SCALAR2_U(n, a, b, c) __CLC_BUILTIN_SCALAR_U(n, a, c, b)
#endif
DEFINE_TERNARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_SCALAR_U, unsigned int*, const unsigned int*, unsigned int)
DEFINE_TERNARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_SCALAR2_U, unsigned int*, unsigned int, const unsigned int*)
#endif
#endif

#ifdef __CLC_BUILTIN_L
DEFINE_TERNARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_L, long*, const long*, const long*)
#ifdef __CLC_BUILTIN_SCALAR_L
#ifndef __CLC_BUILTIN_SCALAR2_L
#define __CLC_BUILTIN_SCALAR2_L(n, a, b, c) __CLC_BUILTIN_SCALAR_L(n, a, c, b)
#endif
DEFINE_TERNARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_SCALAR_L, long*, const long*, long)
DEFINE_TERNARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_SCALAR2_L, long*, long, const long*)
#endif
#endif

#ifdef __CLC_BUILTIN_UL
DEFINE_TERNARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_UL, unsigned long*, const unsigned long*, const unsigned long*)
#ifdef __CLC_BUILTIN_SCALAR_UL
#ifndef __CLC_BUILTIN_SCALAR2_UL
#define __CLC_BUILTIN_SCALAR2_UL(n, a, b, c) __CLC_BUILTIN_SCALAR_UL(n, a, c, b)
#endif
DEFINE_TERNARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_SCALAR_UL, unsigned long*, const unsigned long*, unsigned long)
DEFINE_TERNARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_SCALAR2_UL, unsigned long*, unsigned long, const unsigned long*)
#endif
#endif

#ifdef cl_khr_fp16
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
#ifdef __CLC_BUILTIN_H
DEFINE_TERNARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_H, half*, const half*, const half*)
#ifdef __CLC_BUILTIN_SCALAR_H
#ifndef __CLC_BUILTIN_SCALAR2_H
#define __CLC_BUILTIN_SCALAR2_H(n, a, b, c) __CLC_BUILTIN_SCALAR_H(n, a, c, b)
#endif
DEFINE_TERNARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_SCALAR_H, half*, const half*, half)
DEFINE_TERNARY_VECTORIZED(__CLC_FUNCTION, __CLC_BUILTIN_SCALAR2_H, half*, half, const half*)
#endif
#endif
#endif

#undef DEFINE_TERNARY_VECTORIZED