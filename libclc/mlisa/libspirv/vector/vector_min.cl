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

#define __CLC_FUNCTION __spirv_ocl_vector_min
#define __CLC_BUILTIN  __cn_vector_min_f32
#define __CLC_BUILTIN_SCALAR __cn_vector_min_scalar_f32

#define __CLC_BUILTIN_INT __cn_vector_min_s32
#define __CLC_BUILTIN_INT_SCALAR __cn_vector_min_scalar_s32
#define __CLC_BUILTIN_UINT __cn_vector_min_u32
#define __CLC_BUILTIN_UINT_SCALAR __cn_vector_min_scalar_u32

#define __CLC_BUILTIN_LONG __cn_vector_min_s64
#define __CLC_BUILTIN_LONG_SCALAR __cn_vector_min_scalar_s64
#define __CLC_BUILTIN_ULONG __cn_vector_min_u64
#define __CLC_BUILTIN_ULONG_SCALAR __cn_vector_min_scalar_u64
#include <../../include/vector_binary_builtin.inc>
