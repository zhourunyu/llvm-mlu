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

_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, float *a, const bool *b, const float *c, const float *d) {
    // a = b ? c : d
    // a = b * c + (1 - b) * d = d + b * (c - d)
    int *dst = (int *)a, *src1 = (int *)c, *src2 = (int *)d;
    const char *mask = (const char *)b;
    __cn_vector_cast_s8_to_s32(n, dst, mask);
    __cn_vector_negate_s32(n, dst, dst);
    __cn_vector_sub_s32(n, src1, src1, src2);
    __cn_vector_and_s32(n, dst, dst, src1);
    __cn_vector_add_s32(n, dst, dst, src2);
    __cn_vector_add_s32(n, src1, src1, src2);
}

_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, float *a, const bool *b, const float *c, float d) {
    int *dst = (int *)a, *src1 = (int *)c, src2 = *(int *)&d;
    const char *mask = (const char *)b;
    __cn_vector_cast_s8_to_s32(n, dst, mask);
    __cn_vector_negate_s32(n, dst, dst);
    __cn_vector_sub_scalar_s32(n, src1, src1, src2);
    __cn_vector_and_s32(n, dst, dst, src1);
    __cn_vector_add_scalar_s32(n, dst, dst, src2);
    __cn_vector_add_scalar_s32(n, src1, src1, src2);
}

_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, float *a, const bool *b, float c, const float *d) {
    int *dst = (int *)a, src1 = *(int *)&c, *src2 = (int *)d;
    const char *mask = (const char *)b;
    __cn_vector_cast_s8_to_s32(n, dst, mask);
    __cn_vector_negate_s32(n, dst, dst);
    __cn_vector_sub_scalar_s32(n, src2, src2, src1);
    __cn_vector_and_s32(n, dst, dst, src2);
    __cn_vector_add_scalar_s32(n, src2, src2, src1);
    __cn_vector_sub_s32(n, dst, src2, dst);
}

_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, int *a, const bool *b, const int *c, const int *d) {
    int *dst = a, *src1 = (int *)c, *src2 = (int *)d;
    const char *mask = (const char *)b;
    __cn_vector_cast_s8_to_s32(n, dst, mask);
    __cn_vector_negate_s32(n, dst, dst);
    __cn_vector_sub_s32(n, src1, src1, src2);
    __cn_vector_and_s32(n, dst, dst, src1);
    __cn_vector_add_s32(n, dst, dst, src2);
    __cn_vector_add_s32(n, src1, src1, src2);
}

_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, int *a, const bool *b, const int *c, int d) {
    int *dst = a, *src1 = (int *)c, src2 = d;
    const char *mask = (const char *)b;
    __cn_vector_cast_s8_to_s32(n, dst, mask);
    __cn_vector_negate_s32(n, dst, dst);
    __cn_vector_sub_scalar_s32(n, src1, src1, src2);
    __cn_vector_and_s32(n, dst, dst, src1);
    __cn_vector_add_scalar_s32(n, dst, dst, src2);
    __cn_vector_add_scalar_s32(n, src1, src1, src2);
}

_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, int *a, const bool *b, int c, const int *d) {
    int *dst = a, src1 = c, *src2 = (int *)d;
    const char *mask = (const char *)b;
    __cn_vector_cast_s8_to_s32(n, dst, mask);
    __cn_vector_negate_s32(n, dst, dst);
    __cn_vector_sub_scalar_s32(n, src2, src2, src1);
    __cn_vector_and_s32(n, dst, dst, src2);
    __cn_vector_add_scalar_s32(n, src2, src2, src1);
    __cn_vector_sub_s32(n, dst, src2, dst);
}

_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, unsigned int *a, const bool *b, const unsigned int *c, const unsigned int *d) {
    int *dst = (int *)a, *src1 = (int *)c, *src2 = (int *)d;
    const char *mask = (const char *)b;
    __cn_vector_cast_s8_to_s32(n, dst, mask);
    __cn_vector_negate_s32(n, dst, dst);
    __cn_vector_sub_s32(n, src1, src1, src2);
    __cn_vector_and_s32(n, dst, dst, src1);
    __cn_vector_add_s32(n, dst, dst, src2);
    __cn_vector_add_s32(n, src1, src1, src2);
}

