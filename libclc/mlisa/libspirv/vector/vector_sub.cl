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

#define __CLC_FUNCTION __spirv_ocl_vector_sub
#define __CLC_BUILTIN  __cn_vector_sub_f32_rn
#define __CLC_BUILTIN_SCALAR __cn_vector_sub_scalar_f32_rn
#define __CLC_BUILTIN_SCALAR_L __cn_scalar_sub_vector_f32_rn

#define __CLC_BUILTIN_INT __cn_vector_sub_s32
#define __CLC_BUILTIN_INT_SCALAR __cn_vector_sub_scalar_s32
#define __CLC_BUILTIN_INT_SCALAR_L __cn_scalar_sub_vector_s32
#define __CLC_BUILTIN_UINT __cn_vector_sub_u32
#define __CLC_BUILTIN_UINT_SCALAR __cn_vector_sub_scalar_u32
#define __CLC_BUILTIN_UINT_SCALAR_L __cn_scalar_sub_vector_u32

#define __CLC_BUILTIN_LONG __cn_vector_sub_s64
#define __CLC_BUILTIN_LONG_SCALAR __cn_vector_sub_scalar_s64
#define __CLC_BUILTIN_LONG_SCALAR_L __cn_scalar_sub_vector_s64
#define __CLC_BUILTIN_ULONG __cn_vector_sub_u64
#define __CLC_BUILTIN_ULONG_SCALAR __cn_vector_sub_scalar_u64
#define __CLC_BUILTIN_ULONG_SCALAR_L __cn_scalar_sub_vector_u64
#include <../../include/vector_binary_builtin.inc>
