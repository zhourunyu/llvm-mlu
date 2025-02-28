//===--- MLISA.cpp - Implement MLISA target feature support ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements MLISA TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "MLISA.h"
#include "Targets.h"
#include "clang/Basic/Builtins.h"
#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetBuiltins.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Frontend/OpenMP/OMPGridValues.h"

using namespace clang;
using namespace clang::targets;

const Builtin::Info MLISATargetInfo::BuiltinInfo[] = {
#define BUILTIN(ID, TYPE, ATTRS)                                               \
  {#ID, TYPE, ATTRS, nullptr, ALL_LANGUAGES, nullptr},
#define LIBBUILTIN(ID, TYPE, ATTRS, HEADER)                                    \
  {#ID, TYPE, ATTRS, HEADER, ALL_LANGUAGES, nullptr},
#define TARGET_BUILTIN(ID, TYPE, ATTRS, FEATURE)                               \
  {#ID, TYPE, ATTRS, nullptr, ALL_LANGUAGES, FEATURE},
#include "clang/Basic/BuiltinsMLISA.def"
};

const char *const MLISATargetInfo::GCCRegNames[] = {"r0"};

MLISATargetInfo::MLISATargetInfo(const llvm::Triple &Triple,
                                const TargetOptions &Opts,
                                unsigned TargetPointerWidth)
    : TargetInfo(Triple) {
  assert(TargetPointerWidth == 64 && "MLISA only supports 64-bit modes.");

  TLSSupported = false;
  VLASupported = false;
  AddrSpaceMap = &MLISAAddrSpaceMap;
  UseAddrSpaceMapMangling = true;
  HasLegalHalfType = true;
  HasFloat16 = true;

  // Define available target features
  // These must be defined in sorted order!
  NoAsmVariants = true;
  MLU = BangArch::TP_210;

  resetDataLayout("e-p:64:64:64-i64:64-v16:16-v32:32-n16:32:64");

  // If possible, get a TargetInfo for our host triple, so we can match its
  // types.
  llvm::Triple HostTriple(Opts.HostTriple);
  if (!HostTriple.isMLISA())
    HostTarget.reset(AllocateTarget(llvm::Triple(Opts.HostTriple), Opts));

  // If no host target, make some guesses about the data layout and return.
  if (!HostTarget) {
    LongWidth = LongAlign = TargetPointerWidth;
    PointerWidth = PointerAlign = TargetPointerWidth;
    switch (TargetPointerWidth) {
    case 64:
      SizeType = TargetInfo::UnsignedLong;
      PtrDiffType = TargetInfo::SignedLong;
      IntPtrType = TargetInfo::SignedLong;
      break;
    default:
      llvm_unreachable("TargetPointerWidth must be 64");
    }
    return;
  }

  // Copy properties from host target.
  PointerWidth = HostTarget->getPointerWidth(/* AddrSpace = */ 0);
  PointerAlign = HostTarget->getPointerAlign(/* AddrSpace = */ 0);
  BoolWidth = HostTarget->getBoolWidth();
  BoolAlign = HostTarget->getBoolAlign();
  IntWidth = HostTarget->getIntWidth();
  IntAlign = HostTarget->getIntAlign();
  HalfWidth = HostTarget->getHalfWidth();
  HalfAlign = HostTarget->getHalfAlign();
  FloatWidth = HostTarget->getFloatWidth();
  FloatAlign = HostTarget->getFloatAlign();
  DoubleWidth = HostTarget->getDoubleWidth();
  DoubleAlign = HostTarget->getDoubleAlign();
  LongWidth = HostTarget->getLongWidth();
  LongAlign = HostTarget->getLongAlign();
  LongLongWidth = HostTarget->getLongLongWidth();
  LongLongAlign = HostTarget->getLongLongAlign();
  MinGlobalAlign = HostTarget->getMinGlobalAlign(/* TypeSize = */ 0);
  NewAlign = HostTarget->getNewAlign();
  DefaultAlignForAttributeAligned =
      HostTarget->getDefaultAlignForAttributeAligned();
  SizeType = HostTarget->getSizeType();
  IntMaxType = HostTarget->getIntMaxType();
  PtrDiffType = HostTarget->getPtrDiffType(/* AddrSpace = */ 0);
  IntPtrType = HostTarget->getIntPtrType();
  WCharType = HostTarget->getWCharType();
  WIntType = HostTarget->getWIntType();
  Char16Type = HostTarget->getChar16Type();
  Char32Type = HostTarget->getChar32Type();
  Int64Type = HostTarget->getInt64Type();
  SigAtomicType = HostTarget->getSigAtomicType();
  ProcessIDType = HostTarget->getProcessIDType();

  UseBitFieldTypeAlignment = HostTarget->useBitFieldTypeAlignment();
  UseZeroLengthBitfieldAlignment = HostTarget->useZeroLengthBitfieldAlignment();
  UseExplicitBitFieldAlignment = HostTarget->useExplicitBitFieldAlignment();
  ZeroLengthBitfieldBoundary = HostTarget->getZeroLengthBitfieldBoundary();

  // This is a bit of a lie, but it controls __GCC_ATOMIC_XXX_LOCK_FREE, and
  // we need those macros to be identical on host and device, because (among
  // other things) they affect which standard library classes are defined, and
  // we need all classes to be defined on both the host and device.
  MaxAtomicInlineWidth = HostTarget->getMaxAtomicInlineWidth();

  // Properties intentionally not copied from host:
  // - LargeArrayMinWidth, LargeArrayAlign: Not visible across the
  //   host/device boundary.
  // - SuitableAlign: Not visible across the host/device boundary, and may
  //   correctly be different on host/device, e.g. if host has wider vector
  //   types than device.
  // - LongDoubleWidth, LongDoubleAlign: mlisa's long double type is the same
  //   as its double type, but that's not necessarily true on the host.
  //   TODO: cncc emits a warning when using long double on device; we should
  //   do the same.
}

ArrayRef<const char *> MLISATargetInfo::getGCCRegNames() const {
  return llvm::makeArrayRef(GCCRegNames);
}

bool MLISATargetInfo::hasFeature(StringRef Feature) const {
  return llvm::StringSwitch<bool>(Feature)
  // Has Neural Memory per Core
  .Case("nram", true)
  // Has Weight Memory per Core
  .Case("wram", MLU >= BangArch::TP_210)
  // Has Shared Memory per Cluster
  .Case("sram", MLU >= BangArch::TP_210 && MLU != BangArch::TP_220 &&
                MLU != BangArch::TP_270 && MLU != BangArch::TP_322)
  // Has Level-0 Cache per Core
  .Case("l0c", MLU >= BangArch::TP_210)
  // Has Level-1 Cache per Cluster
  .Case("l1c", false)
  // Has Level-2 Cache per Device
  .Case("l2c", MLU > BangArch::MTP_372)
  // Has Neural Function Unit per Core
  .Case("nfu", true)
  // Has Weight Function Unit per Core
  .Case("wfu", MLU >= BangArch::TP_210)
  // Has Grid Function Unit per Core
  .Case("gfu", MLU > BangArch::MTP_270)
  // Support Stream Vector Compute
  .Case("svc", true)
  // Support Tensorized Compute
  .Case("trc", MLU >= BangArch::TP_322)
  // Support Grid Stencil Compute
  .Case("gsc", MLU >= BangArch::TP_322)
  // Allow half type
  .Case("fp16", true)
  // Allow float type
  .Case("fp32", MLU >= BangArch::TP_210)
  // Allow double type
  .Case("fp64", false)
  // Allow bfloat16 type
  .Case("bf16", MLU >= BangArch::TP_322)
  // BANG v1.0 compute compatibility
  .Case("compute_10", false)
  // BANG v2.0 compute compatibility
  .Case("compute_20", (MLU >= BangArch::TP_210) && (MLU <= BangArch::MTP_290))
  // BANG v3.0 compute compatibility
  .Case("compute_30", (MLU >= BangArch::TP_322) && (MLU <= BangArch::MTP_372))
  // BANG v3.5 compute compatibility
  .Case("compute_35", (MLU > BangArch::MTP_372) && (MLU <= BangArch::MTP_372))
  .Case("tp_220", MLU == BangArch::TP_220)
  .Case("tp_322", MLU == BangArch::TP_322)
  .Case("mtp_220", MLU == BangArch::MTP_220)
  .Case("mtp_372", MLU == BangArch::MTP_372)
  .Default(false);
}

void MLISATargetInfo::getTargetDefines(const LangOptions &Opts,
                      MacroBuilder &Builder) const {
  Builder.defineMacro("__MLU__");
  Builder.defineMacro("__MLISA__");

  // define macro __MLU_NRAM_SIZE__, __MLU_WRAM_SIZE__, and __MLU_SRAM_SIZE__
  int nram_size = 0;
  int wram_size = 0;
  int sram_size = 0;
  switch (MLU) {
  case BangArch::TP_210: {
    nram_size = 256;
    wram_size = 256;
    sram_size = 0;
  } break;
  case BangArch::TP_220: {
    nram_size = 512;
    wram_size = 512;
    sram_size = 0;
  } break;
  case BangArch::TP_270: {
    nram_size = 512;
    wram_size = 1024;
    sram_size = 0;
  } break;
  case BangArch::MTP_220: {
    nram_size = 512;
    wram_size = 512;
    sram_size = 2048;
  } break;
  case BangArch::MTP_270: {
    nram_size = 512;
    wram_size = 1024;
    sram_size = 2048;
  } break;
  case BangArch::MTP_290: {
    nram_size = 512;
    wram_size = 512;
    sram_size = 2048;
  } break;
  case BangArch::TP_322: {
    nram_size = 768;
    wram_size = 1024;
    sram_size = 0;
  } break;
  case BangArch::MTP_372: {
    nram_size = 768;
    wram_size = 1024;
    sram_size = 4096;
  } break;
  case BangArch::MTP_592: {
    nram_size = 512;
    wram_size = 512;
    sram_size = 2048;
  } break;
  case BangArch::LAST:
    break;
  default:
    assert(false && "No MLU arch when compiling BANG device code.");
    break;
  }

  Builder.defineMacro("__MLU_NRAM_SIZE__", std::to_string(nram_size));
  Builder.defineMacro("__MLU_WRAM_SIZE__", std::to_string(wram_size));
  Builder.defineMacro("__MLU_SRAM_SIZE__", std::to_string(sram_size));

  if (Opts.BANGIsDevice) {
    // Set __BANG_ARCH__ for the MLU specified.
    std::string BANGArchCode = [this] {
      switch (MLU) {
      case BangArch::LAST:
        break;
      case BangArch::UNUSED:
      case BangArch::UNKNOWN:
        assert(false && "No MLU arch when compiling BANG device code.");
        return "";
      case BangArch::TP_210:
        return "210";
      case BangArch::TP_220:
        return "220";
      case BangArch::MTP_220:
        return "220";
      case BangArch::TP_270:
        return "270";
      case BangArch::MTP_270:
        return "270";
      case BangArch::MTP_290:
        return "290";
      case BangArch::TP_322:
        return "322";
      case BangArch::MTP_372:
        return "372";
      case BangArch::MTP_592:
        return "592";
      }
      llvm_unreachable("unhandled BangArch");
    }();
    Builder.defineMacro("__BANG_ARCH__", BANGArchCode);
  }
}

ArrayRef<Builtin::Info> MLISATargetInfo::getTargetBuiltins() const {
  return llvm::makeArrayRef(BuiltinInfo, clang::MLISA::LastTSBuiltin -
                                             Builtin::FirstTSBuiltin);
}
