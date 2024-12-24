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

// atanh(x) = log(2 / (1 - x) - 1) / 2
_CLC_INLINE void __cn_vector_atanh_f32(int n, float *y, const float *x) {
    __cn_scalar_sub_vector_f32_rn(n, y, 1.0f, x);
    __cn_scalar_div_vector_f32_rn(n, y, 2.0f, y);
    __cn_vector_sub_scalar_f32_rn(n, y, y, 1.0f);
    __cn_vector_log_f32(n, y, y);
    __cn_vector_div_scalar_f32_rn(n, y, y, 2.0f);
}
#ifdef cl_khr_fp16
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
_CLC_INLINE void __cn_vector_atanh_f16(int n, half *y, const half *x) {
    __cn_scalar_sub_vector_f16_rn(n, y, 1.0h, x);
    __cn_scalar_div_vector_f16_rn(n, y, 2.0h, y);
    __cn_vector_sub_scalar_f16_rn(n, y, y, 1.0h);
    __cn_vector_log_f16(n, y, y);
    __cn_vector_div_scalar_f16_rn(n, y, y, 2.0h);
}
#endif

#define __CLC_FUNCTION __spirv_ocl_vector_atanh
#define __CLC_BUILTIN __cn_vector_atanh
#define __CLC_BUILTIN_F __CLC_XCONCAT(__CLC_BUILTIN, _f32)
#define __CLC_BUILTIN_H __CLC_XCONCAT(__CLC_BUILTIN, _f16)
#include <../../include/vector_unary_builtin.inc>
