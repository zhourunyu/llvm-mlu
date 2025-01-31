#include "clang/Basic/Bang.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/VersionTuple.h"

namespace clang {

const char *BangVersionToString(BangVersion V) {
  return "BANG30";
}

const char *BangVirtualArchToString(BangVirtualArch A) {
  return "COMPUTE30";
}

const char *BangArchToString(BangArch A) {
  return "mtp_372";
}

// The input should have the form "MTP100".
// BangArch StringToBangArch(llvm::StringRef S);
BangArch StringToBangArch(llvm::StringRef S){
  return BangArch::MTP_372;
}

// The input should have the form "MTP100".
BangVirtualArch StringToBangVirtualArch(llvm::StringRef S){
  return BangVirtualArch::COMPUTE_30;
}

/// Get the compute_xx corresponding to an MLUxxx.
BangVirtualArch VirtualArchForBangArch(BangArch A) {
  return BangVirtualArch::COMPUTE_30;
}

/// Get the earliest BangVersion that supports the given BangArch.
BangVersion MinVersionForBangArch(BangArch A){
  return BangVersion::BANG_30;
}



} // namespace clang
