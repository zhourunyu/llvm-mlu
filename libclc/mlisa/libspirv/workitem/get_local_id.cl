//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <spirv/spirv.h>

_CLC_DEF _CLC_OVERLOAD size_t __spirv_LocalInvocationId_x() {
  return __mlvm_read_mlu_sreg_clusterid() * __mlvm_read_mlu_sreg_coredim() + __mlvm_read_mlu_sreg_coreid();
}

_CLC_DEF _CLC_OVERLOAD size_t __spirv_LocalInvocationId_y() {
  return 0;
}

_CLC_DEF _CLC_OVERLOAD size_t __spirv_LocalInvocationId_z() {
  return 0;
}
