//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <spirv/spirv.h>

#include "../../include/libdevice.h"
#include <clcmacro.h>

#define __CLC_FUNCTION __spirv_ocl_vector_ge
#define __CLC_BUILTIN __cn_vector_ge
#define __CLC_BUILTIN_F __CLC_XCONCAT(__CLC_BUILTIN, _f32)
#define __CLC_BUILTIN_H __CLC_XCONCAT(__CLC_BUILTIN, _f16)
#define __CLC_BUILTIN_I __CLC_XCONCAT(__CLC_BUILTIN, _s32)
#define __CLC_BUILTIN_U __CLC_XCONCAT(__CLC_BUILTIN, _u32)
#define __CLC_BUILTIN_L __CLC_XCONCAT(__CLC_BUILTIN, _s64)
#define __CLC_BUILTIN_UL __CLC_XCONCAT(__CLC_BUILTIN, _u64)
#define __CLC_BUILTIN_SCALAR __cn_vector_ge_scalar
#define __CLC_BUILTIN_SCALAR_F __CLC_XCONCAT(__CLC_BUILTIN_SCALAR, _f32)
#define __CLC_BUILTIN_SCALAR_H __CLC_XCONCAT(__CLC_BUILTIN_SCALAR, _f16)
#define __CLC_BUILTIN_SCALAR_I __CLC_XCONCAT(__CLC_BUILTIN_SCALAR, _s32)
#define __CLC_BUILTIN_SCALAR_U __CLC_XCONCAT(__CLC_BUILTIN_SCALAR, _u32)
#define __CLC_BUILTIN_SCALAR_L __CLC_XCONCAT(__CLC_BUILTIN_SCALAR, _s64)
#define __CLC_BUILTIN_SCALAR_UL __CLC_XCONCAT(__CLC_BUILTIN_SCALAR, _u64)
#define __CLC_BUILTIN_SCALAR2 __cn_vector_le_scalar
#define __CLC_BUILTIN_SCALAR2_F(n, a, b, c) __CLC_XCONCAT(__CLC_BUILTIN_SCALAR2, _f32)(n, a, c, b)
#define __CLC_BUILTIN_SCALAR2_H(n, a, b, c) __CLC_XCONCAT(__CLC_BUILTIN_SCALAR2, _f16)(n, a, c, b)
#define __CLC_BUILTIN_SCALAR2_I(n, a, b, c) __CLC_XCONCAT(__CLC_BUILTIN_SCALAR2, _s32)(n, a, c, b)
#define __CLC_BUILTIN_SCALAR2_U(n, a, b, c) __CLC_XCONCAT(__CLC_BUILTIN_SCALAR2, _u32)(n, a, c, b)
#define __CLC_BUILTIN_SCALAR2_L(n, a, b, c) __CLC_XCONCAT(__CLC_BUILTIN_SCALAR2, _s64)(n, a, c, b)
#define __CLC_BUILTIN_SCALAR2_UL(n, a, b, c) __CLC_XCONCAT(__CLC_BUILTIN_SCALAR2, _u64)(n, a, c, b)
#include <../../include/vector_relational_builtin.inc>
