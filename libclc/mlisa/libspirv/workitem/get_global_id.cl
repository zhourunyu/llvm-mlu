//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <spirv/spirv.h>

_CLC_DEF _CLC_OVERLOAD size_t __spirv_GlobalInvocationId_x() {
  return __mlvm_read_mlu_sreg_taskidx() + __spirv_GlobalOffset_x();
}

_CLC_DEF _CLC_OVERLOAD size_t __spirv_GlobalInvocationId_y() {
  return __mlvm_read_mlu_sreg_taskidy() + __spirv_GlobalOffset_y();
}

_CLC_DEF _CLC_OVERLOAD size_t __spirv_GlobalInvocationId_z() {
  return __mlvm_read_mlu_sreg_taskidz() + __spirv_GlobalOffset_z();
}

