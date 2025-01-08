//===-- RemoveAttributesPass.h - Utils for removing function attributes ------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_UTILS_REMOVEATTRIBUTES_H
#define LLVM_TRANSFORMS_UTILS_REMOVEATTRIBUTES_H

#include "llvm/IR/PassManager.h"
#include "llvm/Support/CommandLine.h"

namespace llvm {

class RemoveAttributesPass : public PassInfoMixin<RemoveAttributesPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);

  static bool isRequired() { return true; }
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_UTILS_REMOVEATTRIBUTES_H
