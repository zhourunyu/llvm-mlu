//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "utils.h"
#include <clcmacro.h>

#ifndef __CLC_BUILTIN
#define __CLC_BUILTIN __CLC_XCONCAT(__clc_, __CLC_FUNCTION)
#endif

_CLC_DEF _CLC_OVERLOAD void __CLC_FUNCTION(size_t n, float *a, const float *b, const float *c) { __CLC_BUILTIN(n, a, b, c); }
#ifdef __CLC_BUILTIN_SCALAR
#ifndef __CLC_BUILTIN_SCALAR_L
#define __CLC_BUILTIN_SCALAR_L(n, a, b, c) __CLC_BUILTIN_SCALAR(n, a, c, b)
#endif
_CLC_DEF _CLC_OVERLOAD void __CLC_FUNCTION(size_t n, float *a, const float *b, float c) { __CLC_BUILTIN_SCALAR(n, a, b, c); }
_CLC_DEF _CLC_OVERLOAD void __CLC_FUNCTION(size_t n, float *a, float b, const float *c) { __CLC_BUILTIN_SCALAR_L(n, a, b, c); }
#endif

#ifdef __CLC_BUILTIN_INT
_CLC_DEF _CLC_OVERLOAD void __CLC_FUNCTION(size_t n, int *a, const int *b, const int *c) { __CLC_BUILTIN_INT(n, a, b, c); }
#ifdef __CLC_BUILTIN_INT_SCALAR
#ifndef __CLC_BUILTIN_INT_SCALAR_L
#define __CLC_BUILTIN_INT_SCALAR_L(n, a, b, c) __CLC_BUILTIN_INT_SCALAR(n, a, c, b)
#endif
_CLC_DEF _CLC_OVERLOAD void __CLC_FUNCTION(size_t n, int *a, const int *b, int c) { __CLC_BUILTIN_INT_SCALAR(n, a, b, c); }
_CLC_DEF _CLC_OVERLOAD void __CLC_FUNCTION(size_t n, int *a, int b, const int *c) { __CLC_BUILTIN_INT_SCALAR_L(n, a, b, c); }
#endif
#endif

#ifdef __CLC_BUILTIN_UINT
_CLC_DEF _CLC_OVERLOAD void __CLC_FUNCTION(size_t n, unsigned int *a, const unsigned int *b, const unsigned int *c) { __CLC_BUILTIN_UINT(n, a, b, c); }
#ifdef __CLC_BUILTIN_UINT_SCALAR
#ifndef __CLC_BUILTIN_UINT_SCALAR_L
#define __CLC_BUILTIN_UINT_SCALAR_L(n, a, b, c) __CLC_BUILTIN_UINT_SCALAR(n, a, c, b)
#endif
_CLC_DEF _CLC_OVERLOAD void __CLC_FUNCTION(size_t n, unsigned int *a, const unsigned int *b, unsigned int c) { __CLC_BUILTIN_UINT_SCALAR(n, a, b, c); }
_CLC_DEF _CLC_OVERLOAD void __CLC_FUNCTION(size_t n, unsigned int *a, unsigned int b, const unsigned int *c) { __CLC_BUILTIN_UINT_SCALAR_L(n, a, b, c); }
#endif
#endif
