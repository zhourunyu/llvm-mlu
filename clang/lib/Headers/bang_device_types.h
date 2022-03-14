/*===-----------------------------------------------------------------------===
 *
 * Copyright (C) [2020] by Cambricon, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *===-----------------------------------------------------------------------===
 */
#ifndef  __BANG_DEVICE_TYPES_H__
#define  __BANG_DEVICE_TYPES_H__

#ifndef __BANG__
#error "This file is for BANG compilation only."
#endif  // __BANG__

#define __mlu_host__    __attribute__((mlu_host))
#define __mlu_global__  __attribute__((mlu_global))
#define __mlu_entry__   __attribute__((mlu_global))
#define __mlu_device__  __attribute__((mlu_device))
#define __mlu_func__    __attribute__((mlu_device, always_inline))
#define __mlu_builtin__ __attribute__((mlu_builtin, always_inline))

#define __nram__        __attribute__((mlu_device, mlu_nram))
#define __wram__        __attribute__((mlu_device, mlu_wram))
#define __ldram__       __attribute__((mlu_device, mlu_ldram))
#define __mlu_shared__  __attribute__((mlu_device, mlu_shared))
#define __mlu_const__   __attribute__((mlu_device, mlu_const))

#define __mlu_simt__    __attribute__((mlu_device, mlu_simt, always_inline))
#define __grid__        __attribute__((mlu_device, mlu_grid))

#define MAX_HALF_VALUE 65504
#define MIN_HALF_VALUE -65504
#define P_HALF_INF 0x7C00
#define N_HALF_INF 0xFC00
#define MAX_INT_VALUE 2147483647
#define MIN_INT_VALUE -2147483648

#include <stdint.h>
typedef int8_t fix8;
typedef int8_t int8;
typedef int16_t int16;

struct int4x2_t {
  unsigned char value;
};

struct bfloat16_t {
  unsigned short value;
};

typedef enum {
  GDRAM2NRAM = 0,
  NRAM2GDRAM,
  LDRAM2NRAM,
  NRAM2LDRAM,
  GDRAM2WRAM,
  WRAM2GDRAM,
  LDRAM2WRAM,
  WRAM2LDRAM,
  NRAM2NRAM,
  NRAM2WRAM,
  WRAM2NRAM,
  GDRAM2SRAM,
  SRAM2GDRAM,
  LDRAM2SRAM,
  SRAM2LDRAM,
  SRAM2NRAM,
  NRAM2SRAM,
  SRAM2WRAM,
  WRAM2SRAM,
  GDRAM2GDRAM,
  GDRAM2LDRAM,
  LDRAM2GDRAM,
  LDRAM2LDRAM,
  SRAM2SRAM,
} mluMemcpyDirection_t;

struct uint3 {
  unsigned int x, y, z;
};

struct dim3 {
  unsigned int x, y, z;
#if defined(__cplusplus)
#if __cplusplus >= 201103L
  constexpr dim3(unsigned int vx = 1,
                 unsigned int vy = 1,
                 unsigned int vz = 1)
    : x(vx), y(vy), z(vz) {}
  constexpr dim3(uint3 v) : x(v.x), y(v.y), z(v.z) {}
  constexpr operator uint3(void) const {
    return uint3{x, y, z};
  }
#else
  dim3(unsigned int vx = 1,
       unsigned int vy = 1,
       unsigned int vz = 1)
    : x(vx), y(vy), z(vz) {}
  dim3(uint3 v) :
    x(v.x), y(v.y), z(v.z) {}
  operator uint3(void) const {
    uint3 t; t.x = x; t.y = y; t.z = z; return t;
  }
#endif
#endif /* __cplusplus */
};

// -----------------------------------------------------------------------------
// CompareMode - fusion compare filter compare mode
// -----------------------------------------------------------------------------
enum CompareMode{
    CMP_EQ,
    CMP_NE,
    CMP_LT,
    CMP_LE,
    CMP_GT,
    CMP_GE
};

// -----------------------------------------------------------------------------
// va_arg - macros used in device-side variadic function
// -----------------------------------------------------------------------------
#if __BANG_ARCH__ >= 200
typedef char * _mlu_va_list;
#define _MLU_ADDRESSOF(v)   (&(v) )
#define _MLU_ALIGNOF(n)     ((sizeof(n) + 8 - 1) & ~(8 - 1))
#define _mlu_va_start(ap,v) (ap = (_mlu_va_list)_MLU_ADDRESSOF(v) +  _MLU_ALIGNOF(v)) // NOLINT
#define _mlu_va_arg(ap,t)   (*((t*)((ap += _MLU_ALIGNOF(t)) - _MLU_ALIGNOF(t))))
#define _mlu_va_end(ap)     (ap = (_mlu_va_list)0 )
#define _mlu_va_ptr(ap,t)   (((t*)((ap += _MLU_ALIGNOF(t)) - _MLU_ALIGNOF(t))))
#endif  //  __BANG_ARCH__ >= 200

