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
#ifdef cl_khr_fp16
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
_CLC_INLINE half __cn_scalar_atanh_f16(half x) {
    return __cn_scalar_log_f16((1 + x) / (1 - x)) / 2;
}
#endif

#define __CLC_FUNCTION __spirv_ocl_atanh
#define __CLC_BUILTIN __cn_scalar_atanh
#define __CLC_BUILTIN_F __CLC_XCONCAT(__CLC_BUILTIN, _f32)
#define __CLC_BUILTIN_H __CLC_XCONCAT(__CLC_BUILTIN, _f16)
#include <math/unary_builtin.inc>
