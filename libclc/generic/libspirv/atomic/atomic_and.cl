//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <spirv/spirv.h>

// TODO: Stop manually mangling this name. Need C++ namespaces to get the exact mangling.

#define IMPL(TYPE, TYPE_MANGLED, AS, AS_MANGLED, FN_NAME)                                                                   \
  _CLC_DEF TYPE                                                                                                             \
      _Z17__spirv_AtomicAndPU3##AS_MANGLED##TYPE_MANGLED##N5__spv5Scope4FlagENS1_19MemorySemanticsMask4FlagE##TYPE_MANGLED( \
          volatile AS TYPE *p, enum Scope scope,                                                                            \
          enum MemorySemanticsMask semantics, TYPE val) {                                                                   \
    return FN_NAME(p, val);                                                                                                 \
  }

IMPL(int, i, global, AS1, __sync_fetch_and_and)
IMPL(unsigned int, j, global, AS1, __sync_fetch_and_and)
IMPL(int, i, local, AS3, __sync_fetch_and_and)
IMPL(unsigned int, j, local, AS3, __sync_fetch_and_and)

#ifdef cl_khr_int64_extended_atomics
IMPL(long, l, global, AS1, __sync_fetch_and_and_8)
IMPL(unsigned long, m, global, AS1, __sync_fetch_and_and_8)
IMPL(long, l, local, AS3, __sync_fetch_and_and_8)
IMPL(unsigned long, m, local, AS3, __sync_fetch_and_and_8)
IMPL(long, x, global, AS1, __sync_fetch_and_and_8)
IMPL(unsigned long, y, global, AS1, __sync_fetch_and_and_8)
IMPL(long, x, local, AS3, __sync_fetch_and_and_8)
IMPL(unsigned long, y, local, AS3, __sync_fetch_and_and_8)
#endif
#undef IMPL