// -----------------------------------------------------------------------------
// Tensor data types
// -----------------------------------------------------------------------------
typedef __attribute__((__vector_size__(8*sizeof(int)), aligned(32))) int v8i32;

template<typename T>
struct tid { v8i32 tid; };

// -----------------------------------------------------------------------------
// Grid data types
// Note: Use padded type for datatypes with bitwidth less than 32 (e.g. v1f16
//       is actually vector of float with vector size 2), since scalars are
//       32-bit aligned on MLU architecture, and using v2f16 to represent
//       half might be confusing to users.
// -----------------------------------------------------------------------------
typedef __attribute__((__vector_size__(sizeof(float)), aligned(1024))) float v1f32;
typedef __attribute__((__vector_size__(2 * sizeof(half)), aligned(1024))) half v2f16;
typedef __attribute__((__vector_size__(2 * sizeof(half)), aligned(1024))) half v1f16;
typedef __attribute__((__vector_size__(sizeof(int32_t)), aligned(1024))) int32_t v1s32;
typedef __attribute__((__vector_size__(2 * sizeof(int16_t)), aligned(1024))) int16_t v2s16;
typedef __attribute__((__vector_size__(2 * sizeof(int16_t)), aligned(1024))) int16_t v1s16;
typedef __attribute__((__vector_size__(4 * sizeof(char)), aligned(1024))) char v4s8;
typedef __attribute__((__vector_size__(4 * sizeof(char)), aligned(1024))) char v1s8;
typedef __attribute__((__vector_size__(sizeof(uint32_t)), aligned(1024))) uint32_t v1u32;
typedef __attribute__((__vector_size__(2 * sizeof(uint16_t)), aligned(1024))) uint16_t v2u16;
typedef __attribute__((__vector_size__(2 * sizeof(uint16_t)), aligned(1024))) uint16_t v1u16;
typedef __attribute__((__vector_size__(4 * sizeof(unsigned char)), aligned(1024))) unsigned char v4u8;
typedef __attribute__((__vector_size__(4 * sizeof(unsigned char)), aligned(1024))) unsigned char v1u8;

struct __f32x1 { v1f32 __elem; };
struct __f16x2 { v2f16 __elem; };
struct __f16x1 { v1f16 __elem; };
struct __s32x1 { v1s32 __elem; };
struct __s16x2 { v2s16 __elem; };
struct __s16x1 { v1s16 __elem; };
struct __s8x4 { v4s8 __elem; };
struct __s8x1 { v1s8 __elem; };
struct __u32x1 { v1u32 __elem; };
struct __u16x2 { v2u16 __elem; };
struct __u16x1 { v1u16 __elem; };
struct __u8x4 { v4u8 __elem; };
struct __u8x1 { v1u8 __elem; };

#define __grid_f32x1 __grid__ __f32x1
#define __grid_f16x2 __grid__ __f16x2
#define __grid_f16x1 __grid__ __f16x1
#define __grid_s32x1 __grid__ __s32x1
#define __grid_s16x2 __grid__ __s16x2
#define __grid_s16x1 __grid__ __s16x1
#define __grid_s8x4 __grid__ __s8x4
#define __grid_s8x1 __grid__ __s8x1
#define __grid_u32x1 __grid__ __u32x1
#define __grid_u16x2 __grid__ __u16x2
#define __grid_u16x1 __grid__ __u16x1
#define __grid_u8x4 __grid__ __u8x4
#define __grid_u8x1 __grid__ __u8x1

enum mluFusionOpCode{
    FUSION_FMA,
    FUSION_FMS,
    FUSION_FAM,
    FUSION_FSM,
    FUSION_FAA,
    FUSION_FAS,
    FUSION_FSS,
    FUSION_FSA
};

enum mluPoolBPOverlap {
  OVERLAP_ACC,
  OVERLAP_COVER
};

enum TRAP_BANG_ID {
  TRAP_BANG_V1_UNSUPPORTED = -24,
  TRAP_BANG_MAXIMUM_PIPELINE,
  TRAP_BANG_INVALID_COMPARE_MODE,
  TRAP_BANG_INVALID_FUSION_OP,
  TRAP_BANG_UNSUPPORTED_STRIDE_MEMCPY_DIR,
  TRAP_BANG_UNSUPPORTED_MEMCPY_DIR,
  TRAP_BANG_MPUCORE_UNSUPPORTED,
  TRAP_BANG_SHARED_MEM_UNSUPPORTED,
  TRAP_BANG_UNIMPLEMENTED_API,
  TRAP_BANG_INVALID_SIZE_RESTRICTION,
  TRAP_BANG_INVALID_POOLBP_OVERLAP_OPTION,
  TRAP_BANG_INTERNAL_ERR_END = -1,
  TRAP_BANG_UNDEF = 0
};

#endif  // __BANG_DEVICE_TYPES_H__
