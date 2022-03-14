/*===------- bang_math_decls.h - Cambricon Neuware Math API Declares -------===
 *
 * Copyright (C) [2021] by Cambricon, Inc.
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
#ifndef __BANG_MATH_DECLS_H__
#define __BANG_MATH_DECLS_H__

#ifndef __BANG__
#error "This file is for BANG compilation only."
#endif  // __BANG__

#include <bang.h>

#define __DEVICE__ __mlu_device__

extern "C" {

/**
 * @brief
 * Determine whether the floating-point value x is an infinite value.
 *
 * @return
 * - scalar: returns a nonzero scalar y if and only if x is infinite.
 * - vector: returns a nonzero vector y[i] if and only if x[i] is infinite.
 *
 * @attention
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ int __cn_scalar_isinf_f32(float x);
__DEVICE__ short __cn_scalar_isinf_f16(half x);
__DEVICE__ void __cn_vector_isinf_f32(int n, int* y, const float* x);
__DEVICE__ void __cn_vector_isinf_f16(int n, short* y, const half* x);

/**
 * Determine whether the floating-point value x is a NaN.
 *
 * @return
 * - scalar: returns a nonzero scalar y if and only if x is NaN.
 * - vector: returns a nonzero vector y[i] if and only if x[i] is NaN.
 *
 * @attention
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ int __cn_scalar_isnan_f32(float x);
__DEVICE__ short __cn_scalar_isnan_f16(half x);
__DEVICE__ void __cn_vector_isnan_f32(int n, int* y, const float* x);
__DEVICE__ void __cn_vector_isnan_f16(int n, short* y, const half* x);

/**
 * @brief
 * Compute the reciprocal of x in different rounding mode.
 *
 * @return
 * - scalar: y = 1 / x.
 * - vector: y[i] = 1 / x[i].
 *
 * @attention
 * - max to 1 ULP(https://hal.inria.fr/inria-00070503/document) error for full range.
 * - dn: round-down mode (to negative infinity).
 * - rn: round-to_nearest-even rounding mode.
 * - up: round-up mode (to positive infinity).
 * - tz: round-towards-zero mode.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_rcp_f32_dn(float x);
__DEVICE__ float __cn_scalar_rcp_f32_up(float x);
__DEVICE__ float __cn_scalar_rcp_f32_tz(float x);
__DEVICE__ float __cn_scalar_rcp_f32_rn(float x);
__DEVICE__ half  __cn_scalar_rcp_f16_dn(half x);
__DEVICE__ half  __cn_scalar_rcp_f16_up(half x);
__DEVICE__ half  __cn_scalar_rcp_f16_tz(half x);
__DEVICE__ half  __cn_scalar_rcp_f16_rn(half x);
__DEVICE__ void  __cn_vector_rcp_f32_dn(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_rcp_f32_up(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_rcp_f32_tz(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_rcp_f32_rn(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_rcp_f16_dn(int n, half* y, const half* x);
__DEVICE__ void  __cn_vector_rcp_f16_up(int n, half* y, const half* x);
__DEVICE__ void  __cn_vector_rcp_f16_tz(int n, half* y, const half* x);
__DEVICE__ void  __cn_vector_rcp_f16_rn(int n, half* y, const half* x);

/**
 * @brief
 * Compute the sum of x and y in different rouding mode.
 *
 * @return
 * - scalar: z = (x + y).
 * - vector: z[i] = (x[i] + y[i]).
 *
 * @attention
 * - dn: round-down mode (to negative infinity).
 * - rn: round-to_nearest-even rounding mode.
 * - up: round-up mode (to positive infinity).
 * - tz: round-towards-zero mode.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_add_f32_dn(float x, float y);
__DEVICE__ float __cn_scalar_add_f32_up(float x, float y);
__DEVICE__ float __cn_scalar_add_f32_tz(float x, float y);
__DEVICE__ float __cn_scalar_add_f32_rn(float x, float y);
__DEVICE__ half  __cn_scalar_add_f16_dn(half x, half y);
__DEVICE__ half  __cn_scalar_add_f16_up(half x, half y);
__DEVICE__ half  __cn_scalar_add_f16_tz(half x, half y);
__DEVICE__ half  __cn_scalar_add_f16_rn(half x, half y);
__DEVICE__ uint32_t __cn_scalar_add_u32(uint32_t x, uint32_t y);
__DEVICE__ int32_t  __cn_scalar_add_s32(int32_t x, int32_t y);
__DEVICE__ uint64_t __cn_scalar_add_u64(uint64_t x, uint64_t y);
__DEVICE__ int64_t  __cn_scalar_add_s64(int64_t x, int64_t y);
__DEVICE__ void  __cn_vector_add_f32_dn(int n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_add_f32_up(int n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_add_f32_tz(int n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_add_f32_rn(int n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_add_f16_dn(int n, half* z, const half* x, const half* y);
__DEVICE__ void  __cn_vector_add_f16_up(int n, half* z, const half* x, const half* y);
__DEVICE__ void  __cn_vector_add_f16_tz(int n, half* z, const half* x, const half* y);
__DEVICE__ void  __cn_vector_add_f16_rn(int n, half* z, const half* x, const half* y);
__DEVICE__ void  __cn_vector_add_u32(int n, uint32_t* z, const uint32_t* x, const uint32_t* y);
__DEVICE__ void  __cn_vector_add_s32(int n, int32_t* z, const int32_t* x, const int32_t* y);
__DEVICE__ void  __cn_vector_add_u64(int n, uint64_t* z, const uint64_t* x, const uint64_t* y);
__DEVICE__ void  __cn_vector_add_s64(int n, int64_t* z, const int64_t* x, const int64_t* y);

/**
 * @brief
 * Compute the difference of x and y in different rouding mode.
 *
 * @return
 * - scalar: z = (x - y).
 * - vector: z[i] = (x[i] - y[i]).
 *
 * @attention
 * - dn: round-down mode (to -Inf).
 * - rn: round-to_nearest-even rounding mode.
 * - up: round-up mode (to +Inf).
 * - tz: round-towards-zero mode.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_sub_f32_dn(float x, float y);
__DEVICE__ float __cn_scalar_sub_f32_up(float x, float y);
__DEVICE__ float __cn_scalar_sub_f32_tz(float x, float y);
__DEVICE__ float __cn_scalar_sub_f32_rn(float x, float y);
__DEVICE__ half  __cn_scalar_sub_f16_dn(half x, half y);
__DEVICE__ half  __cn_scalar_sub_f16_up(half x, half y);
__DEVICE__ half  __cn_scalar_sub_f16_tz(half x, half y);
__DEVICE__ half  __cn_scalar_sub_f16_rn(half x, half y);
__DEVICE__ int32_t  __cn_scalar_sub_s32(int32_t x, int32_t y);
__DEVICE__ uint32_t __cn_scalar_sub_u32(uint32_t x, uint32_t y);
__DEVICE__ void  __cn_vector_sub_f32_dn(int n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_sub_f32_up(int n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_sub_f32_tz(int n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_sub_f32_rn(int n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_sub_f16_dn(int n, half* z, const half* x, const half* y);
__DEVICE__ void  __cn_vector_sub_f16_up(int n, half* z, const half* x, const half* y);
__DEVICE__ void  __cn_vector_sub_f16_tz(int n, half* z, const half* x, const half* y);
__DEVICE__ void  __cn_vector_sub_f16_rn(int n, half* z, const half* x, const half* y);
__DEVICE__ void  __cn_vector_sub_s32(int n, int32_t* z, int32_t* x, int32_t* y);
__DEVICE__ void  __cn_vector_sub_u32(int n, uint32_t* z, uint32_t* x, uint32_t* y);

/**
 * @brief
 * Compute the product of x and y in different rouding mode.
 *
 * @return
 * - scalar: z = (x * y).
 * - vector: z[i] = (x[i] * y[i]).
 *
 * @attention
 * - dn: round-down mode (to -Inf).
 * - rn: round-to_nearest-even rounding mode.
 * - up: round-up mode (to +Inf).
 * - tz: round-towards-zero mode.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 * - compute_20 scalar/vector f16/f32: +-NAN and +-INF are not supported.
 * - compute_30 vector u32/s32: the performance is optimal when n > 64 && n % 64 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_mul_f32_dn(float x, float y);
__DEVICE__ float __cn_scalar_mul_f32_up(float x, float y);
__DEVICE__ float __cn_scalar_mul_f32_tz(float x, float y);
__DEVICE__ float __cn_scalar_mul_f32_rn(float x, float y);
__DEVICE__ half  __cn_scalar_mul_f16_dn(half x, half y);
__DEVICE__ half  __cn_scalar_mul_f16_up(half x, half y);
__DEVICE__ half  __cn_scalar_mul_f16_tz(half x, half y);
__DEVICE__ half  __cn_scalar_mul_f16_rn(half x, half y);
__DEVICE__ uint32_t __cn_scalar_mul_u32(uint32_t x, uint32_t y);
__DEVICE__ int32_t __cn_scalar_mul_s32(int32_t x, int32_t y);
__DEVICE__ void  __cn_vector_mul_f32_dn(int n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_mul_f32_up(int n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_mul_f32_tz(int n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_mul_f32_rn(int n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_mul_f16_dn(int n, half* z, const half* x, const half* y);
__DEVICE__ void  __cn_vector_mul_f16_up(int n, half* z, const half* x, const half* y);
__DEVICE__ void  __cn_vector_mul_f16_tz(int n, half* z, const half* x, const half* y);
__DEVICE__ void  __cn_vector_mul_f16_rn(int n, half* z, const half* x, const half* y);
__DEVICE__ void __cn_vector_mul_u32(int n, uint32_t* z, const uint32_t* x, const uint32_t* y);
__DEVICE__ void __cn_vector_mul_s32(int n, int32_t* z, const int32_t* x, const int32_t* y);

/**
 * @brief
 * Divide two value x by y in different rouding mode.
 *
 * @return
 * - scalar: z = (x / y).
 * - vector: z[i] = (x[i] / y[i]).
 *
 * @attention
 * - f32: x and y only surpport range [-Inf, -1.0e-38f] and [+1.0e-38f, +Inf].
 * - f16: x and y only surpport range [-Inf, -1.0e-05f] and [+1.0e-05f, +Inf].
 * - when the result of f16 is greater than 65504, then the function returns 65504.
 * - max to 2 ULP(https://hal.inria.fr/inria-00070503/document) error for supported range.
 * - dn: round-down mode (to -Inf).
 * - rn: round-to_nearest-even rounding mode.
 * - up: round-up mode (to +Inf).
 * - tz: round-towards-zero mode.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_div_f32_dn(float x, float y);
__DEVICE__ float __cn_scalar_div_f32_up(float x, float y);
__DEVICE__ float __cn_scalar_div_f32_tz(float x, float y);
__DEVICE__ float __cn_scalar_div_f32_rn(float x, float y);
__DEVICE__ int32_t  __cn_scalar_div_s32(int32_t x, int32_t y);
__DEVICE__ uint32_t __cn_scalar_div_u32(uint32_t x, uint32_t y);
__DEVICE__ half  __cn_scalar_div_f16_dn(half x, half y);
__DEVICE__ half  __cn_scalar_div_f16_up(half x, half y);
__DEVICE__ half  __cn_scalar_div_f16_tz(half x, half y);
__DEVICE__ half  __cn_scalar_div_f16_rn(half x, half y);
__DEVICE__ void  __cn_vector_div_f32_dn(int n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_div_f32_up(int n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_div_f32_tz(int n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_div_f32_rn(int n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_div_s32(int n, int32_t* z, const int32_t* x, const int32_t* y);
__DEVICE__ void  __cn_vector_div_u32(int n, uint32_t* z, const uint32_t* x, const uint32_t* y);
__DEVICE__ void  __cn_vector_div_f16_dn(int n, half* z, const half* x, const half* y);
__DEVICE__ void  __cn_vector_div_f16_up(int n, half* z, const half* x, const half* y);
__DEVICE__ void  __cn_vector_div_f16_tz(int n, half* z, const half* x, const half* y);
__DEVICE__ void  __cn_vector_div_f16_rn(int n, half* z, const half* x, const half* y);

/**
 * @brief
 * Calculate the floating-point remainder of x/y. The floating point remainder
 * of the division operation x/y calculated by this function is exactly the
 * value x - N * y, where N is x / y with its fractional part truncated.
 * The computed value will have the same sign as x, and its magnitude will be
 * less than magnitude of y.
 *
 * @return
 * - scalar: z = remainder of x / y.
 * - vector: z[i] = remainder of x[i] / y[i].
 *
 * @attention
 * - mod(+0, y) return +0 if y is not zero.
 * - mod(-0, y) return -0 if y is not zero.
 * - mod(x, +Inf) return x if x is finite.
 * - mod(x, -Inf) return x if x is finite.
 * - mod(x, y) return NaN if x is +Inf/-Inf or y is zero.
 * - mod(NaN, y) return NaN.
 * - mod(x, NaN) return NaN.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_mod_f32(float x, float y);
__DEVICE__ half  __cn_scalar_mod_f16(half x, half y);
__DEVICE__ void  __cn_vector_mod_f32(int n, float* z, const float* y, const float* x);
__DEVICE__ void  __cn_vector_mod_f16(int n, half* z, const half* y, const half* x);

/**
 * @brief
 * Calculate the nonnegative square root of input x.
 *
 * @return
 * - scalar: y = sqrt(x).
 * - vector: y[i] = sqrt(x[i]).
 *
 * @attention
 * - sqrt(+0) returns +0.
 * - sqrt(-0) returns -0.
 * - sqrt(+Inf) returns +Inf.
 * - sqrt(x) returns NaN if x < 0.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_sqrt_f32(float x);
__DEVICE__ half  __cn_scalar_sqrt_f16(half x);
__DEVICE__ void  __cn_vector_sqrt_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_sqrt_f16(int n, half* y, const half* x);

/**
 * @brief
 * Calculate the reciprocal of the nonnegative square root of x.
 *
 * @return
 * - scalar: y = rsqrt(x).
 * - vector: y[i] = rsqrt(x[i]).
 *
 * @attention
 * - rsqrt(+Inf) returns +0.
 * - rsqrt(+0) returns +Inf.
 * - rsqrt(-0) returns -Inf.
 * - rsqrt(x) returns NaN if x < 0.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ half  __cn_scalar_rsqrt_f16(half x);
__DEVICE__ float __cn_scalar_rsqrt_f32(float x);
__DEVICE__ void  __cn_vector_rsqrt_f16(int n, half* y, const half* x);
__DEVICE__ void  __cn_vector_rsqrt_f32(int n, float* y, const float* x);

/**
 * @brief
 * Calculate the absolute value of input argument x.
 *
 * @return
 * - scalar: y = abs(x).
 * - vector: y[i] = abs(x[i]).
 *
 * @attention
 * - abs(+Inf) returns +Inf.
 * - abs(-Inf) returns +Inf.
 * - abs(+0) returns 0.
 * - abs(-0) returns 0.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_abs_f32(float x);
__DEVICE__ half  __cn_scalar_abs_f16(half x);
__DEVICE__ void  __cn_vector_abs_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_abs_f16(int n, half* y, const half* x);

/**
 * @brief
 * Determins the maximum numeric value of the arguments x and y,
 * Treats NaN arguments as missing data. If one argument is a NaN and the other
 * is legitimate numeric value, the numeric value is chosen.
 *
 * @return
 * - scalar: z = max(x, y).
 * - vector: z[i] = max(x[i], y[i]).
 *
 * @attention
 * - max(NaN, NaN) returns NaN.
 * - max(NaN, Numeric) returns Numeric.
 * - max(Numeric, NaN) returns Numeric.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_max_f32(float y, float x);
__DEVICE__ half  __cn_scalar_max_f16(half y, half x);
__DEVICE__ uint32_t __cn_scalar_max_u32(uint32_t y, uint32_t x);
__DEVICE__ int32_t  __cn_scalar_max_s32(int32_t y, int32_t x);
__DEVICE__ uint64_t __cn_scalar_max_u64(uint64_t y, uint64_t x);
__DEVICE__ int64_t  __cn_scalar_max_s64(int64_t y, int64_t x);
__DEVICE__ void  __cn_vector_max_f32(int n, float* z, const float* y, const float* x);
__DEVICE__ void  __cn_vector_max_f16(int n, half* z, const half* y, const half* x);
__DEVICE__ void  __cn_vector_max_u32(int n, uint32_t* z, const uint32_t* y, const uint32_t* x);
__DEVICE__ void  __cn_vector_max_s32(int n, int32_t* z, const int32_t* x, const int32_t* y);
__DEVICE__ void  __cn_vector_max_u64(int n, uint64_t* z, const uint64_t* x, const uint64_t* y);
__DEVICE__ void  __cn_vector_max_s64(int n, int64_t* z, const int64_t* x, const int64_t* y);

/**
 * @brief
 * Determins the minimum numeric value of the arguments x and y,
 * Treats NaN arguments as missing data. If one argument is a NaN and the other
 * is legitimate numeric value, the numeric value is chosen.
 *
 * @return
 * - scalar: z = min(x, y).
 * - vector: z[i] = min(x[i], y[i]).
 *
 * @attention
 * - min(NaN, NaN) returns NaN.
 * - min(NaN, Numeric) returns Numeric.
 * - min(Numeric, NaN) returns Numeric.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 * - compute_30 vector f16/f32/u32/s32: the performance is optimal when n > 64 && n % 64 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_min_f32(float y, float x);
__DEVICE__ half  __cn_scalar_min_f16(half y, half x);
__DEVICE__ uint32_t __cn_scalar_min_u32(uint32_t y, uint32_t x);
__DEVICE__ int32_t __cn_scalar_min_s32(int32_t y, int32_t x);
__DEVICE__ void  __cn_vector_min_f32(int n, float* z, const float* y, const float* x);
__DEVICE__ void  __cn_vector_min_f16(int n, half* z, const half* y, const half* x);
__DEVICE__ void __cn_vector_min_u32(int n, uint32_t* z, const uint32_t* x, const uint32_t* y);
__DEVICE__ void __cn_vector_min_s32(int n, int32_t* z, const int32_t* x, const int32_t* y);

/**
 * @brief
 * Round x to the nearest integer value in floating-point format, with halfway
 * cases rounded awary from zero.
 *
 * @return
 * - scalar: returns rounded integer value of x.
 * - vector: returns rounded integer vector of x[i].
 *
 * @attention
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_round_f32(float x);
__DEVICE__ half  __cn_scalar_round_f16(half x);
__DEVICE__ void  __cn_vector_round_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_round_f16(int n, half* y, const half* x);

/**
 * @brief
 * Round x to the nearest integer value that dose not exceed x in magnitude.
 *
 * @return
 * - scalar: returns truncated integer value of x.
 * - vector: returns truncated integer vector of x[i].
 *
 * @attention
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_trunc_f32(float x);
__DEVICE__ half  __cn_scalar_trunc_f16(half x);
__DEVICE__ void  __cn_vector_trunc_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_trunc_f16(int n, half* y, const half* x);

/**
 * @brief
 * Compute the smallest integer value not less than x.
 *
 * @return
 * - scalar: returns integer value of ceil(x) expressed as floating-point number.
 * - vector: returns integer vector of ceil(x[i]) expressed as floating-point number.
 *
 * @attention
 * - ceil(+0) returns +0.
 * - ceil(-0) returns -0.
 * - ceil(+Inf) returns +Inf.
 * - ceil(-Inf) returns -Inf.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_ceil_f32(float x);
__DEVICE__ half  __cn_scalar_ceil_f16(half x);
__DEVICE__ void  __cn_vector_ceil_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_ceil_f16(int n, half* y, const half* x);

/**
 * @brief
 * Calculates the largest integer value which is less than or equal to x.
 *
 * @return
 * - scalar: returns integer value of x expressed as floating-point number.
 * - vector: returns integer vector of x[i] expressed as floating-point number.
 *
 * @return
 * - scalar:
 * - vector:
 *
 * @attention
 * - floor(+0) returns +0.
 * - floor(-0) returns -0.
 * - floor(+Inf) returns +Inf.
 * - floor(-Inf) returns -Inf.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_floor_f32(float x);
__DEVICE__ half  __cn_scalar_floor_f16(half x);
__DEVICE__ void  __cn_vector_floor_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_floor_f16(int n, half* y, const half* x);

/**
 * @brief
 * Calculate the value of the error function for the input x.
 *
 * @return
 * - scalar: y = erf(x).
 * - vector: y[i] = erf(x[i]).
 *
 * @attention
 * - erf(+0) returns +0.
 * - erf(-0) returns -0.
 * - erf(+Inf) returns +1.
 * - erf(-Inf) returns -1.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_erf_f32(float x);
__DEVICE__ half  __cn_scalar_erf_f16(half x);
__DEVICE__ void  __cn_vector_erf_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_erf_f16(int n, half* y, const half* x);

/**
 * @brief
 * Calculate the inverse error function of the input ix, for ix in the
 * interval [-1, +1]. The inverse error function finds the value x that
 * satisfies ix = erf(x), for -1 <= ix <= +1, and -Inf <= x <= +Inf.
 *
 * @return
 * - scalar: y = erfinv(x).
 * - vector: y[i] = erfinv(x[i]).
 *
 * @attention
 * - erfinv(+1) returns +Inf.
 * - erfinv(-1) returns -Inf.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_erfinv_f32(float x);
__DEVICE__ half  __cn_scalar_erfinv_f16(half x);
__DEVICE__ void  __cn_vector_erfinv_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_erfinv_f16(int n, half* y, const half* x);

/**
 * @brief
 * Calculate the inverse complementary error function of the input ix,
 * for ix in the interval [0, 2]. The inverse complementary error function
 * finds the value x that satisfies ix = erfc(x), for 0 <= ix <= 2,
 * and -Inf <= x <= +Inf.
 *
 * @return
 * - scalar: y = erfcinv(x).
 * - vector: y[i] = erfcinv(x[i]).
 *
 * @attention
 * - erfinv(+1) returns +Inf.
 * - erfinv(-1) returns -Inf.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_erfcinv_f32(float x);
__DEVICE__ half  __cn_scalar_erfcinv_f16(half x);
__DEVICE__ void  __cn_vector_erfcinv_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_erfcinv_f16(int n, half* y, const half* x);

/**
 * @brief
 *
 * @return
 * - scalar: y = sin(x).
 * - vector: y[i] = sin(x[i]).
 *
 * @attention
 * - max to 2 ULP(https://hal.inria.fr/inria-00070503/document) error for full range.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_sin_f32(float x);
__DEVICE__ half  __cn_scalar_sin_f16(half x);
__DEVICE__ void  __cn_vector_sin_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_sin_f16(int n, half* y, const half* x);

/**
 * @brief
 *
 * @return
 * - scalar: y = cos(x).
 * - vector: y[i] = cos(x[i]).
 *
 * @attention
 * - max to 2 ULP(https://hal.inria.fr/inria-00070503/document) error for full range.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_cos_f32(float x);
__DEVICE__ half  __cn_scalar_cos_f16(half x);
__DEVICE__ void  __cn_vector_cos_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_cos_f16(int n, half* y, const half* x);

/**
 * @brief
 * Calculate the tangent of the input x (measured in radians).
 *
 * @return
 * - scalar: y = tan(x).
 * - vector: y[i] = tan(x[i]).
 *
 * @attention
 * - tan(+0) returns +0.
 * - tan(-0) returns -0.
 * - tan(+Inf) returns NaN.
 * - tan(-Inf) returns NaN.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_tan_f32(float x);
__DEVICE__ half  __cn_scalar_tan_f16(half x);
__DEVICE__ void  __cn_vector_tan_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_tan_f16(int n, half* y, const half* x);

/**
 * @brief
 * Calculate the principal value of the arc sine of the input x.
 *
 * @return
 * - scalar: y = asin(x).
 * - vector: y[i] = asin(x[i]).
 *
 * @attention
 * - result will be radians, in the interval [-M_PI_2, +M_PI_2] for x inside [-1, +1.
 * - asin(0) returns +0.
 * - asin(x) returns NaN for x outside [-1, +1].
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_asin_f32(float x);
__DEVICE__ half  __cn_scalar_asin_f16(half x);
__DEVICE__ void  __cn_vector_asin_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_asin_f16(int n, half* y, const half* x);

/**
 * @brief
 * Calculate the principal value of the arc cosine of the input x.
 *
 * @return
 * - scalar: y = acos(x).
 * - vector: y[i] = acos(x[i]).
 *
 * @attention
 * - result will be radians, in the interval [0, M_PI] for x inside [-1, +1.
 * - acos(1) returns +0.
 * - acos(x) returns NaN for x outside [-1, +1].
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_acos_f32(float x);
__DEVICE__ half  __cn_scalar_acos_f16(half x);
__DEVICE__ void  __cn_vector_acos_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_acos_f16(int n, half* y, const half* x);

/**
 * @brief
 * Calculate the principal value of the arc tangent of the input x.
 *
 * @return
 * - scalar: y = atan(x).
 * - vector: y[i] = atan(x[i]).
 *
 * @attention
 * - Result will be in radians, int the interval [-M_PI_2, +M_PI_2].
 * - atan(0) returns +0.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 * - compute_20 scalar/vector f16/f32: +-NAN and +-INF are not supported.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_atan_f32(float x);
__DEVICE__ half  __cn_scalar_atan_f16(half x);
__DEVICE__ void  __cn_vector_atan_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_atan_f16(int n, half* y, const half* x);

/**
  * @brief
  * Calculate the principal value of the arc tangent of the ratio of first and
  * second input x / y. The qudrant of the result is determined by the signs of
  * inputs x and y.
  *
  * @return
  * - scalar: y = atan2(x).
  * - vector: y[i] = atan2(x[i]).
  *
  * @attention
  * - result will be in radians, in the interval [-M_PI, +M_PI].
  * - atan2(0, 1) returns +0.
  * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
  * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
  *
  * @note
  * Library Availability:
  * - libdevice.compute_20.bc
  * - libdevice.compute_30.bc
  * - libdevice.compute_35.bc
  **/
