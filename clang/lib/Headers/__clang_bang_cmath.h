/*===-------- __clang_bang_cmath.h - Device-side MLU cmath support ---------===
 *
 * Copyright (C) [2018] by Cambricon, Inc.
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
#ifndef __CLANG_BANG_CMATH_H__
#define __CLANG_BANG_CMATH_H__



#ifndef __MLU_H__
#include <mlu.h>
#endif  // __MLU_H__

#ifdef __BANG_ARCH__  // Only included for device-side

#include <assert.h>
#include <math.h>

#include <__clang_bang_math_tables.h>

// BANG lets us use various std math functions on the device side.  This file
// works in concert with __clang_bang_math_forward_declares.h to make this work.
//
// Specifically, the forward-declares header declares __mlu_device__ overloads for
// these functions in the global namespace, then pulls them into namespace std
// with 'using' statements.  Then this file implements those functions, after
// their implementations have been pulled in.
//
// It's important that we declare the functions in the global namespace and pull
// them into namespace std with using statements, as opposed to simply declaring
// these functions in namespace std, because our device functions need to
// overload the standard library functions, which may be declared in the global
// namespace or in std, depending on the degree of conformance of the stdlib
// implementation.  Declaring in the global namespace and pulling into namespace
// std covers all of the known knowns.

#define GET_FLOAT_WORD(i, f)                    \
  do {                                          \
    asm ("mv.gpr.gpr %[dst], %[src];\n\t"       \
         : [dst]"+&r"((i))                      \
         : [src]"r"((f))                        \
         );                                     \
  } while(0)
#define SET_FLOAT_WORD(f, i)                    \
  do {                                          \
    asm ("mv.gpr.gpr %[dst], %[src];\n\t"       \
         : [dst]"+&r"((f))                      \
         : [src]"r"((i))                        \
         );                                     \
  } while(0)

#pragma push_macro("__DEVICE__")
#define __DEVICE__ static __inline__ __mlu_func__ __mlu_builtin__

__DEVICE__ int __isnanf(float __x) throw() {
  int ix;
  GET_FLOAT_WORD(ix, __x);
  ix &= 0x7fffffff;
  ix = 0x7f800000 - ix;
  return (int)(((unsigned int)(ix)) >> 31);
}
__DEVICE__ int __isnan(float __x) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0;
}
__DEVICE__ int __isinf(float __x) throw() {
  int ix, t;
  GET_FLOAT_WORD(ix, __x);
  t = ix & 0x7fffffff;
  t ^= 0x7f800000;
  t |= -t;
  return ~(t >> 31) & (ix >> 30);
}
__DEVICE__ float fabsf(float __x) throw() {
  return __builtin_fabsf(__x);
}
__DEVICE__ float fabs(float __x) throw() {
  return ::fabsf(__x);
}
__DEVICE__ long abs(long __n) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0;
}
__DEVICE__ float abs(float __x) throw() {
  return ::fabsf(__x);
}
__DEVICE__ float acos(float __x) throw() {
  return ::acosf(__x);
}
__DEVICE__ float acosf(float __x) throw() {
  const float one = 1.0f;                  // 0x3F800000
  const float pi = 3.1415925026e+00f;      // 0x40490fda
  const float pio2_hi = 1.5707962513e+00f; // 0x3fc90fda
  const float pio2_lo = 7.5497894159e-08f; // 0x33a22168
  // Pade approximant of (asin(x) - x)
  const float pS0 = 1.6666667163e-01f;     // 0x3e2aaaab
  const float pS1 = -3.2556581497e-01f;    // 0xbea6b090
  const float pS2 = 2.0121252537e-01f;     // 0x3e4e0aa8
  const float pS3 = -4.0055535734e-02f;    // 0xbd241146
  const float pS4 = 7.9153501429e-04f;     // 0x3a4f7f04
  const float pS5 = 3.4793309169e-05f;     // 0x3811ef08
  const float qS1 = -2.4033949375e+00f;    // 0xc019d139
  const float qS2 = 2.0209457874e+00f;     // 0x4001572d
  const float qS3 = -6.8828397989e-01f;    // 0xbf303361
  const float qS4 = 7.7038154006e-02f;     // 0x3d9dc62e
  float z, p, q, r, w, s, c, df;
  int32_t hx, ix;
  GET_FLOAT_WORD(hx, __x);
  ix = hx & 0x7fffffff;
  if (ix == 0x3f800000) {  // |x| == 1
    if (hx > 0) {
      return 0.0f;  // acos(1) = 0
    } else {
      return pi + 2.0f * pio2_lo;  // acos(-1)= pi
    }
  } else if (ix > 0x3f800000) {  // |x| >= 1
    return NAN;  // acos(|x|>1) is NaN
  }
  if (ix < 0x3f000000) {  // |x| < 0.5
    if (ix <= 0x32800000) {  // if |x| <= 2**-26
      return pio2_hi + pio2_lo;
    }
    z = __x * __x;
    p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
    q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
    r = p / q;
    return pio2_hi - (__x - (pio2_lo - __x * r));
  } else if (hx < 0) {  // x < -0.5
    z = (one + __x) * 0.5f;
    p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
    q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
    s = sqrtf(z);
    r = p / q;
    w = r * s - pio2_lo;
    return pi - 2.0f * (s + w);
  } else {      // x > 0.5
    int32_t idf;
    z = (one - __x) * 0.5f;
    s = sqrtf(z);
    df = s;
    GET_FLOAT_WORD(idf, df);
    SET_FLOAT_WORD(df, idf & 0xfffff000);
    c = (z - df * df) / (s + df);
    p = z * (pS0 + z * (pS1 + z * (pS2 + z * (pS3 + z * (pS4 + z * pS5)))));
    q = one + z * (qS1 + z * (qS2 + z * (qS3 + z * qS4)));
    r = p / q;
    w = r * s + c;
    return 2.0f * (df + w);
  }
}
__DEVICE__ float acosh(float __x) throw() {
  return ::acoshf(__x);
}
__DEVICE__ float acoshf(float __x) throw() {
  const float one = 1.0f;
  const float ln2 = 6.9314718246e-01f;  // 0x3f317218
  float t;
  int32_t hx;
  GET_FLOAT_WORD(hx, __x);
  if (hx < 0x3f800000) {  // x < 1
    return NAN;
  } else if (hx >= 0x4d800000) {  // x > 2**28
    if (hx >= 0x7f800000) {  // x is inf of NaN
      return __x;
    } else {
      return logf(__x) + ln2;  // acosh(huge) = log(2x)
    }
  } else if (hx == 0x3f800000) {
    return 0.0f;  // acosh(1) = 0
  } else if (hx > 0x40000000) {  // 2**28 > x > 2
    t = __x * __x;
    return logf(2.0f * __x - one / (__x + sqrtf(t - one)));
  } else {        // 1 < x < 2
    t = __x - one;
    return log1pf(t + sqrtf(2.0f * t + t * t));
  }
}
__DEVICE__ float asin(float __x) throw() {
  return ::asinf(__x);
}
__DEVICE__ float asinf(float __x) throw() {
  const float one = 1.0000000000e+00f;  // 0x3F800000
  const float huge = 1.000e+30f;
  const float pio2_hi = 1.57079637050628662109375f;
  const float pio2_lo = -4.37113900018624283e-8f;
  const float pio4_hi = 0.785398185253143310546875f;
  const float p0 = 1.666675248e-1f;
  const float p1 = 7.495297643e-2f;
  const float p2 = 4.547037598e-2f;
  const float p3 = 2.417951451e-2f;
  const float p4 = 4.216630880e-2f;
  float t, w, p, q, c, r, s;
  int hx, ix;
  GET_FLOAT_WORD(hx, __x);
  ix = hx & 0x7fffffff;
  if (ix == 0x3f800000) {
    return __x * pio2_hi + __x * pio2_lo;
  } else if (ix > 0x3f800000) {  // |x|>= 1
    SET_FLOAT_WORD(w, 0x7fc00000);  // asin(|x|>1) is NaN
    return w;
  } else if (ix < 0x3f000000) {  // |x|<0.5
    if (ix < 0x32000000) {  // if |x| < 2**-27
      if (huge + __x > one) {
        return __x;  // return x with inexact if x!=0
      }
    } else {
      t = __x * __x;
      w = t * (p0 + t * (p1 + t * (p2 + t * (p3 + t * p4))));
      return __x + __x * w;
    }
  }
  // 1 > |x| >= 0.5
  w = one - fabsf(__x);
  t = w * 0.5f;
  p = t * (p0 + t * (p1 + t * (p2 + t * (p3 + t * p4))));
  s = ::sqrtf(t);
  if (ix >= 0x3F79999A) {  // if |x| > 0.975
    t = pio2_hi - (2.0f * (s + s * p) - pio2_lo);
  } else {
    int iw;
    w = s;
    GET_FLOAT_WORD(iw, w);
    SET_FLOAT_WORD(w, iw & 0xfffff000);
    c = (t - w * w) / (s + w);
    r = p;
    p = 2.0f * s * r - (pio2_lo - 2.0f * c);
    q = pio4_hi - 2.0f * w;
    t = pio4_hi - (p - q);
  }
  return (hx > 0) ? t : -t;
}
__DEVICE__ float asinh(float __x) throw() {
  return ::asinhf(__x);
}
__DEVICE__ float asinhf(float __x) throw() {
  const float one = 1.0f;
  const float ln2 = 6.9314718246e-01f;   // 0x3f317218
  float w, xa, t;
  int32_t hx, ix;
  GET_FLOAT_WORD(hx, __x);
  ix = hx & 0x7fffffff;
  if (ix < 0x38000000) {  // |x| < 2**-14
    return __x;  // asinh(tiny) = x
  }
  if (ix > 0x47000000) {  // |x| > 2**14
    if (ix >= 0x7f800000) {
      return __x;  // x is inf or NaN
    }
    w = logf(fabsf(__x)) + ln2;  // asinh(huge) = log(2x)
  } else {
    xa = fabsf(__x);
    if (ix > 0x40000000) {  // 2**14 > |x| > 2.0
      w = logf(2.0f * xa + one / (sqrtf(xa * xa + one) + xa));
    } else {  // 2.0 > |x| > 2**-14
      t = xa * xa;
      w = log1pf(xa + t / (one + sqrtf(one + t)));
    }
  }
  return copysignf(w, __x);
}
__DEVICE__ float atan(float __x) throw() {
  return ::atanf(__x);
}
__DEVICE__ float atanf(float __x) throw() {
  const float one = 1.0f;
  const float atan_hi0 = 4.6364760399e-01f;  // atan(0.5) hi 0x3eed6338
  const float atan_lo0 = 5.0121582440e-09f;  // atan(0.5) lo 0x31ac3769
  const float atan_hi1 = 7.8539812565e-01f;  // atan(1.0) hi 0x3f490fda
  const float atan_lo1 = 3.7748947079e-08f;  // atan(1.0) lo 0x33222168
  const float atan_hi2 = 9.8279368877e-01f;  // atan(1.5) hi 0x3f7b985e
  const float atan_lo2 = 3.4473217170e-08f;  // atan(1.5) lo 0x33140fb4
  const float atan_hi3 = 1.5707962513e+00f;  // atan(inf) hi 0x3fc90fda
  const float atan_lo3 = 7.5497894159e-08f;  // atan(inf) lo 0x33a22168
  const float T00 =  3.3333334327e-01;  // 0x3eaaaaab
  const float T01 = -2.0000000298e-01;  // 0xbe4ccccd
  const float T02 =  1.4285714924e-01;  // 0x3e124925
  const float T03 = -1.1111110449e-01;  // 0xbde38e38
  const float T04 =  9.0908870101e-02;  // 0x3dba2e6e
  const float T05 = -7.6918758452e-02;  // 0xbd9d8795
  const float T06 =  6.6610731184e-02;  // 0x3d886b35
  const float T07 = -5.8335702866e-02;  // 0xbd6ef16b
  const float T08 =  4.9768779427e-02;  // 0x3d4bda59
  const float T09 = -3.6531571299e-02;  // 0xbd15a221
  const float T10 =  1.6285819933e-02;  // 0x3c8569d7
  float w, s1, s2, z, x;
  float atan_hi, atan_lo;
  int32_t ix, hx, id;

  GET_FLOAT_WORD(hx, __x);
  ix = hx & 0x7fffffff;
  if (ix >= 0x4c000000) {  // if |x| >= 2^25
    if (ix > 0x7f800000) {  // NaN
      return NAN;
    }
    if (hx > 0) {
      return atan_hi3 + atan_lo3;
    } else {
      return -atan_hi3 - atan_lo3;
    }
  }
  if (ix < 0x3ee00000) {  // |x| < 0.4375
    if (ix < 0x31000000) {  // |x| < 2^-29
      return __x;  // atan(small) = x
    }
    id = -1;
    x = __x;
  } else {
    x = fabsf(__x);
    if (ix < 0x3f980000) {  // |x| < 1.1875
      if (ix < 0x3f300000) {  // 7/16 <= |x| < 11/16
        id = 0;
        atan_hi = atan_hi0;
        atan_lo = atan_lo0;
        x = (2.0f * x - one) / (2.0f + x);
      } else {  // 11/16 <= |x| < 19/16
        id = 1;
        atan_hi = atan_hi1;
        atan_lo = atan_lo1;
        x = (x - one) / (x + one);
      }
    } else {
      if (ix < 0x401c0000) {  // |x| < 2.4375
        id = 2;
        atan_hi = atan_hi2;
        atan_lo = atan_lo2;
        x = (x - 1.5f) / (one + 1.5f * x);
      } else {  // 2.4375 <= |x| < 2^66
        id = 3;
        atan_hi = atan_hi3;
        atan_lo = atan_lo3;
        x = -1.0f / x;
      }
    }
  }
  // end of argument reduction
  z = x * x;
  w = z * z;
  // break sum into odd and even poly
  s1 = z * (T00 + w * (T02 + w * (T04 + w * (T06 + w * (T08 + w * T10)))));
  s2 = w * (T01 + w * (T03 + w * (T05 + w * (T07 + w * T09))));
  if (id < 0) {
    return x - x * (s1 + s2);
  } else {
    z = atan_hi - ((x * (s1 + s2) - atan_lo) - x);
    return (hx < 0) ? -z : z;
  }
}
__DEVICE__ float atanh(float __x) throw() {
  return ::atanhf(__x);
}
__DEVICE__ float atanhf(float __x) throw() {
  float xa = fabsf(__x);
  float t;
  if (xa < 0.5f) {
    if (xa < 0x1.0p-28f) {
      return __x;  // atanh(small) = x
    }
    t = xa + xa;
    t = 0.5f * log1pf(t + t * xa / (1.0f - xa));
  } else if (xa < 1.0f) {
    t = 0.5f * log1pf((xa + xa) / (1.0f - xa));
  } else if (xa == 1.0f) {
    t = INFINITY;
  } else {
    t = NAN;
  }
  return copysignf(t, __x);
}
__DEVICE__ float atan2(float __y, float __x) throw() {
  return ::atan2f(__y, __x);
}
__DEVICE__ float atan2f(float __y, float __x) throw() {
  const float zero = 0.0f;
  const float pi_o_4 = 7.8539818525e-01f;  // 0x3f490fdb
  const float pi_o_2 = 1.5707963705e+00f;  // 0x3fc90fdb
  const float pi = 3.1415927410e+00f;      // 0x40490fdb
  const float pi_lo = -8.7422776573e-08f;  // 0xb3bbbd2e
  float z;
  int32_t k, m, hx, hy, ix, iy;

  GET_FLOAT_WORD(hx, __x);
  ix = hx & 0x7fffffff;
  GET_FLOAT_WORD(hy, __y);
  iy = hy & 0x7fffffff;
  if ((ix > 0x7f800000) || (iy > 0x7f800000)) {  // x or y is NaN
    return NAN;
  }
  if (hx == 0x3f800000) {  // x == 1.0f
    return ::atanf(__y);
  }
  m = ((hy >> 31) & 1) | ((hx >> 30) & 2);  // 2*sign(x)+sign(y)

  // when y = 0
  if (iy == 0) {
    switch(m) {
    case 0:
    case 1: return __y;  // atan(+-0,+anything)=+-0
    case 2: return  pi;  // atan(+0,-anything) = pi
    case 3: return -pi;  // atan(-0,-anything) =-pi
    }
  }
  // when x = 0
  if (ix==0) {
    return (hy < 0) ? -pi_o_2 : pi_o_2;
  }
  // when x is INF
  if (ix == 0x7f800000) {
    if (iy == 0x7f800000) {
      switch(m) {
      case 0: return  pi_o_4;          // atan(+INF,+INF)
      case 1: return -pi_o_4;          // atan(-INF,+INF)
      case 2: return  3.0f * pi_o_4;   // atan(+INF,-INF)
      case 3: return -3.0f * pi_o_4;   // atan(-INF,-INF)
      }
    } else {
      switch(m) {
      case 0: return  zero;    // atan(+...,+INF)
      case 1: return -zero;    // atan(-...,+INF)
      case 2: return  pi;      // atan(+...,-INF)
      case 3: return -pi;      // atan(-...,-INF)
      }
    }
  }
  // when y is INF
  if (iy == 0x7f800000) {
    return (hy < 0) ? -pi_o_2: pi_o_2;
  }

  // compute y/x
  k = (iy - ix) >> 23;
  if (k > 60) {  // |y/x| > 2**60
    z = pi_o_2 + 0.5f * pi_lo;
  } else if (hx < 0 && k < -60) { // |y|/x < -2**60
    z = 0.0f;
  } else {  // safe to do y/x
    z = ::atanf(fabsf(__y / __x));
  }
  switch (m) {
  case 0: return z;                  // atan(+,+)
  case 1: return -z;                 // atan(-,+)
  case 2: return pi - (z - pi_lo);   // atan(+,-)
  default: return (z - pi_lo) - pi;  // atan(-,-)
  }
}
__DEVICE__ float cbrtf(float __x) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ half ceil(half __x) throw() {
  return __mlvm_scalar_cvti32_f16_up(__x);
}
__DEVICE__ float ceilf(float __x) throw() {
  return __mlvm_scalar_cvti32_f32_up(__x);
}
__DEVICE__ float ceil(float __x) throw() {
  return ::ceilf(__x);
}
__DEVICE__ float cosf(float __x) throw() {
  return __builtin_cosf(__x);
}
__DEVICE__ float cos(float __x) throw() {
  return ::cosf(__x);
}
__DEVICE__ float cosh(float __x) throw() {
  return ::coshf(__x);
}
__DEVICE__ float coshf(float __x) throw() {
  const float one = 1.0f;
  float t, w;
  int32_t ix;

  GET_FLOAT_WORD(ix,__x);
  ix &= 0x7fffffff;

  // x is INF or NaN
  if (ix > 0x7f800000) {
    return __x;
  } else if (ix == 0x7f800000) {
    return INFINITY;
  }

  // |x| in [0,22]
  if (ix < 0x41b00000) {
    // |x| in [0,0.5*ln2], return 1+expm1(|x|)^2/(2*exp(|x|))
    if (ix < 0x3eb17218) {
      if (ix < 0x24000000) {
        return one;  // cosh(tiny) = 1
      }
      t = expm1f(fabsf(__x));
      w = one + t;
      return one + (t * t) / (w + w);
    }

    // |x| in [0.5*ln2,22], return (exp(|x|)+1/exp(|x|))/2
    t = expf(fabsf(__x));
    return 0.5f * t + 0.5f / t;
  }

  // |x| in [22, log(max float)] return half * exp(|x|)
  if (ix < 0x42b17180) {
    return 0.5f * expf(fabsf(__x));
  }

  // |x| in [log(max float), overflowthresold]
  if (ix <= 0x42b2d4fc) {
    w = expf(0.5f * fabsf(__x));
    t = 0.5f * w;
    return t * w;
  }

  // |x| > overflowthresold, cosh(x) overflow
  return INFINITY;
}
__DEVICE__ float copysignf(float __x, float __y) throw() {
  return __builtin_copysignf(__x, __y);
}
__DEVICE__ float copysign(float __x, float __y) throw() {
  return ::copysignf(__x, __y);
}
__DEVICE__ float exp(float __x) throw() {
  return ::expf(__x);
}

#if (__BANG_ARCH__ != 303) && (__BANG_ARCH__ != 306)
__DEVICE__ float expf(float __x) throw() {
  const float himark = 88.72283935546875f;
  const float lomark = -103.972084045410f;
  const float lg2_h = 6.93145752e-01f;
  const float lg2_l = 1.42860654e-06f;
  float dx, pz, res, ex, dx_t;
  float dxt_h, dxt_l, b, av, bv, ar, br;
  float dxt2_h, dxt2_l, dxt3_h, dxt3_l, dxt4_h, dxt4_l;
  float t, z, t1, t2;
  int tval, n;

  if (__isnanf(__x)) {
    return __x;
  } else if (__x < himark && __x > lomark) {
    n = __float2int_rd(__x * (float) M_LOG2E);
    dx_t = __x - n * lg2_h;

    b = -n * lg2_l;
    dxt_h = dx_t + b;
    bv = dxt_h - dx_t;
    av = dxt_h - bv;
    br = b - bv;
    ar = dx_t - av;
    dxt_l = ar + br;

    t = ((dxt_h + dxt_l) * 512.0f);
    tval = __float2int_rd(t);

    if (fabs(t - tval) == 0.5f) {
      int fl = truncf(t);
      if (dxt_h >= 0.0f) {
        if (((fl & 1) == 1) && (dxt_l < 0.0f)) {
          tval -= 1;
        } else if (((fl & 1) == 0) && (dxt_l > 0.0f)) {
          tval += 1;
        }
      } else {
        if (((fl & 1) == 0) && (dxt_l < 0.0f)) {
          tval -= 1;
        } else if (((fl & 1) == 1) && (dxt_l > 0)) {
          tval += 1;
        }
      }
    }

    dx = dxt_h - tval * 1.953125e-3f;
    b = dxt_l;
    dxt2_h = dx + b;
    bv = dxt2_h - dx;
    av = dxt2_h - bv;
    br = b - bv;
    ar = dx - av;
    dxt2_l = ar + br;

    b = -__bang_math_expf_de[tval + 178];
    dxt3_h = dxt2_h + b;
    bv = dxt3_h - dxt2_h;
    av = dxt3_h - bv;
    br = b - bv;
    ar = dxt2_h - av;
    dxt3_l = ar + br;

    dxt3_l += dxt2_l;
    dxt4_h = dxt3_h + dxt3_l;
    dxt4_l = dxt3_l - (dxt4_h - dxt3_h);

    ex = __bang_math_expf_ae[tval + 178];

    z = dxt4_h * dxt4_h;
    z += 2.0f * dxt4_l;
    t1 = 4.16666679e-02f * z + 0.5f;
    t2 = 1.66666672e-01f * z + 1.0f;
    pz = t1 * z + t2 * dxt4_h;

    res = pz * ex + ex;
    res = ::scalbnf(res, n);
    return res;
  } else if (__x < himark) {
    // Exceptional cases:
    return 0.0f;
  } else {
    return INFINITY;
  }
}
#endif // __BANG_ARCH__ != 303 && __BANG_ARCH__ != 306

__DEVICE__ float expm1(float __x) throw() {
  return ::expm1f(__x);
}
__DEVICE__ float expm1f(float __x) throw() {
  const float huge = 1.0e+30f;
  const float one = 1.0f;
  const float o_threshold = 8.8721679688e+01f;  // 0x42b17180
  const float ln2_hi = 6.9313812256e-01f;       // 0x3f317180
  const float ln2_lo = 9.0580006145e-06f;       // 0x3717f7d1
  const float invln2 = 1.4426950216e+00f;       // 0x3fb8aa3b
  // scaled coefficients related to expm1
  const float Q1 = -3.3333335072e-02f;          // 0xbd088889
  const float Q2 = 1.5873016091e-03f;           // 0x3ad00d01
  const float Q3 = -7.9365076090e-05f;          // 0xb8a670cd
  const float Q4 = 4.0082177293e-06f;           // 0x36867e54
  const float Q5 = -2.0109921195e-07f;          // 0xb457edbb
  float y, hi, lo, c, t, e, hxs, hfx, r1;
  int32_t k, xsb;
  uint32_t hx;

  GET_FLOAT_WORD(hx, __x);
  xsb = hx & 0x80000000;  // sign bit of x
  y = fabsf(__x);
  hx &= 0x7fffffff;  // high word of |x|

  // filter out huge and non-finite argument
  if (hx >= 0x4195b844) {  //if |x|>=27*ln2
    if (hx >= 0x42b17218) {  // if |x|>=88.721...
      if (hx > 0x7f800000) {  // NaN
        return NAN;
      }
      if (hx == 0x7f800000) {
        return (xsb == 0) ? __x : -1.0f;  // exp(+-inf)={inf,-1}
      }
      if (__x > o_threshold) {
        return INFINITY;  // overflow
      }
    }
    if (xsb != 0) {  // x < -27*ln2, return -1.0 with inexact
      return -1.0f;
    }
  }

  // argument reduction
  if (hx > 0x3eb17218) {   // if  |x| > 0.5 ln2
    if (hx < 0x3F851592) { // and |x| < 1.5 ln2
      if (xsb == 0) {
        hi = __x - ln2_hi;
        lo = ln2_lo;
        k = 1;
      } else {
        hi = __x + ln2_hi;
        lo = -ln2_lo;
        k = -1;
      }
    } else {
      k = invln2 * __x + ((xsb == 0) ? 0.5f : -0.5f);
      t = k;
      hi = __x - t * ln2_hi;  // t*ln2_hi is exact here
      lo = t * ln2_lo;
    }
    __x = hi - lo;
    c = (hi - __x) - lo;
  }
  else if (hx < 0x33000000) {  // when |x|<2**-25, return x
    t = huge + __x;  // return x with inexact flags when x!=0
    return __x - (t - (huge + __x));
  } else {
    k = 0;
  }

  // x is now in primary range
  hfx = 0.5f * __x;
  hxs = __x * hfx;
  r1 = one + hxs * (Q1 + hxs * (Q2 + hxs * (Q3 + hxs * (Q4 + hxs * Q5))));
  t = 3.0f - r1 * hfx;
  e = hxs * ((r1 - t) / (6.0f - __x * t));
  if (k == 0) {  // c is 0
    return __x - (__x * e - hxs);
  } else {
    e = __x * (e - c) - c;
    e -= hxs;
    if (k == -1) {
      return 0.5f * (__x - e) - 0.5f;
    }
    if (k == 1) {
      if (__x < -0.25f) {
        return -2.0f * (e - (__x + 0.5f));
      } else {
        return one + 2.0f * (__x - e);
      }
    }
    if (k <= -2 || k > 56) {  // suffice to return exp(x)-1
      int32_t i;
      y = one - (e - __x);
      GET_FLOAT_WORD(i, y);
      SET_FLOAT_WORD(y, i + (k << 23));  // add k to y's exponent
      return y - one;
    }
    t = one;
    if (k < 23) {
      int32_t i;
      SET_FLOAT_WORD(t, 0x3f800000 - (0x1000000 >> k));  // t=1-2^-k
      y = t - (e - __x);
      GET_FLOAT_WORD(i, y);
      SET_FLOAT_WORD(y, i + (k << 23));  // add k to y's exponent
    } else {
      int32_t i;
      SET_FLOAT_WORD(t, (0x7f - k) << 23);  // 2^-k
      y = __x - (e + t);
      y += one;
      GET_FLOAT_WORD(i, y);
      SET_FLOAT_WORD(y, i + (k << 23));  // add k to y's exponent
    }
  }
  return y;
}
__DEVICE__ float erfcf(float __x) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ float erff(float __x) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ float exp2f(float __x) throw() {
  return __mlvm_scalar_pow2_f32(__x);
}
__DEVICE__ float exp2(float __x) throw() {
  return ::exp2f(__x);
}
__DEVICE__ float fdimf(float __x) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ half floor(half __x) throw() {
  return __mlvm_scalar_cvti32_f16_dn(__x);
}
__DEVICE__ float floorf(float __x) throw() {
  return __mlvm_scalar_cvti32_f32_dn(__x);
}
__DEVICE__ float floor(float __x) throw() {
  return ::floorf(__x);
}
__DEVICE__ float fmaxf(float __x, float __y) throw() {
  return __builtin_fmaxf(__x, __y);
}
__DEVICE__ float fmax(float __x, float __y) throw() {
  return ::fmaxf(__x, __y);
}
__DEVICE__ float fminf(float __x, float __y) throw() {
  return __builtin_fminf(__x, __y);
}
__DEVICE__ float fmin(float __x, float __y) throw() {
  return ::fminf(__x, __y);
}
__DEVICE__ float fmaf(float __x, float __y, float __z) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ float fmodf(float __x, float __y) throw() {
  if ((__isinf(__x) && !__isnanf(__y)) ||
      (!__isnanf(__x) && __y == 0) ||
      (__isnanf(__x) || __isnanf(__y))) {
    // return NaN for special cases.
    return ::copysignf(NAN, __x);
  } else if (!__isinf(__x) && __isinf(__y)) {
    // return x for special cases.
    return __x;
  } else {
    // common cases.

    union { float f; uint32_t u; }ux = {__x};
    union { float f; uint32_t u; }uy = {__y};

    uint32_t absx = ux.u & 0x7fffffffU;
    uint32_t absy = uy.u & 0x7fffffffU;

    if (absx <= absy) {
      if (absx == absy) {
        ux.u &= 0x80000000;
        return ux.f;
      }
      return __x;
    }

    // Extract exponent.
    int32_t expx = absx >> 23;
    int32_t expy = absy >> 23;
    // Extract mantissa.
    int32_t sx = absx & 0x007fffff;
    int32_t sy = absy & 0x007fffff;

    // Denormal x.
    if (expx == 0) {
      // Calculate shift to move leading bit to exponent field.
      int clzl_absx = 8 * sizeof(uint64_t);
      for (uint64_t absx64 = absx; absx64 != 0; absx64 >>= 1) {
        --clzl_absx;
      }
      uint32_t shift = clzl_absx - (8 * sizeof(uint64_t) - 24);
      // Do the shift.
      sx <<= shift;
      // Adjust the biased exponent accordingly, -1 here for the implicit
      // shift to make implicit denorm leading bit explicit.
      expx = 1 - shift;
    }

    // Denormal y.
    if (expy == 0) {
      // Calculate shift to move leading bit to exponent field.
      int clzl_absy = 8 * sizeof(uint64_t);
      for (uint64_t absy64 = absy; absy64 != 0; absy64 >>= 1) {
        --clzl_absy;
      }
      uint32_t shift = clzl_absy - (8 * sizeof(uint64_t) - 24);
      // Do the shift.
      sy <<= shift;
      // Adjust the biased exponent accordingly, -1 here for the implicit
      // shift to make implicit denorm leading bit explicit.
      expy = 1 - shift;
    }

    // Make implicit bits explicit.
    sx |= 0x00800000;
    sy |= 0x00800000;

    // Do iterate util the exponent of x is the same as y.
    // Calculate the number of bits we can safely shave off before
    // we reach parity.
    int32_t idiff = expx - expy;
    int32_t shift = 0;
    int32_t mask;

    // Since idiff is always >= 0 here, it is safe to enter.
    // We always shift by shift + 1 here, so in the first iteration,
    // the worst that we can happen.
    do {
      // Move the leading bit of x to the 23rd bit.
      sx <<= shift;

      // Keep track that we did that.
      idiff += ~shift;

      // The two values both have the 23rd bit set as the
      // leading bit (24 bit unsigned number).
      // Subract one from the other. This gives us a signed 23 bit
      // number in the range {-0x00ffffff ... 0x00ffffff}.
      sx -= sy;

      // Record the sign.
      mask = sx >> 31;

      // Shift x left 1 to restore a 24 bit quantity.
      // This is potentially 1 shift too far, which we patch up later.
      sx += sx;

      // If negative, we add back sy to restore to postiveness.
      // This is the same as x - y + 0.5y = -0.5y instead of x - y.
      // We've effectively hoisted the subtract that would have appeared in
      // the next loop iteration here, and saved a test+branch in exchange
      // for a shift and and. (The add happens either way.)
      sx += sy & mask;

      // Figure out how far we need to shift sx to get the leading bit
      // into the 23rd position.
      int clzl_sx = 8 * sizeof(uint64_t);
      for (uint64_t sx64 = sx; sx64 != 0; sx64 >>= 1) {
        --clzl_sx;
      }
      shift = clzl_sx - (8 * sizeof(uint64_t) - 24);
    } while (idiff >= shift && sx != 0);

    // We may have gone too far.
    if (idiff < 0) {
      // Rewind a little.
      // If sx - sy succeeded, it was the right thing to do,
      // the only thing we did wrong was the shift.
      // If sx - sy yielded a negative number, then we shouldn't have
      // done that either.
      sx += sy & mask;
      sx >>= 1;
      // Debug code to make sure we didn't do something dumb here.
      idiff = 0;
    }

    // We may have some bits left over to shift in.
    sx <<= idiff;

    // Convert back to float.
    if (sx == 0) {
      ux.u &= 0x80000000;
      return ux.f;
    }

    // Figure out how far we need to shift in order to move leading
    // bit into exponent field.
    int clzl_sx = 8 * sizeof(uint64_t);
    for (uint64_t sx64 = sx; sx64 != 0; sx64 >>= 1) {
      --clzl_sx;
    }
    shift = clzl_sx - (8 * sizeof(uint64_t) - 24);
    // Move leading bit into exponent field.
    sx <<= shift;
    // Account for the shift in the exponent.
    expy -= shift;
    // Remove leading bit.
    sx &= 0x007fffff;
    // Or in sign.
    sx |= ux.u & 0x80000000;
    if (expy > 0) {
      ux.u = sx | (expy << 23);
      return ux.f;
    }

    // Denormal.
    expy += 126;
    ux.u = sx | (expy << 23);
    return ux.f * 0x1.0p-126f;
  }
}
__DEVICE__ float fmod(float __x, float __y) throw() {
  return ::fmodf(__x, __y);
}
__DEVICE__ float frexp(float __x, int *__exp) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ float frexpf(float __x, int *__exp) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ float hypotf(float __x, float __y) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ int ilogbf(float __x) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0;
}
#if defined(__GNUC__)
#undef isnan
__DEVICE__ bool isnan(float __x) throw() {
  return (__isnanf(__x) != 0);
}
#undef isinf
__DEVICE__ int isinf(float __x) throw() {
  return __isinf(__x);
}
#endif  // defined(__GNUC__)
__DEVICE__ float ldexp(float __arg, int __exp) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ float log2f(float __x) throw() {
  return __builtin_log2f(__x);
}
__DEVICE__ float log2(float __x) throw() {
  return ::log2f(__x);
}
__DEVICE__ float log(float __x) throw() {
  return ::logf(__x);
}
__DEVICE__ float logf(float __x) throw() {
  const float ln2_hi = 6.9313812256e-01f;  // 0x3f317180
  const float ln2_lo = 9.0580006145e-06f;  // 0x3717f7d1
  const float two25 = 3.355443200e+07f;    // 0x4c000000
  const float Lg1 = 6.6666668653e-01f;     // 0x3F2AAAAB
  const float Lg2 = 4.0000000596e-01f;     // 0x3ECCCCCD
  const float Lg3 = 2.8571429849e-01f;     // 0x3E924925
  const float Lg4 = 2.2222198546e-01f;     // 0x3E638E29
  const float Lg5 = 1.8183572590e-01f;     // 0x3E3A3325
  const float Lg6 = 1.5313838422e-01f;     // 0x3E1CD04F
  const float Lg7 = 1.4798198640e-01f;     // 0x3E178897
  const float zero = 0.0f;
  float hfsq, f, s, z, R, w, t1, t2, dk;
  int32_t k, ix, i, j;

  GET_FLOAT_WORD(ix, __x);

  k = 0;
  if (ix < 0x00800000) {      // x < 2**-126
    if ((ix & 0x7fffffff) == 0) {
      return -INFINITY;  // log(+-0)=-inf
    }
    if (ix < 0) {
      return NAN;  // log(-#) = NaN
    }
    // subnormal number, scale up x
    k -= 25;
    __x *= two25;
    GET_FLOAT_WORD(ix, __x);
  }
  if (ix >= 0x7f800000) {
    return __x;
  }
  k += (ix >> 23) - 127;
  ix &= 0x007fffff;
  i = (ix + (0x95f64 << 3)) & 0x800000;
  SET_FLOAT_WORD(__x, ix | (i ^ 0x3f800000));  // normalize x or x/2
  k += (i >> 23);
  f = __x - 1.0f;
  if ((0x007fffff & (15 + ix)) < 16) {  // |f| < 2**-20
    if (f == zero) {
      if (k == 0) {
        return zero;
      } else {
        dk = (float) k;
        return dk * ln2_hi + dk * ln2_lo;
      }
    }
    R = f * f * (0.5f - 0.33333333333333333f * f);
    if (k == 0) {
      return f - R;
    } else {
      dk = (float) k;
      return dk * ln2_hi - ((R - dk * ln2_lo) - f);
    }
  }
  s = f / (2.0f + f);
  dk = (float) k;
  z = s * s;
  i = ix - (0x6147a << 3);
  w = z * z;
  j = (0x6b851 << 3) - ix;
  t1 = w * (Lg2 + w * (Lg4 + w * Lg6));
  t2 = z * (Lg1 + w * (Lg3 + w * (Lg5 + w * Lg7)));
  i |= j;
  R = t2 + t1;
  if (i > 0) {
    hfsq = 0.5f * f * f;
    if (k == 0) {
      return f - (hfsq - s * (hfsq + R));
    } else {
      return dk * ln2_hi - ((hfsq - (s * (hfsq + R) + dk * ln2_lo)) - f);
    }
  } else {
    if (k == 0) {
      return f - s * (f - R);
    } else {
      return dk * ln2_hi - ((s * (f - R) - dk * ln2_lo) - f);
    }
  }
}
__DEVICE__ float log10(float __x) throw() {
  return ::log10f(__x);
}
__DEVICE__ float log10f(float __x) throw() {
  const float two25 = 3.3554432000e+07f;      // 0x4c000000
  const float ivln10 = 4.3429449201e-01f;     // 0x3ede5bd9
  const float log10_2hi = 3.0102920532e-01f;  // 0x3e9a2080
  const float log10_2lo = 7.9034151668e-07f;  // 0x355427db
  float y, z;
  int32_t i, k, hx;

  GET_FLOAT_WORD(hx, __x);

  k = 0;
  if (hx < 0x00800000) {      // x < 2**-126
    if ((hx & 0x7fffffff) == 0) {
      return -INFINITY;  // log(+-0)=-inf
    }
    if (hx < 0) {
      return NAN;  // log(-#) = NaN
    }
    // subnormal number, scale up x
    k -= 25;
    __x *= two25;
    GET_FLOAT_WORD(hx, __x);
  }
  if (hx >= 0x7f800000) {
    return __x;
  }
  k += (hx >> 23) - 127;
  i = ((uint32_t) k & 0x80000000) >> 31;
  hx = (hx & 0x007fffff) | ((0x7f - i) << 23);
  y = (float) (k + i);
  SET_FLOAT_WORD(__x, hx);
  z = y * log10_2lo + ivln10 * ::logf(__x);
  return z + y * log10_2hi;
}
__DEVICE__ float log1pf(float __x) throw() {
  const float ln2_hi = 6.9313812256e-01f;   // 0x3f317180
  const float ln2_lo = 9.0580006145e-06f;   // 0x3717f7d1
  const float two25 = 3.355443200e+07f;     // 0x4c000000
  const float Lp1 = 6.6666668653e-01f;      // 0x3F2AAAAB
  const float Lp2 = 4.0000000596e-01f;      // 0x3ECCCCCD
  const float Lp3 = 2.8571429849e-01f;      // 0x3E924925
  const float Lp4 = 2.2222198546e-01f;      // 0x3E638E29
  const float Lp5 = 1.8183572590e-01f;      // 0x3E3A3325
  const float Lp6 = 1.5313838422e-01f;      // 0x3E1CD04F
  const float Lp7 = 1.4798198640e-01f;      // 0x3E178897
  const float zero = 0.0f;
  float hfsq, f, c, s, z, R, u;
  int32_t k, hx, hu, ax;

  GET_FLOAT_WORD(hx, __x);
  ax = hx & 0x7fffffff;

  k = 1;
  if (hx < 0x3ed413d7) {      // x < 0.41422
    if (ax >= 0x3f800000) {   // x <= -1.0
      if (__x == -1.0f) {
        return -INFINITY;  // log1p(-1)=-inf
      } else {
        return NAN;  // log1p(x<-1)=NaN
      }
    }
    if (ax < 0x31000000) {     // |x| < 2**-29
      if (ax < 0x24800000) {   // |x| < 2**-54
        return __x;
      } else {
        return __x - __x * __x * 0.5f;
      }
    }
    if (hx > 0 || hx <= ((int32_t) 0xbe95f61f)) {  // -0.2929<x<0.41422
      k = 0;
      f = __x;
      hu = 1;
    }
  }
  if (hx >= 0x7f800000) {
    return __x;
  }
  if (k != 0) {
    if (hx < 0x5a000000) {
      u = 1.0f + __x;
      GET_FLOAT_WORD(hu,u);
      k = (hu >> 23) - 127;
      if (k > 0) {
        c = 1.0f - (u - __x);
      } else {
        c = __x - (u - 1.0f);
      }
      c /= u;
    } else {
      u = __x;
      GET_FLOAT_WORD(hu, u);
      k = (hu >> 23) - 127;
      c = 0;
    }
    hu &= 0x007fffff;
    if (hu < 0x3504f7) {
      SET_FLOAT_WORD(u, hu | 0x3f800000);  // normalize u
    } else {
      k += 1;
      SET_FLOAT_WORD(u, hu | 0x3f000000);  // normalize u/2
      hu = (0x00800000 - hu) >> 2;
    }
    f = u - 1.0f;
  }
  hfsq = 0.5f * f * f;
  if (hu == 0) {  // |f| < 2**-20
    if (f == zero) {
      if (k == 0) {
        return zero;
      } else {
        c += k * ln2_lo;
        return k * ln2_hi + c;
      }
    }
    R = hfsq * (1.0f - 0.66666666666666666f * f);
    if (k == 0) {
      return f - R;
    } else {
      return k * ln2_hi - ((R - (k * ln2_lo + c)) - f);
    }
  }
  s = f / (2.0f + f);
  z = s * s;
  R = z * (Lp1 + z * (Lp2 + z * (Lp3 + z * (Lp4 + z * (Lp5 + z * (Lp6 + z * Lp7))))));
  if (k == 0) {
    return f - (hfsq - s * (hfsq + R));
  } else {
    return k * ln2_hi - ((hfsq - (s * (hfsq + R) + (k * ln2_lo + c))) - f);
  }
}
__DEVICE__ float log1p(float __x) throw() {
  return ::log1pf(__x);
}
__DEVICE__ float logbf(float __x) throw() {
  int32_t ix, rix;

  GET_FLOAT_WORD (ix, __x);
  ix &= 0x7fffffff;  // high |x|
  if (ix == 0) {
    return -INFINITY;
  }
  if (ix == 0x7f800000) {
    return INFINITY;
  }
  if (ix > 0x7f800000) {
    return __x;
  }
  if ((rix = ix >> 23) == 0) {
    // POSIX specifies that denormal number is treated as
    // though it were normalized.
    rix -= 22 - __mlvm_scalar_findlast1_u32(ix);
  }
  return (float) (rix - 127);
}
__DEVICE__ float logb(float __x) throw() {
  return ::logbf(__x);
}
__DEVICE__ float ldexpf(float __x, int __exp) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ float lgammaf(float __x) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ long int lrintf(float __x) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0;
}
__DEVICE__ long long int llrintf(float __x) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0;
}
__DEVICE__ long int lroundf(float __x) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0;
}
__DEVICE__ long long int llroundf(float __x) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0;
}
__DEVICE__ float modf(float __x, float *__iptr) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ float modff(float __x, float *__iptr) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ float nearbyintf(float __x) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ float nextafterf(float __x, float __y) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ float powf(float __x, float __y) throw() {
  const float tiny = 1.0e-30f;
  const float zero = 0.0f;
  const float one = 1.0f;
  const float two = 2.0f;
  const float two24 = 16777216.0f;
  // poly coefs for (3/2)*(log(x)-2s-2/3*s**3) = 3 * atanh(s)
  const float L1 = 6.0000002384e-01f;  // 0x3f19999a  = 3/5
  const float L2 = 4.2857143283e-01f;  // 0x3edb6db7  = 3/7
  const float L3 = 3.3333334327e-01f;  // 0x3eaaaaab  = 3/9
  const float L4 = 2.7272811532e-01f;  // 0x3e8ba305  = 3/11
  const float L5 = 2.3066075146e-01f;  // 0x3e6c3255  = 3/13
  const float L6 = 2.0697501302e-01f;  // 0x3e53f142  ~ 3/15
  const float P1 = 1.6666667163e-01f;  // 0x3e2aaaab
  const float P2 = -2.7777778450e-03f;  // 0xbb360b61
  const float P3 = 6.6137559770e-05f;  // 0x388ab355
  const float P4 = -1.6533901999e-06f;  // 0xb5ddea0e
  const float P5 = 4.1381369442e-08f;  // 0x3331bb4c
  const float lg2 = 6.9314718246e-01f;  // 0x3f317218
  const float lg2_h = 6.93145752e-01f;  // 0x3f317200
  const float lg2_l = 1.42860654e-06f;  // 0x35bfbe8c
  const float ovt = 4.2995665694e-08f;  // -(128-log2(ovfl+.5ulp))
  const float cp = 9.6179670095e-01f;  // 0x3f76384f =2/(3ln2)
  const float cp_h = 9.6179199219e-01f;  // 0x3f763800 =head of cp
  const float cp_l = 4.7017383622e-06f;  // 0x369dc3a0 =tail of cp_h
  const float ivln2 = 1.4426950216e+00f;  // 0x3fb8aa3b =1/ln2
  const float ivln2_h = 1.4426879883e+00f;  // 0x3fb8aa00 =16b 1/ln2
  const float ivln2_l = 7.0526075433e-06f; // 0x36eca570 =1/ln2 tail

  float z, ax, z_h, z_l, p_h, p_l;
  float y1, t1, t2, r, s, t, u, v, w;
  float bp, dp_h, dp_l;
  int i, j, k, yisint, n;
  int hx, hy, ix, iy, is;

  if (__x == 2.0f) {
    return ::exp2f(__y);
  }

  GET_FLOAT_WORD(hx, __x);
  GET_FLOAT_WORD(hy, __y);
  ix = hx & 0x7fffffff;
  iy = hy & 0x7fffffff;

  // y==zero: x**0 = 1
  if (iy == 0) {
    return one;
  }

  // x==+-1
  if (__x == 1.0f) {
    return one;
  }
  if (__x == -1.0f && isinf(__y)) {
    return one;
  }

  // +-NaN return NaN
  if (ix > 0x7f800000 || iy > 0x7f800000) {
    return NAN;
  }

  // determine if y is an odd int when x < 0
  // yisint = 0 ... y is not an integer
  // yisint = 1 ... y is an odd int
  // yisint = 2 ... y is an even int
  yisint = 0;
  if (hx < 0) {
    if (iy >= 0x4b800000) {  // even integer y
      yisint = 2;
    } else if (iy >= 0x3f800000) {
      k = (iy >> 23) - 0x7f;     // exponent
      j = iy >> (23 - k);
      if ((j << (23 - k)) == iy) {
        yisint = 2 - (j & 1);
      }
    }
  }

  // special value of y
  if (iy == 0x7f800000) {  // y is +-inf
    if (ix == 0x3f800000) {
      return NAN;  // inf**+-1 is NaN
    } else if (ix > 0x3f800000) {  // (|x|>1)**+-inf = inf,0
      return (hy >= 0)? INFINITY: zero;
    } else {  // (|x|<1)**-,+inf = inf,0
      return (hy < 0)? INFINITY: zero;
    }
  }
  if (iy == 0x3f800000) {  // y is +-1
    if (hy < 0) {
      return one / __x;
    } else {
      return __x;
    }
  }
  if (hy == 0x40000000) {  // y is 2
    if (isinf(__x) == 0) {
      return __x * __x;
    } else {
      return INFINITY;
    }
  }
  if (hy == 0x3f000000) {  // y is 0.5
    if (hx >= 0) {  // x >= +0
      return ::sqrtf(__x);
    }
  }

  ax = ::fabsf(__x);
  // special value of x
  if (ix == 0x3f800000) {  // x is -1
    if (yisint == 1) {
      return -one;
    } else if (yisint == 2) {
      return one;
    } else {
      return -NAN;
    }
  }

  if (ix == 0) {  // x is +-0
    if (hx < 0 && yisint == 1) {
      return (hy > 0) ? -zero : -INFINITY;
    }
    else {
      return (hy > 0) ? zero : INFINITY;
    }
  }

  if (ix == 0x7f800000) {  // x is +-inf
    if (hx < 0 && yisint == 1) {
      return (hy < 0) ? -zero : -INFINITY;
    }
    else {
      return (hy < 0) ? zero : INFINITY;
    }
  }
  // (x<0)**(non-int) is NaN
  if (((((unsigned int) hx >> 31) - 1) | yisint) == 0) {
    return NAN;
  }

  // |y| is huge
  if (iy > 0x4d000000) {  // if |y| > 2**27
    // over/underflow if x is not close to one
    if (ix < 0x3f7ffff8) {
      return (hy < 0)? INFINITY : tiny * tiny;
    }
    if (ix > 0x3f800007) {
      return (hy > 0)? INFINITY : tiny * tiny;
    }
    // now |1-x| is tiny <= 2**-20, suffice to compute
    // log(x) by x-x^2/2+x^3/3-x^4/4
    t = ax - 1.0f;  // t has 20 trailing zeros
    w = (t * t) * (0.5f - t * (0.333333333333f - t * 0.25f));
    u = ivln2_h * t;  // ivln2_h has 16 sig. bits
    v = t * ivln2_l - w * ivln2;
    t1 = u + v;
    GET_FLOAT_WORD(is, t1);
    SET_FLOAT_WORD(t1, is & 0xfffff000);
    t2 = v - (t1 - u);
  } else {
    float s2, s_h, s_l, t_h, t_l;
    // Avoid internal underflow for tiny y.  The exact value
    // of y does not matter if |y| <= 2**-32.
    if (iy < 0x2f800000) {
      SET_FLOAT_WORD(__y, (hy & 0x80000000) | 0x2f800000);
    }
    n = 0;
    // take care subnormal number
    if (ix < 0x00800000) {
      ax *= two24;
      n -= 24;
      GET_FLOAT_WORD(ix,ax);
    }
    n += (ix >> 23) - 0x7f;
    j = ix & 0x007fffff;
    // determine interval
    ix = j | 0x3f800000;  // normalize ix
    if (j <= 0x1cc471) {  // |x|<sqrt(3/2)
      k = 0;
    } else if (j < 0x5db3d7) {  // |x|<sqrt(3)
      k = 1;
    } else {
      k = 0;
      n += 1;
      ix -= 0x00800000;
    }
    SET_FLOAT_WORD(ax, ix);

    if (k == 0) {
      bp = 1.0f;
      dp_h = 0.0f;
      dp_l = 0.0f;
    } else {
      bp = 1.5f;
      // dp = ln(2/3) / ln(2)
      dp_h = 5.84960938e-01f;
      dp_l = 1.56322085e-06f;
    }

    // compute s = s_h+s_l = (x-1)/(x+1) or (x-1.5)/(x+1.5)
    u = ax - bp;  // bp[0]=1.0, bp[1]=1.5
    v = one / (ax + bp);
    s = u * v;
    s_h = s;
    GET_FLOAT_WORD(is, s_h);
    SET_FLOAT_WORD(s_h, is & 0xfffff000);
    // t_h=ax+bp[k] High
    SET_FLOAT_WORD(t_h,
        ((((ix >> 1) | 0x20000000) + 0x00400000 + (k<<21))
         & 0xfffff000));
    t_l = ax - (t_h - bp);
    s_l = v * ((u - s_h * t_h) - s_h * t_l);
    // compute log(ax)
    s2 = s * s;
    r = s2 * s2 * (L1 + s2 * (L2 + s2 * (L3 + s2 * (L4 + s2 * (L5 + s2 * L6)))));
    r += s_l * (s_h + s);
    s2  = s_h * s_h;
    t_h = 3.0f + s2 + r;
    GET_FLOAT_WORD(is, t_h);
    SET_FLOAT_WORD(t_h, is & 0xfffff000);
    t_l = r - ((t_h - 3.0f) - s2);
    // u+v = s*(1+...)
    u = s_h * t_h;
    v = s_l * t_h + t_l * s;
    // 2/(3log2)*(s+...)
    p_h = u + v;
    GET_FLOAT_WORD(is, p_h);
    SET_FLOAT_WORD(p_h, is & 0xfffff000);
    p_l = v - (p_h - u);
    z_h = cp_h * p_h;  // cp_h+cp_l = 2/(3*log2)
    z_l = cp_l * p_h + p_l * cp + dp_l;
    // log2(ax) = (s+..)*2/(3*log2) = n + dp_h + z_h + z_l
    t = (float) n;
    t1 = (((z_h + z_l) + dp_h) + t);
    GET_FLOAT_WORD(is, t1);
    SET_FLOAT_WORD(t1, is & 0xfffff000);
    t2 = z_l - (((t1 - t) - dp_h) - z_h);
  }

  s = one;  // s (sign of result -ve**odd) = -1 else = 1
  if (((((unsigned int) hx >> 31) - 1) | (yisint - 1)) == 0) {
    s = -one;  // (-ve)**(odd int)
  }

  // split up y into y1+y2 and compute (y1+y2)*(t1+t2)
  GET_FLOAT_WORD(is, __y);
  SET_FLOAT_WORD(y1, is & 0xfffff000);
  p_l = (__y - y1) * t1 + __y * t2;
  p_h = y1 * t1;
  z = p_l + p_h;
  GET_FLOAT_WORD(j, z);
  if (j > 0x43000000) {  // if z > 128
    return ::copysignf(INFINITY, s);  // overflow
  } else if (j == 0x43000000) { // if z == 128
    if (p_l + ovt > z - p_h) {
      return ::copysignf(INFINITY, s);  // overflow
    }
  } else if ((j & 0x7fffffff) > 0x43160000) {  // z <= -150
    return s * tiny * tiny;  // underflow
  } else if ((unsigned int) j == 0xc3160000) {  // z == -150
    if (p_l <= z - p_h) {
      return s * tiny * tiny;  // underflow
    }
  }

  // compute 2**(p_h+p_l)
  i = j & 0x7fffffff;
  k = (i >> 23) - 0x7f;
  n = 0;
  if (i > 0x3f000000) {  // if |z| > 0.5, set n = [z+0.5]
    n = j + (0x00800000 >> (k+1));
    k = ((n & 0x7fffffff) >> 23) - 0x7f;  // new k for n
    SET_FLOAT_WORD(t, n & ~(0x007fffff >> k));
    n = ((n & 0x007fffff) | 0x00800000) >> (23 - k);
    if (j < 0) {
      n = -n;
    }
    p_h -= t;
  }
  t = p_l + p_h;
  GET_FLOAT_WORD(is, t);
  SET_FLOAT_WORD(t, is & 0xfffff000);
  u = t * lg2_h;
  v = (p_l - (t - p_h)) * lg2 + t * lg2_l;
  z = u + v;
  w = v - (z - u);
  t = z * z;
  t1 = z - t * (P1 + t * (P2 + t * (P3 + t * (P4 + t * P5))));
  r = (z * t1) / (t1 - two) - (w + z * w);
  z = one - (r - z);
  GET_FLOAT_WORD(j, z);
  j += (n << 23);
  if ((j >> 23) <= 0) {  // subnormal output
    z = ::scalbnf(z, n);
  } else {
    SET_FLOAT_WORD(z, j);
  }
  return s * z;
}
__DEVICE__ float pow(float __base, float __exp) throw() {
  return ::powf(__base, __exp);
}
__DEVICE__ float remainderf(float __x, float __y) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ float remquof(float __x, float __y, int * __quo) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ float rintf(float __x) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ half round(half __x) throw() {
  return __mlvm_scalar_cvti32_f16_rd(__x);
}
__DEVICE__ float roundf(float __x) throw() {
  return __mlvm_scalar_cvti32_f32_rd(__x);
}
__DEVICE__ float round(float __x) throw() {
  return ::roundf(__x);
}
__DEVICE__ float sinf(float __x) throw() {
  return __builtin_sinf(__x);
}
__DEVICE__ float sin(float __x) throw() {
  return ::sinf(__x);
}
__DEVICE__ float sinh(float __x) throw() {
  return ::sinhf(__x);
}
__DEVICE__ float sinhf(float __x) throw() {
  const float one = 1.0f;
  float t, w, h;
  int32_t ix, jx;

  GET_FLOAT_WORD(jx, __x);
  ix = jx & 0x7fffffff;

  // x is INF or NaN
  if (ix >= 0x7f800000) {
    return __x;
  }

  h = 0.5f;
  if (jx < 0) {
    h = -0.5f;
  }
  // |x| in [0,22], return sign(x)*0.5*(E+E/(E+1)))
  if (ix < 0x41b00000) {  // |x| < 22
    if (ix < 0x31800000) {  // |x| < 2**-28
      return __x;  // sinh(tiny) = x
    }
    t = expm1f(fabsf(__x));
    if (ix < 0x3f800000) {  // |x| < 1
      return h * (2.0f * t - t * t / (t + one));
    }
    return h * (t + t / (t + one));
  }

  // |x| in [22, log(max float)] return 0.5*exp(|x|)
  if (ix < 0x42b17180) {
    return h * expf(fabsf(__x));
  }

  // |x| in [log(max float), overflowthresold]
  if (ix <= 0x42b2d4fc) {
    w = expf(0.5f * fabsf(__x));
    t = h * w;
    return t * w;
  }

  // |x| > overflowthresold, sinh(x) overflow
  return copysignf(INFINITY, __x);
}
__DEVICE__ float scalbnf(float __x, int __exp) throw() {
  const float two25 = 3.355443200e+07f;  // 0x4c000000
  const float twom25 = 2.9802322388e-08f;  // 0x33000000
  int k, ix;
  GET_FLOAT_WORD(ix, __x);
  k = (ix & 0x7f800000) >> 23;  // extract exponent
  if (k == 0) {  // 0 or subnormal x
    if ((ix & 0x7fffffff) == 0) {  // +-0
      return __x;
    }
    __x *= two25;
    GET_FLOAT_WORD(ix, __x);
    k = ((ix & 0x7f800000) >> 23) - 25;
  }
  if (k == 0xff) {  // NaN or Inf
    return __x;
  }
  if (__exp < -50000) {
    return ::copysignf(0.0f, __x);  // underflow
  }
  if (__exp > 50000 || k + __exp > 0xfe) {
    return ::copysignf(INFINITY, __x);  // overflow
  }
  // Now k and n are bounded we know that k = k+n does not
  // overflow.
  k = k + __exp;
  if (k > 0) {  // normal result
    SET_FLOAT_WORD(__x, (ix & 0x807fffff) | (k << 23));
    return __x;
  }
  if (k <= -25) {
      return ::copysignf(0.0f, __x);  // underflow
  }
  k += 25;        // subnormal result
  SET_FLOAT_WORD(__x, (ix & 0x807fffff) | (k << 23));
  return __x * twom25;
}
__DEVICE__ float scalblnf(float __x, long int __exp) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ float sqrtf(float __x) throw() {
  return __builtin_sqrtf(__x);
}
__DEVICE__ float sqrt(float __x) throw() {
  return ::sqrtf(__x);
}

#if (__BANG_ARCH__ != 303) && (__BANG_ARCH__ != 306)
__DEVICE__ int __kernel_rem_pio2f(float *x, float *y, int e0, int nx) throw() {
  const float zero = 0.0f;
  const float one = 1.0f;
  const float two8 = 2.5600000000e+02f;  // 0x43800000
  const float twon8 = 3.9062500000e-03f; // 0x3b800000
  int32_t jz, jx, jv, jp, jk, carry, n, iq[20], i, j, k, m, q0, ih;
  float z, fw, f[20], fq[20], q[20], fv;

  // initialize jk
  jk = 9;
  jp = jk;

  // determine jx, jv, q0, note that 3>q0
  jx = nx - 1;
  jv = (e0 - 3) / 8;
  if (jv < 0) {
    jv = 0;
  }
  q0 = e0 - 8 * (jv + 1);

  // set up f[0] to f[jx+jk] where f[jx+jk] = ipio2[jv+jk]
  j = jv - jx;
  m = jx + jk;
  for (i = 0; i <= m; i++, j++) {
    f[i] = (j < 0) ? zero : (float) __bang_math_rem_ipio2[j];
  }

  // compute q[0],q[1],...q[jk]
  for (i = 0; i <= jk; i++) {
    fw = 0.0f;
    for (j = 0; j <= jx; j++) {
      fw += x[j] * f[jx + i - j];
    }
    q[i] = fw;
  }

  jz = jk;
recompute:
  // distill q[] into iq[] reversingly
  for (i = 0, j = jz, z = q[jz]; j > 0; i++, j--) {
    fw = (float) ((int32_t) (twon8 * z));
    iq[i] = (int32_t) (z - two8 * fw);
    z = q[j-1] + fw;
  }

  // compute n
  z = scalbnf(z, q0);   // actual value of z
  z -= 8.0f * floorf(z * 0.125f);  // trim off integer >= 8
  n = (int32_t) z;
  z -= (float) n;
  ih = 0;
  if (q0 > 0) {  // need iq[jz-1] to determine n
    i = iq[jz-1] >> (8 - q0);
    n += i;
    iq[jz-1] -= i << (8 - q0);
    ih = iq[jz-1] >> (7 - q0);
  } else if (q0 == 0) {
    ih = iq[jz-1] >> 7;
  } else if (z >= 0.5f) {
    ih = 2;
  }

  if (ih > 0) {  // q > 0.5
    n += 1;
    carry = 0;
    for (i = 0; i < jz; i++) {  // compute 1-q
      j = iq[i];
      if (carry == 0) {
        if (j != 0) {
          carry = 1;
          iq[i] = 0x100 - j;
        }
      } else {
        iq[i] = 0xff - j;
      }
    }
    if (q0 > 0) {  // rare case: chance is 1 in 12
      switch (q0) {
      case 1:
        iq[jz - 1] &= 0x7f;
        break;
      case 2:
        iq[jz - 1] &= 0x3f;
        break;
      }
    }
    if (ih == 2) {
      z = one - z;
      if (carry != 0) {
        z -= scalbnf(one, q0);
      }
    }
  }

  // check if recomputation is needed
  if (z == zero) {
    j = 0;
    for (i = jz - 1; i >= jk; i--) {
      j |= iq[i];
    }
    if (j == 0) {  // need recomputation
      // k = no. of terms needed
      k = 1;
      while (iq[jk - k] == 0) {
        k++;
      }

      // add q[jz+1] to q[hz+k]
      for (i = jz + 1; i <= jz + k; i++) {
        f[jx + i] = (float) __bang_math_rem_ipio2[jv + i];
        fw = 0.0f;
        for (j = 0; j <= jx; j++) {
          fw += x[j] * f[jx + i - j];
        }
        q[i] = fw;
      }
      jz += k;
      goto recompute;
    }
  }

  // chop off zero terms
  if (z == 0.0f) {
    jz -= 1;
    q0 -= 8;
    while (iq[jz] == 0) {
      jz--;
      q0 -= 8;
    }
  } else {  // break z into 8-bit if necessary
    z = scalbnf(z, -q0);
    if (z >= two8) {
      fw = (float) ((int32_t) (twon8 * z));
      iq[jz] = (int32_t) (z - two8 * fw);
      jz += 1;
      q0 += 8;
      iq[jz] = (int32_t) fw;
    } else {
      iq[jz] = (int32_t) z;
    }
  }

  // convert integer "bit" chunk to floating-point value
  fw = scalbnf(one, q0);
  for (i = jz; i >= 0; i--) {
    q[i] = fw * (float) iq[i];
    fw *= twon8;
  }

  // compute PIo2[0,...,jp]*q[jz,...,0]
  for (i=jz; i>=0; i--) {
    fw = 0.0f;
    for (k = 0; k <= jp && k <= jz - i; k++) {
      fw += __bang_math_rem_pio2[k] * q[i + k];
    }
    fq[jz - i] = fw;
  }

  // compress fq[] into y[]
  fv = 0.0f;
  for (i = jz; i >= 0; i--) {
    fv = fv + fq[i];
  }
  y[0] = (ih == 0) ? fv: -fv;
  fv = fq[0] - fv;
  for (i = 1; i <= jz; i++) {
    fv = fv + fq[i];
  }
  y[1] = (ih == 0) ? fv: -fv;
  return n & 7;
}
__DEVICE__ int32_t __rem_pio2f(float x, float *y) throw() {
  const float zero = 0.0f;                  // 0x00000000
  const float two8 = 2.5600000000e+02f;     // 0x43800000
  const float invpio2 = 6.3661980629e-01f;  // 0x3f22f984
  const float pio2_1 = 1.5707855225e+00f;   // 0x3fc90f80
  const float pio2_1t = 1.0804334124e-05f;  // 0x37354443
  const float pio2_2 = 1.0804273188e-05f;   // 0x37354400
  const float pio2_2t = 6.0770999344e-11f;  // 0x2e85a308
  const float pio2_3 = 6.0770943833e-11f;   // 0x2e85a300
  const float pio2_3t = 6.1232342629e-17f;  // 0x248d3132

  float z, w, t, r, fn;
  float tx[3];
  int32_t e0, i, j, nx, n, ix, hx;

  GET_FLOAT_WORD(hx, x);
  ix = hx & 0x7fffffff;
  if (ix <= 0x3f490fd8) {  // |x| ~<= pi/4 , no need for reduction
    y[0] = x;
    y[1] = 0;
    return 0;
  }
  if (ix < 0x4016cbe4) {  // |x| < 3pi/4, special case with n=+-1
    if (hx > 0) {
      z = x - pio2_1;
      if ((ix & 0xfffffff0) != 0x3fc90fd0) {  // 24+24 bit pi OK
        y[0] = z - pio2_1t;
        y[1] = (z - y[0]) - pio2_1t;
      } else {    // near pi/2, use 24+24+24 bit pi
        z -= pio2_2;
        y[0] = z - pio2_2t;
        y[1] = (z - y[0]) - pio2_2t;
      }
      return 1;
    } else {  // negative x
      z = x + pio2_1;
      if ((ix & 0xfffffff0) != 0x3fc90fd0) {  // 24+24 bit pi OK
        y[0] = z + pio2_1t;
        y[1] = (z - y[0]) + pio2_1t;
      } else {    // near pi/2, use 24+24+24 bit pi
        z += pio2_2;
        y[0] = z + pio2_2t;
        y[1] = (z - y[0]) + pio2_2t;
      }
      return -1;
    }
  }
  if (ix <= 0x43490f80) {  // |x| ~<= 2^7*(pi/2), medium size
    t = fabsf(x);
    n = __float2int_rd(t * invpio2);
    fn = (float) n;
    r = t - fn * pio2_1;
    w = fn * pio2_1t;  // 1st round good to 40 bit
    if (n < 32 &&
        (int32_t) (ix & 0xffffff00) != __bang_math_rem_npio2_hw[n - 1]) {
      y[0] = r - w;  // quick check no cancellation
    } else {
      uint32_t high;
      j = ix >> 23;
      y[0] = r - w;
      GET_FLOAT_WORD(high, y[0]);
      i = j - ((high >> 23) & 0xff);
      if (i > 8) {  // 2nd iteration needed, good to 57
        t = r;
        w = fn * pio2_2;
        r = t - w;
        w = fn * pio2_2t - ((t - r) - w);
        y[0] = r - w;
        GET_FLOAT_WORD(high, y[0]);
        i = j - ((high >> 23) & 0xff);
        if (i > 25) {  // 3rd iteration need, 74 bits acc
          t = r;       // will cover all possible cases
          w = fn * pio2_3;
          r = t - w;
          w = fn * pio2_3t - ((t - r) - w);
          y[0] = r - w;
        }
      }
    }
    y[1] = (r - y[0]) - w;
    if (hx < 0) {
      y[0] = -y[0];
      y[1] = -y[1];
      return -n;
    } else {
      return n;
    }
  }
  // all other (large) arguments
  if (ix >= 0x7f800000) {  // x is Inf or NaN
    y[0] = NAN;
    y[1] = NAN;
    return 0;
  }
  // set z = scalbn(|x|,ilogb(x)-7)
  e0 = (ix >> 23) - 134;   // e0 = ilogb(z)-7;
  SET_FLOAT_WORD(z, ix - ((int32_t) (e0 << 23)));
  for (i = 0; i < 2; i++) {
    tx[i] = (float) ((int32_t) (z));
    z = (z - tx[i]) * two8;
  }
  tx[2] = z;
  nx = 3;
  while (tx[nx - 1] == zero) {
    nx--;  // skip zero terms
  }
  n = __kernel_rem_pio2f(tx, y, e0, nx);
  if (hx < 0) {
    y[0] = -y[0];
    y[1] = -y[1];
    return -n;
  }
  return n;
}
#endif // __BANG_ARCH__ != 303 && __BANG_ARCH__ != 306

__DEVICE__ float __kernel_tanf(float x, float y, int iy) throw() {
  const float one = 1.0f;                  // 0x3f800000
  const float pio4 = 7.8539812565e-01f;    // 0x3f490fda
  const float pio4lo = 3.7748947079e-08f;  // 0x33222168
  const float T00 = 3.3333334327e-01f;     // 0x3eaaaaab
  const float T01 = 1.3333334029e-01f;     // 0x3e088889
  const float T02 = 5.3968254477e-02f;     // 0x3d5d0dd1
  const float T03 = 2.1869488060e-02f;     // 0x3cb327a4
  const float T04 = 8.8632395491e-03f;     // 0x3c11371f
  const float T05 = 3.5920790397e-03f;     // 0x3b6b6916
  const float T06 = 1.4562094584e-03f;     // 0x3abede48
  const float T07 = 5.8804126456e-04f;     // 0x3a1a26c8
  const float T08 = 2.4646313977e-04f;     // 0x398137b9
  const float T09 = 7.8179444245e-05f;     // 0x38a3f445
  const float T10 = 7.1407252108e-05f;     // 0x3895c07a
  const float T11 = -1.8558637748e-05f;    // 0xb79bae5f
  const float T12 = 2.5907305826e-05f;     // 0x37d95384

  float z, r, v, w, s;
  int32_t ix, hx;
  GET_FLOAT_WORD(hx, x);
  ix = hx & 0x7fffffff;
  if (ix < 0x39000000) {  // x < 2**-13
    if ((ix | (iy + 1)) == 0) {
      return one / fabsf(x);
    } else if (iy == 1) {
      return x;
    } else {
      return -one / x;
    }
  }
  if (ix >= 0x3f2ca140) {  // |x|>=0.6744
    if (hx < 0) {
      x = -x;
      y = -y;
    }
    z = pio4 - x;
    w = pio4lo - y;
    x = z + w;
    y = 0.0f;
    if (fabsf (x) < 0x1p-13f) {
      return (1 - ((hx >> 30) & 2)) * iy * (1.0f - 2 * iy * x);
    }
  }
  z = x * x;
  w = z * z;
  // Break x^5*(T[1]+x^2*T[2]+...) into
  // x^5(T[1]+x^4*T[3]+...+x^20*T[11]) +
  // x^5(x^2*(T[2]+x^4*T[4]+...+x^22*[T12]))
  r = T01 + w * (T03 + w * (T05 + w * (T07 + w * (T09 + w * T11))));
  v = z * (T02 + w * (T04 + w * (T06 + w * (T08 + w * (T10 + w * T12)))));
  s = z * x;
  r = y + z * (s * (r + v) + y);
  r += T00 * s;
  w = x + r;
  if (ix >= 0x3f2ca140) {
    v = (float) iy;
    return (1 - ((hx >> 30) & 2)) * (v - 2.0f * (x - (w * w / (w + v) - r)));
  }
  if (iy == 1) {
    return w;
  } else {                 // if allow error up to 2 ulp,
    float a, t;            // simply return -1.0 / (x + r) here
    int32_t i;             // compute -1.0 / (x + r) accurately
    z = w;
    GET_FLOAT_WORD(i, z);
    SET_FLOAT_WORD(z, i & 0xfffff000);
    v = r - (z - x);  // z+v = r+x
    t = a = -1.0f / w;
    GET_FLOAT_WORD(i, t);
    SET_FLOAT_WORD(t, i & 0xfffff000);
    s = 1.0f + t * z;
    return t + a * (s + t * v);
  }
}
__DEVICE__ float tan(float __x) throw() {
  return ::tanf(__x);
}

#if (__BANG_ARCH__ != 303) && (__BANG_ARCH__ != 306)
__DEVICE__ float tanf(float __x) throw() {
  float y[2];
  int32_t n, ix, iy;

  GET_FLOAT_WORD(ix, __x);

  ix &= 0x7fffffff;
  if (ix <= 0x3f490fda) {  // |x| ~< pi/4
    return __kernel_tanf(__x, 0.0f, 1);
  } else if (ix >= 0x7f800000) {  // tan(Inf or NaN) is NaN
    return NAN;
  } else {  // argument reduction needed
    n = __rem_pio2f(__x, y);    // iy = 1 if n is even
    iy = 1 - ((n & 1) << 1);  // iy = -1 if n is odd
    return __kernel_tanf(y[0], y[1], iy);
  }
}
__DEVICE__ float tanh(float __x) throw() {
  return ::tanhf(__x);
}
#endif // __BANG_ARCH__ != 303 && __BANG_ARCH__ != 306

__DEVICE__ float tanhf(float __x) throw() {
  const float one = 1.0f;
  const float two = 2.0f;
  float t, z;
  int32_t jx, ix;

  GET_FLOAT_WORD(jx, __x);
  ix = jx & 0x7fffffff;

  // x is INF or NaN
  if (ix >= 0x7f800000) {
    if (ix == 0x7f800000) {
      return copysignf(one, __x);  // tanh(+-inf)=+-1
    } else {
      return NAN;  // tanh(NaN) = NaN
    }
  }

  // |x| < 22
  if (ix < 0x41b00000) {
    if (ix < 0x24000000) {  // |x| < 2**-55
      return __x;  // tanh(small) = small
    }
    if (ix >= 0x3f800000) {  // |x| >= 1
      t = expm1f(two * fabsf(__x));
      z = one - two / (t + two);
    } else {
      t = expm1f(-two * fabsf(__x));
      z = -t / (t + two);
    }
  } else {
    z = one;  // |x| > 22, return +-1
  }
  return (jx >= 0) ? z : -z;
}
__DEVICE__ float tgammaf(float __x) throw() {
  // Not Implemented
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_UNIMPLEMENTED_API));
  return 0.0f;
}
__DEVICE__ float truncf(float __x) throw() {
  return __mlvm_scalar_cvti32_f32_up(__x);
}
__DEVICE__ float trunc(float __x) throw() {
  return ::truncf(__x);
}

// We need to define these overloads in exactly the namespace our standard
// library uses (including the right inline namespace), otherwise they won't be
// picked up by other functions in the standard library (e.g. functions in
// <complex>).  Thus the ugliness below.
#ifdef _LIBCPP_BEGIN_NAMESPACE_STD
_LIBCPP_BEGIN_NAMESPACE_STD
#else
namespace std {
#ifdef _GLIBCXX_BEGIN_NAMESPACE_VERSION
_GLIBCXX_BEGIN_NAMESPACE_VERSION
#endif
#endif

// Pull the new overloads we defined above into namespace std.
using ::acos;
using ::acosh;
using ::asin;
using ::asinh;
using ::atan;
using ::atan2;
using ::atanh;
using ::cbrt;
using ::ceil;
using ::copysign;
using ::cos;
using ::cosh;
using ::erf;
using ::erfc;
using ::exp;
using ::exp2;
using ::expm1;
using ::fabs;
using ::fdim;
using ::floor;
using ::fma;
using ::fmax;
using ::fmin;
using ::fmod;
using ::fpclassify;
using ::frexp;
using ::hypot;
using ::ilogb;
using ::isfinite;
using ::isgreater;
using ::isgreaterequal;
using ::isless;
using ::islessequal;
using ::islessgreater;
using ::isnormal;
using ::isunordered;
using ::ldexp;
using ::lgamma;
using ::llrint;
using ::log;
using ::log10;
using ::log1p;
using ::log2;
using ::logb;
using ::lrint;
using ::lround;
using ::nearbyint;
using ::nextafter;
using ::pow;
using ::remainder;
using ::remquo;
using ::rint;
using ::round;
using ::scalbln;
using ::scalbn;
using ::signbit;
using ::sin;
using ::sinh;
using ::sqrt;
using ::tan;
using ::tanh;
using ::tgamma;
using ::trunc;

// Well this is fun: We need to pull these symbols in for libc++, but we can't
// pull them in with libstdc++, because its ::isinf and ::isnan are different
// than its std::isinf and std::isnan.
#ifndef __GLIBCXX__
using ::isinf;
using ::isnan;
#endif

// Finally, pull the "foobarf" functions that BANG defines in its headers into
// namespace std.
using ::acosf;
using ::acoshf;
using ::asinf;
using ::asinhf;
using ::atan2f;
using ::atanf;
using ::atanhf;
using ::cbrtf;
using ::ceilf;
using ::copysignf;
using ::cosf;
using ::coshf;
using ::erfcf;
using ::erff;
using ::exp2f;
using ::expf;
using ::expm1f;
using ::fabsf;
using ::fdimf;
using ::floorf;
using ::fmaf;
using ::fmaxf;
using ::fminf;
using ::fmodf;
using ::frexpf;
using ::hypotf;
using ::ilogbf;
using ::ldexpf;
using ::lgammaf;
using ::llrintf;
using ::llroundf;
using ::log10f;
using ::log1pf;
using ::log2f;
using ::logbf;
using ::logf;
using ::lrintf;
using ::lroundf;
using ::modff;
using ::nearbyintf;
using ::nextafterf;
using ::nexttowardf;
using ::nexttowardf;
using ::powf;
using ::remainderf;
using ::remquof;
using ::rintf;
using ::roundf;
using ::scalblnf;
using ::scalbnf;
using ::sinf;
using ::sinhf;
using ::sqrtf;
using ::tanf;
using ::tanhf;
using ::tgammaf;
using ::truncf;

#ifdef _LIBCPP_END_NAMESPACE_STD
_LIBCPP_END_NAMESPACE_STD
#else
#ifdef _GLIBCXX_BEGIN_NAMESPACE_VERSION
_GLIBCXX_END_NAMESPACE_VERSION
#endif
} // namespace std
#endif

#pragma pop_macro("__DEVICE__")

#endif  // __BANG_ARCH__ Only included for device-side

#endif  // __CLANG_BANG_CMATH_H__
