#include "clang/Basic/Bang.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/VersionTuple.h"

namespace clang {

const char *CnrtVersionToString(CnrtVersion V) {
  switch (V) {
  case CnrtVersion::UNKNOWN:
    return "unknown";
  case CnrtVersion::CNRT_66:
    return "6.6";
  case CnrtVersion::CNRT_67:
    return "6.7";
  case CnrtVersion::CNRT_68:
    return "6.8";
  case CnrtVersion::CNRT_610:
    return "6.10";
  }
  llvm_unreachable("invalid enum");
}

CnrtVersion NeuwareStringToVersion(const llvm::Twine &S) {
  return llvm::StringSwitch<CnrtVersion>(S.str())
      .Case("6.6", CnrtVersion::CNRT_66)
      .Case("6.7", CnrtVersion::CNRT_67)
      .Case("6.8", CnrtVersion::CNRT_68)
      .Case("6.10", CnrtVersion::CNRT_610)
      .Default(CnrtVersion::UNKNOWN);
}

namespace {
struct BangArchToStringMap {
  BangArch arch;
  const char *arch_name;
  const char *virtual_arch_name;
};
} // namespace

#define TP(tp, ca)                                                          \
  { BangArch::TP_##tp, "tp_" #tp, ca }
#define MTP(mtp, ca)                                                        \
  { BangArch::MTP_##mtp, "mtp_" #mtp, ca }

static const BangArchToStringMap arch_names[] = {
    // clang-format off
    {BangArch::UNUSED, "", ""},
    TP(210, "compute_20"), TP(220, "compute_20"), TP(270, "compute_20"),
    MTP(220, "compute_20"), MTP(270, "compute_20"), MTP(290, "compute_20"),
    TP(322, "compute_30"), MTP(372, "compute_30"),
    MTP(592, "compute_50"),
    // clang-format on
};
#undef TP
#undef MTP

const char *BangArchToString(BangArch A) {
  auto result = std::find_if(
      std::begin(arch_names), std::end(arch_names),
      [A](const BangArchToStringMap &map) { return A == map.arch; });
  if (result == std::end(arch_names))
    return "unknown";
  return result->arch_name;
}

const char *BangArchToVirtualArchString(BangArch A) {
  auto result = std::find_if(
      std::begin(arch_names), std::end(arch_names),
      [A](const BangArchToStringMap &map) { return A == map.arch; });
  if (result == std::end(arch_names))
    return "unknown";
  return result->virtual_arch_name;
}

BangArch StringToBangArch(llvm::StringRef S){
  auto result = std::find_if(
      std::begin(arch_names), std::end(arch_names),
      [S](const BangArchToStringMap &map) { return S == map.arch_name; });
  if (result == std::end(arch_names))
    return BangArch::UNKNOWN;
  return result->arch;
}

CnrtVersion MinVersionForBangArch(BangArch A){
  return CnrtVersion::CNRT_66;
}

CnrtVersion MaxVersionForBangArch(BangArch A){
  return CnrtVersion::LATEST;
}

CnrtVersion ToCnrtVersion(llvm::VersionTuple Version) {
  int IVer =
      Version.getMajor() * 100 + Version.getMinor().getValueOr(0);
  switch(IVer) {
  case 606:
    return CnrtVersion::CNRT_66;
  case 607:
    return CnrtVersion::CNRT_67;
  case 608:
    return CnrtVersion::CNRT_68;
  case 610:
    return CnrtVersion::CNRT_610;
  default:
    return CnrtVersion::UNKNOWN;
  }
}
} // namespace clang
