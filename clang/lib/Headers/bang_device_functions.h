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
#ifndef  __BANG_DEVICE_FUNCTIONS_H__
#define  __BANG_DEVICE_FUNCTIONS_H__

#ifndef __BANG__
#error "This file is for BANG compilation only."
#endif  // __BANG__

#include <bang_device_types.h>

#ifndef __CLANG_BANG_BUILTIN_VARS_H__
#include <__clang_bang_builtin_vars.h>
#endif  // __CLANG_BANG_BUILTIN_VARS_H__

#ifndef __CLANG_BANG_RUNTIME_WRAPPER_H__
#include <__clang_bang_runtime_wrapper.h>
#endif  // __CLANG_BANG_RUNTIME_WRAPPER_H__

// -----------------------------------------------------------------------------
// macro: __BARRIER, only for BANG 1.0
// -----------------------------------------------------------------------------
#define __BARRIER(count) __asm__ __volatile__("barrier %0;" : : "i"(count))

// -----------------------------------------------------------------------------
// macro: __breakdump_scalar and __breakdump_vector
// -----------------------------------------------------------------------------
#define __BUILTIN_BREAKDUMP1(a)                                         \
  ({ __asm__ volatile("breakdump.gpr \t%[src];\n\t" : : [src] "r"(a)); })
#define __BUILTIN_BREAKDUMP2(a, b)                            \
  ({                                                          \
    __asm__ volatile("breakdump.gpr \t%[src0], %[src1];\n\t"  \
                     :                                        \
                     : [src0] "r"(a), [src1] "r"(b));         \
  })
#define __BUILTIN_BREAKDUMP3(a, b, c)                                 \
  ({                                                                  \
    __asm__ volatile("breakdump.gpr \t%[src0], %[src1], %[src2];\n\t" \
                     :                                                \
                     : [src0] "r"(a), [src1] "r"(b), [src2] "r"(c));  \
  })
#define __BUILTIN_BREAKDUMP4(a, b, c, d)                                \
  ({                                                                    \
    __asm__ volatile("breakdump.gpr \t%[src0], %[src1], %[src2], %[src3];\n\t" \
                     :                                                  \
                     : [src0] "r"(a), [src1] "r"(b), [src2] "r"(c),     \
                       [src3] "r"(d));                                  \
  })
#define __BUILTIN_BREAKDUMP5(a, b, c, d, e)                             \
  ({                                                                    \
    __asm__ volatile(                                                   \
                     "breakdump.gpr \t%[src0], %[src1], %[src2], %[src3], %[src4];\n\t" \
                     :                                                  \
                     : [src0] "r"(a), [src1] "r"(b), [src2] "r"(c), [src3] "r"(d), \
                       [src4] "r"(e));                                  \
  })
#define __BUILTIN_BREAKDUMP6(a, b, c, d, e, f)                          \
  ({                                                                    \
    __asm__ volatile(                                                   \
                     "breakdump.gpr \t%[src0], %[src1], %[src2], %[src3], " \
                     "%[src4], %[src5];\n\t"                            \
                     :                                                  \
                     : [src0] "r"(a), [src1] "r"(b), [src2] "r"(c), [src3] "r"(d), \
                       [src4] "r"(e), [src5] "r"(f));                   \
  })
#define GetMacro(_1, _2, _3, _4, _5, _6, NAME, ...) NAME
#define __breakdump_scalar(...)                                         \
  GetMacro(__VA_ARGS__, __BUILTIN_BREAKDUMP6, __BUILTIN_BREAKDUMP5,     \
           __BUILTIN_BREAKDUMP4, __BUILTIN_BREAKDUMP3, __BUILTIN_BREAKDUMP2, \
           __BUILTIN_BREAKDUMP1, ...)(__VA_ARGS__)
#define __breakdump_vector(src, Bytes, AddrSpace) \
  __breakdump_vector_##AddrSpace(src, Bytes)
#define __breakdump_vector_NRAM(src, Bytes)       \
  ({ __mlvm_stream_breakdump_nram(src, Bytes); })

// -----------------------------------------------------------------------------
// macro: __bang_printf
// -----------------------------------------------------------------------------
#define __BANG_PRINTF0(f)                                                      \
  ({                                                                           \
    __asm__ volatile("dumpprintf [%[fmt]];\n\t"                                \
                     ::[fmt]"p"(f));                                           \
  })

#define __BANG_PRINTF1(f, a)                                                   \
  ({                                                                           \
    __asm__ volatile("dumpprintf [%[fmt]], %[src1];\n\t"                       \
                     ::[fmt]"p"(f), [src1]"r"(a));                             \
  })

#define __BANG_PRINTF2(f, a, b)                                                \
  ({                                                                           \
    __asm__ volatile("dumpprintf [%[fmt]], %[src1], %[src2];\n\t"              \
                     ::[fmt]"p"(f), [src1]"r"(a), [src2]"r"(b));               \
  })

#define __BANG_PRINTF3(f, a, b, c)                                             \
  ({                                                                           \
    __asm__ volatile("dumpprintf [%[fmt]], %[src1], %[src2], %[src3];\n\t"     \
                     ::[fmt]"p"(f), [src1]"r"(a), [src2]"r"(b), [src3]"r"(c)); \
  })

#define __BANG_PRINTF4(f, a, b, c, d)                                          \
  ({                                                                           \
    __asm__ volatile("dumpprintf [%[fmt]], %[src1], %[src2], %[src3]"          \
                     ", %[src4];\n\t"                                          \
                     ::[fmt]"p"(f), [src1]"r"(a), [src2]"r"(b),                \
                      [src3]"r"(c), [src4]"r"(d));                             \
  })

#define __BANG_PRINTF5(f, a, b, c, d, e)                                       \
  ({                                                                           \
    __asm__ volatile("dumpprintf [%[fmt]], %[src1], %[src2], %[src3], "        \
                     "%[src4], %[src5];\n\t"                                   \
                     ::[fmt]"p"(f), [src1]"r"(a), [src2]"r"(b),                \
                      [src3]"r"(c), [src4]"r"(d), [src5]"r"(e));               \
  })

#define __BANG_PRINTF6(f, a, b, c, d, e, g)                                    \
  ({                                                                           \
    __asm__ volatile("dumpprintf [%[fmt]], %[src1], %[src2], %[src3], "        \
                     "%[src4], %[src5], %[src6];\n\t"                          \
                     ::[fmt]"p"(f), [src1]"r"(a), [src2]"r"(b),                \
                      [src3]"r"(c), [src4]"r"(d), [src5]"r"(e),                \
                      [src6]"r"(g));                                           \
  })

#define __BANG_PRINTF7(f, a, b, c, d, e, g, h)                                 \
  ({                                                                           \
    __asm__ volatile("dumpprintf [%[fmt]], %[src1], %[src2], %[src3], "        \
                     "%[src4], %[src5], %[src6], %[src7];\n\t"                 \
                     ::[fmt]"p"(f), [src1]"r"(a), [src2]"r"(b),                \
                      [src3]"r"(c), [src4]"r"(d), [src5]"r"(e),                \
                      [src6]"r"(g), [src7]"r"(h));                             \
  })

#define __BANG_PRINTF8(f, a, b, c, d, e, g, h, i)                              \
  ({                                                                           \
    __asm__ volatile("dumpprintf [%[fmt]], %[src1], %[src2], %[src3], "        \
                     "%[src4], %[src5], %[src6], %[src7], %[src8]"             \
                     ";\n\t"                                                   \
                     ::[fmt]"p"(f), [src1]"r"(a), [src2]"r"(b),                \
                      [src3]"r"(c), [src4]"r"(d), [src5]"r"(e),                \
                      [src6]"r"(g), [src7]"r"(h), [src8]"r"(i));               \
  })

#define __BANG_PRINTF9(f, a, b, c, d, e, g, h, i, j)                           \
  ({                                                                           \
    __asm__ volatile("dumpprintf [%[fmt]], %[src1], %[src2], %[src3], "        \
                     "%[src4], %[src5], %[src6], %[src7], %[src8], "           \
                     "%[src9];\n\t"                                            \
                     ::[fmt]"p"(f), [src1]"r"(a), [src2]"r"(b),                \
                      [src3]"r"(c), [src4]"r"(d), [src5]"r"(e),                \
                      [src6]"r"(g), [src7]"r"(h), [src8]"r"(i),                \
                      [src9]"r"(j));                                           \
  })

#define __BANG_PRINTF10(f, a, b, c, d, e, g, h, i, j, k)                       \
  ({                                                                           \
    __asm__ volatile("dumpprintf [%[fmt]], %[src1], %[src2], %[src3], "        \
                     "%[src4], %[src5], %[src6], %[src7], %[src8], "           \
                     "%[src9], %[src10];\n\t"                                  \
                     ::[fmt]"p"(f), [src1]"r"(a), [src2]"r"(b),                \
                      [src3]"r"(c), [src4]"r"(d), [src5]"r"(e),                \
                      [src6]"r"(g), [src7]"r"(h), [src8]"r"(i),                \
                      [src9]"r"(j), [src10]"r"(k));                            \
  })

#define __BANG_PRINTF11(f, a, b, c, d, e, g, h, i, j, k, l)                    \
  ({                                                                           \
    __asm__ volatile("dumpprintf [%[fmt]], %[src1], %[src2], %[src3], "        \
                     "%[src4], %[src5], %[src6], %[src7], %[src8], "           \
                     "%[src9], %[src10], %[src11];\n\t"                        \
                     ::[fmt]"p"(f), [src1]"r"(a), [src2]"r"(b),                \
                      [src3]"r"(c), [src4]"r"(d), [src5]"r"(e),                \
                      [src6]"r"(g), [src7]"r"(h), [src8]"r"(i),                \
                      [src9]"r"(j), [src10]"r"(k), [src11]"r"(l));             \
  })

#define __BANG_PRINTF12(f, a, b, c, d, e, g, h, i, j, k, l, m)                 \
  ({                                                                           \
    __asm__ volatile("dumpprintf [%[fmt]], %[src1], %[src2], %[src3], "        \
                     "%[src4], %[src5], %[src6], %[src7], %[src8], "           \
                     "%[src9], %[src10], %[src11], %[src12];\n\t"              \
                     ::[fmt]"p"(f), [src1]"r"(a), [src2]"r"(b),                \
                      [src3]"r"(c), [src4]"r"(d), [src5]"r"(e),                \
                      [src6]"r"(g), [src7]"r"(h), [src8]"r"(i),                \
                      [src9]"r"(j), [src10]"r"(k), [src11]"r"(l),              \
                      [src12]"r"(m));                                          \
  })

#define __BANG_PRINTF13(f, a, b, c, d, e, g, h, i, j, k, l, m, n)              \
  ({                                                                           \
    __asm__ volatile("dumpprintf [%[fmt]], %[src1], %[src2], %[src3], "        \
                     "%[src4], %[src5], %[src6], %[src7], %[src8], "           \
                     "%[src9], %[src10], %[src11], %[src12], %[src13];\n\t"    \
                     ::[fmt]"p"(f), [src1]"r"(a), [src2]"r"(b),                \
                      [src3]"r"(c), [src4]"r"(d), [src5]"r"(e),                \
                      [src6]"r"(g), [src7]"r"(h), [src8]"r"(i),                \
                      [src9]"r"(j), [src10]"r"(k), [src11]"r"(l),              \
                      [src12]"r"(m), [src13]"r"(n));                           \
  })

#define __BANG_PRINTF14(f, a, b, c, d, e, g, h, i, j, k, l, m, n, o)           \
  ({                                                                           \
    __asm__ volatile("dumpprintf [%[fmt]], %[src1], %[src2], %[src3], "        \
                     "%[src4], %[src5], %[src6], %[src7], %[src8], "           \
                     "%[src9], %[src10], %[src11], %[src12], %[src13], "       \
                     "%[src14];\n\t"                                           \
                     ::[fmt]"p"(f), [src1]"r"(a), [src2]"r"(b),                \
                      [src3]"r"(c), [src4]"r"(d), [src5]"r"(e),                \
                      [src6]"r"(g), [src7]"r"(h), [src8]"r"(i),                \
                      [src9]"r"(j), [src10]"r"(k), [src11]"r"(l),              \
                      [src12]"r"(m), [src13]"r"(n), [src14]"r"(o));            \
  })

#define __BANG_PRINTF15(f, a, b, c, d, e, g, h, i, j, k, l, m, n, o, p)        \
  ({                                                                           \
    __asm__ volatile("dumpprintf [%[fmt]], %[src1], %[src2], %[src3], "        \
                     "%[src4], %[src5], %[src6], %[src7], %[src8], "           \
                     "%[src9], %[src10], %[src11], %[src12], %[src13], "       \
                     "%[src14], %[src15];\n\t"                                 \
                     ::[fmt]"p"(f), [src1]"r"(a), [src2]"r"(b),                \
                      [src3]"r"(c), [src4]"r"(d), [src5]"r"(e),                \
                      [src6]"r"(g), [src7]"r"(h), [src8]"r"(i),                \
                      [src9]"r"(j), [src10]"r"(k), [src11]"r"(l),              \
                      [src12]"r"(m), [src13]"r"(n), [src14]"r"(o),             \
                      [src15]"r"(p));                                          \
  })

#define __BANG_PRINTF16(f, a, b, c, d, e, g, h, i, j, k, l, m, n, o, p, q)     \
  ({                                                                           \
    __asm__ volatile("dumpprintf [%[fmt]], %[src1], %[src2], %[src3], "        \
                     "%[src4], %[src5], %[src6], %[src7], %[src8], "           \
                     "%[src9], %[src10], %[src11], %[src12], %[src13], "       \
                     "%[src14], %[src15], %[src16];\n\t"                       \
                     ::[fmt]"p"(f), [src1]"r"(a), [src2]"r"(b),                \
                      [src3]"r"(c), [src4]"r"(d), [src5]"r"(e),                \
                      [src6]"r"(g), [src7]"r"(h), [src8]"r"(i),                \
                      [src9]"r"(j), [src10]"r"(k), [src11]"r"(l),              \
                      [src12]"r"(m), [src13]"r"(n), [src14]"r"(o),             \
                      [src15]"r"(p), [src16]"r"(q));                           \
  })

#define PrintfMacro(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10,                   \
                    _11, _12, _13, _14, _15, _16, _17,  NAME, ...) NAME

#define __bang_printf(...)                                                     \
  PrintfMacro(__VA_ARGS__, __BANG_PRINTF16, __BANG_PRINTF15,                   \
              __BANG_PRINTF14, __BANG_PRINTF13, __BANG_PRINTF12,               \
              __BANG_PRINTF11, __BANG_PRINTF10, __BANG_PRINTF9,                \
              __BANG_PRINTF8, __BANG_PRINTF7, __BANG_PRINTF6,                  \
              __BANG_PRINTF5, __BANG_PRINTF4, __BANG_PRINTF3,                  \
              __BANG_PRINTF2, __BANG_PRINTF1, __BANG_PRINTF0, ...)(__VA_ARGS__)

// -----------------------------------------------------------------------------
// assert - abort the program if assertion is false
// -----------------------------------------------------------------------------
static __mlu_func__ __mlu_builtin__ void __assert_fail(const char *__message,
                                                       const char *__file,
                                                       unsigned int __line,
                                                       const char *__function) {
  __bang_printf("%s:%d: %s: BANG Assertion \'%s\' failed.\n",
                __file, __line, __function, __message);
  asm("exit -1;\n\t");
}

static __mlu_func__ __mlu_builtin__ void __assert(const char *__assertion,
                                                  const char *__file,
                                                  int __line) {
  __bang_printf("%s:%d: BANG Assertion \'%s\'.\n",
                __file, __line, __assertion);
}

// -----------------------------------------------------------------------------
// isXSpace - check the pointer's address space
// -----------------------------------------------------------------------------
static __mlu_func__ __mlu_builtin__ bool __is_nram(const void *ptr) {
#if (__BANG_ARCH__ < 300) || (__BANG_ARCH__ == 392)
  // 512KB
  return (ptr >= (void*)0x200000U) && (ptr <= (void*)0x27FFFFU);
#elif (__BANG_ARCH__ == 320) || (__BANG_ARCH__ == 321)
  // 1280KB
  return (ptr >= (void*)0x100000U) && (ptr < (void*)0x240000U);
#elif (__BANG_ARCH__ == 303)
  // 384KB
  return (ptr >= (void*)0x200000U) && (ptr < (void*)0x260000U);
#elif (__BANG_ARCH__ == 306)
  // 640KB
  return (ptr >= (void*)0x200000U) && (ptr < (void*)0x2A0000U);
#else
  // 768KB
  return (ptr >= (void*)0x200000U) && (ptr < (void*)0x2C0000U);
#endif
}
static __mlu_func__ __mlu_builtin__ bool __is_wram(const void *ptr) {
#if (__BANG_ARCH__ == 303) || (__BANG_ARCH__ == 306)
  return false;
#elif (__BANG_ARCH__ == 120)
  // 256KB
  return (ptr >= (void*)0x300000U) && (ptr < (void*)0x340000U);
#elif (__BANG_ARCH__ == 100) || (__BANG_ARCH__ == 270) || (__BANG_ARCH__ == 322) || (__BANG_ARCH == 372)
  // 1MB
  return (ptr >= (void*)0x300000U) && (ptr <= (void*)0x3FFFFFU);
#elif (__BANG_ARCH__ == 320) || (__BANG_ARCH__ == 321)
  // 688KB
  return (ptr >= (void*)0x300000U) && (ptr < (void*)0x3AC000U);
#else
  // 512KB
  return (ptr >= (void*)0x300000U) && (ptr <= (void*)0x37FFFFU);
#endif
}
static __mlu_func__ __mlu_builtin__ bool __is_sram(const void *ptr) {
#if (__BANG_ARCH__ < 200) || (__BANG_ARCH__ == 303) || (__BANG_ARCH__ == 306) || (__BANG_ARCH__ == 320) || (__BANG_ARCH__ == 321)
  return false;
#elif __BANG_ARCH__ == 322
  // 1536KB
  return (ptr >= (void*)0x400000U) && (ptr < (void*)0x580000U);
#elif __BANG_ARCH__ == 372
  // 4MB
  return (ptr >= (void*)0x400000U) && (ptr <= (void*)0x7FFFFFU);
#else
  // 2MB
  return (ptr >= (void*)0x400000U) && (ptr <= (void*)0x5FFFFFU);
#endif
}

// -----------------------------------------------------------------------------
// isXPUCore - check current thread if running on IPU or MPU
// -----------------------------------------------------------------------------
#if (__BANG_ARCH__ != 303) && (__BANG_ARCH__ != 306)
static __mlu_func__ __mlu_builtin__ bool __is_ipu() {
  return coreId != 0x80;
}
static __mlu_func__ __mlu_builtin__ bool __is_mpu() {
  return coreId == 0x80;
}
#endif

// -----------------------------------------------------------------------------
// Bang C/C++ builtin function: pv
// -----------------------------------------------------------------------------
#if __BANG_ARCH__ >= 200
static __mlu_func__ __mlu_builtin__ void __bang_lock(int lock_id_0, int lock_id_1) {
  __asm__ __volatile__("pv.p.dma0_sync.dma1_sync %[id0], %[id1];\n\t"
                       ::[id0]"r"(lock_id_0),[id1]"r"(lock_id_1));
}

static __mlu_func__ __mlu_builtin__ void __bang_unlock(int lock_id_0, int lock_id_1) {
  __asm__ __volatile__("pv.v.dma0_sync.dma1_sync %[id0], %[id1];\n\t"
                       ::[id0]"r"(lock_id_0),[id1]"r"(lock_id_1));
}
#endif  // __BANG_ARCH__ >= 200

// -----------------------------------------------------------------------------
// Bang C/C++ builtin function: sync all Intelligent Process Unit
// -----------------------------------------------------------------------------
#if __BANG_ARCH__ >= 320 && __BANG_ARCH__ <= 322
static __mlu_func__ __mlu_builtin__ void __sync_all_ipu() {
  if (coreId != 0x80) {
    __asm__ __volatile__("sync;\n\t");
  }
}
#elif __BANG_ARCH__ >= 200
static __mlu_func__ __mlu_builtin__ void __sync_all_ipu() {
  if (coreId != 0x80) {
    if (clusterDim == 0) {
      __asm__ __volatile__("sync;\n\t");
    } else if (clusterDim == 1) {
      __asm__ __volatile__("barrier.sync.local 32768, %[cnt];\n\t"
                           ::[cnt]"r"(coreDim));
    } else {
      __asm__ __volatile__("barrier.sync.global 32769, %[cnt];\n\t"
                           ::[cnt]"r"(clusterDim * coreDim));
    }
  }
}
#endif  // __BANG_ARCH__ >= 320 && __BANG_ARCH__ <= 322

// -----------------------------------------------------------------------------
// Bang C/C++ builtin function: sync all Memory Process Unit
// -----------------------------------------------------------------------------
#if __BANG_ARCH__ >= 320 && __BANG_ARCH__ <= 322
static __mlu_func__ __mlu_builtin__ void __sync_all_mpu() {
  if (coreId == 0x80) {
    __asm__ __volatile__("sync;\n\t");
  }
}
#elif __BANG_ARCH__ >= 200 && __BANG_ARCH__ != 303 && __BANG_ARCH__ != 306
static __mlu_func__ __mlu_builtin__ void __sync_all_mpu() {
  if (coreId == 0x80) {
    if (clusterDim == 0 || clusterDim == 1) {
      __asm__ __volatile__("sync;\n\t");
    } else {
      __asm__ __volatile__("barrier.sync.global 32770, %[cnt];\n\t"
                           ::[cnt]"r"(clusterDim));
    }
  }
}
#endif  // __BANG_ARCH__ >= 320 && __BANG_ARCH__ <= 322

// -----------------------------------------------------------------------------
// Bang C/C++ builtin function: sync the threads in HW cluster
// -----------------------------------------------------------------------------
#if __BANG_ARCH__ >= 200
static __mlu_func__ __mlu_builtin__ void __sync_cluster() {
  if (clusterDim == 0) {
    __asm__ __volatile__("sync;\n\t");
  } else {
    __asm__ __volatile__("barrier.sync.local 32771, %[cnt];\n\t"
                         ::[cnt]"r"(coreDim + 1));
  }
}
#else
static __mlu_func__ __mlu_builtin__ void __sync_cluster() {
  if (clusterDim == 0) {
    __asm__ __volatile__("sync;\n\t");
    return;
  }
  switch (clusterId) {
    case 0:
      __BARRIER(4);
      break;
    case 1:
      __BARRIER(4);
      break;
    case 2:
      __BARRIER(4);
      break;
    case 3:
      __BARRIER(4);
      break;
    case 4:
      __BARRIER(4);
      break;
    case 5:
      __BARRIER(4);
      break;
    case 6:
      __BARRIER(4);
      break;
    case 7:
      __BARRIER(4);
      break;
    case 8:
      __BARRIER(4);
      break;
    default:
      break;
  }
}
#endif  // __BANG_ARCH__ >= 200

// -----------------------------------------------------------------------------
// Bang C/C++ builtin function: sync all threads in one task
// -----------------------------------------------------------------------------
#if __BANG_ARCH__ >= 320 && __BANG_ARCH__ <= 322
static __mlu_func__ __mlu_builtin__ void __sync_all() {
  if (clusterDim == 0) {
    __asm__ __volatile__("sync;\n\t");
  } else {
    __asm__ __volatile__("barrier.sync.local 32772, %[cnt];\n\t"
                           ::[cnt]"r"(coreDim + 1));
  }
}
#elif __BANG_ARCH__ >= 200
static __mlu_func__ __mlu_builtin__ void __sync_all() {
  if (clusterDim == 0) {
    __asm__ __volatile__("sync;\n\t");
  } else if (clusterDim == 1) {
    __asm__ __volatile__("barrier.sync.local 32772, %[cnt];\n\t"
                         ::[cnt]"r"(coreDim + 1));
  } else {
    __asm__ __volatile__("barrier.sync.global 32773, %[cnt];\n\t"
                         ::[cnt]"r"(clusterDim * (coreDim + 1)));
  }
}
#else
static __mlu_func__ __mlu_builtin__ void __sync_all() {
  switch (clusterDim) {
    case 0:
      __asm__ __volatile__("sync;\n\t");
      break;
    case 1:
      if (taskDim == 2) {
        __BARRIER(2);  // only for BANG 1.0 cnPlugin DP=2 mode
      } else {
        __BARRIER(4);
      }
      break;
    case 2:
      __BARRIER(8);
      break;
    case 4:
      __BARRIER(16);
      break;
    case 8:
      __BARRIER(32);
      break;
    default:
      break;
  }
}
#endif  // __BANG_ARCH__ >= 320 && __BANG_ARCH__ <= 322

// -----------------------------------------------------------------------------
// Bang C builtin function: sync compute or memcpy inside one thread or union
// -----------------------------------------------------------------------------
#if __BANG_ARCH__ >= 200
static __mlu_func__ __mlu_builtin__ void __sync_compute() {
  asm ("sync.compute;\n\t");
}

static __mlu_func__ __mlu_builtin__ void __sync_copy_nram_to_nram() {
  asm ("sync.compute;\n\t");
}

static __mlu_func__ __mlu_builtin__ void __sync_copy_nram_to_dram() {
  asm ("sync.copy.dram.nram;\n\t");
}

static __mlu_func__ __mlu_builtin__ void __sync_copy_dram_to_nram() {
  asm ("sync.copy.nram.dram;\n\t");
}

#if (__BANG_ARCH__ != 303) && (__BANG_ARCH__ != 306)
static __mlu_func__ __mlu_builtin__ void __sync_copy_wram_to_dram() {
  asm ("sync.copy.dram.wram;\n\t");
}

static __mlu_func__ __mlu_builtin__ void __sync_copy_dram_to_wram() {
  asm ("sync.copy.wram.dram;\n\t");
}

#if __MLU_SRAM_SIZE__ != 0
static __mlu_func__ __mlu_builtin__ void __sync_copy_sram_to_dram() {
  asm ("sync.copy.dram.sram;\n\t");
}

static __mlu_func__ __mlu_builtin__ void __sync_copy_dram_to_sram() {
  asm ("sync.copy.sram.dram;\n\t");
}

static __mlu_func__ __mlu_builtin__ void __sync_copy_nram_to_sram() {
  asm ("sync.copy.sram.nram;\n\t");
}

static __mlu_func__ __mlu_builtin__ void __sync_copy_sram_to_nram() {
  asm ("sync.copy.nram.sram;\n\t");
}

static __mlu_func__ __mlu_builtin__ void __sync_copy_wram_to_sram() {
  asm ("sync.copy.sram.wram;\n\t");
}

static __mlu_func__ __mlu_builtin__ void __sync_copy_sram_to_wram() {
  asm ("sync.copy.wram.sram;\n\t");
}

static __mlu_func__ __mlu_builtin__ void __sync_copy_sram_to_sram() {
  asm ("sync.copy.sram.sram;\n\t");
  __sync_all();
}
#endif  // __MLU_SRAM_SIZE__ != 0
#endif  // __BANG_ARCH__ != 303 && __BANG_ARCH__ != 306
#endif  // __BANG_ARCH__ >= 200

