//===-- LowerWGNramMemory.cpp - SYCL kernel local memory allocation pass -===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// See intro comments in the header.
//===----------------------------------------------------------------------===//

#include "llvm/SYCLLowerIR/LowerWGNramMemory.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Pass.h"

using namespace llvm;

#define DEBUG_TYPE "LowerWGNramMemory"

static constexpr char SYCL_ALLOCNRAMMEM_CALL[] = "__sycl_allocateNramMemory";
static constexpr char NRAMMEMORY_GV_PREF[] = "WGNramMem";

namespace {
class SYCLLowerWGNramMemoryLegacy : public ModulePass {
public:
  static char ID;

  SYCLLowerWGNramMemoryLegacy() : ModulePass(ID) {
    initializeSYCLLowerWGNramMemoryLegacyPass(
        *PassRegistry::getPassRegistry());
  }

  bool runOnModule(Module &M) override {
    ModuleAnalysisManager DummyMAM;
    auto PA = Impl.run(M, DummyMAM);
    return !PA.areAllPreserved();
  }

private:
  SYCLLowerWGNramMemoryPass Impl;
};
} // namespace

char SYCLLowerWGNramMemoryLegacy::ID = 0;
INITIALIZE_PASS(SYCLLowerWGNramMemoryLegacy, "sycllowerwgnrammemory",
                "Replace __sycl_allocateNramMemory with allocation of memory "
                "in nram address space",
                false, false)

ModulePass *llvm::createSYCLLowerWGNramMemoryLegacyPass() {
  return new SYCLLowerWGNramMemoryLegacy();
}

// TODO: It should be checked that __sycl_allocateNramMemory (or its source
// form - sycl_nram_memory) does not occur:
//  - in a function (other than user lambda/functor)
//  - in a loop
//  - in a non-convergent control flow
// to make it consistent with OpenCL restriction.
// But LLVM pass is not the best place to diagnose these cases.
// Error checking should be done in the front-end compiler.
static void lowerAllocaNramMemCall(CallInst *CI, Module &M) {
  assert(CI);

  Value *ArgSize = CI->getArgOperand(0);
  uint64_t Size = cast<llvm::ConstantInt>(ArgSize)->getZExtValue();
  Value *ArgAlign = CI->getArgOperand(1);
  uint64_t Alignment = cast<llvm::ConstantInt>(ArgAlign)->getZExtValue();

  IRBuilder<> Builder(CI);
  Type *NramMemArrayTy = ArrayType::get(Builder.getInt8Ty(), Size);
  unsigned NramAS =
      CI->getFunctionType()->getReturnType()->getPointerAddressSpace();
  auto *NramMemArrayGV =
      new GlobalVariable(M,                                // module
                         NramMemArrayTy,                   // type
                         false,                            // isConstant
                         GlobalValue::InternalLinkage,     // Linkage
                         UndefValue::get(NramMemArrayTy),  // Initializer
                         NRAMMEMORY_GV_PREF,               // Name prefix
                         nullptr,                          // InsertBefore
                         GlobalVariable::NotThreadLocal,   // ThreadLocalMode
                         NramAS                            // AddressSpace
      );
  NramMemArrayGV->setAlignment(Align(Alignment));

  Value *GVPtr =
      Builder.CreatePointerCast(NramMemArrayGV, Builder.getInt8PtrTy(NramAS));
  CI->replaceAllUsesWith(GVPtr);
}

static bool allocaWGNramMemory(Module &M) {
  Function *ANMFunc = M.getFunction(SYCL_ALLOCNRAMMEM_CALL);
  if (!ANMFunc)
    return false;

  assert(ANMFunc->isDeclaration() && "should have declaration only");

  SmallVector<CallInst *, 4> DelCalls;
  for (User *U : ANMFunc->users()) {
    auto *CI = cast<CallInst>(U);
    lowerAllocaNramMemCall(CI, M);
    DelCalls.push_back(CI);
  }

  for (auto *CI : DelCalls) {
    assert(CI->use_empty() && "removing live instruction");
    CI->eraseFromParent();
  }

  // Remove __sycl_allocateNramMemory declaration.
  assert(ANMFunc->use_empty() && "__sycl_allocateNramMemory is still in use");
  ANMFunc->eraseFromParent();

  return true;
}

PreservedAnalyses SYCLLowerWGNramMemoryPass::run(Module &M,
                                                  ModuleAnalysisManager &) {
  if (allocaWGNramMemory(M))
    return PreservedAnalyses::none();
  return PreservedAnalyses::all();
}
