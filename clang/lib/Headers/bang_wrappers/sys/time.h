/*===---------------- sys/time.h - Standard header for BANG ----------------===
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
#ifndef __CLANG_BANG_WRAPPERS_SYS_TIME_H_
#define __CLANG_BANG_WRAPPERS_SYS_TIME_H_

// Compatible with GNUC 4.x.x
#ifdef __BANG_ARCH__
#ifndef __USE_MISC
#define __USE_MISC
#endif  // __USE_MISC
#endif  // __BANG_ARCH__

#include_next <sys/time.h>

#include <bang_device_types.h>

// -----------------------------------------------------------------------------
// gettimeofday - get time, not support get timezone
// -----------------------------------------------------------------------------
#if __BANG_ARCH__ >= 200
static __mlu_func__ __mlu_builtin__ int gettimeofday(struct timeval *__restrict __tv,
                                                     struct timezone *__restrict __tz) {
  unsigned int time_low = 0;
  unsigned int time_high = 0;
  unsigned int time_usec = 0;
  unsigned int time_sec = 0;
  __asm__ volatile("sync;\n\t");
#if __BANG_ARCH__ < 300
  __asm__ volatile("mv.sreg.gpr %%perf_read, 1;\n\t"
                   ::);
#endif
  __asm__ volatile("mv.gpr.sreg %[time_low], %%perf_time_stample_low;\n\t"
                   "mv.gpr.sreg %[time_high], %%perf_time_stample_high;\n\t"
                   "sll.gpr.u48 %[time_high], %[time_high], 32;\n\t"
                   "add.gpr.u48 %[time_usec], %[time_high], %[time_low];\n\t"
                   "add.gpr.u48 %[time_sec], %[time_high], %[time_low];\n\t"
                   // calc tv_usec
                   "rem.gpr.u32 %[time_low], %[time_usec], 25000000;\n\t"
                   "div.gpr.u32 %[time_usec], %[time_low], 25;\n\t"
                   // calc tv_sec
                   "sub.gpr.u48 %[time_sec], %[time_sec], %[time_low];\n\t"
                   "srl.gpr.u48 %[time_sec], %[time_sec], 16;\n\t"
                   "div.gpr.u32 %[time_sec], %[time_sec], 382;\n\t"
                   :[time_sec]"+&r"(time_sec),
                    [time_usec]"+&r"(time_usec),
                    [time_low]"+&r"(time_low),
                    [time_high]"+&r"(time_high)
                   );
  (__tv)->tv_usec = time_usec;
  (__tv)->tv_sec = time_sec;
  return 0;
}
#else  // __BANG_ARCH__ >= 200
static __mlu_func__ __mlu_builtin__ int gettimeofday(struct timeval *__restrict __tv,
                                                     struct timezone *__restrict __tz) {
  // BANG v1.x Not Supported.
  asm volatile("exit %[status];\n\t"
               ::[status]"i"(TRAP_BANG_V1_UNSUPPORTED));
  return -1;
}
#endif  // __BANG_ARCH__ >= 200

#endif  // __CLANG_BANG_WRAPPERS_SYS_TIME_H_
