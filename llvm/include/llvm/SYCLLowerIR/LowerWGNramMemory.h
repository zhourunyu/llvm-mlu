//===-- LowerWGNramMemory.h - SYCL kernel nram memory allocation pass ---===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// This pass does the following for each allocate call to
// __sycl_allocateNramMemory(Size, Alignment) function at the kernel scope:
// - inserts a global (in scope of a program) byte array of Size bytes with
//   specified alignment in work item nram address space.
// - replaces allocate call with access to this memory.
//
// For example, the following IR code in a kernel function:
//   define spir_kernel void @KernelA() {
//     %0 = call spir_func i8 addrspace(3)* @__sycl_allocateNramMemory(
//         i64 128, i64 4)
//     %1 = bitcast i8 addrspace(3)* %0 to i32 addrspace(3)*
//   }
//
// is translated to the following:
//   @WGNramMem = internal addrspace(3) global [128 x i8] undef, align 4
//   define spir_kernel void @KernelA() {
//     %0 = bitcast i8 addrspace(3)* getelementptr inbounds (
//         [128 x i8], [128 x i8] addrspace(3)* @WGNramMem, i32 0, i32 0)
//         to i32 addrspace(3)*
//   }
//===----------------------------------------------------------------------===//

#ifndef LLVM_SYCLLOWERIR_LOWERWGNRAMMEMORY_H
#define LLVM_SYCLLOWERIR_LOWERWGNRAMMEMORY_H

#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"

namespace llvm {

class SYCLLowerWGNramMemoryPass
    : public PassInfoMixin<SYCLLowerWGNramMemoryPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &);
};

ModulePass *createSYCLLowerWGNramMemoryLegacyPass();
void initializeSYCLLowerWGNramMemoryLegacyPass(PassRegistry &);

} // namespace llvm

#endif // LLVM_SYCLLOWERIR_LOWERWGNRAMMEMORY_H
