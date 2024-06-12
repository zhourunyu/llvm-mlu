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

#define __CLC_FUNCTION __spirv_ocl_vector_le
#define __CLC_BUILTIN  __cn_vector_le_f32
#define __CLC_BUILTIN_SCALAR __cn_vector_le_scalar_f32
#define __CLC_BUILTIN_SCALAR_L(n, a, b, c) __cn_vector_ge_scalar_f32(n, a, c, b)

#define __CLC_BUILTIN_INT __cn_vector_le_s32
#define __CLC_BUILTIN_INT_SCALAR __cn_vector_le_scalar_s32
#define __CLC_BUILTIN_INT_SCALAR_L(n, a, b, c) __cn_vector_ge_scalar_s32(n, a, c, b)
#define __CLC_BUILTIN_UINT __cn_vector_le_u32
#define __CLC_BUILTIN_UINT_SCALAR __cn_vector_le_scalar_u32
#define __CLC_BUILTIN_UINT_SCALAR_L(n, a, b, c) __cn_vector_ge_scalar_u32(n, a, c, b)
#include <../../include/vector_relational_builtin.inc>