// -----------------------------------------------------------------------------
// Bang C/C++ builtin function: memory copy operation
// -----------------------------------------------------------------------------
static __mlu_func__ __mlu_builtin__ void __memcpy(void* dst,
                                                  const void* src,
                                                  int size,
                                                  mluMemcpyDirection_t dir) {
  switch (dir) {
  default : {
    // Unsupport memcpy direction.
    asm volatile("exit %[status];\n\t"
                 ::[status]"i"(TRAP_BANG_UNSUPPORTED_MEMCPY_DIR));
    break;
  }
  case GDRAM2NRAM:  { __mlvm_memcpy_gdram_to_nram(dst, (void*)src, size); break; }
  case NRAM2GDRAM:  { __mlvm_memcpy_nram_to_gdram(dst, (void*)src, size); break; }
  case LDRAM2NRAM:  { __mlvm_memcpy_ldram_to_nram(dst, (void*)src, size); break; }
  case NRAM2LDRAM:  { __mlvm_memcpy_nram_to_ldram(dst, (void*)src, size); break; }
#if (__BANG_ARCH__ != 303) && (__BANG_ARCH__ != 306)
  case GDRAM2WRAM:  { __mlvm_memcpy_gdram_to_wram(dst, (void*)src, size); break; }
  case WRAM2GDRAM:  { __mlvm_memcpy_wram_to_gdram(dst, (void*)src, size); break; }
  case LDRAM2WRAM:  { __mlvm_memcpy_ldram_to_wram(dst, (void*)src, size); break; }
  case WRAM2LDRAM:  { __mlvm_memcpy_wram_to_ldram(dst, (void*)src, size); break; }
  case NRAM2NRAM:   { __mlvm_memcpy_nram_to_nram(dst, (void*)src, size); break; }
  case NRAM2WRAM:   { __mlvm_memcpy_nram_to_wram(dst, (void*)src, size); break; }
  case WRAM2NRAM:   { __mlvm_memcpy_wram_to_nram(dst, (void*)src, size); break; }
#if __MLU_SRAM_SIZE__ != 0
  case LDRAM2SRAM:  { __mlvm_memcpy_ldram_to_sram(dst, (void*)src, size); break; }
  case SRAM2LDRAM:  { __mlvm_memcpy_sram_to_ldram(dst, (void*)src, size); break; }
  case SRAM2NRAM:   { __mlvm_memcpy_sram_to_nram(dst, (void*)src, size); break; }
  case NRAM2SRAM:   { __mlvm_memcpy_nram_to_sram(dst, (void*)src, size); break; }
  case SRAM2WRAM:   { __mlvm_memcpy_sram_to_wram(dst, (void*)src, size); break; }
  case WRAM2SRAM:   { __mlvm_memcpy_wram_to_sram(dst, (void*)src, size); break; }
  case GDRAM2SRAM:  { __mlvm_memcpy_gdram_to_sram(dst, (void*)src, size); break; }
  case SRAM2GDRAM:  { __mlvm_memcpy_sram_to_gdram(dst, (void*)src, size); break; }
#endif  // __MLU_SRAM_SIZE != 0
#endif  // __BANG_ARCH__ != 303 && __BANG_ARCH__ != 306
  case GDRAM2GDRAM: { __mlvm_memcpy_gdram_to_gdram(dst, (void*)src, size); break; }
  case GDRAM2LDRAM: { __mlvm_memcpy_gdram_to_ldram(dst, (void*)src, size); break; }
  case LDRAM2GDRAM: { __mlvm_memcpy_ldram_to_gdram(dst, (void*)src, size); break; }
  }
}

// -----------------------------------------------------------------------------
// Bang C/C++ builtin function: memory copy operation in the dimension
// -----------------------------------------------------------------------------
#if __BANG_ARCH__ >= 300
#if __MLU_SRAM_SIZE__ != 0
static __mlu_func__ __mlu_builtin__ void __memcpy(void* dst,
                                                  const void* src,
                                                  int size,
                                                  mluMemcpyDirection_t dir,
                                                  int dst_stride0,
                                                  int dst_segnum1,
                                                  int dst_stride1,
                                                  int dst_segnum2,
                                                  int src_stride0,
                                                  int src_segnum1,
                                                  int src_stride1,
                                                  int src_segnum2,
                                                  int id_dst_cluster) {
  if (dir != SRAM2SRAM) {
    // Unsupport stride memcpy direction.
    asm volatile("exit %[status];\n\t"
                 ::[status]"i"(TRAP_BANG_UNSUPPORTED_STRIDE_MEMCPY_DIR));
  }
  __mlvm_memcpy_3D_sram_to_sram(dst, (void*)src, size,
                                dst_stride0, dst_segnum1, dst_stride1,
                                dst_segnum2, src_stride0, src_segnum1,
                                src_stride1, src_segnum2, id_dst_cluster);
}

static __mlu_func__ __mlu_builtin__ void __memcpy_async(void* dst,
                                                        const void* src,
                                                        int size,
                                                        mluMemcpyDirection_t dir,
                                                        int dst_stride0,
                                                        int dst_segnum1,
                                                        int dst_stride1,
                                                        int dst_segnum2,
                                                        int src_stride0,
                                                        int src_segnum1,
                                                        int src_stride1,
                                                        int src_segnum2,
                                                        int id_dst_cluster) {
  if (dir != SRAM2SRAM) {
    // Unsupport stride memcpy direction.
    asm volatile("exit %[status];\n\t"
                 ::[status]"i"(TRAP_BANG_UNSUPPORTED_STRIDE_MEMCPY_DIR));
  }
  __mlvm_memcpy_3D_sram_to_sram_async(dst, (void*)src, size,
                                      dst_stride0, dst_segnum1, dst_stride1,
                                      dst_segnum2, src_stride0, src_segnum1,
                                      src_stride1, src_segnum2, id_dst_cluster);
}
#endif   // __MLU_SRAM_SIZE__ !=0

static __mlu_func__ __mlu_builtin__ void __memcpy(void* dst,
                                                  const void* src,
                                                  int size,
                                                  mluMemcpyDirection_t dir,
                                                  int dst_stride0,
                                                  int dst_segnum1,
                                                  int dst_stride1,
                                                  int dst_segnum2,
                                                  int src_stride0,
                                                  int src_segnum1,
                                                  int src_stride1,
                                                  int src_segnum2) {
  switch (dir) {
    default : {
      // Unsupport stride memcpy direction.
      asm volatile("exit %[status];\n\t"
                   ::[status]"i"(TRAP_BANG_UNSUPPORTED_STRIDE_MEMCPY_DIR));
      break;
    }
    // move
    case NRAM2NRAM: {
      __mlvm_memcpy_3D_nram_to_nram(dst, (void*)src, size,
                                    dst_stride0, dst_segnum1, dst_stride1,
                                    dst_segnum2, src_stride0, src_segnum1,
                                    src_stride1, src_segnum2);
      break;
    }
    case LDRAM2LDRAM: {
      __mlvm_memcpy_3D_ldram_to_ldram(dst, (void*)src, size,
                                      dst_stride0, dst_segnum1, dst_stride1,
                                      dst_segnum2, src_stride0, src_segnum1,
                                      src_stride1, src_segnum2);
      break;
    }
    case GDRAM2GDRAM: {
      __mlvm_memcpy_3D_gdram_to_gdram(dst, (void*)src, size,
                                      dst_stride0, dst_segnum1, dst_stride1,
                                      dst_segnum2, src_stride0, src_segnum1,
                                      src_stride1, src_segnum2);
      break;
    }
#if (__BANG_ARCH__ != 303) && (__BANG_ARCH__ != 306)
    case NRAM2WRAM: {
      __mlvm_memcpy_3D_nram_to_wram(dst, (void*)src, size,
                                    dst_stride0, dst_segnum1, dst_stride1,
                                    dst_segnum2, src_stride0, src_segnum1,
                                    src_stride1, src_segnum2);
      break;
    }
    case WRAM2NRAM: {
      __mlvm_memcpy_3D_wram_to_nram(dst, (void*)src, size,
                                    dst_stride0, dst_segnum1, dst_stride1,
                                    dst_segnum2, src_stride0, src_segnum1,
                                    src_stride1, src_segnum2);
      break;
    }
#endif  // (__BANG_ARCH__ != 303) && (__BANG_ARCH__ != 306)
    case GDRAM2LDRAM: {
      __mlvm_memcpy_3D_gdram_to_ldram(dst, (void*)src, size,
                                      dst_stride0, dst_segnum1, dst_stride1,
                                      dst_segnum2, src_stride0, src_segnum1,
                                      src_stride1, src_segnum2);
      break;
    }
    case LDRAM2GDRAM: {
      __mlvm_memcpy_3D_ldram_to_gdram(dst, (void*)src, size,
                                      dst_stride0, dst_segnum1, dst_stride1,
                                      dst_segnum2, src_stride0, src_segnum1,
                                      src_stride1, src_segnum2);
      break;
    }
    // load
    case GDRAM2NRAM: {
      __mlvm_memcpy_3D_gdram_to_nram(dst, (void*)src, size,
                                     dst_stride0, dst_segnum1, dst_stride1,
                                     dst_segnum2, src_stride0, src_segnum1,
                                     src_stride1, src_segnum2);
      break;
    }
#if __MLU_SRAM_SIZE__ != 0
    case SRAM2NRAM: {
      __mlvm_memcpy_3D_sram_to_nram(dst, (void*)src, size,
                                    dst_stride0, dst_segnum1, dst_stride1,
                                    dst_segnum2, src_stride0, src_segnum1,
                                    src_stride1, src_segnum2);
      break;
    }
    case SRAM2WRAM: {
      __mlvm_memcpy_3D_sram_to_wram(dst, (void*)src, size,
                                    dst_stride0, dst_segnum1, dst_stride1,
                                    dst_segnum2, src_stride0, src_segnum1,
                                    src_stride1, src_segnum2);
      break;
    }
    case GDRAM2SRAM: {
      __mlvm_memcpy_3D_gdram_to_sram(dst, (void*)src, size,
                                     dst_stride0, dst_segnum1, dst_stride1,
                                     dst_segnum2, src_stride0, src_segnum1,
                                     src_stride1, src_segnum2);
      break;
    }
    case LDRAM2SRAM: {
      __mlvm_memcpy_3D_ldram_to_sram(dst, (void*)src, size,
                                     dst_stride0, dst_segnum1, dst_stride1,
                                     dst_segnum2, src_stride0, src_segnum1,
                                     src_stride1, src_segnum2);
      break;
    }
#endif //  __MLU_SRAM_SIZE__ != 0
    case LDRAM2NRAM: {
      __mlvm_memcpy_3D_ldram_to_nram(dst, (void*)src, size,
                                     dst_stride0, dst_segnum1, dst_stride1,
                                     dst_segnum2, src_stride0, src_segnum1,
                                     src_stride1, src_segnum2);
      break;
    }
#if (__BANG_ARCH__ != 303) && (__BANG_ARCH__ != 306)
    case GDRAM2WRAM: {
      __mlvm_memcpy_3D_gdram_to_wram(dst, (void*)src, size,
                                     dst_stride0, dst_segnum1, dst_stride1,
                                     dst_segnum2, src_stride0, src_segnum1,
                                     src_stride1, src_segnum2);
      break;
    }
    case LDRAM2WRAM: {
      __mlvm_memcpy_3D_ldram_to_wram(dst, (void*)src, size,
                                     dst_stride0, dst_segnum1, dst_stride1,
                                     dst_segnum2, src_stride0, src_segnum1,
                                     src_stride1, src_segnum2);
      break;
    }
#endif  // (__BANG_ARCH__ != 303) && (__BANG_ARCH__ != 306)
    // store
    case NRAM2GDRAM: {
      __mlvm_memcpy_3D_nram_to_gdram(dst, (void*)src, size,
                                     dst_stride0, dst_segnum1, dst_stride1,
                                     dst_segnum2, src_stride0, src_segnum1,
                                     src_stride1, src_segnum2);
      break;
    }
    case NRAM2LDRAM: {
      __mlvm_memcpy_3D_nram_to_ldram(dst, (void*)src, size,
                                     dst_stride0, dst_segnum1, dst_stride1,
                                     dst_segnum2, src_stride0, src_segnum1,
                                     src_stride1, src_segnum2);
      break;
    }
#if __MLU_SRAM_SIZE__ != 0
    case NRAM2SRAM: {
      __mlvm_memcpy_3D_nram_to_sram(dst, (void*)src, size,
                                    dst_stride0, dst_segnum1, dst_stride1,
                                    dst_segnum2, src_stride0, src_segnum1,
                                    src_stride1, src_segnum2);
      break;
    }
    case WRAM2SRAM: {
      __mlvm_memcpy_3D_wram_to_sram(dst, (void*)src, size,
                                    dst_stride0, dst_segnum1, dst_stride1,
                                    dst_segnum2, src_stride0, src_segnum1,
                                    src_stride1, src_segnum2);
      break;
    }
    case SRAM2GDRAM: {
      __mlvm_memcpy_3D_sram_to_gdram(dst, (void*)src, size,
                                     dst_stride0, dst_segnum1, dst_stride1,
                                     dst_segnum2, src_stride0, src_segnum1,
                                     src_stride1, src_segnum2);
      break;
    }
    case SRAM2LDRAM: {
      __mlvm_memcpy_3D_sram_to_ldram(dst, (void*)src, size,
                                     dst_stride0, dst_segnum1, dst_stride1,
                                     dst_segnum2, src_stride0, src_segnum1,
                                     src_stride1, src_segnum2);
      break;
    }
#endif //  __MLU_SRAM_SIZE__ != 0
#if  (__BANG_ARCH__ != 303) && (__BANG_ARCH__ != 306)
    case WRAM2GDRAM: {
      __mlvm_memcpy_3D_wram_to_gdram(dst, (void*)src, size,
                                     dst_stride0, dst_segnum1, dst_stride1,
                                     dst_segnum2, src_stride0, src_segnum1,
                                     src_stride1, src_segnum2);
      break;
    }
    case WRAM2LDRAM: {
      __mlvm_memcpy_3D_wram_to_ldram(dst, (void*)src, size,
                                     dst_stride0, dst_segnum1, dst_stride1,
                                     dst_segnum2, src_stride0, src_segnum1,
                                     src_stride1, src_segnum2);
      break;
    }
#endif  // __BANG_ARCH__ != 303 __BANG_ARCH__ != 306
  }
}
static __mlu_func__ __mlu_builtin__ void __memcpy_async(void* dst,
                                                        const void* src,
                                                        int size,
                                                        mluMemcpyDirection_t dir,
                                                        int dst_stride0,
                                                        int dst_segnum1,
                                                        int dst_stride1,
                                                        int dst_segnum2,
                                                        int src_stride0,
                                                        int src_segnum1,
                                                        int src_stride1,
                                                        int src_segnum2) {
  switch (dir) {
    default : {
      // Unsupport stride memcpy async direction.
      asm volatile("exit %[status];\n\t"
                   ::[status]"i"(TRAP_BANG_UNSUPPORTED_STRIDE_MEMCPY_DIR));
      break;
    }
    // move
    case NRAM2NRAM: {
      __mlvm_memcpy_3D_nram_to_nram_async(dst, (void*)src, size,
                                          dst_stride0, dst_segnum1, dst_stride1,
                                          dst_segnum2, src_stride0, src_segnum1,
                                          src_stride1, src_segnum2);
      break;
    }
    case LDRAM2LDRAM: {
      __mlvm_memcpy_3D_ldram_to_ldram_async(dst, (void*)src, size,
                                            dst_stride0, dst_segnum1, dst_stride1,
                                            dst_segnum2, src_stride0, src_segnum1,
                                            src_stride1, src_segnum2);
      break;
    }
    case GDRAM2GDRAM: {
      __mlvm_memcpy_3D_gdram_to_gdram_async(dst, (void*)src, size,
                                            dst_stride0, dst_segnum1, dst_stride1,
                                            dst_segnum2, src_stride0, src_segnum1,
                                            src_stride1, src_segnum2);
      break;
    }
#if (__BANG_ARCH__ != 303) && (__BANG_ARCH__ != 306)
    case NRAM2WRAM: {
      __mlvm_memcpy_3D_nram_to_wram_async(dst, (void*)src, size,
                                          dst_stride0, dst_segnum1, dst_stride1,
                                          dst_segnum2, src_stride0, src_segnum1,
                                          src_stride1, src_segnum2);
      break;
    }
    case WRAM2NRAM: {
      __mlvm_memcpy_3D_wram_to_nram_async(dst, (void*)src, size,
                                          dst_stride0, dst_segnum1, dst_stride1,
                                          dst_segnum2, src_stride0, src_segnum1,
                                          src_stride1, src_segnum2);
      break;
    }
#endif  // __BANG_ARCH__ != 303 && __BANG_ARCH__ != 306
    case GDRAM2LDRAM: {
      __mlvm_memcpy_3D_gdram_to_ldram_async(dst, (void*)src, size,
                                            dst_stride0, dst_segnum1, dst_stride1,
                                            dst_segnum2, src_stride0, src_segnum1,
                                            src_stride1, src_segnum2);
      break;
    }
    case LDRAM2GDRAM: {
      __mlvm_memcpy_3D_ldram_to_gdram_async(dst, (void*)src, size,
                                            dst_stride0, dst_segnum1, dst_stride1,
                                            dst_segnum2, src_stride0, src_segnum1,
                                            src_stride1, src_segnum2);
      break;
    }
    // load
    case GDRAM2NRAM: {
      __mlvm_memcpy_3D_gdram_to_nram_async(dst, (void*)src, size,
                                           dst_stride0, dst_segnum1, dst_stride1,
                                           dst_segnum2, src_stride0, src_segnum1,
                                           src_stride1, src_segnum2);
      break;
    }
    case LDRAM2NRAM: {
      __mlvm_memcpy_3D_ldram_to_nram_async(dst, (void*)src, size,
                                           dst_stride0, dst_segnum1, dst_stride1,
                                           dst_segnum2, src_stride0, src_segnum1,
                                           src_stride1, src_segnum2);
      break;
    }
#if (__BANG_ARCH__ != 303) && (__BANG_ARCH__ != 306)
    case GDRAM2WRAM: {
      __mlvm_memcpy_3D_gdram_to_wram_async(dst, (void*)src, size,
                                           dst_stride0, dst_segnum1, dst_stride1,
                                           dst_segnum2, src_stride0, src_segnum1,
                                           src_stride1, src_segnum2);
      break;
    }
    case LDRAM2WRAM: {
      __mlvm_memcpy_3D_ldram_to_wram_async(dst, (void*)src, size,
                                           dst_stride0, dst_segnum1, dst_stride1,
                                           dst_segnum2, src_stride0, src_segnum1,
                                           src_stride1, src_segnum2);
      break;
    }
#endif  // (__BANG_ARCH__ != 303) && (__BANG_ARCH__ != 306)
#if __MLU_SRAM_SIZE__ != 0
    case SRAM2NRAM: {
      __mlvm_memcpy_3D_sram_to_nram_async(dst, (void*)src, size,
                                          dst_stride0, dst_segnum1, dst_stride1,
                                          dst_segnum2, src_stride0, src_segnum1,
                                          src_stride1, src_segnum2);
      break;
    }
    case SRAM2WRAM: {
      __mlvm_memcpy_3D_sram_to_wram_async(dst, (void*)src, size,
                                          dst_stride0, dst_segnum1, dst_stride1,
                                          dst_segnum2, src_stride0, src_segnum1,
                                          src_stride1, src_segnum2);
      break;
    }
    case GDRAM2SRAM: {
      __mlvm_memcpy_3D_gdram_to_sram_async(dst, (void*)src, size,
                                           dst_stride0, dst_segnum1, dst_stride1,
                                           dst_segnum2, src_stride0, src_segnum1,
                                           src_stride1, src_segnum2);
      break;
    }
    case LDRAM2SRAM: {
      __mlvm_memcpy_3D_ldram_to_sram_async(dst, (void*)src, size,
                                           dst_stride0, dst_segnum1, dst_stride1,
                                           dst_segnum2, src_stride0, src_segnum1,
                                           src_stride1, src_segnum2);
      break;
    }
#endif //  __MLU_SRAM_SIZE__ != 0
    // store
    case NRAM2GDRAM: {
      __mlvm_memcpy_3D_nram_to_gdram_async(dst, (void*)src, size,
                                           dst_stride0, dst_segnum1, dst_stride1,
                                           dst_segnum2, src_stride0, src_segnum1,
                                           src_stride1, src_segnum2);
      break;
    }
    case NRAM2LDRAM: {
      __mlvm_memcpy_3D_nram_to_ldram_async(dst, (void*)src, size,
                                           dst_stride0, dst_segnum1, dst_stride1,
                                           dst_segnum2, src_stride0, src_segnum1,
                                           src_stride1, src_segnum2);
      break;
    }
#if (__BANG_ARCH__ != 303) && (__BANG_ARCH__ != 306)
    case WRAM2GDRAM: {
      __mlvm_memcpy_3D_wram_to_gdram_async(dst, (void*)src, size,
                                           dst_stride0, dst_segnum1, dst_stride1,
                                           dst_segnum2, src_stride0, src_segnum1,
                                           src_stride1, src_segnum2);
      break;
    }
    case WRAM2LDRAM: {
      __mlvm_memcpy_3D_wram_to_ldram_async(dst, (void*)src, size,
                                           dst_stride0, dst_segnum1, dst_stride1,
                                           dst_segnum2, src_stride0, src_segnum1,
                                           src_stride1, src_segnum2);
      break;
    }
#endif  // __BANG_ARCH__ != 303 && __BANG_ARCH__ != 306
#if __MLU_SRAM_SIZE__ != 0
    case NRAM2SRAM: {
      __mlvm_memcpy_3D_nram_to_sram_async(dst, (void*)src, size,
                                          dst_stride0, dst_segnum1, dst_stride1,
                                          dst_segnum2, src_stride0, src_segnum1,
                                          src_stride1, src_segnum2);
      break;
    }
    case WRAM2SRAM: {
      __mlvm_memcpy_3D_wram_to_sram_async(dst, (void*)src, size,
                                          dst_stride0, dst_segnum1, dst_stride1,
                                          dst_segnum2, src_stride0, src_segnum1,
                                          src_stride1, src_segnum2);
      break;
    }
    case SRAM2GDRAM: {
      __mlvm_memcpy_3D_sram_to_gdram_async(dst, (void*)src, size,
                                           dst_stride0, dst_segnum1, dst_stride1,
                                           dst_segnum2, src_stride0, src_segnum1,
                                           src_stride1, src_segnum2);
      break;
    }
    case SRAM2LDRAM: {
      __mlvm_memcpy_3D_sram_to_ldram_async(dst, (void*)src, size,
                                           dst_stride0, dst_segnum1, dst_stride1,
                                           dst_segnum2, src_stride0, src_segnum1,
                                           src_stride1, src_segnum2);
      break;
    }
#endif //  __MLU_SRAM_SIZE__ != 0
  }
}
#endif  // __BANG_ARCH__ >= 300

// -----------------------------------------------------------------------------
// Bang C/C++ builtin function: memory async-copy operation
// -----------------------------------------------------------------------------
static __mlu_func__ __mlu_builtin__ void __memcpy_async(void* dst,
                                                        const void* src,
                                                        int size,
                                                        mluMemcpyDirection_t dir) {
  switch (dir) {
  default : {
    // Unsupport memcpy direction.
    asm volatile("exit %[status];\n\t"
                 ::[status]"i"(TRAP_BANG_UNSUPPORTED_MEMCPY_DIR));
    break;
  }
  case GDRAM2NRAM:  { __mlvm_memcpy_gdram_to_nram_async(dst, (void*)src, size); break; }
  case NRAM2GDRAM:  { __mlvm_memcpy_nram_to_gdram_async(dst, (void*)src, size); break; }
#if (__BANG_ARCH__ != 303) && (__BANG_ARCH__ != 306)
  case GDRAM2WRAM:  { __mlvm_memcpy_gdram_to_wram_async(dst, (void*)src, size); break; }
  case NRAM2WRAM:   { __mlvm_memcpy_nram_to_wram_async(dst, (void*)src, size); break; }
  case WRAM2NRAM:   { __mlvm_memcpy_wram_to_nram_async(dst, (void*)src, size); break; }
#endif  // __BANG_ARCH__ != 303 && __BANG_ARCH__ != 306
    // NOTE: async copy from nram to nram is equal to sync copy nram to nram,
    // as nram is onchip memory and copy instruction belong to compute stream.
  case NRAM2NRAM:   { __mlvm_memcpy_nram_to_nram(dst, (void*)src, size); break; }
#if __MLU_SRAM_SIZE__ != 0
  case SRAM2NRAM:   { __mlvm_memcpy_sram_to_nram_async(dst, (void*)src, size); break; }
  case NRAM2SRAM:   { __mlvm_memcpy_nram_to_sram_async(dst, (void*)src, size); break; }
  case SRAM2WRAM:   { __mlvm_memcpy_sram_to_wram_async(dst, (void*)src, size); break; }
  case WRAM2SRAM:   { __mlvm_memcpy_wram_to_sram_async(dst, (void*)src, size); break; }
  case GDRAM2SRAM:  { __mlvm_memcpy_gdram_to_sram_async(dst, (void*)src, size); break; }
  case SRAM2GDRAM:  { __mlvm_memcpy_sram_to_gdram_async(dst, (void*)src, size); break; }
#endif  // __MLU_SRAM_SIZE__ != 0
  case GDRAM2LDRAM: { __mlvm_memcpy_gdram_to_ldram_async(dst, (void*)src, size); break; }
  case GDRAM2GDRAM: { __mlvm_memcpy_gdram_to_gdram_async(dst, (void*)src, size); break; }
  }
}

// -----------------------------------------------------------------------------
// Bang C/C++ builtin function: stride memory copy operation
// -----------------------------------------------------------------------------
static __mlu_func__ __mlu_builtin__ void __memcpy(void* dst,
                                                  const void* src,
                                                  int size,
                                                  mluMemcpyDirection_t dir,
                                                  int dst_stride,
                                                  int src_stride,
                                                  int segnum) {
  switch (dir) {
  default : {
    // Unsupport stride memcpy direction.
    asm volatile("exit %[status];\n\t"
                 ::[status]"i"(TRAP_BANG_UNSUPPORTED_STRIDE_MEMCPY_DIR));
    break;
  }
  case GDRAM2NRAM: {
    __mlvm_memcpy_stride_gdram_to_nram(dst, (void*)src, size,
                                       dst_stride, src_stride, segnum);
    break;
  }
  case NRAM2GDRAM: {
    __mlvm_memcpy_stride_nram_to_gdram(dst, (void*)src, size,
                                       dst_stride, src_stride, segnum);
    break;
  }
  case LDRAM2NRAM: {
    __mlvm_memcpy_stride_ldram_to_nram(dst, (void*)src, size,
                                       dst_stride, src_stride, segnum);
    break;
  }
  case NRAM2LDRAM: {
    __mlvm_memcpy_stride_nram_to_ldram(dst, (void*)src, size,
                                       dst_stride, src_stride, segnum);
    break;
  }
  case NRAM2NRAM:  {
    __mlvm_memcpy_stride_nram_to_nram(dst, (void*)src, size,
                                      dst_stride, src_stride, segnum);
    break;
  }
#if __BANG_ARCH__ >= 200
#if (__BANG_ARCH__ != 303) && (__BANG_ARCH__ != 306)
  case GDRAM2WRAM: {
    __mlvm_memcpy_stride_gdram_to_wram(dst, (void*)src, size,
                                       dst_stride, src_stride, segnum);
    break;
  }
#if __MLU_SRAM_SIZE__ != 0
  case SRAM2NRAM:  {
    __mlvm_memcpy_stride_sram_to_nram(dst, (void*)src, size,
                                      dst_stride, src_stride, segnum);
    break;
  }
  case SRAM2WRAM: {
    __mlvm_memcpy_stride_sram_to_wram(dst, (void*)src, size,
                                      dst_stride, src_stride, segnum);
    break;
  }
  case NRAM2SRAM:  {
    __mlvm_memcpy_stride_nram_to_sram(dst, (void*)src, size,
                                      dst_stride, src_stride, segnum);
    break;
  }
  case SRAM2GDRAM:  {
    __mlvm_memcpy_stride_sram_to_gdram(dst, (void*)src, size,
                                       dst_stride, src_stride, segnum);
    break;
  }
  case GDRAM2SRAM:  {
    __mlvm_memcpy_stride_gdram_to_sram(dst, (void*)src, size,
                                       dst_stride, src_stride, segnum);
    break;
  }
  case SRAM2LDRAM:  {
    __mlvm_memcpy_stride_sram_to_ldram(dst, (void*)src, size,
                                       dst_stride, src_stride, segnum);
    break;
  }
  case LDRAM2SRAM:  {
    __mlvm_memcpy_stride_ldram_to_sram(dst, (void*)src, size,
                                       dst_stride, src_stride, segnum);
    break;
  }
#endif //  __MLU_SRAM_SIZE__ != 0
  case NRAM2WRAM:  {
    __mlvm_memcpy_stride_nram_to_wram(dst, (void*)src, size,
                                      dst_stride, src_stride, segnum);
    break;
  }
#endif  // __BANG_ARCH__ != 303 && __BANG_ARCH__ != 306
  case LDRAM2GDRAM:  {
    __mlvm_memcpy_stride_ldram_to_gdram(dst, (void*)src, size,
                                        dst_stride, src_stride, segnum);
    break;
  }
  case GDRAM2LDRAM:  {
    __mlvm_memcpy_stride_gdram_to_ldram(dst, (void*)src, size,
                                        dst_stride, src_stride, segnum);
    break;
  }
  case GDRAM2GDRAM: {
    __mlvm_memcpy_stride_gdram_to_gdram(dst, (void*)src, size,
                                        dst_stride, src_stride, segnum);
    break;
  }
  case LDRAM2LDRAM: {
    __mlvm_memcpy_stride_ldram_to_ldram(dst, (void*)src, size,
                                        dst_stride, src_stride, segnum);
    break;
  }
#endif  // __BANG_ARCH__ >= 200
  }
}

