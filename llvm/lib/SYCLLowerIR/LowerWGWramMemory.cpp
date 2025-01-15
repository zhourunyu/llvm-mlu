//===-- LowerWGWramMemory.cpp - SYCL kernel wram memory allocation pass -===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// See intro comments in the header.
//===----------------------------------------------------------------------===//

#include "llvm/SYCLLowerIR/LowerWGWramMemory.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Pass.h"

using namespace llvm;

#define DEBUG_TYPE "LowerWGWramMemory"

static constexpr char SYCL_ALLOCWRAMMEM_CALL[] = "__sycl_allocateWramMemory";
static constexpr char WRAMMEMORY_GV_PREF[] = "WGWramMem";

namespace {
class SYCLLowerWGWramMemoryLegacy : public ModulePass {
public:
  static char ID;

  SYCLLowerWGWramMemoryLegacy() : ModulePass(ID) {
    initializeSYCLLowerWGWramMemoryLegacyPass(
        *PassRegistry::getPassRegistry());
  }

  bool runOnModule(Module &M) override {
    ModuleAnalysisManager DummyMAM;
    auto PA = Impl.run(M, DummyMAM);
    return !PA.areAllPreserved();
  }

private:
  SYCLLowerWGWramMemoryPass Impl;
};
} // namespace

char SYCLLowerWGWramMemoryLegacy::ID = 0;
INITIALIZE_PASS(SYCLLowerWGWramMemoryLegacy, "sycllowerwgwrammemory",
                "Replace __sycl_allocateWramMemory with allocation of memory "
                "in wram address space",
                false, false)

ModulePass *llvm::createSYCLLowerWGWramMemoryLegacyPass() {
  return new SYCLLowerWGWramMemoryLegacy();
}

// TODO: It should be checked that __sycl_allocateWramMemory (or its source
// form - sycl_wram_memory) does not occur:
//  - in a function (other than user lambda/functor)
//  - in a loop
//  - in a non-convergent control flow
// to make it consistent with OpenCL restriction.
// But LLVM pass is not the best place to diagnose these cases.
// Error checking should be done in the front-end compiler.
static void lowerAllocaWramMemCall(CallInst *CI, Module &M) {
  assert(CI);

  Value *ArgSize = CI->getArgOperand(0);
  uint64_t Size = cast<llvm::ConstantInt>(ArgSize)->getZExtValue();
  Value *ArgAlign = CI->getArgOperand(1);
  uint64_t Alignment = cast<llvm::ConstantInt>(ArgAlign)->getZExtValue();

  IRBuilder<> Builder(CI);
  Type *WramMemArrayTy = ArrayType::get(Builder.getInt8Ty(), Size);
  unsigned WramAS =
      CI->getFunctionType()->getReturnType()->getPointerAddressSpace();
  auto *WramMemArrayGV =
      new GlobalVariable(M,                                // module
                         WramMemArrayTy,                   // type
                         false,                            // isConstant
                         GlobalValue::InternalLinkage,     // Linkage
                         UndefValue::get(WramMemArrayTy),  // Initializer
                         WRAMMEMORY_GV_PREF,               // Name prefix
                         nullptr,                          // InsertBefore
                         GlobalVariable::NotThreadLocal,   // ThreadLocalMode
                         WramAS                            // AddressSpace
      );
  WramMemArrayGV->setAlignment(Align(Alignment));

  Value *GVPtr =
      Builder.CreatePointerCast(WramMemArrayGV, Builder.getInt8PtrTy(WramAS));
  CI->replaceAllUsesWith(GVPtr);
}

static bool allocaWGWramMemory(Module &M) {
  Function *ANMFunc = M.getFunction(SYCL_ALLOCWRAMMEM_CALL);
  if (!ANMFunc)
    return false;

  assert(ANMFunc->isDeclaration() && "should have declaration only");

  SmallVector<CallInst *, 4> DelCalls;
  for (User *U : ANMFunc->users()) {
    auto *CI = cast<CallInst>(U);
    lowerAllocaWramMemCall(CI, M);
    DelCalls.push_back(CI);
  }

  for (auto *CI : DelCalls) {
    assert(CI->use_empty() && "removing live instruction");
    CI->eraseFromParent();
  }

  // Remove __sycl_allocateWramMemory declaration.
  assert(ANMFunc->use_empty() && "__sycl_allocateWramMemory is still in use");
  ANMFunc->eraseFromParent();

  return true;
}

PreservedAnalyses SYCLLowerWGWramMemoryPass::run(Module &M,
                                                  ModuleAnalysisManager &) {
  if (allocaWGWramMemory(M))
    return PreservedAnalyses::none();
  return PreservedAnalyses::all();
}
