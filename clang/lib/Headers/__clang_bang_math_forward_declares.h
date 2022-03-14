/*===- __clang_math_forward_declares.h - Prototypes of __device__ math fns --===
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
#ifndef __CLANG__BANG_MATH_FORWARD_DECLARES_H__
#define __CLANG__BANG_MATH_FORWARD_DECLARES_H__



#include <bang_device_types.h>

// This file forward-declares of some math functions we (or the BANG headers)
// will define later.  We need to do this, and do it before cmath is included,
// because the standard library may have constexpr math functions.  In the
// absence of a prior __mlu_device__ decl, those constexpr functions may become
// implicitly host+device.  host+device functions can't be overloaded, so that
// would preclude the use of our own __mlu_device__ overloads for these functions.

#pragma push_macro("__DEVICE__")
#define __DEVICE__ static __inline__ __mlu_func__ __mlu_builtin__

__DEVICE__ half abs(half) throw();
__DEVICE__ float abs(float) throw();
__DEVICE__ int abs(int) throw();
__DEVICE__ long abs(long) throw();
__DEVICE__ float acos(float) throw();
__DEVICE__ float acosf(float) throw();
__DEVICE__ float acosh(float) throw();
__DEVICE__ float acoshf(float) throw();
__DEVICE__ float asin(float) throw();
__DEVICE__ float asinf(float) throw();
__DEVICE__ float asinh(float) throw();
__DEVICE__ float asinhf(float) throw();
__DEVICE__ float atan2(float, float) throw();
__DEVICE__ float atan2f(float, float) throw();
__DEVICE__ float atan(float) throw();
__DEVICE__ float atanf(float) throw();
__DEVICE__ float atanh(float) throw();
__DEVICE__ float atanhf(float) throw();
__DEVICE__ float cbrt(float) throw();
__DEVICE__ float cbrtf(float) throw();
__DEVICE__ half ceil(half) throw();
__DEVICE__ float ceil(float) throw();
__DEVICE__ float ceilf(float) throw();
__DEVICE__ float copysign(float, float) throw();
__DEVICE__ float copysignf(float, float) throw();
__DEVICE__ float cos(float) throw();
__DEVICE__ float cosf(float) throw();
__DEVICE__ float cosh(float) throw();
__DEVICE__ float coshf(float) throw();
__DEVICE__ float erfc(float) throw();
__DEVICE__ float erfcf(float) throw();
__DEVICE__ float erf(float) throw();
__DEVICE__ float erff(float) throw();
__DEVICE__ float exp2(float) throw();
__DEVICE__ float exp2f(float) throw();
__DEVICE__ float exp(float) throw();
__DEVICE__ float expf(float) throw();
__DEVICE__ float expm1(float) throw();
__DEVICE__ float expm1f(float) throw();
__DEVICE__ float fabs(float) throw();
__DEVICE__ float fabsf(float) throw();
__DEVICE__ float fdim(float, float) throw();
__DEVICE__ float fdimf(float, float) throw();
__DEVICE__ half floor(half) throw();
__DEVICE__ float floor(float) throw();
__DEVICE__ float floorf(float) throw();
__DEVICE__ float fma(float, float, float) throw();
__DEVICE__ float fmaf(float, float, float) throw();
__DEVICE__ half fmax(half, half) throw();
__DEVICE__ float fmax(float, float) throw();
__DEVICE__ float fmaxf(float, float) throw();
__DEVICE__ half fmin(half, half) throw();
__DEVICE__ float fmin(float, float) throw();
__DEVICE__ float fminf(float, float) throw();
__DEVICE__ float fmod(float, float) throw();
__DEVICE__ float fmodf(float, float) throw();
__DEVICE__ int fpclassify(float) throw();
__DEVICE__ float frexp(float, int *) throw();
__DEVICE__ float frexpf(float, int *) throw();
__DEVICE__ float hypot(float, float) throw();
__DEVICE__ float hypotf(float, float) throw();
__DEVICE__ int ilogb(double) throw();
__DEVICE__ int ilogbf(float) throw();
__DEVICE__ bool isfinite(float) throw();
__DEVICE__ bool isgreater(float, float) throw();
__DEVICE__ bool isgreaterequal(float, float) throw();
__DEVICE__ int isinf(float) throw();
__DEVICE__ bool isless(float, float) throw();
__DEVICE__ bool islessequal(float, float) throw();
__DEVICE__ bool islessgreater(float, float) throw();
__DEVICE__ bool isnan(float) throw();
__DEVICE__ bool isnormal(float) throw();
__DEVICE__ bool isunordered(float, float) throw();
__DEVICE__ long labs(long) throw();
__DEVICE__ float ldexp(float, int) throw();
__DEVICE__ float ldexpf(float, int) throw();
__DEVICE__ float lgamma(float) throw();
__DEVICE__ float lgammaf(float) throw();
__DEVICE__ float log10(float) throw();
__DEVICE__ float log10f(float) throw();
__DEVICE__ float log1p(float) throw();
__DEVICE__ float log1pf(float) throw();
__DEVICE__ float log2(float) throw();
__DEVICE__ float log2f(float) throw();
__DEVICE__ float logb(float) throw();
__DEVICE__ float logbf(float) throw();
__DEVICE__ float log(float) throw();
__DEVICE__ float logf(float) throw();
__DEVICE__ long lrint(float) throw();
__DEVICE__ long lrintf(float) throw();
__DEVICE__ long lround(float) throw();
__DEVICE__ long lroundf(float) throw();
__DEVICE__ float modf(float, float *) throw();
__DEVICE__ float modff(float, float *) throw();
__DEVICE__ float nan(const char *) throw();
__DEVICE__ float nanf(const char *) throw();
__DEVICE__ float nearbyint(float) throw();
__DEVICE__ float nearbyintf(float) throw();
__DEVICE__ float nextafter(float, float) throw();
__DEVICE__ float nextafterf(float, float) throw();
__DEVICE__ float nexttoward(float, double) throw();
__DEVICE__ float nexttowardf(float, double) throw();
__DEVICE__ float pow(float, float) throw();
__DEVICE__ float powf(float, float) throw();
__DEVICE__ float remainder(float, float) throw();
__DEVICE__ float remainderf(float, float) throw();
__DEVICE__ float remquo(float, float, int *) throw();
__DEVICE__ float remquof(float, float, int *) throw();
__DEVICE__ float rint(float) throw();
__DEVICE__ float rintf(float) throw();
__DEVICE__ half round(half) throw();
__DEVICE__ float round(float) throw();
__DEVICE__ float roundf(float) throw();
__DEVICE__ float scalbln(float, long) throw();
__DEVICE__ float scalblnf(float, long) throw();
__DEVICE__ float scalbn(float, int) throw();
__DEVICE__ float scalbnf(float, int) throw();
__DEVICE__ bool signbit(float) throw();
__DEVICE__ float sin(float) throw();
__DEVICE__ float sinf(float) throw();
__DEVICE__ float sinh(float) throw();
__DEVICE__ float sinhf(float) throw();
__DEVICE__ float sqrt(float) throw();
__DEVICE__ float sqrtf(float) throw();
__DEVICE__ float tan(float) throw();
__DEVICE__ float tanf(float) throw();
__DEVICE__ float tanh(float) throw();
__DEVICE__ float tanhf(float) throw();
__DEVICE__ float tgamma(float) throw();
__DEVICE__ float tgammaf(float) throw();
__DEVICE__ float trunc(float) throw();
__DEVICE__ float truncf(float) throw();

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

using ::abs;
using ::acos;
using ::acosf;
using ::acosh;
using ::acoshf;
using ::asin;
using ::asinf;
using ::asinh;
using ::asinhf;
using ::atan;
using ::atanf;
using ::atan2;
using ::atan2f;
using ::atanh;
using ::atanhf;
using ::cbrt;
using ::cbrtf;
using ::ceil;
using ::ceilf;
using ::copysign;
using ::copysignf;
using ::cos;
using ::cosf;
using ::cosh;
using ::coshf;
using ::erf;
using ::erff;
using ::erfc;
using ::erfcf;
using ::exp;
using ::expf;
using ::exp2;
using ::exp2f;
using ::expm1;
using ::expm1f;
using ::fabs;
using ::fabsf;
using ::fdim;
using ::fdimf;
using ::floor;
using ::floorf;
using ::fma;
using ::fmaf;
using ::fmax;
using ::fmaxf;
using ::fmin;
using ::fminf;
using ::fmod;
using ::fmodf;
using ::fpclassify;
using ::frexp;
using ::frexpf;
using ::hypot;
using ::hypotf;
using ::ilogb;
using ::ilogbf;
using ::isfinite;
using ::isgreater;
using ::isgreaterequal;
using ::isinf;
using ::isless;
using ::islessequal;
using ::islessgreater;
using ::isnan;
using ::isnormal;
using ::isunordered;
using ::labs;
using ::ldexp;
using ::ldexpf;
using ::lgamma;
using ::lgammaf;
using ::log;
using ::logf;
using ::log10;
using ::log10f;
using ::log1p;
using ::log1pf;
using ::log2;
using ::log2f;
using ::logb;
using ::logbf;
using ::lrint;
using ::lrintf;
using ::lround;
using ::lroundf;
using ::modf;
using ::modff;
using ::nan;
using ::nanf;
using ::nearbyint;
using ::nearbyintf;
using ::nextafter;
using ::nextafterf;
using ::nexttoward;
using ::nexttowardf;
using ::pow;
using ::powf;
using ::remainder;
using ::remainderf;
using ::remquo;
using ::remquof;
using ::rint;
using ::rintf;
using ::round;
using ::roundf;
using ::scalbln;
using ::scalbn;
using ::signbit;
using ::sin;
using ::sinf;
using ::sinh;
using ::sinhf;
using ::sqrt;
using ::sqrtf;
using ::tan;
using ::tanf;
using ::tanh;
using ::tanhf;
using ::tgamma;
using ::tgammaf;
using ::trunc;
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

#endif
