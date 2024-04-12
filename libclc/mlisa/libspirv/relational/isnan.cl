//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <spirv/spirv.h>

#include "../../include/libdevice.h"
#include <relational.h>

_CLC_DEF _CLC_OVERLOAD bool __spirv_IsNan(float x) { return __cn_scalar_isnan_f32(x); }

_CLC_DEFINE_RELATIONAL_UNARY_VEC_ALL(schar, __spirv_IsNan, float)
