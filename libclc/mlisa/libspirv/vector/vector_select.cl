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

// a = b * c + (1 - b) * d = d + b * (c - d)
_CLC_INLINE void __cn_vector_select_s32(size_t n, int *a, const unsigned char *b, const int *c, const int *d) {
    int *dst = a, *src1 = (int *)c, *src2 = (int *)d;
    __cn_vector_cast_u8_to_s32(n, dst, b);
    __cn_vector_negate_s32(n, dst, dst);
    __cn_vector_sub_s32(n, src1, src1, src2);
    __cn_vector_and_s32(n, dst, dst, src1);
    __cn_vector_add_s32(n, dst, dst, src2);
    __cn_vector_add_s32(n, src1, src1, src2);
}
_CLC_INLINE void __cn_vector_select_scalar_s32(size_t n, int *a, const unsigned char *b, const int *c, int d) {
    int *dst = a, *src1 = (int *)c, src2 = d;
    __cn_vector_cast_u8_to_s32(n, dst, b);
    __cn_vector_negate_s32(n, dst, dst);
    __cn_vector_sub_scalar_s32(n, src1, src1, src2);
    __cn_vector_and_s32(n, dst, dst, src1);
    __cn_vector_add_scalar_s32(n, dst, dst, src2);
    __cn_vector_add_scalar_s32(n, src1, src1, src2);
}
_CLC_INLINE void __cn_scalar_select_vector_s32(size_t n, int *a, const unsigned char *b, int c, const int *d) {
    int *dst = a, src1 = c, *src2 = (int *)d;
    __cn_vector_cast_u8_to_s32(n, dst, b);
    __cn_vector_negate_s32(n, dst, dst);
    __cn_vector_sub_scalar_s32(n, src2, src2, src1);
    __cn_vector_and_s32(n, dst, dst, src2);
    __cn_vector_add_scalar_s32(n, src2, src2, src1);
    __cn_vector_sub_s32(n, dst, src2, dst);
}

_CLC_INLINE void __cn_vector_select_s16(size_t n, short *a, const unsigned char *b, const short *c, const short *d) {
    short *dst = a, *src1 = (short *)c, *src2 = (short *)d;
    __cn_vector_cast_u8_to_s16(n, dst, b);
    __cn_vector_negate_s16(n, dst, dst);
    __cn_vector_sub_s16(n, src1, src1, src2);
    __cn_vector_and_s16(n, dst, dst, src1);
    __cn_vector_add_s16(n, dst, dst, src2);
    __cn_vector_add_s16(n, src1, src1, src2);
}
_CLC_INLINE void __cn_vector_select_scalar_s16(size_t n, short *a, const unsigned char *b, const short *c, short d) {
    short *dst = a, *src1 = (short *)c, src2 = d;
    __cn_vector_cast_u8_to_s16(n, dst, b);
    __cn_vector_negate_s16(n, dst, dst);
    __cn_vector_sub_scalar_s16(n, src1, src1, src2);
    __cn_vector_and_s16(n, dst, dst, src1);
    __cn_vector_add_scalar_s16(n, dst, dst, src2);
    __cn_vector_add_scalar_s16(n, src1, src1, src2);
}
_CLC_INLINE void __cn_scalar_select_vector_s16(size_t n, short *a, const unsigned char *b, short c, const short *d) {
    short *dst = a, src1 = c, *src2 = (short *)d;
    __cn_vector_cast_u8_to_s16(n, dst, b);
    __cn_vector_negate_s16(n, dst, dst);
    __cn_vector_sub_scalar_s16(n, src2, src2, src1);
    __cn_vector_and_s16(n, dst, dst, src2);
    __cn_vector_add_scalar_s16(n, src2, src2, src1);
    __cn_vector_sub_s16(n, dst, src2, dst);
}

