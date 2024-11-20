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

_CLC_DEF _CLC_OVERLOAD void __CLC_FUNCTION(size_t n, float *a, const float *b) { __CLC_BUILTIN(n, a, b); }

#ifdef __CLC_BUILTIN_INT
_CLC_DEF _CLC_OVERLOAD void __CLC_FUNCTION(size_t n, int *a, const int *b) { __CLC_BUILTIN_INT(n, a, b); }
#endif

#ifdef __CLC_BUILTIN_UINT
_CLC_DEF _CLC_OVERLOAD void __CLC_FUNCTION(size_t n, unsigned int *a, const unsigned int *b) { __CLC_BUILTIN_UINT(n, a, b); }
#endif

#ifdef __CLC_BUILTIN_LONG
_CLC_DEF _CLC_OVERLOAD void __CLC_FUNCTION(size_t n, long *a, const long *b) { __CLC_BUILTIN_LONG(n, a, b); }
#endif

#ifdef __CLC_BUILTIN_ULONG
_CLC_DEF _CLC_OVERLOAD void __CLC_FUNCTION(size_t n, unsigned long *a, const unsigned long *b) { __CLC_BUILTIN_ULONG(n, a, b); }
#endif