/*===---- __clang_bang_runtime_wrapper.h - MLU runtime support -------------===
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
#ifndef __CLANG_BANG_RUNTIME_WRAPPER_H__
#define __CLANG_BANG_RUNTIME_WRAPPER_H__

#if defined(__BANG__) && defined(__clang__)

// Include some forward declares that must come before cmath.
#include <__clang_bang_math_forward_declares.h>

// Include some standard headers to avoid BANG headers including them
// while some required macros (like __THROW) are in a weird state.
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// Preserve common macros that will be changed below by us or by BANG
// headers.
#pragma push_macro("__THROW")
#pragma push_macro("__BANG_ARCH__")

#include <bang.h>
#if !defined(BANG_VERSION)
#error "bang.h did not define BANG_VERSION"
#elif BANG_VERSION < 100 || BANG_VERSION > 300
#error "Unsupported BANG version!"
#endif

// Make largest subset of device functions available during host
// compilation -- BANG 2.0 for the time being.
#ifndef __BANG_ARCH__
#define __BANG_ARCH__ 200
#endif

#include "__clang_bang_builtin_vars.h"

// Restore state of __BANG_ARCH__ and __THROW we had on entry.
#pragma pop_macro("__BANG_ARCH__")
#pragma pop_macro("__THROW")

#include <__clang_bang_cmath.h>
#include <__clang_bang_intrinsics.h>

// The code defines __mlu_device__ overloads of std::min/max, but only if we're
// <= C++11.  In C++14, these functions are constexpr, and so are implicitly
// __mlu_host__ __mlu_device__.
//
// We don't support the initializer_list overloads because
// initializer_list::begin() and end() are not __mlu_host__ __mlu_device__
// functions.
//
// When compiling in C++14 mode, we could force std::min/max to have different
// implementations for host and device, by declaring the device overloads
// before the constexpr overloads appear.  We choose not to do this because

//  a) why write our own implementation when we can use one from the standard
//     library? and
//  b) libstdc++ is evil and declares min/max inside a header that is included
//     *before* we include <algorithm>.  So we'd have to unconditionally
//     declare our __mlu_device__ overloads of min/max, but that would pollute
//     things for people who choose not to include <algorithm>.

#ifdef _LIBCPP_BEGIN_NAMESPACE_STD
_LIBCPP_BEGIN_NAMESPACE_STD
#else
namespace std {
#ifdef _GLIBCXX_BEGIN_NAMESPACE_VERSION
_GLIBCXX_BEGIN_NAMESPACE_VERSION
#endif
#endif

static float max(double __a, double __b) throw() {
#if __BANG_ARCH__ < 200
  return 0.0f;
#else
  return (float)__a > (float)__b ? (float)__a : (float)__b;
#endif
}

static float min(double __a, double __b) throw() {
#if __BANG_ARCH__ < 200
  return 0.0f;
#else
  return (float)__a < (float)__b ? (float)__a : (float)__b;
#endif
}

static float max(double __a, float __b) throw() {
#if __BANG_ARCH__ < 200
  return 0.0f;
#else
  return (float)__a > __b ? (float)__a : __b;
#endif
}

static float min(double __a, float __b) throw() {
#if __BANG_ARCH__ < 200
  return 0.0f;
#else
  return (float)__a < __b ? (float)__a : __b;
#endif
}

static float max(float __a, double __b) throw() {
#if __BANG_ARCH__ < 200
  return 0.0f;
#else
  return __a > (float)__b ? __a : (float)__b;
#endif
}

static float min(float __a, double __b) throw() {
#if __BANG_ARCH__ < 200
  return 0.0f;
#else
  return __a < (float)__b ? __a : (float)__b;
#endif
}

static half max(half __a, double __b) throw() {
#if __BANG_ARCH__ < 200
  return 0.0f;
#else
  return __a > (half)__b ? __a : (half)__b;
#endif
}

static half min(half __a, double __b) throw() {
#if __BANG_ARCH__ < 200
  return 0.0f;
#else
  return __a < (half)__b ? __a : (half)__b;
#endif
}

static half max(half __a, float __b) throw() {
#if __BANG_ARCH__ < 200
  return 0.0f;
#else
  return __a > (half)__b ? __a : (half)__b;
#endif
}

static half min(half __a, float __b) throw() {
#if __BANG_ARCH__ < 200
  return 0.0f;
#else
  return __a < (half)__b ? __a : (half)__b;
#endif
}

#ifdef _LIBCPP_END_NAMESPACE_STD
_LIBCPP_END_NAMESPACE_STD
#else
#ifdef _GLIBCXX_BEGIN_NAMESPACE_VERSION
_GLIBCXX_END_NAMESPACE_VERSION
#endif
} // namespace std
#endif

#if __cplusplus <= 201103L

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

template <class __T, class __Cmp>
inline __mlu_func__ __mlu_builtin__ const __T &
max(const __T &__a, const __T &__b, __Cmp __cmp) {
  return __cmp(__a, __b) ? __b : __a;
}

template <class __T>
inline __mlu_func__ __mlu_builtin__ const __T &
max(const __T &__a, const __T &__b) {
  return __a < __b ? __b : __a;
}

template <class __T, class __Cmp>
inline __mlu_func__ __mlu_builtin__ const __T &
min(const __T &__a, const __T &__b, __Cmp __cmp) {
  return __cmp(__b, __a) ? __b : __a;
}

template <class __T>
inline __mlu_func__ __mlu_builtin__ const __T &
min(const __T &__a, const __T &__b) {
  return __a < __b ? __a : __b;
}

#ifdef _LIBCPP_END_NAMESPACE_STD
_LIBCPP_END_NAMESPACE_STD
#else
#ifdef _GLIBCXX_BEGIN_NAMESPACE_VERSION
_GLIBCXX_END_NAMESPACE_VERSION
#endif
} // namespace std
#endif

#endif // __cplusplus <= 201103L

#endif  //  defined(__BANG__) && defined(__clang__)

#endif  // __CLANG_BANG_RUNTIME_WRAPPER_H__
