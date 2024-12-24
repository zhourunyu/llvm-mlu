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

#define VECTOR_CAST(DST_TYPE, SRC_TYPE, __CLC_BUILTIN) \
_CLC_DEF _CLC_OVERLOAD void __spirv_ocl_vector_cast(size_t n, DST_TYPE *a, const SRC_TYPE *b) { \
    __CLC_BUILTIN(n, a, b); \
}

VECTOR_CAST(signed char,    float,          __cn_vector_cast_f32_to_s8_tz)
VECTOR_CAST(unsigned char,  float,          __cn_vector_cast_f32_to_u8_tz)
VECTOR_CAST(short,          float,          __cn_vector_cast_f32_to_s16_tz)
VECTOR_CAST(unsigned short, float,          __cn_vector_cast_f32_to_u16_tz)
VECTOR_CAST(int,            float,          __cn_vector_cast_f32_to_s32_tz)
VECTOR_CAST(unsigned int,   float,          __cn_vector_cast_f32_to_u32_tz)
VECTOR_CAST(long,           float,          __cn_vector_cast_f32_to_s64_tz)
VECTOR_CAST(unsigned long,  float,          __cn_vector_cast_f32_to_u64_tz)
VECTOR_CAST(float,          signed char,    __cn_vector_cast_s8_to_f32)
VECTOR_CAST(unsigned char,  signed char,    __cn_vector_cast_s8_to_u8)
VECTOR_CAST(short,          signed char,    __cn_vector_cast_s8_to_s16)
VECTOR_CAST(unsigned short, signed char,    __cn_vector_cast_s8_to_u16)
VECTOR_CAST(int,            signed char,    __cn_vector_cast_s8_to_s32)
VECTOR_CAST(unsigned int,   signed char,    __cn_vector_cast_s8_to_u32)
VECTOR_CAST(long,           signed char,    __cn_vector_cast_s8_to_s64)
VECTOR_CAST(unsigned long,  signed char,    __cn_vector_cast_s8_to_u64)
VECTOR_CAST(float,          unsigned char,  __cn_vector_cast_u8_to_f32)
VECTOR_CAST(signed char,    unsigned char,  __cn_vector_cast_u8_to_s8)
VECTOR_CAST(short,          unsigned char,  __cn_vector_cast_u8_to_s16)
VECTOR_CAST(unsigned short, unsigned char,  __cn_vector_cast_u8_to_u16)
VECTOR_CAST(int,            unsigned char,  __cn_vector_cast_u8_to_s32)
VECTOR_CAST(unsigned int,   unsigned char,  __cn_vector_cast_u8_to_u32)
VECTOR_CAST(long,           unsigned char,  __cn_vector_cast_u8_to_s64)
VECTOR_CAST(unsigned long,  unsigned char,  __cn_vector_cast_u8_to_u64)
VECTOR_CAST(float,          short,          __cn_vector_cast_s16_to_f32)
VECTOR_CAST(signed char,    short,          __cn_vector_cast_s16_to_s8)
VECTOR_CAST(unsigned char,  short,          __cn_vector_cast_s16_to_u8)
VECTOR_CAST(unsigned short, short,          __cn_vector_cast_s16_to_u16)
VECTOR_CAST(int,            short,          __cn_vector_cast_s16_to_s32)
VECTOR_CAST(unsigned int,   short,          __cn_vector_cast_s16_to_u32)
VECTOR_CAST(long,           short,          __cn_vector_cast_s16_to_s64)
VECTOR_CAST(unsigned long,  short,          __cn_vector_cast_s16_to_u64)
VECTOR_CAST(float,          unsigned short, __cn_vector_cast_u16_to_f32)
VECTOR_CAST(signed char,    unsigned short, __cn_vector_cast_u16_to_s8)
VECTOR_CAST(unsigned char,  unsigned short, __cn_vector_cast_u16_to_u8)
VECTOR_CAST(short,          unsigned short, __cn_vector_cast_u16_to_s16)
VECTOR_CAST(int,            unsigned short, __cn_vector_cast_u16_to_s32)
VECTOR_CAST(unsigned int,   unsigned short, __cn_vector_cast_u16_to_u32)
VECTOR_CAST(long,           unsigned short, __cn_vector_cast_u16_to_s64)
VECTOR_CAST(unsigned long,  unsigned short, __cn_vector_cast_u16_to_u64)
VECTOR_CAST(float,          int,            __cn_vector_cast_s32_to_f32_rn)
VECTOR_CAST(signed char,    int,            __cn_vector_cast_s32_to_s8)
VECTOR_CAST(unsigned char,  int,            __cn_vector_cast_s32_to_u8)
VECTOR_CAST(short,          int,            __cn_vector_cast_s32_to_s16)
VECTOR_CAST(unsigned short, int,            __cn_vector_cast_s32_to_u16)
VECTOR_CAST(unsigned int,   int,            __cn_vector_cast_s32_to_u32)
VECTOR_CAST(long,           int,            __cn_vector_cast_s32_to_s64)
VECTOR_CAST(unsigned long,  int,            __cn_vector_cast_s32_to_u64)
VECTOR_CAST(float,          unsigned int,   __cn_vector_cast_u32_to_f32_rn)
VECTOR_CAST(signed char,    unsigned int,   __cn_vector_cast_u32_to_s8)
VECTOR_CAST(unsigned char,  unsigned int,   __cn_vector_cast_u32_to_u8)
VECTOR_CAST(short,          unsigned int,   __cn_vector_cast_u32_to_s16)
VECTOR_CAST(unsigned short, unsigned int,   __cn_vector_cast_u32_to_u16)
VECTOR_CAST(int,            unsigned int,   __cn_vector_cast_u32_to_s32)
VECTOR_CAST(long,           unsigned int,   __cn_vector_cast_u32_to_s64)
VECTOR_CAST(unsigned long,  unsigned int,   __cn_vector_cast_u32_to_u64)
VECTOR_CAST(float,          long,           __cn_vector_cast_s64_to_f32_rn)
VECTOR_CAST(signed char,    long,           __cn_vector_cast_s64_to_s8)
VECTOR_CAST(unsigned char,  long,           __cn_vector_cast_s64_to_u8)
VECTOR_CAST(short,          long,           __cn_vector_cast_s64_to_s16)
VECTOR_CAST(unsigned short, long,           __cn_vector_cast_s64_to_u16)
VECTOR_CAST(int,            long,           __cn_vector_cast_s64_to_s32)
VECTOR_CAST(unsigned int,   long,           __cn_vector_cast_s64_to_u32)
VECTOR_CAST(unsigned long,  long,           __cn_vector_cast_s64_to_u64)
VECTOR_CAST(float,          unsigned long,  __cn_vector_cast_u64_to_f32_rn)
VECTOR_CAST(signed char,    unsigned long,  __cn_vector_cast_u64_to_s8)
VECTOR_CAST(unsigned char,  unsigned long,  __cn_vector_cast_u64_to_u8)
VECTOR_CAST(short,          unsigned long,  __cn_vector_cast_u64_to_s16)
VECTOR_CAST(unsigned short, unsigned long,  __cn_vector_cast_u64_to_u16)
VECTOR_CAST(int,            unsigned long,  __cn_vector_cast_u64_to_s32)
VECTOR_CAST(unsigned int,   unsigned long,  __cn_vector_cast_u64_to_u32)
VECTOR_CAST(long,           unsigned long,  __cn_vector_cast_u64_to_s64)

