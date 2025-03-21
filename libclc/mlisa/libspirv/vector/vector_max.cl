//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <spirv/spirv.h>

#include "../../include/libdevice.h"
#include <clcmacro.h>

// use relu when y == 0
#define VECTOR_MAX_SCALAR_RELU(TYPE, NAME)                                \
void __vector_max_scalar_##NAME(int n, TYPE *z, const TYPE *x, TYPE y) {  \
  if (y == (TYPE)0)                                                       \
    __mlvm_stream_relu_##NAME(z, (TYPE *)x, n);                           \
  else                                                                    \
    __cn_vector_max_scalar_##NAME(n, z, x, y);                            \
}
#define VECTOR_MAX_SCALAR(TYPE, NAME)                                     \
void __vector_max_scalar_##NAME(int n, TYPE *z, const TYPE *x, TYPE y) {  \
  __cn_vector_max_scalar_##NAME(n, z, x, y);                              \
}
VECTOR_MAX_SCALAR_RELU(float, f32)
VECTOR_MAX_SCALAR_RELU(half, f16)
VECTOR_MAX_SCALAR_RELU(int, s32)
VECTOR_MAX_SCALAR(unsigned int, u32)
VECTOR_MAX_SCALAR(long, s64)
VECTOR_MAX_SCALAR(unsigned long, u64)
#undef VECTOR_MAX_SCALAR_RELU
#undef VECTOR_MAX_SCALAR

#define __CLC_FUNCTION __spirv_ocl_vector_max
#define __CLC_BUILTIN __cn_vector_max
#define __CLC_BUILTIN_F __CLC_XCONCAT(__CLC_BUILTIN, _f32)
#define __CLC_BUILTIN_H __CLC_XCONCAT(__CLC_BUILTIN, _f16)
#define __CLC_BUILTIN_I __CLC_XCONCAT(__CLC_BUILTIN, _s32)
#define __CLC_BUILTIN_U __CLC_XCONCAT(__CLC_BUILTIN, _u32)
#define __CLC_BUILTIN_L __CLC_XCONCAT(__CLC_BUILTIN, _s64)
#define __CLC_BUILTIN_UL __CLC_XCONCAT(__CLC_BUILTIN, _u64)
#define __CLC_BUILTIN_SCALAR __vector_max_scalar
#define __CLC_BUILTIN_SCALAR_F __CLC_XCONCAT(__CLC_BUILTIN_SCALAR, _f32)
#define __CLC_BUILTIN_SCALAR_H __CLC_XCONCAT(__CLC_BUILTIN_SCALAR, _f16)
#define __CLC_BUILTIN_SCALAR_I __CLC_XCONCAT(__CLC_BUILTIN_SCALAR, _s32)
#define __CLC_BUILTIN_SCALAR_U __CLC_XCONCAT(__CLC_BUILTIN_SCALAR, _u32)
#define __CLC_BUILTIN_SCALAR_L __CLC_XCONCAT(__CLC_BUILTIN_SCALAR, _s64)
#define __CLC_BUILTIN_SCALAR_UL __CLC_XCONCAT(__CLC_BUILTIN_SCALAR, _u64)
#include <../../include/vector_binary_builtin.inc>