// -----------------------------------------------------------------------------
// Bang C/C++ builtin function: stride memory async-copy operation
// -----------------------------------------------------------------------------
static __mlu_func__ __mlu_builtin__ void __memcpy_async(void* dst,
                                                        const void* src,
                                                        int size,
                                                        mluMemcpyDirection_t dir,
                                                        int dst_stride,
                                                        int src_stride,
                                                        int segnum) {
  switch (dir) {
  default : {
    // Unsupport stride memcpy direction.
    asm volatile("exit %[status];\n\t"
                 ::[status]"i"(TRAP_BANG_UNSUPPORTED_STRIDE_MEMCPY_DIR));
    break;
  }
  case NRAM2NRAM: {
    __mlvm_memcpy_stride_nram_to_nram_async(dst, (void*)src, size,
                                            dst_stride, src_stride, segnum);
    break;
  }
  case GDRAM2NRAM: {
    __mlvm_memcpy_stride_gdram_to_nram_async(dst, (void*)src, size,
                                             dst_stride, src_stride, segnum);
    break;
  }
  case NRAM2GDRAM: {
    __mlvm_memcpy_stride_nram_to_gdram_async(dst, (void*)src, size,
                                             dst_stride, src_stride, segnum);
    break;
  }
#if (__BANG_ARCH__ >= 200) && (__BANG_ARCH__ != 303)  && (__BANG_ARCH__ != 306)
#if __MLU_SRAM_SIZE__ != 0
  case SRAM2NRAM:  {
    __mlvm_memcpy_stride_sram_to_nram_async(dst, (void*)src, size,
                                            dst_stride, src_stride, segnum);
    break;
  }
  case SRAM2WRAM: {
    __mlvm_memcpy_stride_sram_to_wram_async(dst, (void*)src, size,
                                            dst_stride, src_stride, segnum);
    break;
  }
  case NRAM2SRAM:  {
    __mlvm_memcpy_stride_nram_to_sram_async(dst, (void*)src, size,
                                            dst_stride, src_stride, segnum);
    break;
  }
  case SRAM2GDRAM:  {
    __mlvm_memcpy_stride_sram_to_gdram_async(dst, (void*)src, size,
                                             dst_stride, src_stride, segnum);
    break;
  }
  case GDRAM2SRAM:  {
    __mlvm_memcpy_stride_gdram_to_sram_async(dst, (void*)src, size,
                                             dst_stride, src_stride, segnum);
    break;
  }
#endif //  __MLU_SRAM_SIZE__ != 0
  case GDRAM2WRAM: {
    __mlvm_memcpy_stride_gdram_to_wram_async(dst, (void*)src, size,
                                             dst_stride, src_stride, segnum);
    break;
  }
  case NRAM2WRAM:  {
    __mlvm_memcpy_stride_nram_to_wram_async(dst, (void*)src, size,
                                            dst_stride, src_stride, segnum);
    break;
  }
#endif  // __BANG_ARCH__ >= 200 && __BANG_ARCH__ != 303 && __BANG_ARCH__ != 306
  case GDRAM2LDRAM:  {
    __mlvm_memcpy_stride_gdram_to_ldram_async(dst, (void*)src, size,
                                              dst_stride, src_stride, segnum);
    break;
  }
  case GDRAM2GDRAM:  {
    __mlvm_memcpy_stride_gdram_to_gdram_async(dst, (void*)src, size,
                                              dst_stride, src_stride, segnum);
    break;
  }
  }
}

#if __BANG_ARCH__ >= 200
#if __MLU_SRAM_SIZE__ != 0
static __mlu_func__ __mlu_builtin__ void __memcpy(void* dst,
                                                  const void* src,
                                                  int size,
                                                  mluMemcpyDirection_t dir,
                                                  int id_dst_cluster) {
  if (dir != SRAM2SRAM) {
    // Unsupport memcpy direction.
    asm volatile("exit %[status];\n\t"
                 ::[status]"i"(TRAP_BANG_UNSUPPORTED_MEMCPY_DIR));
  }
  __mlvm_memcpy_sram_to_sram(dst, (void*)src, size, id_dst_cluster);
}

static __mlu_func__ __mlu_builtin__ void __memcpy(void* dst,
                                                  const void* src,
                                                  int size,
                                                  mluMemcpyDirection_t dir,
                                                  int dst_stride,
                                                  int src_stride,
                                                  int segnum,
                                                  int id_dst_cluster) {
  if (dir != SRAM2SRAM) {
    // Unsupport stride memcpy direction.
    asm volatile("exit %[status];\n\t"
                 ::[status]"i"(TRAP_BANG_UNSUPPORTED_STRIDE_MEMCPY_DIR));
  }
  __mlvm_memcpy_stride_sram_to_sram(dst, (void*)src, size,
                                    dst_stride, src_stride, segnum,
                                    id_dst_cluster);
}
#endif  // __MLU_SRAM_SIZE__ != 0

static __mlu_func__ __mlu_builtin__ void __memcpy_nram_to_nram(void* dst,
                                                               const void* src,
                                                               int size,
                                                               int dst_stride,
                                                               int src_stride,
                                                               int segnum) {
  __mlvm_memcpy_nram_to_nram_stride_fast(dst, (void*)src, size, dst_stride,
                                         src_stride, segnum);
}

#endif  // __BANG_ARCH__ >= 200

#if __BANG_ARCH__ == 306
static __mlu_func__ __mlu_builtin__ void __memcpy_nram_to_remote(void* dst,
                                                                 void* src,
                                                                 int size,
                                                                 int8_t qpid) {
  __mlvm_memcpy_nram_to_remote(dst, src, size, qpid);
}

static __mlu_func__ __mlu_builtin__ void __memcpy_nram_to_remote_async(void* dst,
                                                                       void* src,
                                                                       int size,
                                                                       int8_t qpid) {
  __mlvm_memcpy_nram_to_remote_async(dst, src, size, qpid);
}

static __mlu_func__ __mlu_builtin__ void __memcpy_gdram_to_remote(void* dst,
                                                                  void* src,
                                                                  int size,
                                                                  int8_t qpid) {
  __mlvm_memcpy_gdram_to_remote(dst, src, size, qpid);
}

static __mlu_func__ __mlu_builtin__ void __memcpy_gdram_to_remote_async(void* dst,
                                                                        void* src,
                                                                        int size,
                                                                        int8_t qpid) {
  __mlvm_memcpy_gdram_to_remote_async(dst, src, size, qpid);
}
static __mlu_func__ __mlu_builtin__ void __memcpy_nram_to_remote_tf32(float* dst,
                                                                      float* src,
                                                                      int size,
                                                                      int8_t qpid) {
  __mlvm_memcpy_nram_to_remote_tf32(dst, src, size, qpid);
}

static __mlu_func__ __mlu_builtin__ void __memcpy_nram_to_remote_tf32_async(float* dst,
                                                                            float* src,
                                                                            int size,
                                                                            int8_t qpid) {
  __mlvm_memcpy_nram_to_remote_tf32_async(dst, src, size, qpid);
}

static __mlu_func__ __mlu_builtin__ void __memcpy_gdram_to_remote_tf32(float* dst,
                                                                       float* src,
                                                                       int size,
                                                                       int8_t qpid) {
  __mlvm_memcpy_gdram_to_remote_tf32(dst, src, size, qpid);
}

static __mlu_func__ __mlu_builtin__ void __memcpy_gdram_to_remote_tf32_async(float* dst,
                                                                             float* src,
                                                                             int size,
                                                                             int8_t qpid) {
  __mlvm_memcpy_gdram_to_remote_tf32_async(dst, src, size, qpid);
}

static __mlu_func__ __mlu_builtin__ void __bang_remote_write(void* dst,
                                                             short value,
                                                             int8_t qpid) {
  __mlvm_remote_write(dst, value, qpid);
}

static __mlu_func__ __mlu_builtin__ void __bang_remote_write_sync(void* dst,
                                                                  short value,
                                                                  int8_t qpid) {
  __mlvm_remote_write_sync(dst, value, qpid);
}

static __mlu_func__ __mlu_builtin__ void __bang_remote_write_invalidate(void* dst,
                                                                        short value,
                                                                        int8_t qpid) {
  __mlvm_remote_write_invalidate(dst, value, qpid);
}

static __mlu_func__ __mlu_builtin__ void __bang_remote_write_sync_invalidate(void* dst,
                                                                             short value,
                                                                             int8_t qpid) {
  __mlvm_remote_write_sync_invalidate(dst, value, qpid);
}

static __mlu_func__ __mlu_builtin__ void __bang_qpreset(int8_t qpid) {
  __mlvm_remote_qpreset(qpid);
}

#endif  // __BANG_ARCH__ == 306

