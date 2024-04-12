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

_CLC_INLINE float __cn_scalar_cbrt_f32(float x) {
    return __cn_scalar_sign_f32(x) * __cn_scalar_pow_f32(__cn_scalar_abs_f32(x), 1.0f/3.0f);   // __cn_scalar_pow_f32 requires x >= 0 when y is not an integer
}

#define __CLC_FUNCTION __spirv_ocl_cbrt
#define __CLC_BUILTIN __cn_scalar_cbrt_f32
#define __CLC_BUILTIN_F __cn_scalar_cbrt_f32
#include <math/unary_builtin.inc>