__DEVICE__ float __cn_scalar_atan2_f32(float x, float y);
__DEVICE__ half  __cn_scalar_atan2_f16(half x, half y);
__DEVICE__ void  __cn_vector_atan2_f32(int __n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_atan2_f16(int __n, half* z, const half* x, const half* y);

/**
 * @brief
 * Calculate the hyperbolic sine of the input x.
 *
 * @return
 * - scalar: y = sinh(x).
 * - vector: y[i] = sinh(x[i]).
 *
 * @attention
 * - sinh(+0) returns +0.
 * - sinh(-0) returns -0.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_sinh_f32(float x);
__DEVICE__ half  __cn_scalar_sinh_f16(half x);
__DEVICE__ void  __cn_vector_sinh_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_sinh_f16(int n, half* y, const half* x);

/**
 * @brief
 * Calculate the hyperbolic cosine of the input x.
 *
 * @return
 * - scalar: y = cosh(x).
 * - vector: y[i] = cosh(x[i]).
 *
 * @attention
 * - cosh(0) returns 1.
 * - cosh(+Inf) returns +Inf.
 * - cosh(-Inf) returns +Inf.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_cosh_f32(float x);
__DEVICE__ half  __cn_scalar_cosh_f16(half x);
__DEVICE__ void  __cn_vector_cosh_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_cosh_f16(int n, half* y, const half* x);

/**
 * @brief
 * Calculate the hyperbolic tangent of the input x.
 *
 * @return
 * - scalar: y = tanh(x).
 * - vector: y[i] = tanh(x[i]).
 *
 * @attention
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 * - compute_20 scalar/vector f16/f32: +-NAN and +-INF are not supported.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_tanh_f32(float x);
__DEVICE__ half  __cn_scalar_tanh_f16(half x);
__DEVICE__ void  __cn_vector_tanh_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_tanh_f16(int n, half* y, const half* x);

/**
 * @brief
 * Scale x by 2^n by efficient manipulation of the floating-point exponent.
 *
 * @return
 * - scalar: z = scalbn(x, y).
 * - vector: z[i] = scalbn(x[i], y[i]).
 *
 * @attention
 * - scalbn(+0, y) returns +0.
 * - scalbn(-0, y) returns -0.
 * - scalbn(x, 0) returns x.
 * - scalbn(+Inf, y) returns +Inf.
 * - scalbn(-Inf, y) returns -Inf.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_scalbn_f32(float x, int y);
__DEVICE__ half  __cn_scalar_scalbn_f16(half x, int y);
__DEVICE__ void  __cn_vector_scalbn_f32(int n, float* z, const float* x, const int* y);
__DEVICE__ void  __cn_vector_scalbn_f16(int n, half* z, const half* x, const int* y);

/**
  * @brief
  * Calculate the base e exponential of the input x.
  *
  * @return
  * - scalar: y = exp(x).
  * - vector: y[i] = exp(x[i]).
  *
  * @attention
  * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
  * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
  *
  * @note
  * Library Availability:
  * - libdevice.compute_20.bc
  * - libdevice.compute_30.bc
  * - libdevice.compute_35.bc
  **/
__DEVICE__ float __cn_scalar_exp_f32(float x);
__DEVICE__ half  __cn_scalar_exp_f16(half x);
__DEVICE__ void  __cn_vector_exp_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_exp_f16(int n, half* y, const half* x);

/**
 * @brief
 * Calculate the base 2 exponential of the input x.
 *
 * @return
 * - scalar: y = exp2(x).
 * - vector: y[i] = exp2(x[i]).
 *
 * @attention
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_exp2_f32(float x);
__DEVICE__ half  __cn_scalar_exp2_f16(half x);
__DEVICE__ void  __cn_vector_exp2_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_exp2_f16(int n, half* y, const half* x);

/**
 * @brief
 * Calculate the base 10 exponential of the input x.
 *
 * @return
 * - scalar: y = exp10(x).
 * - vector: y[i] = exp10(x[i]).
 *
 * @attention
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_exp10_f32(float x);
__DEVICE__ half  __cn_scalar_exp10_f16(half x);
__DEVICE__ void  __cn_vector_exp10_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_exp10_f16(int n, half* y, const half* x);

/**
 * @brief
 * Calculate the base e exponential of the input x, mimus 1.
 *
 * @return
 * - scalar: y = expm1(x).
 * - vector: y[i] = expm1(x[i]).
 *
 * @attention
 * - returns exp(x) - 1.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_expm1_f32(float x);
__DEVICE__ half  __cn_scalar_expm1_f16(half x);
__DEVICE__ void  __cn_vector_expm1_f32(int n, float* y, float* x);
__DEVICE__ void  __cn_vector_expm1_f16(int n, half* y, half* x);

/**
 * @brief
 * Calculate the length of the hypottenus of a right triangle whose two sides
 * have lengths x and y without undue overflow or underflow.
 *
 * @return
 * - scalar: y = hypot(x, y).
 * - vector: y[i] = hypot(x[i], y[i]).
 *
 * @attention
 * - returns the length of the hypotenuse sqrt(x*x +y*y).
 * - if the correct value would overflow, returns +Inf.
 * - if the correct value would underflow, returns 0.
 * - hypot(x, 0) returns x.
 * - hypot(0, y) returns y.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_hypot_f32(float x, float y);
__DEVICE__ half  __cn_scalar_hypot_f16(half x, half y);
__DEVICE__ void  __cn_vector_hypot_f32(int n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_hypot_f16(int n, half* z, const half* x, const half* y);

/**
 * @brief
 * Calculate the base e logarithm of the input x.
 *
 * @return
 * - scalar: y = log(x).
 * - vector: y[i] = log(x[i]).
 *
 * @attention
 * - log(+0) returns -Inf.
 * - log(-0) returns -Inf.
 * - log(1) returns +0.
 * - log(x) returns NaN for x < 0.
 * - log(+Inf) returns +Inf.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_log_f32(float x);
__DEVICE__ half  __cn_scalar_log_f16(half x);
__DEVICE__ void  __cn_vector_log_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_log_f16(int n, half* y, const half* x);

/**
 * @brief
 * Calculate the base 2 logarithm of the input x.
 *
 * @return
 * - scalar: y = log2(x).
 * - vector: y[i] = log2(x[i]).
 *
 * @attention
 * - log2(+0) returns -Inf.
 * - log2(-0) returns -Inf.
 * - log2(1) returns +0.
 * - log2(x) returns NaN for x < 0.
 * - log2(+Inf) returns +Inf.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_log2_f32(float x);
__DEVICE__ half  __cn_scalar_log2_f16(half x);
__DEVICE__ void  __cn_vector_log2_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_log2_f16(int n, half* y, const half* x);

/**
 * @brief
 * Calculate the base 10 logarithm of the input x.
 *
 * @return
 * - scalar: y = log10(x).
 * - vector: y[i] = log10(x[i]).
 *
 * @attention
 * - log10(+0) returns -Inf.
 * - log10(-0) returns -Inf.
 * - log10(1) returns +0.
 * - log10(x) returns NaN for x < 0.
 * - log10(+Inf) returns +Inf.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_log10_f32(float x);
__DEVICE__ half  __cn_scalar_log10_f16(half x);
__DEVICE__ void  __cn_vector_log10_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_log10_f16(int n, half* y, const half* x);

/**
 * @brief
 * Calculate the value of log(1+x) of the input x.
 *
 * @return
 * - scalar: y = log1p(x).
 * - vector: y[i] = log1p(x[i]).
 *
 * @attention
 * - log1p(+0) returns -Inf.
 * - log1p(-0) returns -Inf.
 * - log1p(-1) returns +0.
 * - log1p(x) returns NaN for x < -1.
 * - log1p(+Inf) returns +Inf.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_log1p_f32(float x);
__DEVICE__ half  __cn_scalar_log1p_f16(half x);
__DEVICE__ void  __cn_vector_log1p_f32(int n, float* y, const float* x);
__DEVICE__ void  __cn_vector_log1p_f16(int n, half* y, const half* x);

/**
 * @brief
 * Calculate the value of x to the power of y.
 *
 * @return
 * - scalar: z = pow(x, y).
 * - vector: z[i] = pow(x[i], y[i]).
 *
 * @attention
 * - pow(+0, y) returns +Inf for an integer y < 0.
 * - pow(-0, y) returns -Inf for an integer y < 0.
 * - pow(+0, y) returns +0 for an odd integer y > 0.
 * - pow(-0, y) returns -0 for an odd integer y > 0.
 * - pow(+0, y) returns +0 for y > 0 and not an odd integer.
 * - pow(-0, y) returns +0 for y > 0 and not an odd integer.
 * - pow(-1, +Inf) returns 1.
 * - pow(-1, -Inf) returns 1.
 * - pow(+1, y) returns 1 for any y, even a NaN.
 * - pow(x, +0) returns 1 for any x, even a NaN.
 * - pow(x, -0) returns 1 for any x, even a NaN.
 * - pow(x, y) returns a NaN for finite x < 0 and finite non-integer y.
 * - pow(x, -Inf) returns +Inf for |x] < 1.
 * - pow(x, -Inf) returns +0 for [x] > 1.
 * - pow(x, +Inf) returns +0 for [x] < 1.
 * - pow(x, +Inf) returns +Inf for [x] > 1.
 * - pow(-Inf, y) returns -0 for an odd integer y < 0.
 * - pow(-Inf, y) returns +0 for not an odd integer y < 0.
 * - pow(-Inf, y) returns -Inf for an odd integer y > 0.
 * - pow(-Inf, y) returns +Inf for not an odd integer y > 0.
 * - pow(+Inf, y) returns +0 for y < 0.
 * - pow(+Inf, y) returns +Inf for y > 0.
 * - compute_20 vector f32: n * sizeof(float) % 128 = 0.
 * - compute_20 vector f16: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_pow_f32(float x, float y);
__DEVICE__ half  __cn_scalar_pow_f16(half x, half y);
__DEVICE__ void  __cn_vector_pow_f32(int n, float* z, const float* x, const float* y);
__DEVICE__ void  __cn_vector_pow_f16(int n, half* z, const half* x, const half* y);

/**
 * @brief
 * Reads the 32-bit, 16-bit word old *a located at the address a in Global-DRAM
 * or Shared-RAM, computes (*a + b), and stores the result back to memory at the
 * same address a. These three operations are performed in one atomic transaction.
 *
 * @return
 * - scalar: d = *a; *a = *a + b; return d.
 *
 * @attention
 * - compute_20: returns +HALF_MAX if final result *a is +Inf.
 * - compute_20: returns -HALF_MAX if final result *a is -Inf.
 * - BANG block type kernel should not use atomic operation, because block task
 *   could not use barrier operation for (dimx * dimy * dimz) multi-task.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_atomic_add_f32(float *a, float b);
__DEVICE__ half __cn_scalar_atomic_add_f16(half *a, half b);
__DEVICE__ int __cn_scalar_atomic_add_s32(int *a, int b);
__DEVICE__ unsigned int __cn_scalar_atomic_add_u32(unsigned int *a, unsigned int b);
__DEVICE__ short __cn_scalar_atomic_add_s16(short *a, short b);
__DEVICE__ unsigned short __cn_scalar_atomic_add_u16(unsigned short *a, unsigned short b);

/**
  * @brief
  * Reads the 32-bit, 16-bit word old *a located at the address a in Global-DRAM
  * or Shared-RAM, and stores b to memory at the same address a. These three
  * operations are performed in one atomic transaction.
  *
  * @return
  * - scalar: d = *a; *a = b; return d.
  *
  * @attention
  * - BANG block type kernel should not use atomic operation, because block task
  *   could not use barrier operation for (dimx * dimy * dimz) multi-task.
  *
  * @note
  * Library Availability:
  * - libdevice.compute_20.bc
  * - libdevice.compute_30.bc
  * - libdevice.compute_35.bc
  **/
__DEVICE__ float __cn_scalar_atomic_exch_f32(float *a, float b);
__DEVICE__ half __cn_scalar_atomic_exch_f16(half *a, half b);
__DEVICE__ int __cn_scalar_atomic_exch_s32(int *a, int b);
__DEVICE__ unsigned int __cn_scalar_atomic_exch_u32(unsigned int *a, unsigned int b);
__DEVICE__ short __cn_scalar_atomic_exch_s16(short *a, short b);
__DEVICE__ unsigned short __cn_scalar_atomic_exch_u16(unsigned short *a, unsigned short b);

/**
  * @brief
  * Reads the 32-bit, 16-bit word old *a located at the address a in Global-DRAM
  * or Shared-RAM, computes max(*a, b), and stores the result back to memory at
  * the same address a. These three operations are performed in one atomic
  * transaction.
  *
  * @return
  * - scalar: d = *a; *a = max(*a, b); return d.
  *
  * @attention
  * - BANG block type kernel should not use atomic operation, because block task
  *   could not use barrier operation for (dimx * dimy * dimz) multi-task.
  *
  * @note
  * Library Availability:
  * - libdevice.compute_20.bc
  * - libdevice.compute_30.bc
  * - libdevice.compute_35.bc
  **/
__DEVICE__ float __cn_scalar_atomic_max_f32(float *a, float b);
__DEVICE__ half __cn_scalar_atomic_max_f16(half *a, half b);
__DEVICE__ int __cn_scalar_atomic_max_s32(int *a, int b);
__DEVICE__ unsigned int __cn_scalar_atomic_max_u32(unsigned int *a, unsigned int b);
__DEVICE__ short __cn_scalar_atomic_max_s16(short *a, short b);
__DEVICE__ unsigned short __cn_scalar_atomic_max_u16(unsigned short *a, unsigned short b);

/**
  * @brief
  * Reads the 32-bit, 16-bit word old *a located at the address a in Global-DRAM
  * or Shared-RAM, computes min(*a, b), and stores the result back to memory at
  * the same address a. These three operations are performed in one atomic
  * transaction.
  *
  * @return
  * - scalar: d = *a; *a = min(*a, b); return d.
  *
  * @attention
  * - BANG block type kernel should not use atomic operation, because block task
  *   could not use barrier operation for (dimx * dimy * dimz) multi-task.
  *
  * @note
  * Library Availability:
  * - libdevice.compute_20.bc
  * - libdevice.compute_30.bc
  * - libdevice.compute_35.bc
  **/
__DEVICE__ float __cn_scalar_atomic_min_f32(float *a, float b);
__DEVICE__ half __cn_scalar_atomic_min_f16(half *a, half b);
__DEVICE__ int __cn_scalar_atomic_min_s32(int *a, int b);
__DEVICE__ unsigned int __cn_scalar_atomic_min_u32(unsigned int *a, unsigned int b);
__DEVICE__ short __cn_scalar_atomic_min_s16(short *a, short b);
__DEVICE__ unsigned short __cn_scalar_atomic_min_u16(unsigned short *a, unsigned short b);

/**
  * @brief
  * Reads the 32-bit, 16-bit word old *a located at the address a in Global-DRAM
  * or Shared-RAM, computes ((*a >= b) ? 0 : (*a + 1)), and stores the result
  * back to memory at the same address a. These three operations are performed
  * in one atomic transaction.
  *
  * @return
  * - scalar: d = *a; *a = (*a >= b) ? 0 : (*a + 1); return d.
  *
  * @attention
  * - BANG block type kernel should not use atomic operation, because block task
  *   could not use barrier operation for (dimx * dimy * dimz) multi-task.
  *
  * @note
  * Library Availability:
  * - libdevice.compute_20.bc
  * - libdevice.compute_30.bc
  * - libdevice.compute_35.bc
  **/
__DEVICE__ unsigned int __cn_scalar_atomic_inc_u32(unsigned int *a, unsigned int b);
__DEVICE__ unsigned short __cn_scalar_atomic_inc_u16(unsigned short *a, unsigned short b);

/**
  * Reads the 32-bit, 16-bit word old *a located at the address a in Global-DRAM
  * or Shared-RAM, computes (((*a == 0) || (*a > b)) ? b : (*a - 1)), and stores
  * the result back to memory at the same address a. These three operations are
  * performed in one atomic transaction.
  *
  * @return
  * - scalar: d = *a; *a = ((*a == 0) || (*a > b)) ? b : (*a - 1); return d.
  *
  * @attention
  * - BANG block type kernel should not use atomic operation, because block task
  *   could not use barrier operation for (dimx * dimy * dimz) multi-task.
  *
  * @attention
  * - BANG block type kernel should not use atomic operation, because block task
  *   could not use barrier operation for (dimx * dimy * dimz) multi-task.
  *
  * @note
  * Library Availability:
  * - libdevice.compute_20.bc
  * - libdevice.compute_30.bc
  * - libdevice.compute_35.bc
  **/
__DEVICE__ unsigned int __cn_scalar_atomic_dec_u32(unsigned int *a, unsigned int b);
__DEVICE__ unsigned short __cn_scalar_atomic_dec_u16(unsigned short *a, unsigned short b);

/**
  * @brief
  * Reads the 32-bit, 16-bit word old *a located at the address a in Global-DRAM
  * or Shared-RAM, computes (*a == b ? c : *a), and stores the result back to
  * memory at the same address a. These three operations are performed in one
  * atomic transaction.
  *
  * @return
  * - scalar: d = *a; *a = (*a == b ? c : *a); return c.
  *
  * @attention
  * - BANG block type kernel should not use atomic operation, because block task
  *   could not use barrier operation for (dimx * dimy * dimz) multi-task.
  *
  * @note
  * Library Availability:
  * - libdevice.compute_20.bc
  * - libdevice.compute_30.bc
  * - libdevice.compute_35.bc
  **/
__DEVICE__ int __cn_scalar_atomic_cas_s32(int *a, int b, int c);
__DEVICE__ unsigned int __cn_scalar_atomic_cas_u32(unsigned int *a, unsigned int b, unsigned int c);
__DEVICE__ short __cn_scalar_atomic_cas_s16(short *a, short b, short c);
__DEVICE__ unsigned short __cn_scalar_atomic_cas_u16(unsigned short *a, unsigned short b, unsigned short c);

/**
  * Reads the 32-bit, 16-bit word old *a located at the address a in Global-DRAM
  * or Shared-RAM, computes bitwise (*a & b), and stores the result back to
  * memory at the same address a. These three operations are performed in one
  * atomic transaction.
  *
  * @return
  * - scalar: d = *a; *a = (*a & b); return d.
  *
  * @attention
  * - BANG block type kernel should not use atomic operation, because block task
  *   could not use barrier operation for (dimx * dimy * dimz) multi-task.
  *
  * @attention
  * - BANG block type kernel should not use atomic operation, because block task
  *   could not use barrier operation for (dimx * dimy * dimz) multi-task.
  *
  * @note
  * Library Availability:
  * - libdevice.compute_20.bc
  * - libdevice.compute_30.bc
  * - libdevice.compute_35.bc
  **/
__DEVICE__ int __cn_scalar_atomic_and_s32(int *a, int b);
__DEVICE__ unsigned int __cn_scalar_atomic_and_u32(unsigned int *a, unsigned int b);
__DEVICE__ short __cn_scalar_atomic_and_s16(short *a, short b);
__DEVICE__ unsigned short __cn_scalar_atomic_and_u16(unsigned short *a, unsigned short b);

/**
  * Reads the 32-bit, 16-bit word old *a located at the address a in Global-DRAM
  * or Shared-RAM, computes bitwise (*a | b), and stores the result back to
  * memory at the same address a. These three operations are performed in one
  * atomic transaction.
  *
  * @return
  * - scalar: d = *a; *a = (*a | b); return d.
  *
  * @attention
  * - BANG block type kernel should not use atomic operation, because block task
  *   could not use barrier operation for (dimx * dimy * dimz) multi-task.
  *
  * @attention
  * - BANG block type kernel should not use atomic operation, because block task
  *   could not use barrier operation for (dimx * dimy * dimz) multi-task.
  *
  * @note
  * Library Availability:
  * - libdevice.compute_20.bc
  * - libdevice.compute_30.bc
  * - libdevice.compute_35.bc
  **/
__DEVICE__ int __cn_scalar_atomic_or_s32(int *a, int b);
__DEVICE__ unsigned int __cn_scalar_atomic_or_u32(unsigned int *a, unsigned int b);
__DEVICE__ short __cn_scalar_atomic_or_s16(short *a, short b);
__DEVICE__ unsigned short __cn_scalar_atomic_or_u16(unsigned short *a, unsigned short b);

/**
  * Reads the 32-bit, 16-bit word old *a located at the address a in Global-DRAM
  * or Shared-RAM, computes bitwise (*a ^ b), and stores the result back to
  * memory at the same address a. These three operations are performed in one
  * atomic transaction.
  *
  * @return
  * - scalar: d = *a; *a = (*a ^ b); return d.
  *
  * @attention
  * - BANG block type kernel should not use atomic operation, because block task
  *   could not use barrier operation for (dimx * dimy * dimz) multi-task.
  *
  * @attention
  * - BANG block type kernel should not use atomic operation, because block task
  *   could not use barrier operation for (dimx * dimy * dimz) multi-task.
  *
  * @note
  * Library Availability:
  * - libdevice.compute_20.bc
  * - libdevice.compute_30.bc
  * - libdevice.compute_35.bc
  **/
__DEVICE__ int __cn_scalar_atomic_xor_s32(int *a, int b);
__DEVICE__ unsigned int __cn_scalar_atomic_xor_u32(unsigned int *a, unsigned int b);
__DEVICE__ short __cn_scalar_atomic_xor_s16(short *a, short b);
__DEVICE__ unsigned short __cn_scalar_atomic_xor_u16(unsigned short *a, unsigned short b);

/**
 * @brief
 * Convert the single-precision float value to half-precision floating point value
 * in round-to-nearest-even mode.
 *
 * @return
 * - half value.
 *
 * @attention
 * - cast_f32_to_f16_rn(x) returns +0.0 when +0.0 <= x <= 2^-25.
 * - cast_f32_to_f16_rn(x) returns -0.0 when -(2^-25) <= x <= -0.0.
 * - cast_f32_to_f16_rn(x) returns +65504.0 when x >= +65504.0.
 * - cast_f32_to_f16_rn(x) returns -65504.0 when x <= -65504.0.
 * - cast_f32_to_f16_rn(+inf) returns +inf.
 * - cast_f32_to_f16_rn(-inf) returns -inf.
 * - cast_f32_to_f16_rn(+nan) returns +nan.
 * - cast_f32_to_f16_rn(-nan) returns +nan.
 * - compute_20 vector x: n * sizeof(float) % 128 = 0.
 * - compute_20 vector y: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ half __cn_scalar_cast_f32_to_f16_rn(float x);
__DEVICE__ void __cn_vector_cast_f32_to_f16_rn(int n, half* y, const float* x);

/**
 * @brief
 * Convert the single-precision float value to signed 32 bit integer value
 * in round-to-nearest-even mode.
 *
 * @return
 * - signed 32 bit integer value.
 *
 * @attention
 * - cast_f32_to_s32_rn(x) returns 2147483647 when x > 2147483647.0.
 * - cast_f32_to_s32_rn(x) returns -2147483648 when x <= -2147483648.0.
 * - cast_f32_to_s32_rn(+0.0) returns 0.
 * - cast_f32_to_s32_rn(-0.0) returns 0.
 * - cast_f32_to_s32_rn(+inf) returns 2147483647.
 * - cast_f32_to_s32_rn(+nan) returns 2147483647.
 * - cast_f32_to_s32_rn(-inf) returns -2147483648.
 * - cast_f32_to_s32_rn(-nan) returns -2147483648.
 * - compute_20 vector x: n * sizeof(float) % 128 = 0.
 * - compute_20 vector y: n * sizeof(int) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ int __cn_scalar_cast_f32_to_s32_rn(float x);
__DEVICE__ void __cn_vector_cast_f32_to_s32_rn(int n, int* y, const float* x);

/**
 * @brief
 * Convert the single-precision float value to signed 32 bit integer value
 * in round-to-negative-infinity mode.
 *
 * @return
 * - signed 32 bit integer value.
 *
 * @attention
 * - cast_f32_to_s32_dn(x) returns 2147483647 when x > 2147483647.0.
 * - cast_f32_to_s32_dn(x) returns -2147483648 when x <= -2147483648.0.
 * - cast_f32_to_s32_dn(+0.0) returns 0.
 * - cast_f32_to_s32_dn(-0.0) returns 0.
 * - cast_f32_to_s32_dn(+inf) returns 2147483647.
 * - cast_f32_to_s32_dn(+nan) returns 2147483647.
 * - cast_f32_to_s32_dn(-inf) returns -2147483648.
 * - cast_f32_to_s32_dn(-nan) returns -2147483648.
 * - compute_20 vector x: n * sizeof(float) % 128 = 0.
 * - compute_20 vector y: n * sizeof(int) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ int __cn_scalar_cast_f32_to_s32_dn(float x);
__DEVICE__ void __cn_vector_cast_f32_to_s32_dn(int n, int* y, const float* x);

/**
 * @brief
 * Convert the single-precision float value to signed 32 bit integer value
 * in round-to-positive-infinity mode.
 *
 * @return
 * - signed 32 bit integer value.
 *
 * @attention
 * - cast_f32_to_s32_up(x) returns 2147483647 when x > 2147483647.0.
 * - cast_f32_to_s32_up(x) returns -2147483648 when x <= -2147483648.0.
 * - cast_f32_to_s32_up(+0.0) returns 0.
 * - cast_f32_to_s32_up(-0.0) returns 0.
 * - cast_f32_to_s32_up(+inf) returns 2147483647.
 * - cast_f32_to_s32_up(+nan) returns 2147483647.
 * - cast_f32_to_s32_up(-inf) returns -2147483648.
 * - cast_f32_to_s32_up(-nan) returns -2147483648.
 * - compute_20 vector x: n * sizeof(float) % 128 = 0.
 * - compute_20 vector y: n * sizeof(int) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ int __cn_scalar_cast_f32_to_s32_up(float x);
__DEVICE__ void __cn_vector_cast_f32_to_s32_up(int n, int* y, const float* x);

/**
 * @brief
 * Convert the single-precision float value to signed 32 bit integer value
 * in round-towards-zero mode.
 *
 * @return
 * - signed 32 bit integer value.
 *
 * @attention
 * - cast_f32_to_s32_tz(x) returns 2147483647 when x > 2147483647.0.
 * - cast_f32_to_s32_tz(x) returns -2147483648 when x <= -2147483648.0.
 * - cast_f32_to_s32_tz(+0.0) returns 0.
 * - cast_f32_to_s32_tz(-0.0) returns 0.
 * - cast_f32_to_s32_tz(+inf) returns 2147483647.
 * - cast_f32_to_s32_tz(+nan) returns 2147483647.
 * - cast_f32_to_s32_tz(-inf) returns -2147483648.
 * - cast_f32_to_s32_tz(-nan) returns -2147483648.
 * - compute_20 vector x: n * sizeof(float) % 128 = 0.
 * - compute_20 vector y: n * sizeof(int) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ int __cn_scalar_cast_f32_to_s32_tz(float x);
__DEVICE__ void __cn_vector_cast_f32_to_s32_tz(int n, int* y, const float* x);

/**
 * @brief
 * Convert the single-precision float value to unsigned 32 bit integer value
 * in round-to-nearest-even mode.
 *
 * @return
 * - unsigned 32 bit integer value.
 *
 * @attention
 * - cast_f32_to_u32_rn(x) returns 4294967295 when x > 4294967295.0.
 * - cast_f32_to_u32_rn(x) returns 0 when x <= +0.0.
 * - cast_f32_to_u32_rn(+inf) returns 4294967295.
 * - cast_f32_to_u32_rn(+nan) returns 4294967295.
 * - cast_f32_to_u32_rn(-inf) returns 0.
 * - cast_f32_to_u32_rn(-nan) returns 0.
 * - compute_20 vector x: n * sizeof(float) % 128 = 0.
 * - compute_20 vector y: n * sizeof(unsigned int) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ unsigned int __cn_scalar_cast_f32_to_u32_rn(float x);
__DEVICE__ void __cn_vector_cast_f32_to_u32_rn(int n, unsigned int* y, const float* x);

/**
 * @brief
 * Convert the half-precision float value to single-precision floating point value.
 *
 * @return
 * - half value.
 *
 * @attention
 * - cast_f16_to_f32(+inf) returns +inf.
 * - cast_f16_to_f32(-inf) returns -inf.
 * - cast_f16_to_f32(+nan) returns +nan.
 * - cast_f16_to_f32(-nan) returns +nan.
 * - compute_20 vector x: n * sizeof(float) % 128 = 0.
 * - compute_20 vector y: n * sizeof(half) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_cast_f16_to_f32(half x);
__DEVICE__ void __cn_vector_cast_f16_to_f32(int n, float* y, const half* x);

/**
 * @brief
 * Convert the signed 32 bit integer value to single-precision float value
 * in round-to-nearest-even mode.
 *
 * @return
 * - float value.
 *
 * @attention
 * - cast_s32_to_f32_rn(0) returns +0.0.
 * - compute_20 vector x: n * sizeof(int) % 128 = 0.
 * - compute_20 vector y: n * sizeof(float) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_cast_s32_to_f32_rn(int x);
__DEVICE__ void __cn_vector_cast_s32_to_f32_rn(int n, float* y, const int* x);

/**
 * @brief
 * Convert signed 32 bit integer value to signed long 64 bit integer value.
 *
 * @return
 * - long 64 bit integer value.
 *
 * @attention
 * - compute_20/30/35: input x should be [+0, +INT_MAX].
 * - compute_20 vector x: n * sizeof(int) % 128 = 0.
 * - compute_20 vector y: n * sizeof(long int) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ long __cn_scalar_cast_s32_to_s64(int x);
__DEVICE__ void __cn_vector_cast_s32_to_s64(int n, long * y, const int* x);

/**
 * @brief
 * Convert unsigned 32 bit integer value to unsigned long 64 bit integer value.
 *
 * @return
 * - long 64 bit integer value.
 *
 * @attention
 * - compute_20/30/35: input x should be [+0, UINT_MAX].
 * - compute_20 vector x: n * sizeof(int) % 128 = 0.
 * - compute_20 vector y: n * sizeof(long int) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ unsigned long __cn_scalar_cast_u32_to_u64(unsigned int x);
__DEVICE__ void __cn_vector_cast_u32_to_u64(int n, unsigned long * y, const unsigned int* x);

/**
 * @brief
 * Convert the unsigned 32 bit integer value to single-precision float value
 * in round-to-nearest-even mode.
 *
 * @return
 * - float value.
 *
 * @attention
 * - cast_u32_to_f32_rn(0) returns +0.0.
 * - compute_20 vector x: n * sizeof(int) % 128 = 0.
 * - compute_20 vector y: n * sizeof(float) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_cast_u32_to_f32_rn(unsigned int x);
__DEVICE__ void __cn_vector_cast_u32_to_f32_rn(int n, float* y, const unsigned int* x);

/**
 * @brief
 * Convert the unsigned 8 bit integer value to signed 64 bit integer value.
 *
 * @return
 * - signed 64 bit integer value.
 *
 * @attention
 * - compute_20 vector x: n * sizeof(unsigned char) % 128 = 0.
 * - compute_20 vector y: n * sizeof(long int) % 128 = 0.
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ long int __cn_scalar_cast_u8_to_s64(unsigned char x);
__DEVICE__ void __cn_vector_cast_u8_to_s64(int n, long int* y, const unsigned char* x);

/**
 * @brief
 * Convert the long 64 bit integer value to float value.
 *
 * @return
 * - float value with rounding mode.
 *
 * @attention
 * - tz: round-towards-zero mode.
 * - compute_20/30/35: input x should be [-INT_MAX, +INT_MAX].
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ float __cn_scalar_cast_s64_to_f32_tz(long x);
__DEVICE__ void __cn_vector_cast_s64_to_f32_tz(int n, float* y, const long* x);

/**
 * @brief
 * Convert the floating point value to long 64 bit value.
 *
 * @return
 * - float value with rounding mode.
 *
 * @attention
 * - tz: round-towards-zero mode.
 * - compute_20/30/35: input x should be [0.0f, +(float)INT_MAX].
 *
 * @note
 * Library Availability:
 * - libdevice.compute_20.bc
 * - libdevice.compute_30.bc
 * - libdevice.compute_35.bc
 **/
__DEVICE__ long __cn_scalar_cast_f32_to_s64_tz(float x);
__DEVICE__ void __cn_vector_cast_f32_to_s64_tz(int n, long* y, const float* x);

}  // extern "C"

#endif  // __BANG_MATH_DECLS_H__