// -----------------------------------------------------------------------------
// Bang C/C++ builtin function: nram set operation
// -----------------------------------------------------------------------------
#if __BANG_ARCH__ < 200
#define __nramset_float __mlvm_memset_nram_stream_f32
#define __nramset_half __mlvm_memset_nram_stream_f16
#define __nramset_int __mlvm_memset_nram_stream_s32
#define __nramset_unsigned_int __mlvm_memset_nram_stream_u32
#define __nramset_short __mlvm_memset_nram_stream_s16
#define __nramset_unsigned_short __mlvm_memset_nram_stream_u16
#else // __BANG_ARCH__ >= 200
static __mlu_func__ __mlu_builtin__ void __nramset_float(float* dst,
                                                         int elem_count,
                                                         float value) {
  __mlvm_memset_nram_stream_f32(dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __nramset_half(half* dst,
                                                        int elem_count,
                                                        half value) {
  __mlvm_memset_nram_stream_f16(dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __nramset_int(int* dst,
                                                       int elem_count,
                                                       int value) {
  __mlvm_memset_nram_stream_s32(dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __nramset_unsigned_int(unsigned int* dst,
                                                                int elem_count,
                                                                unsigned int value) {
  __mlvm_memset_nram_stream_u32(dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __nramset_short(short* dst,
                                                         int elem_count,
                                                         short value) {
  __mlvm_memset_nram_stream_s16(dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __nramset_unsigned_short(unsigned short* dst,
                                                                  int elem_count,
                                                                  unsigned short value) {
  __mlvm_memset_nram_stream_u16(dst, elem_count, value);
}
// 8bit
static __mlu_func__ __mlu_builtin__ void __nramset(void* dst,
                                                   int elem_count,
                                                   char value) {
  __mlvm_memset_nram_stream_s8((char*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __nramset(void* dst,
                                                   int elem_count,
                                                   signed char value) {
  __mlvm_memset_nram_stream_s8((char*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __nramset(void* dst,
                                                   int elem_count,
                                                   unsigned char value) {
  __mlvm_memset_nram_stream_s8((char*)dst, elem_count, value);
}
// 16bit
static __mlu_func__ __mlu_builtin__ void __nramset(void* dst,
                                                   int elem_count,
                                                   half value) {
  __mlvm_memset_nram_stream_f16((half*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __nramset(void* dst,
                                                   int elem_count,
                                                   short value) {
  __mlvm_memset_nram_stream_s16((short*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __nramset(void* dst,
                                                   int elem_count,
                                                   unsigned short value) {
  __mlvm_memset_nram_stream_s16((short*)dst, elem_count, value);
}
// 32bit
static __mlu_func__ __mlu_builtin__ void __nramset(void* dst,
                                                   int elem_count,
                                                   float value) {
  __mlvm_memset_nram_stream_f32((float*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __nramset(void* dst,
                                                   int elem_count,
                                                   int value) {
  __mlvm_memset_nram_stream_s32((int*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __nramset(void* dst,
                                                   int elem_count,
                                                   unsigned int value) {
  __mlvm_memset_nram_stream_s32((int*)dst, elem_count, value);
}
// 8bit
static __mlu_func__ __mlu_builtin__ void __bang_write_value(void* dst,
                                                            int elem_count,
                                                            char value) {
  __mlvm_memset_nram_stream_s8((char*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __bang_write_value(void* dst,
                                                            int elem_count,
                                                            unsigned char value) {
  __mlvm_memset_nram_stream_s8((char*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __bang_write_value(void* dst,
                                                            int elem_count,
                                                            signed char value) {
  __mlvm_memset_nram_stream_s8((char*)dst, elem_count, value);
}
// 16bit
static __mlu_func__ __mlu_builtin__ void __bang_write_value(void* dst,
                                                            int elem_count,
                                                            half value) {
  __mlvm_memset_nram_stream_f16((half*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __bang_write_value(void* dst,
                                                            int elem_count,
                                                            short value) {
  __mlvm_memset_nram_stream_s16((short*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __bang_write_value(void* dst,
                                                            int elem_count,
                                                            unsigned short value) {
  __mlvm_memset_nram_stream_s16((short*)dst, elem_count, value);
}
// 32bit
static __mlu_func__ __mlu_builtin__ void __bang_write_value(void* dst,
                                                            int elem_count,
                                                            float value) {
  __mlvm_memset_nram_stream_f32((float*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __bang_write_value(void* dst,
                                                            int elem_count,
                                                            int value) {
  __mlvm_memset_nram_stream_s32((int*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __bang_write_value(void* dst,
                                                            int elem_count,
                                                            unsigned int value) {
  __mlvm_memset_nram_stream_s32((int*)dst, elem_count, value);
}

#if __BANG_ARCH__ >= 300
// 8bit
static __mlu_func__ __mlu_builtin__ void __bang_write_value(void* dst, int elem_count,
                                                            char value, int stride0,
                                                            int iter1, int stride1,
                                                            int iter2) {
  __mlvm_memset3d_nram_stream_s8((char*)dst, elem_count, value, stride0, iter1,
                                 stride1, iter2);
}
static __mlu_func__ __mlu_builtin__ void __bang_write_value(void* dst, int elem_count,
                                                            signed char value, int stride0,
                                                            int iter1, int stride1,
                                                            int iter2) {
  __mlvm_memset3d_nram_stream_s8((char*)dst, elem_count, value, stride0, iter1,
                                 stride1, iter2);
}
static __mlu_func__ __mlu_builtin__ void __bang_write_value(void* dst, int elem_count,
                                                            unsigned char value, int stride0,
                                                            int iter1, int stride1,
                                                            int iter2) {
  __mlvm_memset3d_nram_stream_s8((char*)dst, elem_count, value, stride0, iter1,
                                 stride1, iter2);
}
// 16bit
static __mlu_func__ __mlu_builtin__ void __bang_write_value(void* dst,
                                                            int elem_count,
                                                            half value, int stride0,
                                                            int iter1, int stride1,
                                                            int iter2) {
  __mlvm_memset3d_nram_stream_f16((half*)dst, elem_count, value, stride0, iter1,
                                  stride1, iter2);
}
static __mlu_func__ __mlu_builtin__ void __bang_write_value(void* dst,
                                                            int elem_count,
                                                            short value, int stride0,
                                                            int iter1, int stride1,
                                                            int iter2) {
  __mlvm_memset3d_nram_stream_s16((short*)dst, elem_count, value, stride0,
                                  iter1, stride1, iter2);
}
static __mlu_func__ __mlu_builtin__ void __bang_write_value(void* dst,
                                                            int elem_count,
                                                            unsigned short value, int stride0,
                                                            int iter1, int stride1,
                                                            int iter2) {
  __mlvm_memset3d_nram_stream_s16((short*)dst, elem_count, value, stride0,
                                  iter1, stride1, iter2);
}
// 32bit
static __mlu_func__ __mlu_builtin__ void __bang_write_value(void* dst,
                                                            int elem_count,
                                                            float value, int stride0,
                                                            int iter1, int stride1,
                                                            int iter2) {
  __mlvm_memset3d_nram_stream_f32((float*)dst, elem_count, value, stride0,
                                  iter1, stride1, iter2);
}
static __mlu_func__ __mlu_builtin__ void __bang_write_value(void* dst,
                                                            int elem_count,
                                                            int value, int stride0,
                                                            int iter1, int stride1,
                                                            int iter2) {
  __mlvm_memset3d_nram_stream_s32((int*)dst, elem_count, value, stride0, iter1,
                                  stride1, iter2);
}
static __mlu_func__ __mlu_builtin__ void __bang_write_value(void* dst,
                                                            int elem_count,
                                                            unsigned int value, int stride0,
                                                            int iter1, int stride1,
                                                            int iter2) {
  __mlvm_memset3d_nram_stream_s32((int*)dst, elem_count, value, stride0, iter1,
                                  stride1, iter2);
}
#endif  // __BANG_ARCH__ >= 300

// 8bit
static __mlu_func__ __mlu_builtin__ void __memset_nram(void* dst,
                                                       int elem_count,
                                                       char value) {
  __mlvm_memset_nram_s8((char*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __memset_nram(void* dst,
                                                       int elem_count,
                                                       signed char value) {
  __mlvm_memset_nram_s8((char*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __memset_nram(void* dst,
                                                       int elem_count,
                                                       unsigned char value) {
  __mlvm_memset_nram_s8((char*)dst, elem_count, value);
}
// 16bit
static __mlu_func__ __mlu_builtin__ void __memset_nram(void* dst,
                                                       int elem_count,
                                                       half value) {
  __mlvm_memset_nram_f16((half*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __memset_nram(void* dst,
                                                       int elem_count,
                                                       short value) {
  __mlvm_memset_nram_s16((short*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __memset_nram(void* dst,
                                                       int elem_count,
                                                       unsigned short value) {
  __mlvm_memset_nram_s16((short*)dst, elem_count, value);
}
// 32bit
static __mlu_func__ __mlu_builtin__ void __memset_nram(void* dst,
                                                       int elem_count,
                                                       float value) {
  __mlvm_memset_nram_f32((float*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __memset_nram(void* dst,
                                                       int elem_count,
                                                       int value) {
  __mlvm_memset_nram_s32((int*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __memset_nram(void* dst,
                                                       int elem_count,
                                                       unsigned int value) {
  __mlvm_memset_nram_s32((int*)dst, elem_count, value);
}
#endif  // __BANG_ARCH__ < 200

// -----------------------------------------------------------------------------
// Bang C/C++ builtin function: sram set operation
// -----------------------------------------------------------------------------
#if __MLU_SRAM_SIZE__ != 0
#if __BANG_ARCH__ >= 200
// 8bit
static __mlu_func__ __mlu_builtin__ void __sramset(void* dst,
                                                   int elem_count,
                                                   char value) {
  __mlvm_memset_sram_s8((char*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __sramset(void* dst,
                                                   int elem_count,
                                                   unsigned char value) {
  __mlvm_memset_sram_s8((char*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __sramset(void* dst,
                                                   int elem_count,
                                                   signed char value) {
  __mlvm_memset_sram_s8((char*)dst, elem_count, value);
}
// 16bit
static __mlu_func__ __mlu_builtin__ void __sramset(void* dst,
                                                   int elem_count,
                                                   half value) {
  __mlvm_memset_sram_f16((half*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __sramset(void* dst,
                                                   int elem_count,
                                                   short value) {
  __mlvm_memset_sram_s16((short*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __sramset(void* dst,
                                                   int elem_count,
                                                   unsigned short value) {
  __mlvm_memset_sram_s16((short*)dst, elem_count, value);
}
// 32bit
static __mlu_func__ __mlu_builtin__ void __sramset(void* dst,
                                                   int elem_count,
                                                   float value) {
  __mlvm_memset_sram_f32((float*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __sramset(void* dst,
                                                   int elem_count,
                                                   int value) {
  __mlvm_memset_sram_s32((int*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __sramset(void* dst,
                                                   int elem_count,
                                                   unsigned int value) {
  __mlvm_memset_sram_s32((int*)dst, elem_count, value);
}
#endif  // __BANG_ARCH__ >= 200
#endif  // __MLU_SRAM_SIZE__ != 0
// -----------------------------------------------------------------------------
// Bang C/C++ builtin function: gdram set operation
// -----------------------------------------------------------------------------
#if __BANG_ARCH__ >= 200
// 8bit
static __mlu_func__ __mlu_builtin__ void __gdramset(void* dst,
                                                    int elem_count,
                                                    char value) {
  __mlvm_memset_gdram_s8((char*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __gdramset(void* dst,
                                                    int elem_count,
                                                    signed char value) {
  __mlvm_memset_gdram_s8((char*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __gdramset(void* dst,
                                                    int elem_count,
                                                    unsigned char value) {
  __mlvm_memset_gdram_s8((char*)dst, elem_count, value);
}
// 16bit
static __mlu_func__ __mlu_builtin__ void __gdramset(void* dst,
                                                    int elem_count,
                                                    half value) {
  __mlvm_memset_gdram_f16((half*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __gdramset(void* dst,
                                                    int elem_count,
                                                    short value) {
  __mlvm_memset_gdram_s16((short*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __gdramset(void* dst,
                                                    int elem_count,
                                                    unsigned short value) {
  __mlvm_memset_gdram_s16((short*)dst, elem_count, value);
}
// 32bit
static __mlu_func__ __mlu_builtin__ void __gdramset(void* dst,
                                                    int elem_count,
                                                    float value) {
  __mlvm_memset_gdram_f32((float*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __gdramset(void* dst,
                                                    int elem_count,
                                                    int value) {
  __mlvm_memset_gdram_s32((int*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __gdramset(void* dst,
                                                    int elem_count,
                                                    unsigned int value) {
  __mlvm_memset_gdram_s32((int*)dst, elem_count, value);
}
#endif  // __BANG_ARCH__ >= 200

// -----------------------------------------------------------------------------
// Bang C/C++ builtin function: ldram set operation
// -----------------------------------------------------------------------------
#if __BANG_ARCH__ >= 200
// 8bit
static __mlu_func__ __mlu_builtin__ void __ldramset(void* dst,
                                                    int elem_count,
                                                    char value) {
  __mlvm_memset_ldram_s8((char*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __ldramset(void* dst,
                                                    int elem_count,
                                                    unsigned char value) {
  __mlvm_memset_ldram_s8((char*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __ldramset(void* dst,
                                                    int elem_count,
                                                    signed char value) {
  __mlvm_memset_ldram_s8((char*)dst, elem_count, value);
}
// 16bit
static __mlu_func__ __mlu_builtin__ void __ldramset(void* dst,
                                                    int elem_count,
                                                    half value) {
  __mlvm_memset_ldram_f16((half*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __ldramset(void* dst,
                                                    int elem_count,
                                                    short value) {
  __mlvm_memset_ldram_s16((short*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __ldramset(void* dst,
                                                    int elem_count,
                                                    unsigned short value) {
  __mlvm_memset_ldram_s16((short*)dst, elem_count, value);
}
// 32bit
static __mlu_func__ __mlu_builtin__ void __ldramset(void* dst,
                                                    int elem_count,
                                                    float value) {
  __mlvm_memset_ldram_f32((float*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __ldramset(void* dst,
                                                    int elem_count,
                                                    int value) {
  __mlvm_memset_ldram_s32((int*)dst, elem_count, value);
}
static __mlu_func__ __mlu_builtin__ void __ldramset(void* dst,
                                                    int elem_count,
                                                    unsigned int value) {
  __mlvm_memset_ldram_s32((int*)dst, elem_count, value);
}
#endif  // __BANG_ARCH__ >= 200

#if __BANG_ARCH__ >= 300
// 8bit
static __mlu_func__ __mlu_builtin__ void __nramset(void* dst, int elem_count,
                                                   char value, int stride0,
                                                   int iter1, int stride1,
                                                   int iter2) {
  __mlvm_memset3d_nram_s8((char*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}

// 16bit
static __mlu_func__ __mlu_builtin__ void __nramset(void* dst,
                                                   int elem_count,
                                                   half value, int stride0,
                                                   int iter1, int stride1,
                                                   int iter2) {
  __mlvm_memset3d_nram_f16((half*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}
static __mlu_func__ __mlu_builtin__ void __nramset(void* dst,
                                                   int elem_count,
                                                   short value, int stride0,
                                                   int iter1, int stride1,
                                                   int iter2) {
  __mlvm_memset3d_nram_s16((short*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}
// 32bit
static __mlu_func__ __mlu_builtin__ void __nramset(void* dst,
                                                   int elem_count,
                                                   float value, int stride0,
                                                   int iter1, int stride1,
                                                   int iter2) {
  __mlvm_memset3d_nram_f32((float*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}
static __mlu_func__ __mlu_builtin__ void __nramset(void* dst,
                                                   int elem_count,
                                                   int value, int stride0,
                                                   int iter1, int stride1,
                                                   int iter2) {
  __mlvm_memset3d_nram_s32((int*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}

// 8bit
static __mlu_func__ __mlu_builtin__ void __sramset(void* dst, int elem_count,
                                                   char value, int stride0,
                                                   int iter1, int stride1,
                                                   int iter2) {
  __mlvm_memset3d_sram_s8((char*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}

// 16bit
static __mlu_func__ __mlu_builtin__ void __sramset(void* dst,
                                                   int elem_count,
                                                   half value, int stride0,
                                                   int iter1, int stride1,
                                                   int iter2) {
  __mlvm_memset3d_sram_f16((half*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}
static __mlu_func__ __mlu_builtin__ void __sramset(void* dst,
                                                   int elem_count,
                                                   short value, int stride0,
                                                   int iter1, int stride1,
                                                   int iter2) {
  __mlvm_memset3d_sram_s16((short*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}
// 32bit
static __mlu_func__ __mlu_builtin__ void __sramset(void* dst,
                                                   int elem_count,
                                                   float value, int stride0,
                                                   int iter1, int stride1,
                                                   int iter2) {
  __mlvm_memset3d_sram_f32((float*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}
static __mlu_func__ __mlu_builtin__ void __sramset(void* dst,
                                                   int elem_count,
                                                   int value, int stride0,
                                                   int iter1, int stride1,
                                                   int iter2) {
  __mlvm_memset3d_sram_s32((int*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}

// 8bit
static __mlu_func__ __mlu_builtin__ void __ldramset(void* dst, int elem_count,
                                                    char value, int stride0,
                                                    int iter1, int stride1,
                                                    int iter2) {
  __mlvm_memset3d_ldram_s8((char*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}

// 16bit
static __mlu_func__ __mlu_builtin__ void __ldramset(void* dst,
                                                    int elem_count,
                                                    half value, int stride0,
                                                    int iter1, int stride1,
                                                    int iter2) {
  __mlvm_memset3d_ldram_f16((half*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}
static __mlu_func__ __mlu_builtin__ void __ldramset(void* dst,
                                                    int elem_count,
                                                    short value, int stride0,
                                                    int iter1, int stride1,
                                                    int iter2) {
  __mlvm_memset3d_ldram_s16((short*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}
// 32bit
static __mlu_func__ __mlu_builtin__ void __ldramset(void* dst,
                                                    int elem_count,
                                                    float value, int stride0,
                                                    int iter1, int stride1,
                                                    int iter2) {
  __mlvm_memset3d_ldram_f32((float*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}
static __mlu_func__ __mlu_builtin__ void __ldramset(void* dst,
                                                    int elem_count,
                                                    int value, int stride0,
                                                    int iter1, int stride1,
                                                    int iter2) {
  __mlvm_memset3d_ldram_s32((int*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}

// 8bit
static __mlu_func__ __mlu_builtin__ void __gdramset(void* dst, int elem_count,
                                                    char value, int stride0,
                                                    int iter1, int stride1,
                                                    int iter2) {
  __mlvm_memset3d_gdram_s8((char*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}
// 16bit
static __mlu_func__ __mlu_builtin__ void __gdramset(void* dst,
                                                    int elem_count,
                                                    half value, int stride0,
                                                    int iter1, int stride1,
                                                    int iter2) {
  __mlvm_memset3d_gdram_f16((half*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}
static __mlu_func__ __mlu_builtin__ void __gdramset(void* dst,
                                                    int elem_count,
                                                    short value, int stride0,
                                                    int iter1, int stride1,
                                                    int iter2) {
  __mlvm_memset3d_gdram_s16((short*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}
// 32bit
static __mlu_func__ __mlu_builtin__ void __gdramset(void* dst,
                                                    int elem_count,
                                                    float value, int stride0,
                                                    int iter1, int stride1,
                                                    int iter2) {
  __mlvm_memset3d_gdram_f32((float*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}
static __mlu_func__ __mlu_builtin__ void __gdramset(void* dst,
                                                    int elem_count,
                                                    int value, int stride0,
                                                    int iter1, int stride1,
                                                    int iter2) {
  __mlvm_memset3d_gdram_s32((int*)dst, elem_count, value, stride0, iter1, stride1, iter2);
}
#endif  // __BANG_ARCH__ >= 300

// -----------------------------------------------------------------------------
// BANG C/C++ builtin functions: dump perf data.
// -----------------------------------------------------------------------------
#if (__BANG_ARCH__ >= 200) && (__BANG_ARCH__ < 300)
// Get executed instruction number.
static __mlu_func__ __mlu_builtin__ unsigned int __bang_perf_executed_inst() {
  unsigned int executed_inst = 0;
  __asm__ volatile("sync;\n\t"
                   "mv.sreg.gpr %%perf_read, 1;\n\t"
                   "mv.gpr.sreg %[executed_inst], %%perf_executed_inst_num;\n\t"
                   : [executed_inst]"+&r"(executed_inst));
  return executed_inst;
}

// Get cache miss number.
static __mlu_func__ __mlu_builtin__ unsigned int __bang_perf_cache_miss() {
  unsigned int cache_miss = 0;
  __asm__ volatile("sync;\n\t"
                   "mv.sreg.gpr %%perf_read, 1;\n\t"
                   "mv.gpr.sreg %[cache_miss], %%perf_cache_miss_num;\n\t"
                   : [cache_miss]"+&r"(cache_miss)
                  );
  return cache_miss;
}

// Get ALU working count (lower 32 bits).
static __mlu_func__ __mlu_builtin__ unsigned int __bang_perf_compute_alu_low() {
  unsigned int alu_low = 0;
  __asm__ volatile("sync;\n\t"
                   "mv.sreg.gpr %%perf_read, 1;\n\t"
                   "mv.gpr.sreg %[alu_low], %%perf_alu_working_count_low;\n\t"
                   : [alu_low]"+&r"(alu_low)
                  );
  return alu_low;
}

// Get ALU working count (higher 32 bits).
static __mlu_func__ __mlu_builtin__ unsigned int __bang_perf_compute_alu_high() {
  unsigned int alu_high = 0;
  __asm__ volatile("sync;\n\t"
                   "mv.sreg.gpr %%perf_read, 1;\n\t"
                   "mv.gpr.sreg %[alu_high], %%perf_alu_working_count_high;\n\t"
                   : [alu_high]"+&r"(alu_high)
                  );
  return alu_high;
}

// Get LT working count (cycle, lower 32 bits).
static __mlu_func__ __mlu_builtin__ unsigned int __bang_perf_compute_wfu_low() {
  unsigned int lt_low = 0;
  __asm__ volatile("sync;\n\t"
                   "mv.sreg.gpr %%perf_read, 1;\n\t"
                   "mv.gpr.sreg %[lt_low], %%perf_lt_working_count_low;\n\t"
                   : [lt_low]"+&r"(lt_low)
                  );
  return lt_low;
}

// Get LT working count (cycle, higher 32 bits).
static __mlu_func__ __mlu_builtin__ unsigned int __bang_perf_compute_wfu_high() {
  unsigned int lt_high = 0;
  __asm__ volatile("sync;\n\t"
                   "mv.sreg.gpr %%perf_read, 1;\n\t"
                   "mv.gpr.sreg %[lt_high], %%perf_lt_working_count_high;\n\t"
                   : [lt_high]"+&r"(lt_high)
                  );
  return lt_high;
}

// Get CT working count (cycle, lower 32 bits).
static __mlu_func__ __mlu_builtin__ unsigned int __bang_perf_compute_nfu_low() {
  unsigned int ct_low = 0;
  __asm__ volatile("sync;\n\t"
                   "mv.sreg.gpr %%perf_read, 1;\n\t"
                   "mv.gpr.sreg %[ct_low], %%perf_ct_working_count_low;\n\t"
                   : [ct_low]"+&r"(ct_low)
                  );
  return ct_low;
}

// Get CT working count (cycle, higher 32 bits).
static __mlu_func__ __mlu_builtin__ unsigned int __bang_perf_compute_nfu_high() {
  unsigned int ct_high = 0;
  __asm__ volatile("sync;\n\t"
                   "mv.sreg.gpr %%perf_read, 1;\n\t"
                   "mv.gpr.sreg %[ct_high], %%perf_ct_working_count_high;\n\t"
                   : [ct_high]"+&r"(ct_high)
                  );
  return ct_high;
}

// Get DRAM read size (Bytes, lower 32 bits).
static __mlu_func__ __mlu_builtin__ unsigned int __bang_perf_memory_dram_read_low() {
  unsigned int iodma_read_low = 0;
  __asm__ volatile("sync;\n\t"
                   "mv.sreg.gpr %%perf_read, 1;\n\t"
                   "mv.gpr.sreg %[iodma_read_low], %%perf_iodma_read_size_low;\n\t"
                   : [iodma_read_low]"+&r"(iodma_read_low)
                  );
  return iodma_read_low;
}

// Get DRAM read size (Bytes, higher 32 bits).
static __mlu_func__ __mlu_builtin__ unsigned int __bang_perf_memory_dram_read_high() {
  unsigned int iodma_read_high = 0;
  __asm__ volatile("sync;\n\t"
                   "mv.sreg.gpr %%perf_read, 1;\n\t"
                   "mv.gpr.sreg %[iodma_read_high], %%perf_iodma_read_size_high;\n\t"
                   : [iodma_read_high]"+&r"(iodma_read_high)
                  );
  return iodma_read_high;
}

// Get DRAM write size (Bytes, lower 32 bits).
static __mlu_func__ __mlu_builtin__ unsigned int __bang_perf_memory_dram_write_low() {
  unsigned int iodma_write_low = 0;
  __asm__ volatile("sync;\n\t"
                   "mv.sreg.gpr %%perf_read, 1;\n\t"
                   "mv.gpr.sreg %[iodma_write_low], %%perf_iodma_write_size_low;\n\t"
                   : [iodma_write_low]"+&r"(iodma_write_low)
                  );
  return iodma_write_low;
}

// Get DRAM write size (Bytes, higher 32 bits).
static __mlu_func__ __mlu_builtin__ unsigned int __bang_perf_memory_dram_write_high() {
  unsigned int iodma_write_high = 0;
  __asm__ volatile("sync;\n\t"
                   "mv.sreg.gpr %%perf_read, 1;\n\t"
                   "mv.gpr.sreg %[iodma_write_high], %%perf_iodma_write_size_high;\n\t"
                   : [iodma_write_high]"+&r"(iodma_write_high)
                  );
  return iodma_write_high;
}

// Get SRAM read size (Bytes, lower 32 bits).
static __mlu_func__ __mlu_builtin__ unsigned int __bang_perf_memory_sram_read_low() {
  unsigned int sram_read_low = 0;
  __asm__ volatile("sync;\n\t"
                   "mv.sreg.gpr %%perf_read, 1;\n\t"
                   "mv.gpr.sreg %[sram_read_low], %%perf_sram_read_size_low;\n\t"
                   : [sram_read_low]"+&r"(sram_read_low)
                  );
  return sram_read_low;
}

// Get SRAM read size (Bytes, higher 32 bits).
static __mlu_func__ __mlu_builtin__ unsigned int __bang_perf_memory_sram_read_high() {
  unsigned int sram_read_high = 0;
  __asm__ volatile("sync;\n\t"
                   "mv.sreg.gpr %%perf_read, 1;\n\t"
                   "mv.gpr.sreg %[sram_read_high], %%perf_sram_read_size_high;\n\t"
                   : [sram_read_high]"+&r"(sram_read_high)
                  );
  return sram_read_high;
}

// Get SRAM write size (Bytes, lower 32 bits).
static __mlu_func__ __mlu_builtin__ unsigned int __bang_perf_memory_sram_write_low() {
  unsigned int sram_write_low = 0;
  __asm__ volatile("sync;\n\t"
                   "mv.sreg.gpr %%perf_read, 1;\n\t"
                   "mv.gpr.sreg %[sram_write_low], %%perf_sram_write_size_low;\n\t"
                   : [sram_write_low]"+&r"(sram_write_low)
                  );
  return sram_write_low;
}

// Get SRAM write size (Bytes, higher 32 bits).
static __mlu_func__ __mlu_builtin__ unsigned int __bang_perf_memory_sram_write_high() {
  unsigned int sram_write_high = 0;
  __asm__ volatile("sync;\n\t"
                   "mv.sreg.gpr %%perf_read, 1;\n\t"
                   "mv.gpr.sreg %[sram_write_high], %%perf_sram_write_size_high;\n\t"
                   : [sram_write_high]"+&r"(sram_write_high)
                  );
  return sram_write_high;
}

// Get broadcast size (Bytes, lower 32 bits).
static __mlu_func__ __mlu_builtin__ unsigned int __bang_perf_memory_broadcast_low() {
  unsigned int broadcast_low = 0;
  __asm__ volatile("sync;\n\t"
                   "mv.sreg.gpr %%perf_read, 1;\n\t"
                   "mv.gpr.sreg %[broadcast_low], %%perf_broadcast_size_low;\n\t"
                   : [broadcast_low]"+&r"(broadcast_low)
                  );
  return broadcast_low;
}

// Get broadcast size (Bytes, higher 32 bits).
static __mlu_func__ __mlu_builtin__ unsigned int __bang_perf_memory_broadcast_high() {
  unsigned int broadcast_high = 0;
  __asm__ volatile("sync;\n\t"
                   "mv.sreg.gpr %%perf_read, 1;\n\t"
                   "mv.gpr.sreg %[broadcast_high], %%perf_broadcast_size_high;\n\t"
                   : [broadcast_high]"+&r"(broadcast_high)
                  );
  return broadcast_high;
}

#endif  // (__BANG_ARCH__ >= 200) && (__BANG_ARCH__ < 300)

#if __BANG_ARCH__ > 306
// -----------------------------------------------------------------------------
// TRC API
// -----------------------------------------------------------------------------
template<typename T>
static __mlu_func__ __mlu_builtin__ void __trc_create(tid<T>& tr, void* ptr,
                                                      int offset0, int size0,
                                                      int stride0, short offset1,
                                                      short size1, int stride1,
                                                      short offset2, short size2) {
  tr.tid = __mlvm_trc_create(ptr, offset0, size0, stride0, offset1,
                             size1, stride1, offset2, size2);
}

template<typename DT, typename ST>
static __mlu_func__ __mlu_builtin__ void __trc_memcpy(tid<DT>& dst, tid<ST>& src,
                                                      mluMemcpyDirection_t dir) {
  switch(dir) {
  default:
    // Unsupport memcpy.
    asm volatile("exit %[status];\n\t"
                 ::[status]"i"(TRAP_BANG_UNSUPPORTED_MEMCPY_DIR));
    break;
  case GDRAM2NRAM:  { __mlvm_trc_memcpy_gdram_to_nram(dst.tid, src.tid); break; }
  case NRAM2GDRAM:  { __mlvm_trc_memcpy_nram_to_gdram(dst.tid, src.tid); break; }
  case LDRAM2NRAM:  { __mlvm_trc_memcpy_ldram_to_nram(dst.tid, src.tid); break; }
  case NRAM2LDRAM:  { __mlvm_trc_memcpy_nram_to_ldram(dst.tid, src.tid); break; }
  case GDRAM2WRAM:  { __mlvm_trc_memcpy_gdram_to_wram(dst.tid, src.tid); break; }
  case WRAM2GDRAM:  { __mlvm_trc_memcpy_wram_to_gdram(dst.tid, src.tid); break; }
  case LDRAM2WRAM:  { __mlvm_trc_memcpy_ldram_to_wram(dst.tid, src.tid); break; }
  case WRAM2LDRAM:  { __mlvm_trc_memcpy_wram_to_ldram(dst.tid, src.tid); break; }
  case NRAM2NRAM:   { __mlvm_trc_memcpy_nram_to_nram(dst.tid, src.tid); break; }
  case NRAM2WRAM:   { __mlvm_trc_memcpy_nram_to_wram(dst.tid, src.tid); break; }
  case WRAM2NRAM:   { __mlvm_trc_memcpy_wram_to_nram(dst.tid, src.tid); break; }
#if __MLU_SRAM_SIZE__ != 0
  case LDRAM2SRAM:  { __mlvm_trc_memcpy_ldram_to_sram(dst.tid, src.tid); break; }
  case SRAM2LDRAM:  { __mlvm_trc_memcpy_sram_to_ldram(dst.tid, src.tid); break; }
  case SRAM2NRAM:   { __mlvm_trc_memcpy_sram_to_nram(dst.tid, src.tid); break; }
  case NRAM2SRAM:   { __mlvm_trc_memcpy_nram_to_sram(dst.tid, src.tid); break; }
  case SRAM2WRAM:   { __mlvm_trc_memcpy_sram_to_wram(dst.tid, src.tid); break; }
  case WRAM2SRAM:   { __mlvm_trc_memcpy_wram_to_sram(dst.tid, src.tid); break; }
  case GDRAM2SRAM:  { __mlvm_trc_memcpy_gdram_to_sram(dst.tid, src.tid); break; }
  case SRAM2GDRAM:  { __mlvm_trc_memcpy_sram_to_gdram(dst.tid, src.tid); break; }
#endif
  case GDRAM2GDRAM: { __mlvm_trc_memcpy_gdram_to_gdram(dst.tid, src.tid); break; }
  case GDRAM2LDRAM: { __mlvm_trc_memcpy_gdram_to_ldram(dst.tid, src.tid); break; }
  case LDRAM2GDRAM: { __mlvm_trc_memcpy_ldram_to_gdram(dst.tid, src.tid); break; }
  }
}

#if __MLU_SRAM_SIZE__ != 0
template<typename DT, typename ST>
static __mlu_func__ __mlu_builtin__ void __trc_memcpy(tid<DT>& dst, tid<ST>& src,
                                                      mluMemcpyDirection_t dir,
                                                      int id_dst_cluster,
                                                      int id_src_cluster) {
  if (dir != SRAM2SRAM) {
    // Unsupport memcpy direction.
    asm volatile("exit %[status];\n\t"
                 ::[status]"i"(TRAP_BANG_UNSUPPORTED_MEMCPY_DIR));
  }
  __mlvm_trc_memcpy_sram_to_sram(dst.tid, src.tid, id_dst_cluster, id_src_cluster);
}
#endif

// 8bit
template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_nramset(tid<T>& dst,
                                                       char value) {
  __mlvm_trc_memset_nram_s8(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_nramset(tid<T>& dst,
                                                       unsigned char value) {
  __mlvm_trc_memset_nram_s8(dst.tid, value);
}

// 16bit
template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_nramset(tid<T>& dst,
                                                       half value) {
  __mlvm_trc_memset_nram_f16(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_nramset(tid<T>& dst,
                                                       short value) {
  __mlvm_trc_memset_nram_s16(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_nramset(tid<T>& dst,
                                                       unsigned short value) {
  __mlvm_trc_memset_nram_u16(dst.tid, value);
}

// 32bit
template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_nramset(tid<T>& dst,
                                                       float value) {
  __mlvm_trc_memset_nram_f32(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_nramset(tid<T>& dst,
                                                       int value) {
  __mlvm_trc_memset_nram_s32(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_nramset(tid<T>& dst,
                                                       unsigned int value) {
  __mlvm_trc_memset_nram_u32(dst.tid, value);
}

// 8bit
#if __MLU_SRAM_SIZE__ != 0
template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_sramset(tid<T>& dst,
                                                       char value) {
  __mlvm_trc_memset_sram_s8(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_sramset(tid<T>& dst,
                                                       unsigned char value) {
  __mlvm_trc_memset_sram_s8(dst.tid, value);
}

// 16bit
template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_sramset(tid<T>& dst,
                                                       half value) {
  __mlvm_trc_memset_sram_f16(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_sramset(tid<T>& dst,
                                                       short value) {
  __mlvm_trc_memset_sram_s16(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_sramset(tid<T>& dst,
                                                       unsigned short value) {
  __mlvm_trc_memset_sram_u16(dst.tid, value);
}

// 32bit
template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_sramset(tid<T>& dst,
                                                       float value) {
  __mlvm_trc_memset_sram_f32(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_sramset(tid<T>& dst,
                                                       int value) {
  __mlvm_trc_memset_sram_s32(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_sramset(tid<T>& dst,
                                                       unsigned int value) {
  __mlvm_trc_memset_sram_u32(dst.tid, value);
}
#endif

// 8bit
template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_gdramset(tid<T>& dst,
                                                        char value) {
  __mlvm_trc_memset_gdram_s8(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_gdramset(tid<T>& dst,
                                                        unsigned char value) {
  __mlvm_trc_memset_gdram_s8(dst.tid, value);
}

// 16bit
template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_gdramset(tid<T>& dst,
                                                        half value) {
  __mlvm_trc_memset_gdram_f16(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_gdramset(tid<T>& dst,
                                                        short value) {
  __mlvm_trc_memset_gdram_s16(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_gdramset(tid<T>& dst,
                                                        unsigned short value) {
  __mlvm_trc_memset_gdram_u16(dst.tid, value);
}

// 32bit
template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_gdramset(tid<T>& dst,
                                                        float value) {
  __mlvm_trc_memset_gdram_f32(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_gdramset(tid<T>& dst,
                                                        int value) {
  __mlvm_trc_memset_gdram_s32(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_gdramset(tid<T>& dst,
                                                        unsigned int value) {
  __mlvm_trc_memset_gdram_u32(dst.tid, value);
}

// 8bit
template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_ldramset(tid<T>& dst,
                                                       char value) {
  __mlvm_trc_memset_ldram_s8(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_ldramset(tid<T>& dst,
                                                        unsigned char value) {
  __mlvm_trc_memset_ldram_s8(dst.tid, value);
}

// 16bit
template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_ldramset(tid<T>& dst,
                                                        half value) {
  __mlvm_trc_memset_ldram_f16(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_ldramset(tid<T>& dst,
                                                        short value) {
  __mlvm_trc_memset_ldram_s16(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_ldramset(tid<T>& dst,
                                                        unsigned short value) {
   __mlvm_trc_memset_ldram_u16(dst.tid, value);
}

// 32bit
template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_ldramset(tid<T>& dst,
                                                        float value) {
  __mlvm_trc_memset_ldram_f32(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_ldramset(tid<T>& dst,
                                                        int value) {
  __mlvm_trc_memset_ldram_s32(dst.tid, value);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_ldramset(tid<T>& dst,
                                                        unsigned int value) {
  __mlvm_trc_memset_ldram_u32(dst.tid, value);
}

template<typename T>
static __mlu_func__ __mlu_builtin__ void __trc_slice(tid<T>& dst, tid<T>& src,
                                                     int offset0, int size0,
                                                     short offset1, short size1,
                                                     short offset2, short size2) {
  dst.tid = __mlvm_trc_slice(src.tid, offset0, size0, offset1, size1, offset2, size2);
}

#if __BANG_ARCH__ >= 320
#define __trc_active_f16(dst, src, activetab, exp_bases, seg_num) \
    __mlvm_trc_active_f16(dst.tid, src.tid, activetab.tid, exp_bases.tid, seg_num)
#define __trc_active_f32(dst, src, activetab, exp_bases, seg_num) \
    __mlvm_trc_active_f32(dst.tid, src.tid, activetab.tid, exp_bases.tid, seg_num)
#endif //__BANG_ARCH__ >= 320

static __mlu_func__ __mlu_builtin__ void __trc_active_sigmoid(tid<float>& dst,
                                                              tid<float>& src) {
  __mlvm_trc_active_sigmoid_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_sigmoid(tid<half>& dst,
                                                              tid<half>& src) {
  __mlvm_trc_active_sigmoid_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_cos(tid<float>& dst,
                                                          tid<float>& src) {
  __mlvm_trc_active_cos_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_cos(tid<half>& dst,
                                                          tid<half>& src) {
  __mlvm_trc_active_cos_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_exp(tid<float>& dst,
                                                          tid<float>& src) {
  __mlvm_trc_active_exp_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_exp(tid<half>& dst,
                                                          tid<half>& src) {
  __mlvm_trc_active_exp_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_sqrt(tid<float>& dst,
                                                           tid<float>& src) {
  __mlvm_trc_active_sqrt_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_sqrt(tid<half>& dst,
                                                           tid<half>& src) {
  __mlvm_trc_active_sqrt_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_sin(tid<float>& dst,
                                                          tid<float>& src) {
  __mlvm_trc_active_sin_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_sin(tid<half>& dst,
                                                          tid<half>& src) {
  __mlvm_trc_active_sin_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_tanh(tid<float>& dst,
                                                           tid<float>& src) {
  __mlvm_trc_active_tanh_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_tanh(tid<half>& dst,
                                                           tid<half>& src) {
  __mlvm_trc_active_tanh_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_sign(tid<float>& dst,
                                                           tid<float>& src) {
  __mlvm_trc_active_sign_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_sign(tid<half>& dst,
                                                           tid<half>& src) {
  __mlvm_trc_active_sign_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_pow2(tid<float>& dst,
                                                           tid<float>& src) {
  __mlvm_trc_active_pow2_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_pow2(tid<half>& dst,
                                                           tid<half>& src) {
  __mlvm_trc_active_pow2_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_gelu(tid<float>& dst,
                                                           tid<float>& src) {
  __mlvm_trc_active_gelu_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_gelu(tid<half>& dst,
                                                           tid<half>& src) {
  __mlvm_trc_active_gelu_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_exphp(tid<float>& dst,
                                                            tid<float>& src) {
  __mlvm_trc_active_exphp_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_exphp(tid<half>& dst,
                                                            tid<half>& src) {
  __mlvm_trc_active_exphp_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_explt0(tid<float>& dst,
                                                             tid<float>& src) {
  __mlvm_trc_active_explt0_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_explt0(tid<half>& dst,
                                                             tid<half>& src) {
  __mlvm_trc_active_explt0_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_sqrthp(tid<float>& dst,
                                                             tid<float>& src) {
  __mlvm_trc_active_sqrthp_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_sqrthp(tid<half>& dst,
                                                             tid<half>& src) {
  __mlvm_trc_active_sqrthp_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_rsqrt(tid<float>& dst,
                                                            tid<float>& src) {
  __mlvm_trc_active_rsqrt_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_rsqrt(tid<half>& dst,
                                                            tid<half>& src) {
  __mlvm_trc_active_rsqrt_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_rsqrthp(tid<float>& dst,
                                                              tid<float>& src) {
  __mlvm_trc_active_rsqrthp_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_rsqrthp(tid<half>& dst,
                                                              tid<half>& src) {
  __mlvm_trc_active_rsqrthp_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_reciprocal(tid<float>& dst,
                                                                 tid<float>& src) {
  __mlvm_trc_active_reciprocal_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_reciprocal(tid<half>& dst,
                                                                 tid<half>& src) {
  __mlvm_trc_active_reciprocal_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_reciprocalhp(tid<float>& dst,
                                                                   tid<float>& src) {
  __mlvm_trc_active_reciprocalhp_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_reciprocalhp(tid<half>& dst,
                                                                   tid<half>& src) {
  __mlvm_trc_active_reciprocalhp_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_recipgt1(tid<float>& dst,
                                                               tid<float>& src) {
  __mlvm_trc_active_recipgt1_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_recipgt1(tid<half>& dst,
                                                               tid<half>& src) {
  __mlvm_trc_active_recipgt1_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_log(tid<float>& dst,
                                                          tid<float>& src) {
  __mlvm_trc_active_log_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_log(tid<half>& dst,
                                                          tid<half>& src) {
  __mlvm_trc_active_log_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_loghp(tid<float>& dst,
                                                            tid<float>& src) {
  __mlvm_trc_active_loghp_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_loghp(tid<half>& dst,
                                                            tid<half>& src) {
  __mlvm_trc_active_loghp_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_geluhp(tid<float>& dst,
                                                             tid<float>& src) {
  __mlvm_trc_active_geluhp_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_active_geluhp(tid<half>& dst,
                                                             tid<half>& src) {
  __mlvm_trc_active_geluhp_f16(dst.tid, src.tid);
}

#if __BANG_ARCH__ >= 320
#define __trc_lut_s16(dst, src, table, size) \
    __mlvm_trc_lut_s16(dst.tid, src.tid, table.tid, size)
#define __trc_lut_s32(dst, src, table, size) \
    __mlvm_trc_lut_s32(dst.tid, src.tid, table.tid, size)
#define __trc_lut_s8(dst, src, table, size) \
    __mlvm_trc_lut_s8(dst.tid, src.tid, table.tid, size)
#endif //__BANG_ARCH__ >= 320

static __mlu_func__ __mlu_builtin__ void __trc_add(tid<float>& dst,
                                                   tid<float>& src1,
                                                   tid<float>& src2) {
  __mlvm_trc_add_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_add(tid<half>& dst,
                                                   tid<half>& src1,
                                                   tid<half>& src2) {
  __mlvm_trc_add_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_add(tid<float>& dst,
                                                   tid<float>& src1,
                                                   float src2) {
  __mlvm_trc_add_scalar_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_add(tid<half>& dst,
                                                   tid<half>& src1,
                                                   half src2) {
  __mlvm_trc_add_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_sub(tid<float>& dst,
                                                   tid<float>& src1,
                                                   tid<float>& src2) {
  __mlvm_trc_sub_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_sub(tid<half>& dst,
                                                   tid<half>& src1,
                                                   tid<half>& src2) {
  __mlvm_trc_sub_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_sub(tid<float>& dst,
                                                   tid<float>& src1,
                                                   float src2) {
  __mlvm_trc_sub_scalar_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_sub(tid<half>& dst,
                                                   tid<half>& src1,
                                                   half src2) {
  __mlvm_trc_sub_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_mult(tid<float>& dst,
                                                    tid<float>& src1,
                                                    tid<float>& src2) {
  __mlvm_trc_mult_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_mult(tid<half>& dst,
                                                    tid<half>& src1,
                                                    tid<half>& src2) {
  __mlvm_trc_mult_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_mult(tid<float>& dst,
                                                    tid<float>& src1,
                                                    float src2) {
  __mlvm_trc_mult_scalar_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_mult(tid<half>& dst,
                                                    tid<half>& src1,
                                                    half src2) {
  __mlvm_trc_mult_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_maxequal(tid<float>& dst,
                                                        tid<float>& src1,
                                                        tid<float>& src2) {
  __mlvm_trc_maxequal_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_maxequal(tid<half>& dst,
                                                        tid<half>& src1,
                                                        tid<half>& src2) {
  __mlvm_trc_maxequal_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_maxequal(tid<float>& dst,
                                                        tid<float>& src1,
                                                        float src2) {
  __mlvm_trc_maxequal_scalar_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_maxequal(tid<half>& dst,
                                                        tid<half>& src1,
                                                        half src2) {
  __mlvm_trc_maxequal_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_minequal(tid<float>& dst,
                                                        tid<float>& src1,
                                                        tid<float>& src2) {
  __mlvm_trc_minequal_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_minequal(tid<half>& dst,
                                                        tid<half>& src1,
                                                        tid<half>& src2) {
  __mlvm_trc_minequal_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_minequal(tid<float>& dst,
                                                        tid<float>& src1,
                                                        float src2) {
  __mlvm_trc_minequal_scalar_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_minequal(tid<half>& dst,
                                                        tid<half>& src1,
                                                        half src2) {
  __mlvm_trc_minequal_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_relun(tid<float>& dst,
                                                     tid<float>& src1,
                                                     float src2) {
  __mlvm_trc_relun_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_relun(tid<half>& dst,
                                                     tid<half>& src1,
                                                     half src2) {
  __mlvm_trc_relun_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_abs(tid<float>& dst,
                                                   tid<float>& src) {
  __mlvm_trc_abs_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_abs(tid<half>& dst,
                                                   tid<half>& src) {
  __mlvm_trc_abs_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_square(tid<float>& dst,
                                                      tid<float>& src) {
  __mlvm_trc_square_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_square(tid<half>& dst,
                                                      tid<half>& src) {
  __mlvm_trc_square_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_accsum(tid<float>& dst,
                                                      tid<float>& src) {
  __mlvm_trc_accsum_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_accsum(tid<half>& dst,
                                                      tid<half>& src) {
  __mlvm_trc_accsum_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_accsum_hp(tid<half>& dst,
                                                         tid<half>& src) {
  __mlvm_trc_accsum_hp_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_relu(tid<float>& dst,
                                                    tid<float>& src) {
  __mlvm_trc_relu_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_relu(tid<half>& dst,
                                                    tid<half>& src) {
  __mlvm_trc_relu_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_and(tid<float>& dst,
                                                   tid<float>& src1,
                                                   tid<float>& src2) {
  __mlvm_trc_and_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_and(tid<half>& dst,
                                                   tid<half>& src1,
                                                   tid<half>& src2) {
  __mlvm_trc_and_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_and(tid<float>& dst,
                                                   tid<float>& src1,
                                                   float src2) {
  __mlvm_trc_and_scalar_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_and(tid<half>& dst,
                                                   tid<half>& src1,
                                                   half src2) {
  __mlvm_trc_and_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_band(tid<float>& dst,
                                                    tid<float>& src1,
                                                    tid<float>& src2) {
  __mlvm_trc_band_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_band(tid<half>& dst,
                                                    tid<half>& src1,
                                                    tid<half>& src2) {
  __mlvm_trc_band_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_band(tid<float>& dst,
                                                    tid<float>& src1,
                                                    float src2) {
  __mlvm_trc_band_scalar_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_band(tid<half>& dst,
                                                    tid<half>& src1,
                                                    half src2) {
  __mlvm_trc_band_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_or(tid<float>& dst,
                                                  tid<float>& src1,
                                                  tid<float>& src2) {
  __mlvm_trc_or_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_or(tid<half>& dst,
                                                  tid<half>& src1,
                                                  tid<half>& src2) {
  __mlvm_trc_or_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_or(tid<float>& dst,
                                                  tid<float>& src1,
                                                  float src2) {
  __mlvm_trc_or_scalar_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_or(tid<half>& dst,
                                                  tid<half>& src1,
                                                  half src2) {
  __mlvm_trc_or_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_bor(tid<float>& dst,
                                                   tid<float>& src1,
                                                   tid<float>& src2) {
  __mlvm_trc_bor_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_bor(tid<half>& dst,
                                                   tid<half>& src1,
                                                   tid<half>& src2) {
  __mlvm_trc_bor_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_bor(tid<float>& dst,
                                                   tid<float>& src1,
                                                   float src2) {
  __mlvm_trc_bor_scalar_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_bor(tid<half>& dst,
                                                   tid<half>& src1,
                                                   half src2) {
  __mlvm_trc_bor_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_xor(tid<float>& dst,
                                                   tid<float>& src1,
                                                   tid<float>& src2) {
  __mlvm_trc_xor_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_xor(tid<half>& dst,
                                                   tid<half>& src1,
                                                   tid<half>& src2) {
  __mlvm_trc_xor_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_xor(tid<float>& dst,
                                                   tid<float>& src1,
                                                   float src2) {
  __mlvm_trc_xor_scalar_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_xor(tid<half>& dst,
                                                   tid<half>& src1,
                                                   half src2) {
  __mlvm_trc_xor_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_bxor(tid<float>& dst,
                                                    tid<float>& src1,
                                                    tid<float>& src2) {
  __mlvm_trc_bxor_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_bxor(tid<half>& dst,
                                                    tid<half>& src1,
                                                    tid<half>& src2) {
  __mlvm_trc_bxor_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_bxor(tid<float>& dst,
                                                    tid<float>& src1,
                                                    float src2) {
  __mlvm_trc_bxor_scalar_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_bxor(tid<half>& dst,
                                                    tid<half>& src1,
                                                    half src2) {
  __mlvm_trc_bxor_scalar_f16(dst.tid, src1.tid, src2);
}
static __mlu_func__ __mlu_builtin__ void __trc_not(tid<float>& dst,
                                                   tid<float>& src) {
  __mlvm_trc_not_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_not(tid<half>& dst,
                                                   tid<half>& src) {
  __mlvm_trc_not_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_bnot(tid<float>& dst,
                                                    tid<float>& src) {
  __mlvm_trc_bnot_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_bnot(tid<half>& dst,
                                                    tid<half>& src) {
  __mlvm_trc_bnot_f16(dst.tid, src.tid);
}


static __mlu_func__ __mlu_builtin__ void __trc_eq(tid<float>& dst,
                                                  tid<float>& src1,
                                                  tid<float>& src2) {
  __mlvm_trc_eq_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_eq(tid<half>& dst,
                                                  tid<half>& src1,
                                                  tid<half>& src2) {
  __mlvm_trc_eq_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_eq(tid<float>& dst,
                                                  tid<float>& src1,
                                                  float src2) {
  __mlvm_trc_eq_scalar_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_eq(tid<half>& dst,
                                                  tid<half>& src1,
                                                  half src2) {
  __mlvm_trc_eq_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_eq_bitindex(tid<float>& dst,
                                                           tid<float>& src1,
                                                           tid<float>& src2) {
  __mlvm_trc_eq_bitindex_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_eq_bitindex(tid<half>& dst,
                                                           tid<half>& src1,
                                                           tid<half>& src2) {
  __mlvm_trc_eq_bitindex_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_eq_bitindex(tid<float>& dst,
                                                           tid<float>& src1,
                                                           float src2) {
  __mlvm_trc_eq_scalar_bitindex_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_eq_bitindex(tid<half>& dst,
                                                           tid<half>& src1,
                                                           half src2) {
  __mlvm_trc_eq_scalar_bitindex_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_ne(tid<float>& dst,
                                                  tid<float>& src1,
                                                  tid<float>& src2) {
  __mlvm_trc_ne_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_ne(tid<half>& dst,
                                                  tid<half>& src1,
                                                  tid<half>& src2) {
  __mlvm_trc_ne_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_ne(tid<float>& dst,
                                                  tid<float>& src1,
                                                  float src2) {
  __mlvm_trc_ne_scalar_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_ne(tid<half>& dst,
                                                  tid<half>& src1,
                                                  half src2) {
  __mlvm_trc_ne_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_ne_bitindex(tid<float>& dst,
                                                           tid<float>& src1,
                                                           tid<float>& src2) {
  __mlvm_trc_ne_bitindex_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_ne_bitindex(tid<half>& dst,
                                                           tid<half>& src1,
                                                           tid<half>& src2) {
  __mlvm_trc_ne_bitindex_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_ne_bitindex(tid<float>& dst,
                                                           tid<float>& src1,
                                                           float src2) {
  __mlvm_trc_ne_scalar_bitindex_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_ne_bitindex(tid<half>& dst,
                                                           tid<half>& src1,
                                                           half src2) {
  __mlvm_trc_ne_scalar_bitindex_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_gt(tid<float>& dst,
                                                  tid<float>& src1,
                                                  tid<float>& src2) {
  __mlvm_trc_gt_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_gt(tid<half>& dst,
                                                  tid<half>& src1,
                                                  tid<half>& src2) {
  __mlvm_trc_gt_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_gt(tid<float>& dst,
                                                  tid<float>& src1,
                                                  float src2) {
  __mlvm_trc_gt_scalar_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_gt(tid<half>& dst,
                                                  tid<half>& src1,
                                                  half src2) {
  __mlvm_trc_gt_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_gt_bitindex(tid<float>& dst,
                                                           tid<float>& src1,
                                                           tid<float>& src2) {
  __mlvm_trc_gt_bitindex_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_gt_bitindex(tid<half>& dst,
                                                           tid<half>& src1,
                                                           tid<half>& src2) {
  __mlvm_trc_gt_bitindex_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_gt_bitindex(tid<float>& dst,
                                                           tid<float>& src1,
                                                           float src2) {
  __mlvm_trc_gt_scalar_bitindex_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_gt_bitindex(tid<half>& dst,
                                                           tid<half>& src1,
                                                           half src2) {
  __mlvm_trc_gt_scalar_bitindex_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_ge(tid<float>& dst,
                                                  tid<float>& src1,
                                                  tid<float>& src2) {
  __mlvm_trc_ge_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_ge(tid<half>& dst,
                                                  tid<half>& src1,
                                                  tid<half>& src2) {
  __mlvm_trc_ge_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_ge(tid<float>& dst,
                                                  tid<float>& src1,
                                                  float src2) {
  __mlvm_trc_ge_scalar_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_ge(tid<half>& dst,
                                                  tid<half>& src1,
                                                  half src2) {
  __mlvm_trc_ge_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_ge_bitindex(tid<float>& dst,
                                                           tid<float>& src1,
                                                           tid<float>& src2) {
  __mlvm_trc_ge_bitindex_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_ge_bitindex(tid<half>& dst,
                                                           tid<half>& src1,
                                                           tid<half>& src2) {
  __mlvm_trc_ge_bitindex_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_ge_bitindex(tid<float>& dst,
                                                           tid<float>& src1,
                                                           float src2) {
  __mlvm_trc_ge_scalar_bitindex_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_ge_bitindex(tid<half>& dst,
                                                           tid<half>& src1,
                                                           half src2) {
  __mlvm_trc_ge_scalar_bitindex_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_lt(tid<float>& dst,
                                                  tid<float>& src1,
                                                  tid<float>& src2) {
  __mlvm_trc_lt_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_lt(tid<float>& dst,
                                                  tid<float>& src1,
                                                  float src2) {
  __mlvm_trc_lt_scalar_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_lt(tid<half>& dst,
                                                  tid<half>& src1,
                                                  half src2) {
  __mlvm_trc_lt_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_lt(tid<half>& dst,
                                                  tid<half>& src1,
                                                  tid<half>& src2) {
  __mlvm_trc_lt_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_lt_bitindex(tid<float>& dst,
                                                           tid<float>& src1,
                                                           tid<float>& src2) {
  __mlvm_trc_lt_bitindex_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_lt_bitindex(tid<half>& dst,
                                                           tid<half>& src1,
                                                           tid<half>& src2) {
  __mlvm_trc_lt_bitindex_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_lt_bitindex(tid<float>& dst,
                                                           tid<float>& src1,
                                                           float src2) {
  __mlvm_trc_lt_scalar_bitindex_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_lt_bitindex(tid<half>& dst,
                                                           tid<half>& src1,
                                                           half src2) {
  __mlvm_trc_lt_scalar_bitindex_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_le(tid<float>& dst,
                                                  tid<float>& src1,
                                                  tid<float>& src2) {
  __mlvm_trc_le_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_le(tid<half>& dst,
                                                  tid<half>& src1,
                                                  tid<half>& src2) {
  __mlvm_trc_le_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_le(tid<float>& dst,
                                                  tid<float>& src1,
                                                  float src2) {
  __mlvm_trc_le_scalar_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_le(tid<half>& dst,
                                                  tid<half>& src1,
                                                  half src2) {
  __mlvm_trc_le_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_le_bitindex(tid<float>& dst,
                                                           tid<float>& src1,
                                                           tid<float>& src2) {
  __mlvm_trc_le_bitindex_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_le_bitindex(tid<half>& dst,
                                                           tid<half>& src1,
                                                           tid<half>& src2) {
  __mlvm_trc_le_bitindex_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_le_bitindex(tid<float>& dst,
                                                           tid<float>& src1,
                                                           float src2) {
  __mlvm_trc_le_scalar_bitindex_f32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_le_bitindex(tid<half>& dst,
                                                           tid<half>& src1,
                                                           half src2) {
  __mlvm_trc_le_scalar_bitindex_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<int16>& src,
                                                    tid<int16>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_f32_int16_int16(dst.tid, src.tid, kernel.tid, kh, kw, pos, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<int16>& src,
                                                    tid<int8>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_f32_int16_int8(dst.tid, src.tid, kernel.tid, kh, kw, pos, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<int8>& src,
                                                    tid<int16>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_f32_int8_int16(dst.tid, src.tid, kernel.tid, kh, kw, pos, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<int8>& src,
                                                    tid<int8>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_f32_int8_int8(dst.tid, src.tid, kernel.tid, kh, kw, pos, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<half>& dst,
                                                    tid<int16>& src,
                                                    tid<int16>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_f16_int16_int16(dst.tid, src.tid, kernel.tid, kh, kw, pos, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<half>& dst,
                                                    tid<int16>& src,
                                                    tid<int8>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_f16_int16_int8(dst.tid, src.tid, kernel.tid, kh, kw, pos, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<half>& dst,
                                                    tid<int8>& src,
                                                    tid<int16>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_f16_int8_int16(dst.tid, src.tid, kernel.tid, kh, kw, pos, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<half>& dst,
                                                    tid<int8>& src,
                                                    tid<int8>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_f16_int8_int8(dst.tid, src.tid, kernel.tid, kh, kw, pos, sh, sw);
}

#if __BANG_ARCH__ != 322
static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<float>& src,
                                                    tid<float>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_f32_f32_f32(dst.tid, src.tid, kernel.tid, kh, kw, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<float>& src,
                                                    tid<half>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_f32_f32_f16(dst.tid, src.tid, kernel.tid, kh, kw, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<half>& src,
                                                    tid<float>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_f32_f16_f32(dst.tid, src.tid, kernel.tid, kh, kw, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<half>& src,
                                                    tid<half>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_f32_f16_f16(dst.tid, src.tid, kernel.tid, kh, kw, sh, sw);
}
#endif // __BANG_ARCH != 322

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<int16>& src,
                                                    tid<int16>& kernel,
                                                    tid<float>& bias,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_bias_f32_int16_int16(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, pos, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<int16>& src,
                                                    tid<int8>& kernel,
                                                    tid<float>& bias,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_bias_f32_int16_int8(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, pos, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<int8>& src,
                                                    tid<int16>& kernel,
                                                    tid<float>& bias,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_bias_f32_int8_int16(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, pos, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<int8>& src,
                                                    tid<int8>& kernel,
                                                    tid<float>& bias,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_bias_f32_int8_int8(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, pos, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<half>& dst,
                                                    tid<int16>& src,
                                                    tid<int16>& kernel,
                                                    tid<half>& bias,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_bias_f16_int16_int16(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, pos, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<half>& dst,
                                                    tid<int16>& src,
                                                    tid<int8>& kernel,
                                                    tid<half>& bias,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_bias_f16_int16_int8(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, pos, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<half>& dst,
                                                    tid<int8>& src,
                                                    tid<int16>& kernel,
                                                    tid<half>& bias,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_bias_f16_int8_int16(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, pos, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<half>& dst,
                                                    tid<int8>& src,
                                                    tid<int8>& kernel,
                                                    tid<half>& bias,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_bias_f16_int8_int8(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, pos, sh, sw);
}

#if __BANG_ARCH__ != 322
static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<float>& src,
                                                    tid<float>& kernel,
                                                    tid<float>& bias,
                                                    short kh,
                                                    short kw,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_bias_f32_f32_f32(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<float>& src,
                                                    tid<half>& kernel,
                                                    tid<float>& bias,
                                                    short kh,
                                                    short kw,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_bias_f32_f32_f16(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<half>& src,
                                                    tid<float>& kernel,
                                                    tid<float>& bias,
                                                    short kh,
                                                    short kw,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_bias_f32_f16_f32(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, sh, sw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<half>& src,
                                                    tid<half>& kernel,
                                                    tid<float>& bias,
                                                    short kh,
                                                    short kw,
                                                    short sh,
                                                    short sw) {
  __mlvm_trc_conv_bias_f32_f16_f16(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, sh, sw);
}
#endif // __BANG_ARCH__ != 322

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<int16>& src,
                                                    tid<int16>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_dilation_f32_int16_int16(dst.tid, src.tid, kernel.tid, kh, kw, pos,
                                           sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<int16>& src,
                                                    tid<int8>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_dilation_f32_int16_int8(dst.tid, src.tid, kernel.tid, kh, kw, pos,
                                          sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<int8>& src,
                                                    tid<int16>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_dilation_f32_int8_int16(dst.tid, src.tid, kernel.tid, kh, kw, pos,
                                          sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<int8>& src,
                                                    tid<int8>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_dilation_f32_int8_int8(dst.tid, src.tid, kernel.tid, kh, kw, pos,
                                         sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<half>& dst,
                                                    tid<int16>& src,
                                                    tid<int16>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_dilation_f16_int16_int16(dst.tid, src.tid, kernel.tid, kh, kw, pos,
                                           sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<half>& dst,
                                                    tid<int16>& src,
                                                    tid<int8>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_dilation_f16_int16_int8(dst.tid, src.tid, kernel.tid, kh, kw, pos,
                                          sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<half>& dst,
                                                    tid<int8>& src,
                                                    tid<int16>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_dilation_f16_int8_int16(dst.tid, src.tid, kernel.tid, kh, kw, pos,
                                          sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<half>& dst,
                                                    tid<int8>& src,
                                                    tid<int8>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_dilation_f16_int8_int8(dst.tid, src.tid, kernel.tid, kh, kw, pos,
                                         sh, sw, in_dh, in_dw, out_dh, out_dw);
}

#if __BANG_ARCH__ != 322
static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<float>& src,
                                                    tid<float>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_dilation_f32_f32_f32(dst.tid, src.tid, kernel.tid, kh, kw,
                                       sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<float>& src,
                                                    tid<half>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_dilation_f32_f32_f16(dst.tid, src.tid, kernel.tid, kh, kw,
                                       sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<half>& src,
                                                    tid<float>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_dilation_f32_f16_f32(dst.tid, src.tid, kernel.tid, kh, kw,
                                       sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<half>& src,
                                                    tid<half>& kernel,
                                                    short kh,
                                                    short kw,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_dilation_f32_f16_f16(dst.tid, src.tid, kernel.tid, kh, kw,
                                       sh, sw, in_dh, in_dw, out_dh, out_dw);
}
#endif // __BANG_ARCH__ != 322

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<int16>& src,
                                                    tid<int16>& kernel,
                                                    tid<float>& bias,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_bias_dilation_f32_int16_int16(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, pos,
                                                sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<int16>& src,
                                                    tid<int8>& kernel,
                                                    tid<float>& bias,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_bias_dilation_f32_int16_int8(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, pos,
                                               sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<int8>& src,
                                                    tid<int16>& kernel,
                                                    tid<float>& bias,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_bias_dilation_f32_int8_int16(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, pos,
                                               sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<int8>& src,
                                                    tid<int8>& kernel,
                                                    tid<float>& bias,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_bias_dilation_f32_int8_int8(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, pos,
                                              sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<half>& dst,
                                                    tid<int16>& src,
                                                    tid<int16>& kernel,
                                                    tid<half>& bias,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_bias_dilation_f16_int16_int16(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, pos,
                                                sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<half>& dst,
                                                    tid<int16>& src,
                                                    tid<int8>& kernel,
                                                    tid<half>& bias,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_bias_dilation_f16_int16_int8(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, pos,
                                               sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<half>& dst,
                                                    tid<int8>& src,
                                                    tid<int16>& kernel,
                                                    tid<half>& bias,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_bias_dilation_f16_int8_int16(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, pos,
                                               sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<half>& dst,
                                                    tid<int8>& src,
                                                    tid<int8>& kernel,
                                                    tid<half>& bias,
                                                    short kh,
                                                    short kw,
                                                    short pos,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_bias_dilation_f16_int8_int8(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw, pos,
                                              sh, sw, in_dh, in_dw, out_dh, out_dw);
}
static __mlu_func__ __mlu_builtin__ void __trc_findfirst1(tid<unsigned int>& dst,
                                                          tid<half>& src) {
  __mlvm_trc_findfirst1_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_findfirst1(tid<unsigned int>& dst,
                                                          tid<float>& src) {
  __mlvm_trc_findfirst1_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_findlast1(tid<unsigned int>& dst,
                                                         tid<half>& src) {
  __mlvm_trc_findlast1_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_findlast1(tid<unsigned int>& dst,
                                                         tid<float>& src) {
  __mlvm_trc_findlast1_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ unsigned int __trc_findfirst1d(tid<half>& src) {
  return __mlvm_trc_findfirst1d_f16(src.tid);
}

static __mlu_func__ __mlu_builtin__ unsigned int __trc_findfirst1d(tid<float>& src) {
  return __mlvm_trc_findfirst1d_f32(src.tid);
}

static __mlu_func__ __mlu_builtin__ unsigned int __trc_findlast1d(tid<half>& src) {
  return __mlvm_trc_findlast1d_f16(src.tid);
}

static __mlu_func__ __mlu_builtin__ unsigned int __trc_findlast1d(tid<float>& src) {
  return __mlvm_trc_findlast1d_f32(src.tid);
}

#if __BANG_ARCH__ != 322
static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<float>& src,
                                                    tid<float>& kernel,
                                                    tid<float>& bias,
                                                    short kh,
                                                    short kw,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_bias_dilation_f32_f32_f32(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw,
                                            sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<float>& src,
                                                    tid<half>& kernel,
                                                    tid<float>& bias,
                                                    short kh,
                                                    short kw,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_bias_dilation_f32_f32_f16(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw,
                                            sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<half>& src,
                                                    tid<float>& kernel,
                                                    tid<float>& bias,
                                                    short kh,
                                                    short kw,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_bias_dilation_f32_f16_f32(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw,
                                            sh, sw, in_dh, in_dw, out_dh, out_dw);
}

static __mlu_func__ __mlu_builtin__ void __trc_conv(tid<float>& dst,
                                                    tid<half>& src,
                                                    tid<half>& kernel,
                                                    tid<float>& bias,
                                                    short kh,
                                                    short kw,
                                                    short sh,
                                                    short sw,
                                                    short in_dh,
                                                    short in_dw,
                                                    short out_dh,
                                                    short out_dw) {
  __mlvm_trc_conv_bias_dilation_f32_f16_f16(dst.tid, src.tid, kernel.tid, bias.tid, kh, kw,
                                            sh, sw, in_dh, in_dw, out_dh, out_dw);
}
#endif // __BANG_ARCH__ != 322

static __mlu_func__ __mlu_builtin__ void __trc_select(tid<half>& dst,
                                                      tid<half>& src1,
                                                      tid<half>& src2) {
  __mlvm_trc_select_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_select(tid<float>& dst,
                                                      tid<float>& src1,
                                                      tid<float>& src2) {
  __mlvm_trc_select_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_select_bitindex(tid<half>& dst,
                                                               tid<half>& src1,
                                                               tid<half>& src2) {
  __mlvm_trc_select_bitindex_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_select_bitindex(tid<float>& dst,
                                                               tid<float>& src1,
                                                               tid<float>& src2) {
  __mlvm_trc_select_bitindex_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ unsigned int __trc_select1d(tid<half>& dst,
                                                                tid<half>& src1,
                                                                tid<half>& src2) {
  return __mlvm_trc_select1d_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ unsigned int __trc_select1d(tid<float>& dst,
                                                                tid<float>& src1,
                                                                tid<float>& src2) {
  return __mlvm_trc_select1d_f32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ unsigned int __trc_select1d_bitindex(tid<half>& dst,
                                                                         tid<half>& src1,
                                                                         tid<half>& src2) {
  return __mlvm_trc_select1d_bitindex_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ unsigned int __trc_select1d_bitindex(tid<float>& dst,
                                                                         tid<float>& src1,
                                                                         tid<float>& src2) {
  return __mlvm_trc_select1d_bitindex_f32(dst.tid, src1.tid, src2.tid);
}

template <typename T1, typename T2>
static __mlu_func__ __mlu_builtin__ void __trc_maskmove(tid<T1>& dst,
                                                        tid<T1>& src1,
                                                        tid<T2>& src2) {
  __mlvm_trc_maskmove(dst.tid, src1.tid, src2.tid);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_pad(tid<T>& dst,
                                                   tid<T>& src,
                                                   unsigned short pad_top,
                                                   unsigned short pad_bottom,
                                                   unsigned short pad_left,
                                                   unsigned short pad_right) {
  __mlvm_trc_pad(dst.tid, src.tid, pad_top, pad_bottom, pad_left, pad_right);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_pad_inplace(tid<T>& dst,
                                                           tid<T>& src,
                                                           unsigned short pad_top,
                                                           unsigned short pad_bottom,
                                                           unsigned short pad_left,
                                                           unsigned short pad_right) {
  __mlvm_trc_pad_inplace(dst.tid, src.tid, pad_top, pad_bottom, pad_left, pad_right);
}

static __mlu_func__ __mlu_builtin__ unsigned int __trc_count(tid<half>& src) {
  return __mlvm_trc_count_f16(src.tid);
}

static __mlu_func__ __mlu_builtin__ unsigned int __trc_count(tid<float>& src) {
  return __mlvm_trc_count_f32(src.tid);
}

static __mlu_func__ __mlu_builtin__ unsigned int __trc_count_bitindex(tid<half>& src) {
  return __mlvm_trc_count_bitindex_f16(src.tid);
}

static __mlu_func__ __mlu_builtin__ unsigned int __trc_count_bitindex(tid<float>& src) {
  return __mlvm_trc_count_bitindex_f32(src.tid);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_writezero(tid<T>& dst) {
  __mlvm_trc_writezero(dst.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_faa(tid<float>& dst,
                                                   tid<float>& src1,
                                                   tid<float>& src2,
                                                   tid<float>& src3) {
  __mlvm_trc_faa_f32(dst.tid, src1.tid, src2.tid, src3.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_faa(tid<half>& dst,
                                                   tid<half>& src1,
                                                   tid<half>& src2,
                                                   tid<half>& src3) {
  __mlvm_trc_faa_f16(dst.tid, src1.tid, src2.tid, src3.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_fas(tid<float>& dst,
                                                   tid<float>& src1,
                                                   tid<float>& src2,
                                                   tid<float>& src3) {
  __mlvm_trc_fas_f32(dst.tid, src1.tid, src2.tid, src3.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_fas(tid<half>& dst,
                                                   tid<half>& src1,
                                                   tid<half>& src2,
                                                   tid<half>& src3) {
  __mlvm_trc_fas_f16(dst.tid, src1.tid, src2.tid, src3.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_fam(tid<float>& dst,
                                                   tid<float>& src1,
                                                   tid<float>& src2,
                                                   tid<float>& src3) {
  __mlvm_trc_fam_f32(dst.tid, src1.tid, src2.tid, src3.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_fam(tid<half>& dst,
                                                   tid<half>& src1,
                                                   tid<half>& src2,
                                                   tid<half>& src3) {
  __mlvm_trc_fam_f16(dst.tid, src1.tid, src2.tid, src3.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_fma(tid<float>& dst,
                                                   tid<float>& src1,
                                                   tid<float>& src2,
                                                   tid<float>& src3) {
  __mlvm_trc_fma_f32(dst.tid, src1.tid, src2.tid, src3.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_fma(tid<half>& dst,
                                                   tid<half>& src1,
                                                   tid<half>& src2,
                                                   tid<half>& src3) {
  __mlvm_trc_fma_f16(dst.tid, src1.tid, src2.tid, src3.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_fms(tid<float>& dst,
                                                   tid<float>& src1,
                                                   tid<float>& src2,
                                                   tid<float>& src3) {
  __mlvm_trc_fms_f32(dst.tid, src1.tid, src2.tid, src3.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_fms(tid<half>& dst,
                                                   tid<half>& src1,
                                                   tid<half>& src2,
                                                   tid<half>& src3) {
  __mlvm_trc_fms_f16(dst.tid, src1.tid, src2.tid, src3.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_fsa(tid<float>& dst,
                                                   tid<float>& src1,
                                                   tid<float>& src2,
                                                   tid<float>& src3) {
  __mlvm_trc_fsa_f32(dst.tid, src1.tid, src2.tid, src3.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_fsa(tid<half>& dst,
                                                   tid<half>& src1,
                                                   tid<half>& src2,
                                                   tid<half>& src3) {
  __mlvm_trc_fsa_f16(dst.tid, src1.tid, src2.tid, src3.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_fsm(tid<float>& dst,
                                                   tid<float>& src1,
                                                   tid<float>& src2,
                                                   tid<float>& src3) {
  __mlvm_trc_fsm_f32(dst.tid, src1.tid, src2.tid, src3.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_fsm(tid<half>& dst,
                                                   tid<half>& src1,
                                                   tid<half>& src2,
                                                   tid<half>& src3) {
  __mlvm_trc_fsm_f16(dst.tid, src1.tid, src2.tid, src3.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_fss(tid<float>& dst,
                                                   tid<float>& src1,
                                                   tid<float>& src2,
                                                   tid<float>& src3) {
  __mlvm_trc_fss_f32(dst.tid, src1.tid, src2.tid, src3.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_fss(tid<half>& dst,
                                                   tid<half>& src1,
                                                   tid<half>& src2,
                                                   tid<half>& src3) {
  __mlvm_trc_fss_f16(dst.tid, src1.tid, src2.tid, src3.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_avgpool(tid<float>& dst,
                                                       tid<float>& src,
                                                       short kw,
                                                       short kh,
                                                       short sw,
                                                       short sh) {
  __mlvm_trc_avgpool_f32(dst.tid, src.tid, kw, kh, sw, sh);
}
static __mlu_func__ __mlu_builtin__ void __trc_avgpool(tid<half>& dst,
                                                       tid<half>& src,
                                                       short kw,
                                                       short kh,
                                                       short sw,
                                                       short sh) {
  __mlvm_trc_avgpool_f16(dst.tid, src.tid, kw, kh, sw, sh);
}

static __mlu_func__ __mlu_builtin__ void __trc_maxpool(tid<float>& dst,
                                                       tid<float>& src,
                                                       short kw,
                                                       short kh,
                                                       short sw,
                                                       short sh) {
  __mlvm_trc_maxpool_f32(dst.tid, src.tid, kw, kh, sw, sh);
}
static __mlu_func__ __mlu_builtin__ void __trc_maxpool(tid<half>& dst,
                                                       tid<half>& src,
                                                       short kw,
                                                       short kh,
                                                       short sw,
                                                       short sh) {
  __mlvm_trc_maxpool_f16(dst.tid, src.tid, kw, kh, sw, sh);
}
static __mlu_func__ __mlu_builtin__ void __trc_maxpool_index(tid<unsigned int>& dst,
                                                             tid<float>& src,
                                                             short kw,
                                                             short kh,
                                                             short sw,
                                                             short sh) {
  __mlvm_trc_maxpool_index_f32(dst.tid, src.tid, kw, kh, sw, sh);
}
static __mlu_func__ __mlu_builtin__ void __trc_maxpool_index(tid<unsigned short>& dst,
                                                             tid<half>& src,
                                                             short kw,
                                                             short kh,
                                                             short sw,
                                                             short sh) {
  __mlvm_trc_maxpool_index_f16(dst.tid, src.tid, kw, kh, sw, sh);
}

static __mlu_func__ __mlu_builtin__ void __trc_maxpool_value_index(tid<float>& dst,
                                                                   tid<float>& src,
                                                                   short kw,
                                                                   short kh,
                                                                   short sw,
                                                                   short sh,
                                                                   short value_index_stride) {
  __mlvm_trc_maxpool_value_index_f32(dst.tid, src.tid, kw, kh, sw, sh, value_index_stride);
}
static __mlu_func__ __mlu_builtin__ void __trc_maxpool_value_index(tid<half>& dst,
                                                                   tid<half>& src,
                                                                   short kw,
                                                                   short kh,
                                                                   short sw,
                                                                   short sh,
                                                                   short value_index_stride) {
  __mlvm_trc_maxpool_value_index_f16(dst.tid, src.tid, kw, kh, sw, sh, value_index_stride);
}

static __mlu_func__ __mlu_builtin__ void __trc_minpool(tid<float>& dst,
                                                       tid<float>& src,
                                                       short kw,
                                                       short kh,
                                                       short sw,
                                                       short sh) {
  __mlvm_trc_minpool_f32(dst.tid, src.tid, kw, kh, sw, sh);
}
static __mlu_func__ __mlu_builtin__ void __trc_minpool(tid<half>& dst,
                                                       tid<half>& src,
                                                       short kw,
                                                       short kh,
                                                       short sw,
                                                       short sh) {
  __mlvm_trc_minpool_f16(dst.tid, src.tid, kw, kh, sw, sh);
}
static __mlu_func__ __mlu_builtin__ void __trc_minpool_index(tid<unsigned int>& dst,
                                                             tid<float>& src,
                                                             short kw,
                                                             short kh,
                                                             short sw,
                                                             short sh) {
  __mlvm_trc_minpool_index_f32(dst.tid, src.tid, kw, kh, sw, sh);
}

static __mlu_func__ __mlu_builtin__ void __trc_minpool_index(tid<unsigned short>& dst,
                                                             tid<half>& src,
                                                             short kw,
                                                             short kh,
                                                             short sw,
                                                             short sh) {
  __mlvm_trc_minpool_index_f16(dst.tid, src.tid, kw, kh, sw, sh);
}
static __mlu_func__ __mlu_builtin__ void __trc_minpool_value_index(tid<float>& dst,
                                                                   tid<float>& src,
                                                                   short kw,
                                                                   short kh,
                                                                   short sw,
                                                                   short sh,
                                                                   short value_index_stride) {
  __mlvm_trc_minpool_value_index_f32(dst.tid, src.tid, kw, kh, sw, sh, value_index_stride);
}
static __mlu_func__ __mlu_builtin__ void __trc_minpool_value_index(tid<half>& dst,
                                                                   tid<half>& src,
                                                                   short kw,
                                                                   short kh,
                                                                   short sw,
                                                                   short sh,
                                                                   short value_index_stride) {
  __mlvm_trc_minpool_value_index_f16(dst.tid, src.tid, kw, kh, sw, sh, value_index_stride);
}

static __mlu_func__ __mlu_builtin__ void __trc_avgpool_dilation(tid<half>& dst,
                                                                tid<half>& src,
                                                                short kw,
                                                                short kh,
                                                                short sw,
                                                                short sh,
                                                                short dw,
                                                                short dh,
                                                                short out_dw,
                                                                short out_dh) {
  __mlvm_trc_avgpool_dilation_f16(dst.tid, src.tid, kw, kh, sw, sh, dw, dh, out_dw, out_dh);
}

static __mlu_func__ __mlu_builtin__ void __trc_avgpool_dilation(tid<float>& dst,
                                                                tid<float>& src,
                                                                short kw,
                                                                short kh,
                                                                short sw,
                                                                short sh,
                                                                short dw,
                                                                short dh,
                                                                short out_dw,
                                                                short out_dh) {
  __mlvm_trc_avgpool_dilation_f32(dst.tid, src.tid, kw, kh, sw, sh, dw, dh, out_dw, out_dh);
}
static __mlu_func__ __mlu_builtin__ void __trc_maxpool_dilation(tid<half>& dst,
                                                                tid<half>& src,
                                                                short kw,
                                                                short kh,
                                                                short sw,
                                                                short sh,
                                                                short dw,
                                                                short dh,
                                                                short out_dw,
                                                                short out_dh) {
  __mlvm_trc_maxpool_dilation_f16(dst.tid, src.tid, kw, kh, sw, sh, dw, dh, out_dw, out_dh);
}
static __mlu_func__ __mlu_builtin__ void __trc_maxpool_dilation(tid<float>& dst,
                                                                tid<float>& src,
                                                                short kw,
                                                                short kh,
                                                                short sw,
                                                                short sh,
                                                                short dw,
                                                                short dh,
                                                                short out_dw,
                                                                short out_dh) {
  __mlvm_trc_maxpool_dilation_f32(dst.tid, src.tid, kw, kh, sw, sh, dw, dh, out_dw, out_dh);
}
static __mlu_func__ __mlu_builtin__ void __trc_maxpool_index_dilation(tid<unsigned short>& dst,
                                                                      tid<half>& src,
                                                                      short kw,
                                                                      short kh,
                                                                      short sw,
                                                                      short sh,
                                                                      short dw,
                                                                      short dh,
                                                                      short out_dw,
                                                                      short out_dh) {
  __mlvm_trc_maxpool_dilation_index_f16(dst.tid, src.tid, kw, kh, sw, sh, dw, dh, out_dw, out_dh);
}

static __mlu_func__ __mlu_builtin__ void __trc_maxpool_index_dilation(tid<unsigned int>& dst,
                                                                      tid<float>& src,
                                                                      short kw,
                                                                      short kh,
                                                                      short sw,
                                                                      short sh,
                                                                      short dw,
                                                                      short dh,
                                                                      short out_dw,
                                                                      short out_dh) {
  __mlvm_trc_maxpool_dilation_index_f32(dst.tid, src.tid, kw, kh, sw, sh, dw, dh, out_dw, out_dh);
}
static __mlu_func__ __mlu_builtin__ void __trc_maxpool_value_index_dilation(tid<half>& dst,
                                                                            tid<half>& src,
                                                                            short kw,
                                                                            short kh,
                                                                            short sw,
                                                                            short sh,
                                                                            short value_index_stride,
                                                                            short dw,
                                                                            short dh,
                                                                            short out_dw,
                                                                            short out_dh) {
  __mlvm_trc_maxpool_dilation_value_index_f16(dst.tid, src.tid, kw,
                                              kh, sw, sh, dw, dh, out_dw, out_dh, value_index_stride);
}
static __mlu_func__ __mlu_builtin__ void __trc_maxpool_value_index_dilation(tid<float>& dst,
                                                                            tid<float>& src,
                                                                            short kw,
                                                                            short kh,
                                                                            short sw,
                                                                            short sh,
                                                                            short value_index_stride,
                                                                            short dw,
                                                                            short dh,
                                                                            short out_dw,
                                                                            short out_dh) {
  __mlvm_trc_maxpool_dilation_value_index_f32(dst.tid, src.tid, kw, kh, sw,
                                              sh, dw, dh, out_dw, out_dh, value_index_stride);
}

static __mlu_func__ __mlu_builtin__ void __trc_minpool_dilation(tid<half>& dst,
                                                                tid<half>& src,
                                                                short kw,
                                                                short kh,
                                                                short sw,
                                                                short sh,
                                                                short dw,
                                                                short dh,
                                                                short out_dw,
                                                                short out_dh) {
  __mlvm_trc_minpool_dilation_f16(dst.tid, src.tid, kw, kh, sw, sh, dw, dh, out_dw, out_dh);
}
static __mlu_func__ __mlu_builtin__ void __trc_minpool_dilation(tid<float>& dst,
                                                                tid<float>& src,
                                                                short kw,
                                                                short kh,
                                                                short sw,
                                                                short sh,
                                                                short dw,
                                                                short dh,
                                                                short out_dw,
                                                                short out_dh) {
  __mlvm_trc_minpool_dilation_f32(dst.tid, src.tid, kw, kh, sw, sh, dw, dh, out_dw, out_dh);
}
static __mlu_func__ __mlu_builtin__ void __trc_minpool_index_dilation(tid<unsigned short>& dst,
                                                                      tid<half>& src,
                                                                      short kw,
                                                                      short kh,
                                                                      short sw,
                                                                      short sh,
                                                                      short dw,
                                                                      short dh,
                                                                      short out_dw,
                                                                      short out_dh) {
  __mlvm_trc_minpool_dilation_index_f16(dst.tid, src.tid, kw, kh, sw, sh, dw, dh, out_dw, out_dh);
}
static __mlu_func__ __mlu_builtin__ void __trc_minpool_index_dilation(tid<unsigned int>& dst,
                                                                      tid<float>& src,
                                                                      short kw,
                                                                      short kh,
                                                                      short sw,
                                                                      short sh,
                                                                      short dw,
                                                                      short dh,
                                                                      short out_dw,
                                                                      short out_dh) {
  __mlvm_trc_minpool_dilation_index_f32(dst.tid, src.tid, kw, kh, sw, sh, dw, dh, out_dw, out_dh);
}
static __mlu_func__ __mlu_builtin__ void __trc_minpool_value_index_dilation(tid<half>& dst,
                                                                            tid<half>& src,
                                                                            short kw,
                                                                            short kh,
                                                                            short sw,
                                                                            short sh,
                                                                            short value_index_stride,
                                                                            short dw,
                                                                            short dh,
                                                                            short out_dw,
                                                                            short out_dh) {
  __mlvm_trc_minpool_dilation_value_index_f16(dst.tid, src.tid, kw, kh,
                                              sw, sh, dw, dh, out_dw, out_dh, value_index_stride);
}
static __mlu_func__ __mlu_builtin__ void __trc_minpool_value_index_dilation(tid<float>& dst,
                                                                            tid<float>& src,
                                                                            short kw,
                                                                            short kh,
                                                                            short sw,
                                                                            short sh,
                                                                            short value_index_stride,
                                                                            short dw,
                                                                            short dh,
                                                                            short out_dw,
                                                                            short out_dh) {
  __mlvm_trc_minpool_dilation_value_index_f32(dst.tid, src.tid, kw, kh,
                                              sw, sh, dw, dh, out_dw, out_dh, value_index_stride);
}
static __mlu_func__ __mlu_builtin__ void __trc_avgpool_bp(tid<half>& dst,
                                                          tid<half>& src,
                                                          short kw,
                                                          short kh,
                                                          short sw,
                                                          short sh) {
  __mlvm_trc_avgpool_bp_f16(dst.tid, src.tid, kw, kh, sw, sh);
}
static __mlu_func__ __mlu_builtin__ void __trc_avgpool_bp(tid<float>& dst,
                                                          tid<float>& src,
                                                          short kw,
                                                          short kh,
                                                          short sw,
                                                          short sh) {
  __mlvm_trc_avgpool_bp_f32(dst.tid, src.tid, kw, kh, sw, sh);
}
static __mlu_func__ __mlu_builtin__ void __trc_avgpool_bp_dilation(tid<half>& dst,
                                                                   tid<half>& src,
                                                                   short kw,
                                                                   short kh,
                                                                   short sw,
                                                                   short sh,
                                                                   short dw,
                                                                   short dh,
                                                                   short out_dw,
                                                                   short out_dh) {
  __mlvm_trc_avgpool_bp_dilation_f16(dst.tid, src.tid, kw, kh, sw, sh, dw, dh, out_dw, out_dh);
}
static __mlu_func__ __mlu_builtin__ void __trc_avgpool_bp_dilation(tid<float>& dst,
                                                                   tid<float>& src,
                                                                   short kw,
                                                                   short kh,
                                                                   short sw,
                                                                   short sh,
                                                                   short dw,
                                                                   short dh,
                                                                   short out_dw,
                                                                   short out_dh) {
  __mlvm_trc_avgpool_bp_dilation_f32(dst.tid, src.tid, kw, kh, sw, sh, dw, dh, out_dw, out_dh);
}
static __mlu_func__ __mlu_builtin__ void __trc_maxpool_bp(tid<half>& dst,
                                                          tid<half>& src,
                                                          tid<unsigned short>& index,
                                                          short kw,
                                                          short kh,
                                                          short sw,
                                                          short sh) {
  __mlvm_trc_maxpool_bp_f16(dst.tid, src.tid, index.tid, kw, kh, sw, sh);
}
static __mlu_func__ __mlu_builtin__ void __trc_maxpool_bp(tid<float>& dst,
                                                          tid<float>& src,
                                                          tid<unsigned int>& index,
                                                          short kw,
                                                          short kh,
                                                          short sw,
                                                          short sh) {
  __mlvm_trc_maxpool_bp_f32(dst.tid, src.tid, index.tid, kw, kh, sw, sh);
}
static __mlu_func__ __mlu_builtin__ void __trc_maxpool_bp_dilation(tid<half>& dst,
                                                                   tid<half>& src,
                                                                   tid<unsigned short>& index,
                                                                   short kw,
                                                                   short kh,
                                                                   short sw,
                                                                   short sh,
                                                                   short dw,
                                                                   short dh,
                                                                   short out_dw,
                                                                   short out_dh) {
  __mlvm_trc_maxpool_bp_dilation_f16(dst.tid, src.tid, index.tid,
                                     kw, kh, sw, sh, dw, dh, out_dw, out_dh);
}
static __mlu_func__ __mlu_builtin__ void __trc_maxpool_bp_dilation(tid<float>& dst,
                                                                   tid<float>& src,
                                                                   tid<unsigned int>& index,
                                                                   short kw,
                                                                   short kh,
                                                                   short sw,
                                                                   short sh,
                                                                   short dw,
                                                                   short dh,
                                                                   short out_dw,
                                                                   short out_dh) {
  __mlvm_trc_maxpool_bp_dilation_f32(dst.tid, src.tid, index.tid,
                                     kw, kh, sw, sh, dw, dh, out_dw, out_dh);
}
static __mlu_func__ __mlu_builtin__ void __trc_unpool(tid<half>& dst,
                                                      tid<half>& src,
                                                      tid<unsigned short>& index,
                                                      short kw,
                                                      short kh,
                                                      short sw,
                                                      short sh) {
  __mlvm_trc_unpool_bp_f16(dst.tid, src.tid, index.tid, kw, kh, sw, sh);
}
static __mlu_func__ __mlu_builtin__ void __trc_unpool(tid<float>& dst,
                                                      tid<float>& src,
                                                      tid<unsigned int>& index,
                                                      short kw,
                                                      short kh,
                                                      short sw,
                                                      short sh) {
  __mlvm_trc_unpool_bp_f32(dst.tid, src.tid, index.tid, kw, kh, sw, sh);
}
static __mlu_func__ __mlu_builtin__ void __trc_unpool_dilation(tid<half>& dst,
                                                               tid<half>& src,
                                                               tid<unsigned short>& index,
                                                               short kw,
                                                               short kh,
                                                               short sw,
                                                               short sh,
                                                               short dw,
                                                               short dh,
                                                               short out_dw,
                                                               short out_dh) {
  __mlvm_trc_unpool_bp_dilation_f16(dst.tid, src.tid, index.tid,
                                    kw, kh, sw, sh, dw, dh, out_dw, out_dh);
}
static __mlu_func__ __mlu_builtin__ void __trc_unpool_dilation(tid<float>& dst,
                                                               tid<float>& src,
                                                               tid<unsigned int>& index,
                                                               short kw,
                                                               short kh,
                                                               short sw,
                                                               short sh,
                                                               short dw,
                                                               short dh,
                                                               short out_dw,
                                                               short out_dh) {
  __mlvm_trc_unpool_bp_dilation_f32(dst.tid, src.tid, index.tid,
                                    kw, kh, sw, sh, dw, dh, out_dw, out_dh);
}

static __mlu_func__ __mlu_builtin__ void __trc_float2half(tid<half>& dst,
                                                          tid<float>& src) {
  __mlvm_trc_cvtf16_f32(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2half_tz(tid<half>& dst,
                                                             tid<float>& src) {
  __mlvm_trc_cvtf16_f32_tz(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2half_oz(tid<half>& dst,
                                                             tid<float>& src) {
  __mlvm_trc_cvtf16_f32_oz(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2half_up(tid<half>& dst,
                                                             tid<float>& src) {
  __mlvm_trc_cvtf16_f32_up(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2half_dn(tid<half>& dst,
                                                             tid<float>& src) {
  __mlvm_trc_cvtf16_f32_dn(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2half_rd(tid<half>& dst,
                                                             tid<float>& src) {
  __mlvm_trc_cvtf16_f32_rd(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2half_rn(tid<half>& dst,
                                                             tid<float>& src) {
  __mlvm_trc_cvtf16_f32_rn(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2half_rm(tid<half>& dst,
                                                             tid<float>& src) {
  __mlvm_trc_cvtf16_f32_rm(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82half(tid<half>& dst,
                                                         tid<int8>& src,
                                                         int position) {
  __mlvm_trc_cvtf16_int8(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82half_tz(tid<half>& dst,
                                                            tid<int8>& src,
                                                            int position) {
  __mlvm_trc_cvtf16_int8_tz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82half_oz(tid<half>& dst,
                                                            tid<int8>& src,
                                                            int position) {
  __mlvm_trc_cvtf16_int8_oz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82half_up(tid<half>& dst,
                                                            tid<int8>& src,
                                                            int position) {
  __mlvm_trc_cvtf16_int8_up(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82half_dn(tid<half>& dst,
                                                            tid<int8>& src,
                                                            int position) {
  __mlvm_trc_cvtf16_int8_dn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82half_rd(tid<half>& dst,
                                                            tid<int8>& src,
                                                            int position) {
  __mlvm_trc_cvtf16_int8_rd(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82half_rn(tid<half>& dst,
                                                            tid<int8>& src,
                                                            int position) {
  __mlvm_trc_cvtf16_int8_rn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82half_rm(tid<half>& dst,
                                                            tid<int8>& src,
                                                            int position) {
  __mlvm_trc_cvtf16_int8_rm(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2half(tid<half>& dst,
                                                          tid<short>& src,
                                                          int position) {
  __mlvm_trc_cvtf16_int16(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2half_tz(tid<half>& dst,
                                                             tid<short>& src,
                                                             int position) {
  __mlvm_trc_cvtf16_int16_tz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2half_oz(tid<half>& dst,
                                                             tid<short>& src,
                                                             int position) {
  __mlvm_trc_cvtf16_int16_oz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2half_up(tid<half>& dst,
                                                             tid<short>& src,
                                                             int position) {
  __mlvm_trc_cvtf16_int16_up(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2half_dn(tid<half>& dst,
                                                             tid<short>& src,
                                                             int position) {
  __mlvm_trc_cvtf16_int16_dn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2half_rd(tid<half>& dst,
                                                             tid<short>& src,
                                                             int position) {
  __mlvm_trc_cvtf16_int16_rd(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2half_rn(tid<half>& dst,
                                                             tid<short>& src,
                                                             int position) {
  __mlvm_trc_cvtf16_int16_rn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2half_rm(tid<half>& dst,
                                                             tid<short>& src,
                                                             int position) {
  __mlvm_trc_cvtf16_int16_rm(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2half(tid<half>& dst,
                                                        tid<int>& src,
                                                        int position) {
  __mlvm_trc_cvtf16_int32(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2half_tz(tid<half>& dst,
                                                           tid<int>& src,
                                                           int position) {
  __mlvm_trc_cvtf16_int32_tz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2half_oz(tid<half>& dst,
                                                           tid<int>& src,
                                                           int position) {
  __mlvm_trc_cvtf16_int32_oz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2half_up(tid<half>& dst,
                                                           tid<int>& src,
                                                           int position) {
  __mlvm_trc_cvtf16_int32_up(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2half_dn(tid<half>& dst,
                                                           tid<int>& src,
                                                           int position) {
  __mlvm_trc_cvtf16_int32_dn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2half_rd(tid<half>& dst,
                                                           tid<int>& src,
                                                           int position) {
  __mlvm_trc_cvtf16_int32_rd(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2half_rn(tid<half>& dst,
                                                           tid<int>& src,
                                                           int position) {
  __mlvm_trc_cvtf16_int32_rn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2half_rm(tid<half>& dst,
                                                           tid<int>& src,
                                                           int position) {
  __mlvm_trc_cvtf16_int32_rm(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2half(tid<half>& dst,
                                                          tid<unsigned char>& src) {
  __mlvm_trc_cvtf16_u8(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2half_tz(tid<half>& dst,
                                                             tid<unsigned char>& src) {
  __mlvm_trc_cvtf16_u8_tz(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2half_oz(tid<half>& dst,
                                                             tid<unsigned char>& src) {
  __mlvm_trc_cvtf16_u8_oz(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2half_up(tid<half>& dst,
                                                             tid<unsigned char>& src) {
  __mlvm_trc_cvtf16_u8_up(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2half_dn(tid<half>& dst,
                                                             tid<unsigned char>& src) {
  __mlvm_trc_cvtf16_u8_dn(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2half_rd(tid<half>& dst,
                                                             tid<unsigned char>& src) {
  __mlvm_trc_cvtf16_u8_rd(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2half_rn(tid<half>& dst,
                                                             tid<unsigned char>& src) {
  __mlvm_trc_cvtf16_u8_rn(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2half_rm(tid<half>& dst,
                                                             tid<unsigned char>& src) {
  __mlvm_trc_cvtf16_u8_rm(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2float(tid<float>& dst,
                                                          tid<half>& src) {
  __mlvm_trc_cvtf32_f16(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2float_tz(tid<float>& dst,
                                                             tid<half>& src) {
  __mlvm_trc_cvtf32_f16_tz(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2float_oz(tid<float>& dst,
                                                             tid<half>& src) {
  __mlvm_trc_cvtf32_f16_oz(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2float_up(tid<float>& dst,
                                                             tid<half>& src) {
  __mlvm_trc_cvtf32_f16_up(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2float_dn(tid<float>& dst,
                                                             tid<half>& src) {
  __mlvm_trc_cvtf32_f16_dn(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2float_rd(tid<float>& dst,
                                                             tid<half>& src) {
  __mlvm_trc_cvtf32_f16_rd(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2float_rn(tid<float>& dst,
                                                             tid<half>& src) {
  __mlvm_trc_cvtf32_f16_rn(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2float_rm(tid<float>& dst,
                                                             tid<half>& src) {
  __mlvm_trc_cvtf32_f16_rm(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82float(tid<float>& dst,
                                                          tid<int8>& src,
                                                          int position) {
  __mlvm_trc_cvtf32_int8(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82float_tz(tid<float>& dst,
                                                             tid<int8>& src,
                                                             int position) {
  __mlvm_trc_cvtf32_int8_tz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82float_oz(tid<float>& dst,
                                                             tid<int8>& src,
                                                             int position) {
  __mlvm_trc_cvtf32_int8_oz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82float_up(tid<float>& dst,
                                                             tid<int8>& src,
                                                             int position) {
  __mlvm_trc_cvtf32_int8_up(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82float_dn(tid<float>& dst,
                                                             tid<int8>& src,
                                                             int position) {
  __mlvm_trc_cvtf32_int8_dn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82float_rd(tid<float>& dst,
                                                             tid<int8>& src,
                                                             int position) {
  __mlvm_trc_cvtf32_int8_rd(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82float_rn(tid<float>& dst,
                                                             tid<int8>& src,
                                                             int position) {
  __mlvm_trc_cvtf32_int8_rn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82float_rm(tid<float>& dst,
                                                             tid<int8>& src,
                                                             int position) {
  __mlvm_trc_cvtf32_int8_rm(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2float(tid<float>& dst,
                                                           tid<short>& src,
                                                           int position) {
  __mlvm_trc_cvtf32_int16(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2float_tz(tid<float>& dst,
                                                              tid<short>& src,
                                                              int position) {
  __mlvm_trc_cvtf32_int16_tz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2float_oz(tid<float>& dst,
                                                              tid<short>& src,
                                                              int position) {
  __mlvm_trc_cvtf32_int16_oz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2float_up(tid<float>& dst,
                                                              tid<short>& src,
                                                              int position) {
  __mlvm_trc_cvtf32_int16_up(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2float_dn(tid<float>& dst,
                                                              tid<short>& src,
                                                              int position) {
  __mlvm_trc_cvtf32_int16_dn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2float_rd(tid<float>& dst,
                                                              tid<short>& src,
                                                              int position) {
  __mlvm_trc_cvtf32_int16_rd(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2float_rn(tid<float>& dst,
                                                              tid<short>& src,
                                                              int position) {
  __mlvm_trc_cvtf32_int16_rn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2float_rm(tid<float>& dst,
                                                              tid<short>& src,
                                                              int position) {
  __mlvm_trc_cvtf32_int16_rm(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2float(tid<float>& dst,
                                                         tid<int>& src,
                                                         int position) {
  __mlvm_trc_cvtf32_int32(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2float_tz(tid<float>& dst,
                                                            tid<int>& src,
                                                            int position) {
  __mlvm_trc_cvtf32_int32_tz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2float_oz(tid<float>& dst,
                                                            tid<int>& src,
                                                            int position) {
  __mlvm_trc_cvtf32_int32_oz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2float_up(tid<float>& dst,
                                                            tid<int>& src,
                                                            int position) {
  __mlvm_trc_cvtf32_int32_up(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2float_dn(tid<float>& dst,
                                                            tid<int>& src,
                                                            int position) {
  __mlvm_trc_cvtf32_int32_dn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2float_rd(tid<float>& dst,
                                                            tid<int>& src,
                                                            int position) {
  __mlvm_trc_cvtf32_int32_rd(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2float_rn(tid<float>& dst,
                                                            tid<int>& src,
                                                            int position) {
  __mlvm_trc_cvtf32_int32_rn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2float_rm(tid<float>& dst,
                                                            tid<int>& src,
                                                            int position) {
  __mlvm_trc_cvtf32_int32_rm(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2float(tid<float>& dst,
                                                           tid<unsigned char>& src) {
  __mlvm_trc_cvtf32_u8(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2float_tz(tid<float>& dst,
                                                              tid<unsigned char>& src) {
  __mlvm_trc_cvtf32_u8_tz(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2float_oz(tid<float>& dst,
                                                              tid<unsigned char>& src) {
  __mlvm_trc_cvtf32_u8_oz(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2float_up(tid<float>& dst,
                                                              tid<unsigned char>& src) {
  __mlvm_trc_cvtf32_u8_up(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2float_dn(tid<float>& dst,
                                                              tid<unsigned char>& src) {
  __mlvm_trc_cvtf32_u8_dn(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2float_rd(tid<float>& dst,
                                                              tid<unsigned char>& src) {
  __mlvm_trc_cvtf32_u8_rd(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2float_rn(tid<float>& dst,
                                                              tid<unsigned char>& src) {
  __mlvm_trc_cvtf32_u8_rn(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2float_rm(tid<float>& dst,
                                                              tid<unsigned char>& src) {
  __mlvm_trc_cvtf32_u8_rm(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2int8(tid<int8>& dst,
                                                         tid<half>& src,
                                                         int position) {
  __mlvm_trc_cvtint8_f16(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2int8_tz(tid<int8>& dst,
                                                            tid<half>& src,
                                                            int position) {
  __mlvm_trc_cvtint8_f16_tz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2int8_oz(tid<int8>& dst,
                                                            tid<half>& src,
                                                            int position) {
  __mlvm_trc_cvtint8_f16_oz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2int8_up(tid<int8>& dst,
                                                            tid<half>& src,
                                                            int position) {
  __mlvm_trc_cvtint8_f16_up(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2int8_dn(tid<int8>& dst,
                                                            tid<half>& src,
                                                            int position) {
  __mlvm_trc_cvtint8_f16_dn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2int8_rd(tid<int8>& dst,
                                                            tid<half>& src,
                                                            int position) {
  __mlvm_trc_cvtint8_f16_rd(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2int8_rn(tid<int8>& dst,
                                                            tid<half>& src,
                                                            int position) {
  __mlvm_trc_cvtint8_f16_rn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2int8_rm(tid<int8>& dst,
                                                            tid<half>& src,
                                                            int position) {
  __mlvm_trc_cvtint8_f16_rm(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2int8(tid<int8>& dst,
                                                          tid<float>& src,
                                                          int position) {
  __mlvm_trc_cvtint8_f32(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2int8_tz(tid<int8>& dst,
                                                             tid<float>& src,
                                                             int position) {
  __mlvm_trc_cvtint8_f32_tz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2int8_oz(tid<int8>& dst,
                                                             tid<float>& src,
                                                             int position) {
  __mlvm_trc_cvtint8_f32_oz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2int8_up(tid<int8>& dst,
                                                             tid<float>& src,
                                                             int position) {
  __mlvm_trc_cvtint8_f32_up(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2int8_dn(tid<int8>& dst,
                                                             tid<float>& src,
                                                             int position) {
  __mlvm_trc_cvtint8_f32_dn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2int8_rd(tid<int8>& dst,
                                                             tid<float>& src,
                                                             int position) {
  __mlvm_trc_cvtint8_f32_rd(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2int8_rn(tid<int8>& dst,
                                                             tid<float>& src,
                                                             int position) {
  __mlvm_trc_cvtint8_f32_rn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2int8_rm(tid<int8>& dst,
                                                             tid<float>& src,
                                                             int position) {
  __mlvm_trc_cvtint8_f32_rm(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2int8(tid<int8>& dst,
                                                          tid<short>& src,
                                                          int dst_position,
                                                          int src_position) {
  __mlvm_trc_cvtint8_int16(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2int8_tz(tid<int8>& dst,
                                                             tid<short>& src,
                                                             int dst_position,
                                                             int src_position) {
  __mlvm_trc_cvtint8_int16_tz(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2int8_oz(tid<int8>& dst,
                                                             tid<short>& src,
                                                             int dst_position,
                                                             int src_position) {
  __mlvm_trc_cvtint8_int16_oz(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2int8_up(tid<int8>& dst,
                                                             tid<short>& src,
                                                             int dst_position,
                                                             int src_position) {
  __mlvm_trc_cvtint8_int16_up(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2int8_dn(tid<int8>& dst,
                                                             tid<short>& src,
                                                             int dst_position,
                                                             int src_position) {
  __mlvm_trc_cvtint8_int16_dn(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2int8_rd(tid<int8>& dst,
                                                             tid<short>& src,
                                                             int dst_position,
                                                             int src_position) {
  __mlvm_trc_cvtint8_int16_rd(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2int8_rn(tid<int8>& dst,
                                                             tid<short>& src,
                                                             int dst_position,
                                                             int src_position) {
  __mlvm_trc_cvtint8_int16_rn(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2int8_rm(tid<int8>& dst,
                                                             tid<short>& src,
                                                             int dst_position,
                                                             int src_position) {
  __mlvm_trc_cvtint8_int16_rm(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2int8(tid<int8>& dst,
                                                        tid<int>& src,
                                                        int dst_position,
                                                        int src_position) {
  __mlvm_trc_cvtint8_int32(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2int8_tz(tid<int8>& dst,
                                                           tid<int>& src,
                                                           int dst_position,
                                                           int src_position) {
  __mlvm_trc_cvtint8_int32_tz(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2int8_oz(tid<int8>& dst,
                                                           tid<int>& src,
                                                           int dst_position,
                                                           int src_position) {
  __mlvm_trc_cvtint8_int32_oz(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2int8_up(tid<int8>& dst,
                                                           tid<int>& src,
                                                           int dst_position,
                                                           int src_position) {
  __mlvm_trc_cvtint8_int32_up(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2int8_dn(tid<int8>& dst,
                                                           tid<int>& src,
                                                           int dst_position,
                                                           int src_position) {
  __mlvm_trc_cvtint8_int32_dn(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2int8_rd(tid<int8>& dst,
                                                           tid<int>& src,
                                                           int dst_position,
                                                           int src_position) {
  __mlvm_trc_cvtint8_int32_rd(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2int8_rn(tid<int8>& dst,
                                                           tid<int>& src,
                                                           int dst_position,
                                                           int src_position) {
  __mlvm_trc_cvtint8_int32_rn(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2int8_rm(tid<int8>& dst,
                                                           tid<int>& src,
                                                           int dst_position,
                                                           int src_position) {
  __mlvm_trc_cvtint8_int32_rm(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2int8(tid<int8>& dst,
                                                          tid<unsigned char>& src,
                                                          int position) {
  __mlvm_trc_cvtint8_u8(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2int8_tz(tid<int8>& dst,
                                                             tid<unsigned char>& src,
                                                             int position) {
  __mlvm_trc_cvtint8_u8_tz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2int8_oz(tid<int8>& dst,
                                                             tid<unsigned char>& src,
                                                             int position) {
  __mlvm_trc_cvtint8_u8_oz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2int8_up(tid<int8>& dst,
                                                             tid<unsigned char>& src,
                                                             int position) {
  __mlvm_trc_cvtint8_u8_up(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2int8_dn(tid<int8>& dst,
                                                             tid<unsigned char>& src,
                                                             int position) {
  __mlvm_trc_cvtint8_u8_dn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2int8_rd(tid<int8>& dst,
                                                             tid<unsigned char>& src,
                                                             int position) {
  __mlvm_trc_cvtint8_u8_rd(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2int8_rn(tid<int8>& dst,
                                                             tid<unsigned char>& src,
                                                             int position) {
  __mlvm_trc_cvtint8_u8_rn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2int8_rm(tid<int8>& dst,
                                                             tid<unsigned char>& src,
                                                             int position) {
  __mlvm_trc_cvtint8_u8_rm(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2short(tid<short>& dst,
                                                          tid<half>& src,
                                                          int position) {
  __mlvm_trc_cvtint16_f16(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2short_tz(tid<short>& dst,
                                                             tid<half>& src,
                                                             int position) {
  __mlvm_trc_cvtint16_f16_tz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2short_oz(tid<short>& dst,
                                                             tid<half>& src,
                                                             int position) {
  __mlvm_trc_cvtint16_f16_oz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2short_up(tid<short>& dst,
                                                             tid<half>& src,
                                                             int position) {
  __mlvm_trc_cvtint16_f16_up(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2short_dn(tid<short>& dst,
                                                             tid<half>& src,
                                                             int position) {
  __mlvm_trc_cvtint16_f16_dn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2short_rd(tid<short>& dst,
                                                             tid<half>& src,
                                                             int position) {
  __mlvm_trc_cvtint16_f16_rd(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2short_rn(tid<short>& dst,
                                                             tid<half>& src,
                                                             int position) {
  __mlvm_trc_cvtint16_f16_rn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2short_rm(tid<short>& dst,
                                                             tid<half>& src,
                                                             int position) {
  __mlvm_trc_cvtint16_f16_rm(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2short(tid<short>& dst,
                                                           tid<float>& src,
                                                           int position) {
  __mlvm_trc_cvtint16_f32(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2short_tz(tid<short>& dst,
                                                              tid<float>& src,
                                                              int position) {
  __mlvm_trc_cvtint16_f32_tz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2short_oz(tid<short>& dst,
                                                              tid<float>& src,
                                                              int position) {
  __mlvm_trc_cvtint16_f32_oz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2short_up(tid<short>& dst,
                                                              tid<float>& src,
                                                              int position) {
  __mlvm_trc_cvtint16_f32_up(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2short_dn(tid<short>& dst,
                                                              tid<float>& src,
                                                              int position) {
  __mlvm_trc_cvtint16_f32_dn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2short_rd(tid<short>& dst,
                                                              tid<float>& src,
                                                              int position) {
  __mlvm_trc_cvtint16_f32_rd(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2short_rn(tid<short>& dst,
                                                              tid<float>& src,
                                                              int position) {
  __mlvm_trc_cvtint16_f32_rn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2short_rm(tid<short>& dst,
                                                              tid<float>& src,
                                                              int position) {
  __mlvm_trc_cvtint16_f32_rm(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82short(tid<short>& dst,
                                                          tid<int8>& src,
                                                          int dst_position,
                                                          int src_position) {
  __mlvm_trc_cvtint16_int8(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82short_tz(tid<short>& dst,
                                                             tid<int8>& src,
                                                             int dst_position,
                                                             int src_position) {
  __mlvm_trc_cvtint16_int8_tz(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82short_oz(tid<short>& dst,
                                                             tid<int8>& src,
                                                             int dst_position,
                                                             int src_position) {
  __mlvm_trc_cvtint16_int8_oz(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82short_up(tid<short>& dst,
                                                             tid<int8>& src,
                                                             int dst_position,
                                                             int src_position) {
  __mlvm_trc_cvtint16_int8_up(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82short_dn(tid<short>& dst,
                                                             tid<int8>& src,
                                                             int dst_position,
                                                             int src_position) {
  __mlvm_trc_cvtint16_int8_dn(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82short_rd(tid<short>& dst,
                                                             tid<int8>& src,
                                                             int dst_position,
                                                             int src_position) {
  __mlvm_trc_cvtint16_int8_rd(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82short_rn(tid<short>& dst,
                                                             tid<int8>& src,
                                                             int dst_position,
                                                             int src_position) {
  __mlvm_trc_cvtint16_int8_rn(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82short_rm(tid<short>& dst,
                                                             tid<int8>& src,
                                                             int dst_position,
                                                             int src_position) {
  __mlvm_trc_cvtint16_int8_rm(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2short(tid<short>& dst,
                                                         tid<int>& src,
                                                         int dst_position,
                                                         int src_position) {
  __mlvm_trc_cvtint16_int32(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2short_tz(tid<short>& dst,
                                                            tid<int>& src,
                                                            int dst_position,
                                                            int src_position) {
  __mlvm_trc_cvtint16_int32_tz(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2short_oz(tid<short>& dst,
                                                            tid<int>& src,
                                                            int dst_position,
                                                            int src_position) {
  __mlvm_trc_cvtint16_int32_oz(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2short_up(tid<short>& dst,
                                                            tid<int>& src,
                                                            int dst_position,
                                                            int src_position) {
  __mlvm_trc_cvtint16_int32_up(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2short_dn(tid<short>& dst,
                                                            tid<int>& src,
                                                            int dst_position,
                                                            int src_position) {
  __mlvm_trc_cvtint16_int32_dn(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2short_rd(tid<short>& dst,
                                                            tid<int>& src,
                                                            int dst_position,
                                                            int src_position) {
  __mlvm_trc_cvtint16_int32_rd(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2short_rn(tid<short>& dst,
                                                            tid<int>& src,
                                                            int dst_position,
                                                            int src_position) {
  __mlvm_trc_cvtint16_int32_rn(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2short_rm(tid<short>& dst,
                                                            tid<int>& src,
                                                            int dst_position,
                                                            int src_position) {
  __mlvm_trc_cvtint16_int32_rm(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2short(tid<short>& dst,
                                                           tid<unsigned char>& src,
                                                           int position) {
  __mlvm_trc_cvtint16_u8(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2short_tz(tid<short>& dst,
                                                              tid<unsigned char>& src,
                                                              int position) {
  __mlvm_trc_cvtint16_u8_tz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2short_oz(tid<short>& dst,
                                                              tid<unsigned char>& src,
                                                              int position) {
  __mlvm_trc_cvtint16_u8_oz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2short_up(tid<short>& dst,
                                                              tid<unsigned char>& src,
                                                              int position) {
  __mlvm_trc_cvtint16_u8_up(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2short_dn(tid<short>& dst,
                                                              tid<unsigned char>& src,
                                                              int position) {
  __mlvm_trc_cvtint16_u8_dn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2short_rd(tid<short>& dst,
                                                              tid<unsigned char>& src,
                                                              int position) {
  __mlvm_trc_cvtint16_u8_rd(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2short_rn(tid<short>& dst,
                                                              tid<unsigned char>& src,
                                                              int position) {
  __mlvm_trc_cvtint16_u8_rn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2short_rm(tid<short>& dst,
                                                              tid<unsigned char>& src,
                                                              int position) {
  __mlvm_trc_cvtint16_u8_rm(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2int(tid<int>& dst,
                                                        tid<half>& src,
                                                        int position) {
  __mlvm_trc_cvtint32_f16(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2int_tz(tid<int>& dst,
                                                           tid<half>& src,
                                                           int position) {
  __mlvm_trc_cvtint32_f16_tz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2int_oz(tid<int>& dst,
                                                           tid<half>& src,
                                                           int position) {
  __mlvm_trc_cvtint32_f16_oz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2int_up(tid<int>& dst,
                                                           tid<half>& src,
                                                           int position) {
  __mlvm_trc_cvtint32_f16_up(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2int_dn(tid<int>& dst,
                                                           tid<half>& src,
                                                           int position) {
  __mlvm_trc_cvtint32_f16_dn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2int_rd(tid<int>& dst,
                                                           tid<half>& src,
                                                           int position) {
  __mlvm_trc_cvtint32_f16_rd(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2int_rn(tid<int>& dst,
                                                           tid<half>& src,
                                                           int position) {
  __mlvm_trc_cvtint32_f16_rn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2int_rm(tid<int>& dst,
                                                           tid<half>& src,
                                                           int position) {
  __mlvm_trc_cvtint32_f16_rm(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2int(tid<int>& dst,
                                                         tid<float>& src,
                                                         int position) {
  __mlvm_trc_cvtint32_f32(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2int_tz(tid<int>& dst,
                                                            tid<float>& src,
                                                            int position) {
  __mlvm_trc_cvtint32_f32_tz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2int_oz(tid<int>& dst,
                                                            tid<float>& src,
                                                            int position) {
  __mlvm_trc_cvtint32_f32_oz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2int_up(tid<int>& dst,
                                                            tid<float>& src,
                                                            int position) {
  __mlvm_trc_cvtint32_f32_up(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2int_dn(tid<int>& dst,
                                                            tid<float>& src,
                                                            int position) {
  __mlvm_trc_cvtint32_f32_dn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2int_rd(tid<int>& dst,
                                                            tid<float>& src,
                                                            int position) {
  __mlvm_trc_cvtint32_f32_rd(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2int_rn(tid<int>& dst,
                                                            tid<float>& src,
                                                            int position) {
  __mlvm_trc_cvtint32_f32_rn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2int_rm(tid<int>& dst,
                                                            tid<float>& src,
                                                            int position) {
  __mlvm_trc_cvtint32_f32_rm(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82int(tid<int>& dst,
                                                        tid<int8>& src,
                                                        int dst_position,
                                                        int src_position) {
  __mlvm_trc_cvtint32_int8(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82int_tz(tid<int>& dst,
                                                           tid<int8>& src,
                                                           int dst_position,
                                                           int src_position) {
  __mlvm_trc_cvtint32_int8_tz(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82int_oz(tid<int>& dst,
                                                           tid<int8>& src,
                                                           int dst_position,
                                                           int src_position) {
  __mlvm_trc_cvtint32_int8_oz(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82int_up(tid<int>& dst,
                                                           tid<int8>& src,
                                                           int dst_position,
                                                           int src_position) {
  __mlvm_trc_cvtint32_int8_up(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82int_dn(tid<int>& dst,
                                                           tid<int8>& src,
                                                           int dst_position,
                                                           int src_position) {
  __mlvm_trc_cvtint32_int8_dn(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82int_rd(tid<int>& dst,
                                                           tid<int8>& src,
                                                           int dst_position,
                                                           int src_position) {
  __mlvm_trc_cvtint32_int8_rd(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82int_rn(tid<int>& dst,
                                                           tid<int8>& src,
                                                           int dst_position,
                                                           int src_position) {
  __mlvm_trc_cvtint32_int8_rn(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82int_rm(tid<int>& dst,
                                                           tid<int8>& src,
                                                           int dst_position,
                                                           int src_position) {
  __mlvm_trc_cvtint32_int8_rm(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2int(tid<int>& dst,
                                                         tid<short>& src,
                                                         int dst_position,
                                                         int src_position) {
  __mlvm_trc_cvtint32_int16(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2int_tz(tid<int>& dst,
                                                            tid<short>& src,
                                                            int dst_position,
                                                            int src_position) {
  __mlvm_trc_cvtint32_int16_tz(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2int_oz(tid<int>& dst,
                                                            tid<short>& src,
                                                            int dst_position,
                                                            int src_position) {
  __mlvm_trc_cvtint32_int16_oz(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2int_up(tid<int>& dst,
                                                            tid<short>& src,
                                                            int dst_position,
                                                            int src_position) {
  __mlvm_trc_cvtint32_int16_up(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2int_dn(tid<int>& dst,
                                                            tid<short>& src,
                                                            int dst_position,
                                                            int src_position) {
  __mlvm_trc_cvtint32_int16_dn(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2int_rd(tid<int>& dst,
                                                            tid<short>& src,
                                                            int dst_position,
                                                            int src_position) {
  __mlvm_trc_cvtint32_int16_rd(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2int_rn(tid<int>& dst,
                                                            tid<short>& src,
                                                            int dst_position,
                                                            int src_position) {
  __mlvm_trc_cvtint32_int16_rn(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2int_rm(tid<int>& dst,
                                                            tid<short>& src,
                                                            int dst_position,
                                                            int src_position) {
  __mlvm_trc_cvtint32_int16_rm(dst.tid, src.tid, dst_position, src_position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2int(tid<int>& dst,
                                                         tid<unsigned char>& src,
                                                         int position) {
  __mlvm_trc_cvtint32_u8(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2int_tz(tid<int>& dst,
                                                            tid<unsigned char>& src,
                                                            int position) {
  __mlvm_trc_cvtint32_u8_tz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2int_oz(tid<int>& dst,
                                                            tid<unsigned char>& src,
                                                            int position) {
  __mlvm_trc_cvtint32_u8_oz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2int_up(tid<int>& dst,
                                                            tid<unsigned char>& src,
                                                            int position) {
  __mlvm_trc_cvtint32_u8_up(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2int_dn(tid<int>& dst,
                                                            tid<unsigned char>& src,
                                                            int position) {
  __mlvm_trc_cvtint32_u8_dn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2int_rd(tid<int>& dst,
                                                            tid<unsigned char>& src,
                                                            int position) {
  __mlvm_trc_cvtint32_u8_rd(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2int_rn(tid<int>& dst,
                                                            tid<unsigned char>& src,
                                                            int position) {
  __mlvm_trc_cvtint32_u8_rn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_uchar2int_rm(tid<int>& dst,
                                                            tid<unsigned char>& src,
                                                            int position) {
  __mlvm_trc_cvtint32_u8_rm(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2uchar(tid<unsigned char>& dst,
                                                          tid<half>& src) {
  __mlvm_trc_cvtu8_f16(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2uchar_tz(tid<unsigned char>& dst,
                                                             tid<half>& src) {
  __mlvm_trc_cvtu8_f16_tz(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2uchar_oz(tid<unsigned char>& dst,
                                                             tid<half>& src) {
  __mlvm_trc_cvtu8_f16_oz(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2uchar_up(tid<unsigned char>& dst,
                                                             tid<half>& src) {
  __mlvm_trc_cvtu8_f16_up(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2uchar_dn(tid<unsigned char>& dst,
                                                             tid<half>& src) {
  __mlvm_trc_cvtu8_f16_dn(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2uchar_rd(tid<unsigned char>& dst,
                                                             tid<half>& src) {
  __mlvm_trc_cvtu8_f16_rd(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2uchar_rn(tid<unsigned char>& dst,
                                                             tid<half>& src) {
  __mlvm_trc_cvtu8_f16_rn(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_half2uchar_rm(tid<unsigned char>& dst,
                                                             tid<half>& src) {
  __mlvm_trc_cvtu8_f16_rm(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2uchar(tid<unsigned char>& dst,
                                                           tid<float>& src) {
  __mlvm_trc_cvtu8_f32(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2uchar_tz(tid<unsigned char>& dst,
                                                              tid<float>& src) {
  __mlvm_trc_cvtu8_f32_tz(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2uchar_oz(tid<unsigned char>& dst,
                                                              tid<float>& src) {
  __mlvm_trc_cvtu8_f32_oz(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2uchar_up(tid<unsigned char>& dst,
                                                              tid<float>& src) {
  __mlvm_trc_cvtu8_f32_up(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2uchar_dn(tid<unsigned char>& dst,
                                                              tid<float>& src) {
  __mlvm_trc_cvtu8_f32_dn(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2uchar_rd(tid<unsigned char>& dst,
                                                              tid<float>& src) {
  __mlvm_trc_cvtu8_f32_rd(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2uchar_rn(tid<unsigned char>& dst,
                                                              tid<float>& src) {
  __mlvm_trc_cvtu8_f32_rn(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_float2uchar_rm(tid<unsigned char>& dst,
                                                              tid<float>& src) {
  __mlvm_trc_cvtu8_f32_rm(dst.tid, src.tid);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82uchar(tid<unsigned char>& dst,
                                                          tid<int8>& src,
                                                          int position) {
  __mlvm_trc_cvtu8_int8(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82uchar_tz(tid<unsigned char>& dst,
                                                             tid<int8>& src,
                                                             int position) {
  __mlvm_trc_cvtu8_int8_tz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82uchar_oz(tid<unsigned char>& dst,
                                                             tid<int8>& src,
                                                             int position) {
  __mlvm_trc_cvtu8_int8_oz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82uchar_up(tid<unsigned char>& dst,
                                                             tid<int8>& src,
                                                             int position) {
  __mlvm_trc_cvtu8_int8_up(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82uchar_dn(tid<unsigned char>& dst,
                                                             tid<int8>& src,
                                                             int position) {
  __mlvm_trc_cvtu8_int8_dn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82uchar_rd(tid<unsigned char>& dst,
                                                             tid<int8>& src,
                                                             int position) {
  __mlvm_trc_cvtu8_int8_rd(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82uchar_rn(tid<unsigned char>& dst,
                                                             tid<int8>& src,
                                                             int position) {
  __mlvm_trc_cvtu8_int8_rn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int82uchar_rm(tid<unsigned char>& dst,
                                                             tid<int8>& src,
                                                             int position) {
  __mlvm_trc_cvtu8_int8_rm(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2uchar(tid<unsigned char>& dst,
                                                           tid<short>& src,
                                                           int position) {
  __mlvm_trc_cvtu8_int16(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2uchar_tz(tid<unsigned char>& dst,
                                                              tid<short>& src,
                                                              int position) {
  __mlvm_trc_cvtu8_int16_tz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2uchar_oz(tid<unsigned char>& dst,
                                                              tid<short>& src,
                                                              int position) {
  __mlvm_trc_cvtu8_int16_oz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2uchar_up(tid<unsigned char>& dst,
                                                              tid<short>& src,
                                                              int position) {
  __mlvm_trc_cvtu8_int16_up(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2uchar_dn(tid<unsigned char>& dst,
                                                              tid<short>& src,
                                                              int position) {
  __mlvm_trc_cvtu8_int16_dn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2uchar_rd(tid<unsigned char>& dst,
                                                              tid<short>& src,
                                                              int position) {
  __mlvm_trc_cvtu8_int16_rd(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2uchar_rn(tid<unsigned char>& dst,
                                                              tid<short>& src,
                                                              int position) {
  __mlvm_trc_cvtu8_int16_rn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_short2uchar_rm(tid<unsigned char>& dst,
                                                              tid<short>& src,
                                                              int position) {
  __mlvm_trc_cvtu8_int16_rm(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2uchar(tid<unsigned char>& dst,
                                                         tid<int>& src,
                                                         int position) {
  __mlvm_trc_cvtu8_int32(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2uchar_tz(tid<unsigned char>& dst,
                                                            tid<int>& src,
                                                            int position) {
  __mlvm_trc_cvtu8_int32_tz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2uchar_oz(tid<unsigned char>& dst,
                                                            tid<int>& src,
                                                            int position) {
  __mlvm_trc_cvtu8_int32_oz(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2uchar_up(tid<unsigned char>& dst,
                                                            tid<int>& src,
                                                            int position) {
  __mlvm_trc_cvtu8_int32_up(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2uchar_dn(tid<unsigned char>& dst,
                                                            tid<int>& src,
                                                            int position) {
  __mlvm_trc_cvtu8_int32_dn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2uchar_rd(tid<unsigned char>& dst,
                                                            tid<int>& src,
                                                            int position) {
  __mlvm_trc_cvtu8_int32_rd(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2uchar_rn(tid<unsigned char>& dst,
                                                            tid<int>& src,
                                                            int position) {
  __mlvm_trc_cvtu8_int32_rn(dst.tid, src.tid, position);
}
static __mlu_func__ __mlu_builtin__ void __trc_int2uchar_rm(tid<unsigned char>& dst,
                                                            tid<int>& src,
                                                            int position) {
  __mlvm_trc_cvtu8_int32_rm(dst.tid, src.tid, position);
}

template <typename T>
static __mlu_func__ __mlu_builtin__ void __trc_move(tid<T> &dst,
                                                    tid<T> &src) {
  switch(sizeof(T)) {
    default:
      // Unsupport memcpy direction.
      asm volatile("exit %[status];\n\t"
                   ::[status]"i"(TRAP_BANG_UNSUPPORTED_MEMCPY_DIR));
      break;
    case 1: { __mlvm_trc_move_b8(dst.tid, src.tid); break; }
    case 2: { __mlvm_trc_move_b16(dst.tid, src.tid); break; }
    case 4: { __mlvm_trc_move_b32(dst.tid, src.tid); break; }
  }
}

static __mlu_func__ __mlu_builtin__ void __trc_argmax(tid<half>& dst,
                                                      tid<half>& src) {
  __mlvm_trc_argmax_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_argmax(tid<float>& dst,
                                                      tid<float>& src) {
  __mlvm_trc_argmax_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_argmaxabs(tid<half>& dst,
                                                         tid<half>& src) {
  __mlvm_trc_argmaxabs_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_argmaxabs(tid<float>& dst,
                                                         tid<float>& src) {
  __mlvm_trc_argmaxabs_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_argmin(tid<half>& dst,
                                                      tid<half>& src) {
  __mlvm_trc_argmin_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_argmin(tid<float>& dst,
                                                      tid<float>& src) {
  __mlvm_trc_argmin_f32(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_argminabs(tid<half>& dst,
                                                         tid<half>& src) {
  __mlvm_trc_argminabs_f16(dst.tid, src.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_argminabs(tid<float>& dst,
                                                         tid<float>& src) {
  __mlvm_trc_argminabs_f32(dst.tid, src.tid);
}

#if (__BANG_ARCH__ != 320) && (__BANG_ARCH__ != 321)

// tensor atomic add
static __mlu_func__ __mlu_builtin__ void __trc_atom_add(tid<unsigned short>& dst,
                                                        tid<unsigned short>& src1,
                                                        tid<unsigned short>& src2) {
  __mlvm_trc_atom_add_u16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add(tid<unsigned int>& dst,
                                                        tid<unsigned int>& src1,
                                                        tid<unsigned int>& src2) {
  __mlvm_trc_atom_add_u32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add(tid<short>& dst,
                                                        tid<short>& src1,
                                                        tid<short>& src2) {
  __mlvm_trc_atom_add_s16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add(tid<int>& dst,
                                                        tid<int>& src1,
                                                        tid<int>& src2) {
  __mlvm_trc_atom_add_s32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add(tid<half>& dst,
                                                        tid<half>& src1,
                                                        tid<half>& src2) {
  __mlvm_trc_atom_add_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add(tid<float>& dst,
                                                        tid<float>& src1,
                                                        tid<float>& src2) {
  __mlvm_trc_atom_add_f32(dst.tid, src1.tid, src2.tid);
}

// tensor atomic add reduce
static __mlu_func__ __mlu_builtin__ void __trc_atom_add_reduce(tid<unsigned short>& dst,
                                                               tid<unsigned short>& src1) {
  __mlvm_trc_atom_add_reduce_u16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add_reduce(tid<unsigned int>& dst,
                                                               tid<unsigned int>& src1) {
  __mlvm_trc_atom_add_reduce_u32(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add_reduce(tid<short>& dst,
                                                               tid<short>& src1) {
  __mlvm_trc_atom_add_reduce_s16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add_reduce(tid<int>& dst,
                                                               tid<int>& src1) {
  __mlvm_trc_atom_add_reduce_s32(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add_reduce(tid<half>& dst,
                                                               tid<half>& src1) {
  __mlvm_trc_atom_add_reduce_f16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add_reduce(tid<float>& dst,
                                                               tid<float>& src1) {
  __mlvm_trc_atom_add_reduce_f32(dst.tid, src1.tid);
}

// tensor atomic add scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_add(tid<unsigned short>& dst,
                                                        tid<unsigned short>& src1,
                                                        unsigned short src2) {
  __mlvm_trc_atom_add_scalar_u16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add(tid<unsigned int>& dst,
                                                        tid<unsigned int>& src1,
                                                        unsigned int src2) {
  __mlvm_trc_atom_add_scalar_u32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add(tid<short>& dst,
                                                        tid<short>& src1,
                                                        short src2) {
  __mlvm_trc_atom_add_scalar_s16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add(tid<int>& dst,
                                                        tid<int>& src1,
                                                        int src2) {
  __mlvm_trc_atom_add_scalar_s32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add(tid<half>& dst,
                                                        tid<half>& src1,
                                                        half src2) {
  __mlvm_trc_atom_add_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add(tid<float>& dst,
                                                        tid<float>& src1,
                                                        float src2) {
  __mlvm_trc_atom_add_scalar_f32(dst.tid, src1.tid, src2);
}

// tensor atomic add reduce scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_add_reduce(tid<unsigned short>& dst,
                                                               unsigned short src1) {
  __mlvm_trc_atom_add_reduce_scalar_u16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add_reduce(tid<unsigned int>& dst,
                                                               unsigned int src1) {
  __mlvm_trc_atom_add_reduce_scalar_u32(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add_reduce(tid<short>& dst,
                                                               short src1) {
  __mlvm_trc_atom_add_reduce_scalar_s16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add_reduce(tid<int>& dst,
                                                               int src1) {
  __mlvm_trc_atom_add_reduce_scalar_s32(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add_reduce(tid<half>& dst,
                                                               half src1) {
  __mlvm_trc_atom_add_reduce_scalar_f16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_add_reduce(tid<float>& dst,
                                                               float src1) {
  __mlvm_trc_atom_add_reduce_scalar_f32(dst.tid, src1);
}

// tensor atomic max
static __mlu_func__ __mlu_builtin__ void __trc_atom_max(tid<unsigned short>& dst,
                                                        tid<unsigned short>& src1,
                                                        tid<unsigned short>& src2) {
  __mlvm_trc_atom_max_u16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max(tid<unsigned int>& dst,
                                                        tid<unsigned int>& src1,
                                                        tid<unsigned int>& src2) {
  __mlvm_trc_atom_max_u32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max(tid<short>& dst,
                                                        tid<short>& src1,
                                                        tid<short>& src2) {
  __mlvm_trc_atom_max_s16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max(tid<int>& dst,
                                                        tid<int>& src1,
                                                        tid<int>& src2) {
  __mlvm_trc_atom_max_s32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max(tid<half>& dst,
                                                        tid<half>& src1,
                                                        tid<half>& src2) {
  __mlvm_trc_atom_max_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max(tid<float>& dst,
                                                        tid<float>& src1,
                                                        tid<float>& src2) {
  __mlvm_trc_atom_max_f32(dst.tid, src1.tid, src2.tid);
}

// tensor atomic max reduce
static __mlu_func__ __mlu_builtin__ void __trc_atom_max_reduce(tid<unsigned short>& dst,
                                                               tid<unsigned short>& src1) {
  __mlvm_trc_atom_max_reduce_u16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max_reduce(tid<unsigned int>& dst,
                                                               tid<unsigned int>& src1) {
  __mlvm_trc_atom_max_reduce_u32(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max_reduce(tid<short>& dst,
                                                               tid<short>& src1) {
  __mlvm_trc_atom_max_reduce_s16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max_reduce(tid<int>& dst,
                                                               tid<int>& src1) {
  __mlvm_trc_atom_max_reduce_s32(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max_reduce(tid<half>& dst,
                                                               tid<half>& src1) {
  __mlvm_trc_atom_max_reduce_f16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max_reduce(tid<float>& dst,
                                                               tid<float>& src1) {
  __mlvm_trc_atom_max_reduce_f32(dst.tid, src1.tid);
}

// tensor atomic max scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_max(tid<unsigned short>& dst,
                                                        tid<unsigned short>& src1,
                                                        unsigned short src2) {
  __mlvm_trc_atom_max_scalar_u16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max(tid<unsigned int>& dst,
                                                        tid<unsigned int>& src1,
                                                        unsigned int src2) {
  __mlvm_trc_atom_max_scalar_u32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max(tid<short>& dst,
                                                        tid<short>& src1,
                                                        short src2) {
  __mlvm_trc_atom_max_scalar_s16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max(tid<int>& dst,
                                                        tid<int>& src1,
                                                        int src2) {
  __mlvm_trc_atom_max_scalar_s32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max(tid<half>& dst,
                                                        tid<half>& src1,
                                                        half src2) {
  __mlvm_trc_atom_max_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max(tid<float>& dst,
                                                        tid<float>& src1,
                                                        float src2) {
  __mlvm_trc_atom_max_scalar_f32(dst.tid, src1.tid, src2);
}

// tensor atomic max reduce scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_max_reduce(tid<unsigned short>& dst,
                                                               unsigned short src1) {
  __mlvm_trc_atom_max_reduce_scalar_u16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max_reduce(tid<unsigned int>& dst,
                                                               unsigned int src1) {
  __mlvm_trc_atom_max_reduce_scalar_u32(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max_reduce(tid<short>& dst,
                                                               short src1) {
  __mlvm_trc_atom_max_reduce_scalar_s16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max_reduce(tid<int>& dst,
                                                               int src1) {
  __mlvm_trc_atom_max_reduce_scalar_s32(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max_reduce(tid<half>& dst,
                                                               half src1) {
  __mlvm_trc_atom_max_reduce_scalar_f16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_max_reduce(tid<float>& dst,
                                                               float src1) {
  __mlvm_trc_atom_max_reduce_scalar_f32(dst.tid, src1);
}

// tensor atomic min
static __mlu_func__ __mlu_builtin__ void __trc_atom_min(tid<unsigned short>& dst,
                                                        tid<unsigned short>& src1,
                                                        tid<unsigned short>& src2) {
  __mlvm_trc_atom_min_u16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min(tid<unsigned int>& dst,
                                                        tid<unsigned int>& src1,
                                                        tid<unsigned int>& src2) {
  __mlvm_trc_atom_min_u32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min(tid<short>& dst,
                                                        tid<short>& src1,
                                                        tid<short>& src2) {
  __mlvm_trc_atom_min_s16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min(tid<int>& dst,
                                                        tid<int>& src1,
                                                        tid<int>& src2) {
  __mlvm_trc_atom_min_s32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min(tid<half>& dst,
                                                        tid<half>& src1,
                                                        tid<half>& src2) {
  __mlvm_trc_atom_min_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min(tid<float>& dst,
                                                        tid<float>& src1,
                                                        tid<float>& src2) {
  __mlvm_trc_atom_min_f32(dst.tid, src1.tid, src2.tid);
}

// tensor atomic min reduce
static __mlu_func__ __mlu_builtin__ void __trc_atom_min_reduce(tid<unsigned short>& dst,
                                                               tid<unsigned short>& src1) {
  __mlvm_trc_atom_min_reduce_u16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min_reduce(tid<unsigned int>& dst,
                                                               tid<unsigned int>& src1) {
  __mlvm_trc_atom_min_reduce_u32(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min_reduce(tid<short>& dst,
                                                               tid<short>& src1) {
  __mlvm_trc_atom_min_reduce_s16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min_reduce(tid<int>& dst,
                                                               tid<int>& src1) {
  __mlvm_trc_atom_min_reduce_s32(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min_reduce(tid<half>& dst,
                                                               tid<half>& src1) {
  __mlvm_trc_atom_min_reduce_f16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min_reduce(tid<float>& dst,
                                                               tid<float>& src1) {
  __mlvm_trc_atom_min_reduce_f32(dst.tid, src1.tid);
}

// tensor atomic min scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_min(tid<unsigned short>& dst,
                                                        tid<unsigned short>& src1,
                                                        unsigned short src2) {
  __mlvm_trc_atom_min_scalar_u16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min(tid<unsigned int>& dst,
                                                        tid<unsigned int>& src1,
                                                        unsigned int src2) {
  __mlvm_trc_atom_min_scalar_u32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min(tid<short>& dst,
                                                        tid<short>& src1,
                                                        short src2) {
  __mlvm_trc_atom_min_scalar_s16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min(tid<int>& dst,
                                                        tid<int>& src1,
                                                        int src2) {
  __mlvm_trc_atom_min_scalar_s32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min(tid<half>& dst,
                                                        tid<half>& src1,
                                                        half src2) {
  __mlvm_trc_atom_min_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min(tid<float>& dst,
                                                        tid<float>& src1,
                                                        float src2) {
  __mlvm_trc_atom_min_scalar_f32(dst.tid, src1.tid, src2);
}

// tensor atomic min reduce scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_min_reduce(tid<unsigned short>& dst,
                                                               unsigned short src1) {
  __mlvm_trc_atom_min_reduce_scalar_u16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min_reduce(tid<unsigned int>& dst,
                                                               unsigned int src1) {
  __mlvm_trc_atom_min_reduce_scalar_u32(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min_reduce(tid<short>& dst,
                                                               short src1) {
  __mlvm_trc_atom_min_reduce_scalar_s16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min_reduce(tid<int>& dst,
                                                               int src1) {
  __mlvm_trc_atom_min_reduce_scalar_s32(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min_reduce(tid<half>& dst,
                                                               half src1) {
  __mlvm_trc_atom_min_reduce_scalar_f16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_min_reduce(tid<float>& dst,
                                                               float src1) {
  __mlvm_trc_atom_min_reduce_scalar_f32(dst.tid, src1);
}

// tensor atomic exch
static __mlu_func__ __mlu_builtin__ void __trc_atom_exch(tid<unsigned short>& dst,
                                                         tid<unsigned short>& src1,
                                                         tid<unsigned short>& src2) {
  __mlvm_trc_atom_exch_u16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch(tid<unsigned int>& dst,
                                                         tid<unsigned int>& src1,
                                                         tid<unsigned int>& src2) {
  __mlvm_trc_atom_exch_u32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch(tid<short>& dst,
                                                         tid<short>& src1,
                                                         tid<short>& src2) {
  __mlvm_trc_atom_exch_s16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch(tid<int>& dst,
                                                         tid<int>& src1,
                                                         tid<int>& src2) {
  __mlvm_trc_atom_exch_s32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch(tid<half>& dst,
                                                         tid<half>& src1,
                                                         tid<half>& src2) {
  __mlvm_trc_atom_exch_f16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch(tid<float>& dst,
                                                         tid<float>& src1,
                                                         tid<float>& src2) {
  __mlvm_trc_atom_exch_f32(dst.tid, src1.tid, src2.tid);
}

// tensor atomic exch reduce
static __mlu_func__ __mlu_builtin__ void __trc_atom_exch_reduce(tid<unsigned short>& dst,
                                                                tid<unsigned short>& src1) {
  __mlvm_trc_atom_exch_reduce_u16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch_reduce(tid<unsigned int>& dst,
                                                                tid<unsigned int>& src1) {
  __mlvm_trc_atom_exch_reduce_u32(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch_reduce(tid<short>& dst,
                                                                tid<short>& src1) {
  __mlvm_trc_atom_exch_reduce_s16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch_reduce(tid<int>& dst,
                                                                tid<int>& src1) {
  __mlvm_trc_atom_exch_reduce_s32(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch_reduce(tid<half>& dst,
                                                                tid<half>& src1) {
  __mlvm_trc_atom_exch_reduce_f16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch_reduce(tid<float>& dst,
                                                                tid<float>& src1) {
  __mlvm_trc_atom_exch_reduce_f32(dst.tid, src1.tid);
}

// tensor atomic exch scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_exch(tid<unsigned short>& dst,
                                                         tid<unsigned short>& src1,
                                                         unsigned short src2) {
  __mlvm_trc_atom_exch_scalar_u16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch(tid<unsigned int>& dst,
                                                         tid<unsigned int>& src1,
                                                         unsigned int src2) {
  __mlvm_trc_atom_exch_scalar_u32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch(tid<short>& dst,
                                                         tid<short>& src1,
                                                         short src2) {
  __mlvm_trc_atom_exch_scalar_s16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch(tid<int>& dst,
                                                         tid<int>& src1,
                                                         int src2) {
  __mlvm_trc_atom_exch_scalar_s32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch(tid<half>& dst,
                                                         tid<half>& src1,
                                                         half src2) {
  __mlvm_trc_atom_exch_scalar_f16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch(tid<float>& dst,
                                                         tid<float>& src1,
                                                         float src2) {
  __mlvm_trc_atom_exch_scalar_f32(dst.tid, src1.tid, src2);
}

// tensor atomic exch reduce scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_exch_reduce(tid<unsigned short>& dst,
                                                                unsigned short src1) {
  __mlvm_trc_atom_exch_reduce_scalar_u16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch_reduce(tid<unsigned int>& dst,
                                                                unsigned int src1) {
  __mlvm_trc_atom_exch_reduce_scalar_u32(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch_reduce(tid<short>& dst,
                                                                short src1) {
  __mlvm_trc_atom_exch_reduce_scalar_s16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch_reduce(tid<int>& dst,
                                                                int src1) {
  __mlvm_trc_atom_exch_reduce_scalar_s32(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch_reduce(tid<half>& dst,
                                                                half src1) {
  __mlvm_trc_atom_exch_reduce_scalar_f16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_exch_reduce(tid<float>& dst,
                                                                float src1) {
  __mlvm_trc_atom_exch_reduce_scalar_f32(dst.tid, src1);
}

// tensor atomic inc
static __mlu_func__ __mlu_builtin__ void __trc_atom_inc(tid<unsigned short>& dst,
                                                        tid<unsigned short>& src1,
                                                        tid<unsigned short>& src2) {
  __mlvm_trc_atom_inc_u16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_inc(tid<unsigned int>& dst,
                                                        tid<unsigned int>& src1,
                                                        tid<unsigned int>& src2) {
  __mlvm_trc_atom_inc_u32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_inc(tid<short>& dst,
                                                        tid<short>& src1,
                                                        tid<short>& src2) {
  __mlvm_trc_atom_inc_s16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_inc(tid<int>& dst,
                                                        tid<int>& src1,
                                                        tid<int>& src2) {
  __mlvm_trc_atom_inc_s32(dst.tid, src1.tid, src2.tid);
}

// tensor atomic inc reduce
static __mlu_func__ __mlu_builtin__ void __trc_atom_inc_reduce(tid<unsigned short>& dst,
                                                               tid<unsigned short>& src1) {
  __mlvm_trc_atom_inc_reduce_u16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_inc_reduce(tid<unsigned int>& dst,
                                                               tid<unsigned int>& src1) {
  __mlvm_trc_atom_inc_reduce_u32(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_inc_reduce(tid<short>& dst,
                                                               tid<short>& src1) {
  __mlvm_trc_atom_inc_reduce_s16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_inc_reduce(tid<int>& dst,
                                                               tid<int>& src1) {
  __mlvm_trc_atom_inc_reduce_s32(dst.tid, src1.tid);
}

// tensor atomic inc scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_inc(tid<unsigned short>& dst,
                                                        tid<unsigned short>& src1,
                                                        unsigned short src2) {
  __mlvm_trc_atom_inc_scalar_u16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_inc(tid<unsigned int>& dst,
                                                        tid<unsigned int>& src1,
                                                        unsigned int src2) {
  __mlvm_trc_atom_inc_scalar_u32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_inc(tid<short>& dst,
                                                        tid<short>& src1,
                                                        short src2) {
  __mlvm_trc_atom_inc_scalar_s16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_inc(tid<int>& dst,
                                                        tid<int>& src1,
                                                        int src2) {
  __mlvm_trc_atom_inc_scalar_s32(dst.tid, src1.tid, src2);
}

// tensor atomic inc reduce scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_inc_reduce(tid<unsigned short>& dst,
                                                               unsigned short src1) {
  __mlvm_trc_atom_inc_reduce_scalar_u16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_inc_reduce(tid<unsigned int>& dst,
                                                               unsigned int src1) {
  __mlvm_trc_atom_inc_reduce_scalar_u32(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_inc_reduce(tid<short>& dst,
                                                               short src1) {
  __mlvm_trc_atom_inc_reduce_scalar_s16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_inc_reduce(tid<int>& dst,
                                                               int src1) {
  __mlvm_trc_atom_inc_reduce_scalar_s32(dst.tid, src1);
}

// tensor atomic dec
static __mlu_func__ __mlu_builtin__ void __trc_atom_dec(tid<unsigned short>& dst,
                                                        tid<unsigned short>& src1,
                                                        tid<unsigned short>& src2) {
  __mlvm_trc_atom_dec_u16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_dec(tid<unsigned int>& dst,
                                                        tid<unsigned int>& src1,
                                                        tid<unsigned int>& src2) {
  __mlvm_trc_atom_dec_u32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_dec(tid<short>& dst,
                                                        tid<short>& src1,
                                                        tid<short>& src2) {
  __mlvm_trc_atom_dec_s16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_dec(tid<int>& dst,
                                                        tid<int>& src1,
                                                        tid<int>& src2) {
  __mlvm_trc_atom_dec_s32(dst.tid, src1.tid, src2.tid);
}

// tensor atomic dec reduce
static __mlu_func__ __mlu_builtin__ void __trc_atom_dec_reduce(tid<unsigned short>& dst,
                                                               tid<unsigned short>& src1) {
  __mlvm_trc_atom_dec_reduce_u16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_dec_reduce(tid<unsigned int>& dst,
                                                               tid<unsigned int>& src1) {
  __mlvm_trc_atom_dec_reduce_u32(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_dec_reduce(tid<short>& dst,
                                                               tid<short>& src1) {
  __mlvm_trc_atom_dec_reduce_s16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_dec_reduce(tid<int>& dst,
                                                               tid<int>& src1) {
  __mlvm_trc_atom_dec_reduce_s32(dst.tid, src1.tid);
}

// tensor atomic dec scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_dec(tid<unsigned short>& dst,
                                                        tid<unsigned short>& src1,
                                                        unsigned short src2) {
  __mlvm_trc_atom_dec_scalar_u16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_dec(tid<unsigned int>& dst,
                                                        tid<unsigned int>& src1,
                                                        unsigned int src2) {
  __mlvm_trc_atom_dec_scalar_u32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_dec(tid<short>& dst,
                                                        tid<short>& src1,
                                                        short src2) {
  __mlvm_trc_atom_dec_scalar_s16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_dec(tid<int>& dst,
                                                        tid<int>& src1,
                                                        int src2) {
  __mlvm_trc_atom_dec_scalar_s32(dst.tid, src1.tid, src2);
}

// tensor atomic dec reduce scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_dec_reduce(tid<unsigned short>& dst,
                                                               unsigned short src1) {
  __mlvm_trc_atom_dec_reduce_scalar_u16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_dec_reduce(tid<unsigned int>& dst,
                                                               unsigned int src1) {
  __mlvm_trc_atom_dec_reduce_scalar_u32(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_dec_reduce(tid<short>& dst,
                                                               short src1) {
  __mlvm_trc_atom_dec_reduce_scalar_s16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_dec_reduce(tid<int>& dst,
                                                               int src1) {
  __mlvm_trc_atom_dec_reduce_scalar_s32(dst.tid, src1);
}

// tensor atomic and
static __mlu_func__ __mlu_builtin__ void __trc_atom_and(tid<unsigned short>& dst,
                                                        tid<unsigned short>& src1,
                                                        tid<unsigned short>& src2) {
  __mlvm_trc_atom_and_u16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_and(tid<unsigned int>& dst,
                                                        tid<unsigned int>& src1,
                                                        tid<unsigned int>& src2) {
  __mlvm_trc_atom_and_u32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_and(tid<short>& dst,
                                                        tid<short>& src1,
                                                        tid<short>& src2) {
  __mlvm_trc_atom_and_s16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_and(tid<int>& dst,
                                                        tid<int>& src1,
                                                        tid<int>& src2) {
  __mlvm_trc_atom_and_s32(dst.tid, src1.tid, src2.tid);
}

// tensor atomic and reduce
static __mlu_func__ __mlu_builtin__ void __trc_atom_and_reduce(tid<unsigned short>& dst,
                                                               tid<unsigned short>& src1) {
  __mlvm_trc_atom_and_reduce_u16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_and_reduce(tid<unsigned int>& dst,
                                                               tid<unsigned int>& src1) {
  __mlvm_trc_atom_and_reduce_u32(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_and_reduce(tid<short>& dst,
                                                               tid<short>& src1) {
  __mlvm_trc_atom_and_reduce_s16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_and_reduce(tid<int>& dst,
                                                               tid<int>& src1) {
  __mlvm_trc_atom_and_reduce_s32(dst.tid, src1.tid);
}

// tensor atomic and scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_and(tid<unsigned short>& dst,
                                                        tid<unsigned short>& src1,
                                                        unsigned short src2) {
  __mlvm_trc_atom_and_scalar_u16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_and(tid<unsigned int>& dst,
                                                        tid<unsigned int>& src1,
                                                        unsigned int src2) {
  __mlvm_trc_atom_and_scalar_u32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_and(tid<short>& dst,
                                                        tid<short>& src1,
                                                        short src2) {
  __mlvm_trc_atom_and_scalar_s16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_and(tid<int>& dst,
                                                        tid<int>& src1,
                                                        int src2) {
  __mlvm_trc_atom_and_scalar_s32(dst.tid, src1.tid, src2);
}

// tensor atomic and reduce scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_and_reduce(tid<unsigned short>& dst,
                                                               unsigned short src1) {
  __mlvm_trc_atom_and_reduce_scalar_u16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_and_reduce(tid<unsigned int>& dst,
                                                               unsigned int src1) {
  __mlvm_trc_atom_and_reduce_scalar_u32(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_and_reduce(tid<short>& dst,
                                                               short src1) {
  __mlvm_trc_atom_and_reduce_scalar_s16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_and_reduce(tid<int>& dst,
                                                               int src1) {
  __mlvm_trc_atom_and_reduce_scalar_s32(dst.tid, src1);
}

// tensor atomic or
static __mlu_func__ __mlu_builtin__ void __trc_atom_or(tid<unsigned short>& dst,
                                                       tid<unsigned short>& src1,
                                                       tid<unsigned short>& src2) {
  __mlvm_trc_atom_or_u16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_or(tid<unsigned int>& dst,
                                                       tid<unsigned int>& src1,
                                                       tid<unsigned int>& src2) {
  __mlvm_trc_atom_or_u32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_or(tid<short>& dst,
                                                       tid<short>& src1,
                                                       tid<short>& src2) {
  __mlvm_trc_atom_or_s16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_or(tid<int>& dst,
                                                       tid<int>& src1,
                                                       tid<int>& src2) {
  __mlvm_trc_atom_or_s32(dst.tid, src1.tid, src2.tid);
}

// tensor atomic or reduce
static __mlu_func__ __mlu_builtin__ void __trc_atom_or_reduce(tid<unsigned short>& dst,
                                                              tid<unsigned short>& src1) {
  __mlvm_trc_atom_or_reduce_u16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_or_reduce(tid<unsigned int>& dst,
                                                              tid<unsigned int>& src1) {
  __mlvm_trc_atom_or_reduce_u32(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_or_reduce(tid<short>& dst,
                                                              tid<short>& src1) {
  __mlvm_trc_atom_or_reduce_s16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_or_reduce(tid<int>& dst,
                                                              tid<int>& src1) {
  __mlvm_trc_atom_or_reduce_s32(dst.tid, src1.tid);
}

// tensor atomic or scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_or(tid<unsigned short>& dst,
                                                       tid<unsigned short>& src1,
                                                       unsigned short src2) {
  __mlvm_trc_atom_or_scalar_u16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_or(tid<unsigned int>& dst,
                                                       tid<unsigned int>& src1,
                                                       unsigned int src2) {
  __mlvm_trc_atom_or_scalar_u32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_or(tid<short>& dst,
                                                       tid<short>& src1,
                                                       short src2) {
  __mlvm_trc_atom_or_scalar_s16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_or(tid<int>& dst,
                                                       tid<int>& src1,
                                                       int src2) {
  __mlvm_trc_atom_or_scalar_s32(dst.tid, src1.tid, src2);
}

// tensor atomic or reduce scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_or_reduce(tid<unsigned short>& dst,
                                                              unsigned short src1) {
  __mlvm_trc_atom_or_reduce_scalar_u16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_or_reduce(tid<unsigned int>& dst,
                                                              unsigned int src1) {
  __mlvm_trc_atom_or_reduce_scalar_u32(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_or_reduce(tid<short>& dst,
                                                              short src1) {
  __mlvm_trc_atom_or_reduce_scalar_s16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_or_reduce(tid<int>& dst,
                                                              int src1) {
  __mlvm_trc_atom_or_reduce_scalar_s32(dst.tid, src1);
}

// tensor atomic xor
static __mlu_func__ __mlu_builtin__ void __trc_atom_xor(tid<unsigned short>& dst,
                                                        tid<unsigned short>& src1,
                                                        tid<unsigned short>& src2) {
  __mlvm_trc_atom_xor_u16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_xor(tid<unsigned int>& dst,
                                                        tid<unsigned int>& src1,
                                                        tid<unsigned int>& src2) {
  __mlvm_trc_atom_xor_u32(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_xor(tid<short>& dst,
                                                        tid<short>& src1,
                                                        tid<short>& src2) {
  __mlvm_trc_atom_xor_s16(dst.tid, src1.tid, src2.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_xor(tid<int>& dst,
                                                        tid<int>& src1,
                                                        tid<int>& src2) {
  __mlvm_trc_atom_xor_s32(dst.tid, src1.tid, src2.tid);
}

// tensor atomic xor reduce
static __mlu_func__ __mlu_builtin__ void __trc_atom_xor_reduce(tid<unsigned short>& dst,
                                                               tid<unsigned short>& src1) {
  __mlvm_trc_atom_xor_reduce_u16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_xor_reduce(tid<unsigned int>& dst,
                                                               tid<unsigned int>& src1) {
  __mlvm_trc_atom_xor_reduce_u32(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_xor_reduce(tid<short>& dst,
                                                               tid<short>& src1) {
  __mlvm_trc_atom_xor_reduce_s16(dst.tid, src1.tid);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_xor_reduce(tid<int>& dst,
                                                               tid<int>& src1) {
  __mlvm_trc_atom_xor_reduce_s32(dst.tid, src1.tid);
}

// tensor atomic xor scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_xor(tid<unsigned short>& dst,
                                                        tid<unsigned short>& src1,
                                                        unsigned short src2) {
  __mlvm_trc_atom_xor_scalar_u16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_xor(tid<unsigned int>& dst,
                                                        tid<unsigned int>& src1,
                                                        unsigned int src2) {
  __mlvm_trc_atom_xor_scalar_u32(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_xor(tid<short>& dst,
                                                        tid<short>& src1,
                                                        short src2) {
  __mlvm_trc_atom_xor_scalar_s16(dst.tid, src1.tid, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_xor(tid<int>& dst,
                                                        tid<int>& src1,
                                                        int src2) {
  __mlvm_trc_atom_xor_scalar_s32(dst.tid, src1.tid, src2);
}

// tensor atomic xor reduce scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_xor_reduce(tid<unsigned short>& dst,
                                                               unsigned short src1) {
  __mlvm_trc_atom_xor_reduce_scalar_u16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_xor_reduce(tid<unsigned int>& dst,
                                                               unsigned int src1) {
  __mlvm_trc_atom_xor_reduce_scalar_u32(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_xor_reduce(tid<short>& dst,
                                                               short src1) {
  __mlvm_trc_atom_xor_reduce_scalar_s16(dst.tid, src1);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_xor_reduce(tid<int>& dst,
                                                               int src1) {
  __mlvm_trc_atom_xor_reduce_scalar_s32(dst.tid, src1);
}

// tensor atomic cas scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_cas(tid<unsigned short>& dst,
                                                        tid<unsigned short>& src1,
                                                        unsigned short src2,
                                                        unsigned short src3) {
  __mlvm_trc_atom_cas_scalar_u16(dst.tid, src1.tid, src2, src3);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_cas(tid<unsigned int>& dst,
                                                        tid<unsigned int>& src1,
                                                        unsigned int src2,
                                                        unsigned int src3) {
  __mlvm_trc_atom_cas_scalar_u32(dst.tid, src1.tid, src2, src3);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_cas(tid<short>& dst,
                                                        tid<short>& src1,
                                                        short src2,
                                                        short src3) {
  __mlvm_trc_atom_cas_scalar_s16(dst.tid, src1.tid, src2, src3);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_cas(tid<int>& dst,
                                                        tid<int>& src1,
                                                        int src2,
                                                        int src3) {
  __mlvm_trc_atom_cas_scalar_s32(dst.tid, src1.tid, src2, src3);
}

// tensor atomic cas reduce scalar
static __mlu_func__ __mlu_builtin__ void __trc_atom_cas_reduce(tid<unsigned short>& dst,
                                                               unsigned short src1,
                                                               unsigned short src2) {
  __mlvm_trc_atom_cas_reduce_scalar_u16(dst.tid, src1, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_cas_reduce(tid<unsigned int>& dst,
                                                               unsigned int src1,
                                                               unsigned int src2) {
  __mlvm_trc_atom_cas_reduce_scalar_u32(dst.tid, src1, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_cas_reduce(tid<short>& dst,
                                                               short src1,
                                                               short src2) {
  __mlvm_trc_atom_cas_reduce_scalar_s16(dst.tid, src1, src2);
}

static __mlu_func__ __mlu_builtin__ void __trc_atom_cas_reduce(tid<int>& dst,
                                                               int src1,
                                                               int src2) {
  __mlvm_trc_atom_cas_reduce_scalar_s32(dst.tid, src1, src2);
}
#endif // (__BANG_ARCH__ != 320) && (__BANG_ARCH__ != 321)


#endif // __BANG_ARCH__ > 306
#endif  // __BANG_DEVICE_FUNCTIONS_H__
