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

#define __CLC_FUNCTION __spirv_ocl_vector_div
#define __CLC_BUILTIN __cn_vector_div
#define __CLC_BUILTIN_F __CLC_XCONCAT(__CLC_BUILTIN, _f32_rn)
#define __CLC_BUILTIN_H __CLC_XCONCAT(__CLC_BUILTIN, _f16_rn)
#define __CLC_BUILTIN_I __CLC_XCONCAT(__CLC_BUILTIN, _s32)
#define __CLC_BUILTIN_U __CLC_XCONCAT(__CLC_BUILTIN, _u32)
#define __CLC_BUILTIN_L __CLC_XCONCAT(__CLC_BUILTIN, _s64)
#define __CLC_BUILTIN_UL __CLC_XCONCAT(__CLC_BUILTIN, _u64)
#define __CLC_BUILTIN_SCALAR __cn_vector_div_scalar
#define __CLC_BUILTIN_SCALAR_F __CLC_XCONCAT(__CLC_BUILTIN_SCALAR, _f32_rn)
#define __CLC_BUILTIN_SCALAR_H __CLC_XCONCAT(__CLC_BUILTIN_SCALAR, _f16_rn)
#define __CLC_BUILTIN_SCALAR_I __CLC_XCONCAT(__CLC_BUILTIN_SCALAR, _s32)
#define __CLC_BUILTIN_SCALAR_U __CLC_XCONCAT(__CLC_BUILTIN_SCALAR, _u32)
#define __CLC_BUILTIN_SCALAR_L __CLC_XCONCAT(__CLC_BUILTIN_SCALAR, _s64)
#define __CLC_BUILTIN_SCALAR_UL __CLC_XCONCAT(__CLC_BUILTIN_SCALAR, _u64)
#define __CLC_BUILTIN_SCALAR2 __cn_scalar_div_vector
#define __CLC_BUILTIN_SCALAR2_F __CLC_XCONCAT(__CLC_BUILTIN_SCALAR2, _f32_rn)
#define __CLC_BUILTIN_SCALAR2_H __CLC_XCONCAT(__CLC_BUILTIN_SCALAR2, _f16_rn)
#define __CLC_BUILTIN_SCALAR2_I __CLC_XCONCAT(__CLC_BUILTIN_SCALAR2, _s32)
#define __CLC_BUILTIN_SCALAR2_U __CLC_XCONCAT(__CLC_BUILTIN_SCALAR2, _u32)
#define __CLC_BUILTIN_SCALAR2_L __CLC_XCONCAT(__CLC_BUILTIN_SCALAR2, _s64)
#define __CLC_BUILTIN_SCALAR2_UL __CLC_XCONCAT(__CLC_BUILTIN_SCALAR2, _u64)
#include <../../include/vector_binary_builtin.inc>
