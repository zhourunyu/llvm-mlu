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

_CLC_INLINE float __cn_scalar_atanh_f32(float x) {
    return __cn_scalar_log_f32((1 + x) / (1 - x)) / 2;
}

#define __CLC_FUNCTION __spirv_ocl_atanh
#define __CLC_BUILTIN __cn_scalar_atanh_f32
#define __CLC_BUILTIN_F __cn_scalar_atanh_f32
#include <math/unary_builtin.inc>
