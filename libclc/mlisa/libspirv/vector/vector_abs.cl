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

#define __CLC_FUNCTION __spirv_ocl_vector_abs
#define __CLC_BUILTIN  __cn_vector_abs_f32

#define __CLC_BUILTIN_INT __cn_vector_abs_s32
#define __CLC_BUILTIN_LONG __cn_vector_abs_s64
#include <../../include/vector_unary_builtin.inc>
