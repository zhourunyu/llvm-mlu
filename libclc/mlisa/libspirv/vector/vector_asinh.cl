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

// asinh(x) = log(x + sqrt(x^2 + 1))
_CLC_INLINE void __cn_vector_asinh_f32(int n, float *y, const float *x) {
    __cn_vector_mul_f32_rn(n, y, x, x);
    __cn_vector_add_scalar_f32_rn(n, y, y, 1.0f);
    __cn_vector_sqrt_f32(n, y, y);
    __cn_vector_add_f32_rn(n, y, y, x);
    __cn_vector_log_f32(n, y, y);
}
#ifdef cl_khr_fp16
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
_CLC_INLINE void __cn_vector_asinh_f16(int n, half *y, const half *x) {
    __cn_vector_mul_f16_rn(n, y, x, x);
    __cn_vector_add_scalar_f16_rn(n, y, y, 1.0h);
    __cn_vector_sqrt_f16(n, y, y);
    __cn_vector_add_f16_rn(n, y, y, x);
    __cn_vector_log_f16(n, y, y);
}
#endif

#define __CLC_FUNCTION __spirv_ocl_vector_asinh
#define __CLC_BUILTIN __cn_vector_asinh
#define __CLC_BUILTIN_F __CLC_XCONCAT(__CLC_BUILTIN, _f32)
#define __CLC_BUILTIN_H __CLC_XCONCAT(__CLC_BUILTIN, _f16)
#include <../../include/vector_unary_builtin.inc>