_CLC_INLINE void __cn_vector_select_s64(size_t n, long *a, const unsigned char *b, const long *c, const long *d) {
    long *dst = a, *src1 = (long *)c, *src2 = (long *)d;
    __cn_vector_cast_u8_to_s64(n, dst, b);
    __cn_vector_negate_s64(n, dst, dst);
    __cn_vector_sub_s64(n, src1, src1, src2);
    __cn_vector_and_s64(n, dst, dst, src1);
    __cn_vector_add_s64(n, dst, dst, src2);
    __cn_vector_add_s64(n, src1, src1, src2);
}
_CLC_INLINE void __cn_vector_select_scalar_s64(size_t n, long *a, const unsigned char *b, const long *c, long d) {
    long *dst = a, *src1 = (long *)c, src2 = d;
    __cn_vector_cast_u8_to_s64(n, dst, b);
    __cn_vector_negate_s64(n, dst, dst);
    __cn_vector_sub_scalar_s64(n, src1, src1, src2);
    __cn_vector_and_s64(n, dst, dst, src1);
    __cn_vector_add_scalar_s64(n, dst, dst, src2);
    __cn_vector_add_scalar_s64(n, src1, src1, src2);
}
_CLC_INLINE void __cn_scalar_select_vector_s64(size_t n, long *a, const unsigned char *b, long c, const long *d) {
    long *dst = a, src1 = c, *src2 = (long *)d;
    __cn_vector_cast_u8_to_s64(n, dst, b);
    __cn_vector_negate_s64(n, dst, dst);
    __cn_vector_sub_scalar_s64(n, src2, src2, src1);
    __cn_vector_and_s64(n, dst, dst, src2);
    __cn_vector_add_scalar_s64(n, src2, src2, src1);
    __cn_vector_sub_s64(n, dst, src2, dst);
}

#define VECTOR_SELECT_32(TYPE)                                                                                                   \
_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, TYPE *a, const unsigned char *b, const TYPE *c, const TYPE *d) { \
    __cn_vector_select_s32(n, (int *)a, b, (const int *)c, (const int *)d);                                                      \
}                                                                                                                                \
_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, TYPE *a, const unsigned char *b, const TYPE *c, TYPE d) {        \
    __cn_vector_select_scalar_s32(n, (int *)a, b, (const int *)c, *(int *)&d);                                                   \
}                                                                                                                                \
_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, TYPE *a, const unsigned char *b, TYPE c, const TYPE *d) {        \
    __cn_scalar_select_vector_s32(n, (int *)a, b, *(int *)&c, (const int *)d);                                                   \
}
#define VECTOR_SELECT_16(TYPE)                                                                                                   \
_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, TYPE *a, const unsigned char *b, const TYPE *c, const TYPE *d) { \
    __cn_vector_select_s16(n, (short *)a, b, (const short *)c, (const short *)d);                                                \
}                                                                                                                                \
_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, TYPE *a, const unsigned char *b, const TYPE *c, TYPE d) {        \
    __cn_vector_select_scalar_s16(n, (short *)a, b, (const short *)c, *(short *)&d);                                             \
}                                                                                                                                \
_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, TYPE *a, const unsigned char *b, TYPE c, const TYPE *d) {        \
    __cn_scalar_select_vector_s16(n, (short *)a, b, *(short *)&c, (const short *)d);                                             \
}
#define VECTOR_SELECT_64(TYPE)                                                                                                   \
_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, TYPE *a, const unsigned char *b, const TYPE *c, const TYPE *d) { \
    __cn_vector_select_s64(n, (long *)a, b, (const long *)c, (const long *)d);                                                   \
}                                                                                                                                \
_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, TYPE *a, const unsigned char *b, const TYPE *c, TYPE d) {        \
    __cn_vector_select_scalar_s64(n, (long *)a, b, (const long *)c, *(long *)&d);                                                \
}                                                                                                                                \
_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_select(size_t n, TYPE *a, const unsigned char *b, TYPE c, const TYPE *d) {        \
    __cn_scalar_select_vector_s64(n, (long *)a, b, *(long *)&c, (const long *)d);                                                \
}

VECTOR_SELECT_32(float)
VECTOR_SELECT_32(int)
VECTOR_SELECT_32(unsigned int)
#ifdef cl_khr_fp16
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
VECTOR_SELECT_16(half)
#endif
VECTOR_SELECT_16(short)
VECTOR_SELECT_16(unsigned short)
VECTOR_SELECT_64(long)
VECTOR_SELECT_64(unsigned long)
