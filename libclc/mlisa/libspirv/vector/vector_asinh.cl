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

_CLC_INLINE void __cn_vector_asinh_f32(int n, float *y, const float *x) {
    __mlvm_stream_square_f32(y, (float *)x, n);
    __cn_vector_add_scalar_f32_rn(n, y, y, 1.0f);
    __cn_vector_sqrt_f32(n, y, y);
    __cn_vector_add_f32_rn(n, y, y, x);
    __cn_vector_log_f32(n, y, y);
}

#define __CLC_FUNCTION __spirv_ocl_vector_asinh
#define __CLC_BUILTIN  __cn_vector_asinh_f32
#include <../../include/vector_unary_builtin.inc>
