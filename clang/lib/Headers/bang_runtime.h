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
#ifndef __BANG_RUNTIME_H__
#define __BANG_RUNTIME_H__

#ifndef __BANG__
#error "This file is for BANG compilation only."
#endif  // __BANG__

#include "cnrt.h"

#if !defined(CNRT_VERSION)
#error "This file needs to include $NEUWARE_HOME/include/cnrt.h."
#endif  // !defined(CNRT_VERSION)

// Current cncc version dependency check
#if __BANG_ARCH__ < 200
// CNRT v4.x not support BANG v1.x, but CNCC v2.x is compatible with BANG v1.x
#if (CNRT_VERSION < 31300)
#error "Current cncc requires cnrt version >= 3.13.0."
#endif
#endif  // __BANG_ARCH__ < 200
#if __BANG_ARCH__ > 200
// BANG v2.x with CNRT < v4.2.1 not support <<<>>> launch kernel.
#if (CNRT_VERSION < 40900)
#error "Current cncc requires cnrt version >= 4.9.0."
#endif
#endif  // __BANG_ARCH__ > 200

// -----------------------------------------------------------------------------
// BANG C/C++ runtime function: launch kernel with syntax foo<<<>>>()
// -----------------------------------------------------------------------------

extern "C" {
cnrtRet_t bangSetupArgument(const void *arg,
                            size_t size,
                            size_t offset);

cnrtRet_t bangConfigureCall(cnrtDim3_t dim,
                            cnrtFunctionType_t ktype,
                            cnrtQueue_t queue);

cnrtRet_t bangLaunch(const void *func);

void __bangRegisterFunction(void **handle,
                            const char *funptr,
                            char *__device_fun,
                            const char *fun,
                            int v1,
                            void* i1,
                            void* i2,
                            cnrtDim3_t* d1,
                            cnrtDim3_t* d2,
                            int* v2);

void ** __bangRegisterFatBinary(void *fatbin);

void __bangUnregisterFatBinary(void ** handle);

void __bangRegisterVar(void **handle,
                       char *hostVar,
                       char *deviceAddress,
                       const char *deviceName,
                       int ext,
                       int size,
                       int constant,
                       int global);

uint16_t __gnu_f2h_ieee(float param);

float __gnu_h2f_ieee(uint16_t param);
}

#endif  // __BANG_RUNTIME_H__
