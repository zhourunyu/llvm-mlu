//===--- Bang.h - Utilities for compiling BANG code  ------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_BASIC_BANG_H
#define LLVM_CLANG_BASIC_BANG_H

namespace llvm {
class StringRef;
class Twine;
class VersionTuple;
} // namespace llvm

namespace clang {

enum class CnrtVersion {
  UNKNOWN,
  CNRT_66,
  CNRT_67,
  CNRT_68,
  CNRT_610,
  LATEST = CNRT_610,
  LATEST_SUPPORTED = CNRT_610,
};
const char *CnrtVersionToString(CnrtVersion V);
// Input is "Major.Minor"
CnrtVersion NeuwareStringToVersion(const llvm::Twine &S);

enum class BangArch {
  UNUSED,
  UNKNOWN,
  TP_210,
  TP_220,
  TP_270,
  MTP_220,
  MTP_270,
  MTP_290,
  TP_322,
  MTP_372,
  MTP_592,
  LAST,
};

const char *BangArchToString(BangArch A);
const char *BangArchToVirtualArchString(BangArch A);

// The input should have the form "mtp_100".
BangArch StringToBangArch(llvm::StringRef S);

/// Get the earliest CnrtVersion that supports the given BangArch.
CnrtVersion MinVersionForBangArch(BangArch A);

/// Get the latest CnrtVersion that supports the given CudaArch.
CnrtVersion MaxVersionForBangArch(BangArch A);

CnrtVersion ToCnrtVersion(llvm::VersionTuple);

} // namespace clang

#endif