_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, unsigned int *a, const bool *b, const unsigned int *c, unsigned int d) {
    int *dst = (int *)a, *src1 = (int *)c, src2 = *(int *)&d;
    const char *mask = (const char *)b;
    __cn_vector_cast_s8_to_s32(n, dst, mask);
    __cn_vector_negate_s32(n, dst, dst);
    __cn_vector_sub_scalar_s32(n, src1, src1, src2);
    __cn_vector_and_s32(n, dst, dst, src1);
    __cn_vector_add_scalar_s32(n, dst, dst, src2);
    __cn_vector_add_scalar_s32(n, src1, src1, src2);
}

_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, unsigned int *a, const bool *b, unsigned int c, const unsigned int *d) {
    int *dst = (int *)a, src1 = *(int *)&c, *src2 = (int *)d;
    const char *mask = (const char *)b;
    __cn_vector_cast_s8_to_s32(n, dst, mask);
    __cn_vector_negate_s32(n, dst, dst);
    __cn_vector_sub_scalar_s32(n, src2, src2, src1);
    __cn_vector_and_s32(n, dst, dst, src2);
    __cn_vector_add_scalar_s32(n, src2, src2, src1);
    __cn_vector_sub_s32(n, dst, src2, dst);
}

_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, long *a, const bool *b, const long *c, const long *d) {
    long *dst = a, *src1 = (long *)c, *src2 = (long *)d;
    const char *mask = (const char *)b;
    __cn_vector_cast_s8_to_s64(n, dst, mask);
    __cn_vector_negate_s64(n, dst, dst);
    __cn_vector_sub_s64(n, src1, src1, src2);
    __cn_vector_and_s64(n, dst, dst, src1);
    __cn_vector_add_s64(n, dst, dst, src2);
    __cn_vector_add_s64(n, src1, src1, src2);
}

_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, long *a, const bool *b, const long *c, long d) {
    long *dst = a, *src1 = (long *)c, src2 = d;
    const char *mask = (const char *)b;
    __cn_vector_cast_s8_to_s64(n, dst, mask);
    __cn_vector_negate_s64(n, dst, dst);
    __cn_vector_sub_scalar_s64(n, src1, src1, src2);
    __cn_vector_and_s64(n, dst, dst, src1);
    __cn_vector_add_scalar_s64(n, dst, dst, src2);
    __cn_vector_add_scalar_s64(n, src1, src1, src2);
}

_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, long *a, const bool *b, long c, const long *d) {
    long *dst = a, src1 = c, *src2 = (long *)d;
    const char *mask = (const char *)b;
    __cn_vector_cast_s8_to_s64(n, dst, mask);
    __cn_vector_negate_s64(n, dst, dst);
    __cn_vector_sub_scalar_s64(n, src2, src2, src1);
    __cn_vector_and_s64(n, dst, dst, src2);
    __cn_vector_add_scalar_s64(n, src2, src2, src1);
    __cn_vector_sub_s64(n, dst, src2, dst);
}

_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, unsigned long *a, const bool *b, const unsigned long *c, const unsigned long *d) {
    long *dst = (long *)a, *src1 = (long *)c, *src2 = (long *)d;
    const char *mask = (const char *)b;
    __cn_vector_cast_s8_to_s64(n, dst, mask);
    __cn_vector_negate_s64(n, dst, dst);
    __cn_vector_sub_s64(n, src1, src1, src2);
    __cn_vector_and_s64(n, dst, dst, src1);
    __cn_vector_add_s64(n, dst, dst, src2);
    __cn_vector_add_s64(n, src1, src1, src2);
}

_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, unsigned long *a, const bool *b, const unsigned long *c, unsigned long d) {
    long *dst = (long *)a, *src1 = (long *)c, src2 = *(long *)&d;
    const char *mask = (const char *)b;
    __cn_vector_cast_s8_to_s64(n, dst, mask);
    __cn_vector_negate_s64(n, dst, dst);
    __cn_vector_sub_scalar_s64(n, src1, src1, src2);
    __cn_vector_and_s64(n, dst, dst, src1);
    __cn_vector_add_scalar_s64(n, dst, dst, src2);
    __cn_vector_add_scalar_s64(n, src1, src1, src2);
}

_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, unsigned long *a, const bool *b, unsigned long c, const unsigned long *d) {
    long *dst = (long *)a, src1 = *(long *)&c, *src2 = (long *)d;
    const char *mask = (const char *)b;
    __cn_vector_cast_s8_to_s64(n, dst, mask);
    __cn_vector_negate_s64(n, dst, dst);
    __cn_vector_sub_scalar_s64(n, src2, src2, src1);
    __cn_vector_and_s64(n, dst, dst, src2);
    __cn_vector_add_scalar_s64(n, src2, src2, src1);
    __cn_vector_sub_s64(n, dst, src2, dst);
}