#ifdef cl_khr_fp16
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
VECTOR_CAST(float,          half,           __cn_vector_cast_f16_to_f32)
VECTOR_CAST(signed char,    half,           __cn_vector_cast_f16_to_s8_tz)
VECTOR_CAST(unsigned char,  half,           __cn_vector_cast_f16_to_u8_tz)
VECTOR_CAST(short,          half,           __cn_vector_cast_f16_to_s16_tz)
VECTOR_CAST(unsigned short, half,           __cn_vector_cast_f16_to_u16_tz)
VECTOR_CAST(int,            half,           __cn_vector_cast_f16_to_s32_tz)
VECTOR_CAST(unsigned int,   half,           __cn_vector_cast_f16_to_u32_tz)
VECTOR_CAST(long,           half,           __cn_vector_cast_f16_to_s64_tz)
VECTOR_CAST(unsigned long,  half,           __cn_vector_cast_f16_to_u64_tz)
VECTOR_CAST(half,           float,          __cn_vector_cast_f32_to_f16_rn)
VECTOR_CAST(half,           signed char,    __cn_vector_cast_s8_to_f16)
VECTOR_CAST(half,           unsigned char,  __cn_vector_cast_u8_to_f16)
VECTOR_CAST(half,           short,          __cn_vector_cast_s16_to_f16_rn)
VECTOR_CAST(half,           unsigned short, __cn_vector_cast_u16_to_f16_rn)
VECTOR_CAST(half,           int,            __cn_vector_cast_s32_to_f16_rn)
VECTOR_CAST(half,           unsigned int,   __cn_vector_cast_u32_to_f16_rn)
VECTOR_CAST(half,           long,           __cn_vector_cast_s64_to_f16_rn)
VECTOR_CAST(half,           unsigned long,  __cn_vector_cast_u64_to_f16_rn)
#endif