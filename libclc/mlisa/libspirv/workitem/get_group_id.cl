//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <spirv/spirv.h>

_CLC_DEF _CLC_OVERLOAD size_t __spirv_WorkgroupId_x() {
  return (__spirv_GlobalInvocationId_x() - __spirv_GlobalOffset_x()) / __spirv_WorkgroupSize_x();
}

_CLC_DEF _CLC_OVERLOAD size_t __spirv_WorkgroupId_y() {
  return (__spirv_GlobalInvocationId_y() - __spirv_GlobalOffset_y()) / __spirv_WorkgroupSize_y();
}

_CLC_DEF _CLC_OVERLOAD size_t __spirv_WorkgroupId_z() {
  return (__spirv_GlobalInvocationId_z() - __spirv_GlobalOffset_z()) / __spirv_WorkgroupSize_z();
}
