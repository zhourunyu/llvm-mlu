/*===------------------ stdlib.h - Standard header for BANG -----------------===
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
#ifndef __CLANG_BANG_WRAPPERS_STDLIB_H_
#define __CLANG_BANG_WRAPPERS_STDLIB_H_

// Avoid some stdint types redefinition for aarch64 cross-compilation.
#if defined(__aarch64__)
#define _BITS_STDINT_INTN_H
#endif
#include_next <stdlib.h>

#include <bang_device_types.h>

// -----------------------------------------------------------------------------
// exit - cause normal process termination
// -----------------------------------------------------------------------------
static __mlu_func__ __mlu_builtin__ void exit(int status) {
  asm("exit %[status];\n\t"
      ::[status]"ri"(status));
}

// -----------------------------------------------------------------------------
// abort - cause abnormal process termination
// -----------------------------------------------------------------------------
static __mlu_func__ __mlu_builtin__ void abort() { exit(-1); }

// -----------------------------------------------------------------------------
// abs - compute the absolute value of an integer
// -----------------------------------------------------------------------------
static __mlu_func__ __mlu_builtin__ int abs(int __x) throw() {
  return __mlvm_scalar_abs_s32(__x);
}

static __mlu_func__ __mlu_builtin__ short abs(short __x) throw() {
  return __mlvm_scalar_abs_s16(__x);
}

#endif  // __CLANG_BANG_WRAPPERS_STDLIB_H_
