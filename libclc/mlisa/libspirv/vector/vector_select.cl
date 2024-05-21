//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <spirv/spirv.h>
#include <clcmacro.h>

#include "../../include/libdevice.h"

_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, float *a, float *b, const float *c, const float *d) {
    unsigned int *dst = (unsigned int *)a, *mask = (unsigned int *)b;
    const unsigned int *src1 = (const unsigned int *)c, *src2 = (const unsigned int *)d;
    // mask = mask? 0xFFFFFFFF : 0
    __mlvm_stream_ne_scalar_s32((int *)mask, (int *)mask, 0, n);
    __cn_vector_negate_s32(n, (int *)mask, (const int *)mask);
    // dst = (src1 & mask) | (src2 & ~mask)
    __cn_vector_and_u32(n, dst, mask, src1);
    __cn_vector_not_u32(n, mask, mask);
    __cn_vector_and_u32(n, mask, mask, src2);
    __cn_vector_or_u32(n, dst, dst, mask);
}