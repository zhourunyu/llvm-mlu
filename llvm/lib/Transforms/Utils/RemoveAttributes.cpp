//===-- RemoveAttributes.cpp - Utils for removing function attributes --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Utils/RemoveAttributes.h"

using namespace llvm;

static cl::list<std::string>
    RemoveAttributes("remove-attrs", cl::ZeroOrMore, cl::Hidden, cl::CommaSeparated,
                     cl::desc("[<attr>,...]"),
                     cl::value_desc("attributes"));

PreservedAnalyses RemoveAttributesPass::run(Function &F,
                                            FunctionAnalysisManager &AM) {
  for (const auto &Attr : RemoveAttributes) {
    Attribute::AttrKind AttrKind = Attribute::getAttrKindFromName(Attr);
    if (AttrKind == Attribute::None) {
      errs() << "Unknown attribute: " << Attr << "\n";
      continue;
    }
    if (F.hasFnAttribute(AttrKind)) {
      F.removeFnAttr(AttrKind);
    }
  }
  return PreservedAnalyses::all();
}
