//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <spirv/spirv.h>

_CLC_DEF _CLC_OVERLOAD size_t __spirv_WorkgroupSize_x() {
  // group size = 1 for BLOCK tasks
  return __mlvm_read_mlu_sreg_clusterdim() ?
    __mlvm_read_mlu_sreg_coredim() : 1;
}

_CLC_DEF _CLC_OVERLOAD size_t __spirv_WorkgroupSize_y() {
  return 1;
}

_CLC_DEF _CLC_OVERLOAD size_t __spirv_WorkgroupSize_z() {
  return 1;
}
