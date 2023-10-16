//===--- Bang.cpp - Bang Tool and ToolChain Implementations -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Bang.h"
#include "CommonArgs.h"
#include "InputInfo.h"
#include "clang/Basic/Bang.h"
#include "clang/Config/config.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/Distro.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/DriverDiagnostic.h"
#include "clang/Driver/Options.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Process.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/TargetParser.h"
#include "llvm/Support/VirtualFileSystem.h"
#include <system_error>

using namespace clang::driver;
using namespace clang::driver::toolchains;
using namespace clang::driver::tools;
using namespace clang;
using namespace llvm::opt;

#if 0  // TODO(zhouxiaoyong): check the device-side library libdevice.bc
// Parses the contents of mlvm/version.txt in an NEUWARE installation.
// It should contain one line of the from e.g. "MLVM Version 2.4.0".
static BangVersion ParseBangVersionFile(llvm::StringRef V) {
  if (!V.startswith("MLVM Version ")) {
    return BangVersion::UNKNOWN;
  }
  V = V.substr(strlen("MLVM Version "));
  int Major = -1, Minor = -1;
  auto First = V.split('.');
  auto Second = First.second.split('.');
  if (First.first.getAsInteger(10, Major) ||
      Second.first.getAsInteger(10, Minor)) {
    return BangVersion::UNKNOWN;
  }
  if (Major == 1) {
    return BangVersion::BANG_10;
  }
  if (Major == 2) {
    return BangVersion::BANG_20;
  }
  return BangVersion::UNKNOWN;
}
#endif

BangInstallationDetector::BangInstallationDetector(
    const Driver &D, const llvm::Triple &HostTriple,
    const llvm::opt::ArgList &Args)
    : D(D) {
  SmallVector<std::string, 4> BangPathCandidates;

  if (Args.hasArg(clang::driver::options::OPT_neuware_path_EQ)) {
    BangPathCandidates.push_back(
        Args.getLastArgValue(clang::driver::options::OPT_neuware_path_EQ).str());
  } else {
    char* neuware_home = std::getenv("NEUWARE_HOME");
    if (neuware_home) {
      BangPathCandidates.push_back(neuware_home);
    }
  }
  
  std::string DefaultNeuwarePath = "/usr/local/neuware";
  if (BangPathCandidates.size() == 0) {
    BangPathCandidates.push_back(DefaultNeuwarePath);
  }

  for (const auto &BangPath : BangPathCandidates) {
    if (BangPath.empty() || !D.getVFS().exists(BangPath))
      continue;

    InstallPath = BangPath;
    BinPath = BangPath + "/bin";
    IncludePath = InstallPath + "/include";
    LibDevicePath = InstallPath + "/mlvm/libdevice";

    auto &FS = D.getVFS();
#if 0  // TODO(zhouxiaoyong): check the depend bin and libdevice.bc
    if (!(FS.exists(IncludePath) && FS.exists(BinPath) &&
          FS.exists(LibDevicePath)))
      continue;
#else
    if (!(FS.exists(IncludePath)))
      continue;
#endif

    if (HostTriple.isArch64Bit() && FS.exists(InstallPath + "/lib64"))
      LibPath = InstallPath + "/lib64";
    else if (FS.exists(InstallPath + "/lib"))
      LibPath = InstallPath + "/lib";
    else
      continue;

#if 0  // TODO(zhouxiaoyong): check the device-side library libdevice.bc
    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> VersionFile =
        FS.getBufferForFile(InstallPath + "/mlvm/version.txt");
    if (!VersionFile) {
      // neuware < v1.2.5 doesn't have a version.txt
      Version = BangVersion::BANG_10;
    } else {
      Version = ParseBangVersionFile((*VersionFile)->getBuffer());
    }

    std::error_code EC;
    for (llvm::sys::fs::directory_iterator LI(LibDevicePath, EC), LE;
         !EC && LI != LE; LI = LI.increment(EC)) {
      StringRef FilePath = LI->path();
      StringRef FileName = llvm::sys::path::filename(FilePath);
      // Process all bitcode filenames that look like libdevice.compute_XX.YY.bc
      const StringRef LibDeviceName = "libdevice.";
      if (!(FileName.startswith(LibDeviceName) && FileName.endswith(".bc")))
        continue;
      StringRef MluArch = FileName.slice(
          LibDeviceName.size(), FileName.find('.', LibDeviceName.size()));
      LibDeviceMap[MluArch] = FilePath.str();
      // Insert map entries for specifc devices with this compute
      // capability. CNCC's choice of the libdevice library version is
      // rather peculiar and depends on the BANG version.
      if (MluArch == "MLU100") {
        LibDeviceMap["mlu100"] = FilePath;
      } else if (MluArch == "MLU200") {
        LibDeviceMap["mlu200"] = FilePath;
      } else if (MluArch == "1H8") {
        LibDeviceMap["1H8"] = FilePath;
      }
    }
#endif

    IsValid = true;
    break;
  }
}

void BangInstallationDetector::AddBangIncludeArgs(
    const ArgList &DriverArgs, ArgStringList &CC1Args) const {
  if (!DriverArgs.hasArg(options::OPT_nobuiltininc)) {
    // Add bang_wrappers/* to our system include path.  This lets us wrap
    // standard library headers.
    SmallString<128> P(D.ResourceDir);
    llvm::sys::path::append(P, "include");
    llvm::sys::path::append(P, "bang_wrappers");
    CC1Args.push_back("-internal-isystem");
    CC1Args.push_back(DriverArgs.MakeArgString(P));
  }

  if (DriverArgs.hasArg(options::OPT_noneuwareinc))
    return;

  if (!isValid()) {
    D.Diag(diag::err_drv_no_neuware_installation);
    return;
  }

  CC1Args.push_back("-internal-isystem");
  CC1Args.push_back(DriverArgs.MakeArgString(getIncludePath()));
  CC1Args.push_back("-include");
  CC1Args.push_back("__clang_bang_runtime_wrapper.h");
}

void BangInstallationDetector::CheckBangVersionSupportsArch(
    BangArch Arch) const {
  if (Arch == BangArch::UNKNOWN || Version == BangVersion::UNKNOWN ||
      ArchsWithBadVersion.count(Arch) > 0)
    return;

  auto RequiredVersion = MinVersionForBangArch(Arch);
  /*
  if (Version < RequiredVersion) {
    ArchsWithBadVersion.insert(Arch);
    D.Diag(diag::err_drv_neuware_version_too_low)
        << InstallPath << BangArchToString(Arch) << BangVersionToString(Version)
        << BangVersionToString(RequiredVersion);
  }
  */
}

void BangInstallationDetector::print(raw_ostream &OS) const {
  if (isValid())
    OS << "Found NEUWARE installation: " << InstallPath << ", version " 
       << "\n";
       // << BangVersionToString(Version) << "\n";
}

void BangInstallationDetector::ParseBangVersionFile(llvm::StringRef V) {
  // TODO(libaolaing): add version checking
}

void BangInstallationDetector::WarnIfUnsupportedVersion() {
  // TODO(libaoliang): add warning information
}

void MLISA::BackendCompiler::ConstructJob(Compilation &C, const JobAction &JA,
                                    const InputInfo &Output,
                                    const InputInfoList &Inputs,
                                    const ArgList &Args,
                                    const char *LinkingOutput) const {
                                    
  return ;
}


void MLISA::Assembler::ConstructJob(Compilation &C, const JobAction &JA,
                                    const InputInfo &Output,
                                    const InputInfoList &Inputs,
                                    const ArgList &Args,
                                    const char *LinkingOutput) const {
  const auto &TC =
      static_cast<const toolchains::BangToolChain &>(getToolChain());
  assert(TC.getTriple().isMLISA() && "Wrong platform");

  // Obtain architecture from the action.
  BangArch mlu_arch = StringToBangArch(JA.getOffloadingArch());
  assert(mlu_arch != BangArch::UNKNOWN &&
         "Device action expected to have an architecture.");

  // Check that our installation's cnas supports mlu_arch.
  if (!Args.hasArg(options::OPT_no_neuware_version_check)) {
    TC.BangInstallation.CheckBangVersionSupportsArch(mlu_arch);
  }

  ArgStringList CmdArgs;
  if (Args.getLastArg(options::OPT_g_Group)) {
    // cnas does not accept -g option if optimization is enabled, so
    // we ignore the compiler's -O* options if we want debug info.
    CmdArgs.push_back("-O0");
    CmdArgs.push_back("-g");
  } else if (Arg *A = Args.getLastArg(options::OPT_O_Group)) {
    // Map the -O we received to -O{0,1,2,3}.
    //
    // TODO: Perhaps we should map host -O2 to cnas -O3. -O3 is cnas's
    // default, so it may correspond more closely to the spirit of clang -O2.

    // -O3 seems like the least-bad option when -Osomething is specified to
    // clang but it isn't handled below.
    StringRef OOpt = "3";
    if (A->getOption().matches(options::OPT_O4) ||
        A->getOption().matches(options::OPT_Ofast))
      OOpt = "3";
    else if (A->getOption().matches(options::OPT_O0))
      OOpt = "0";
    else if (A->getOption().matches(options::OPT_O)) {
      // -Os, -Oz, and -O(anything else) map to -O2, for lack of better options.
      OOpt = llvm::StringSwitch<const char *>(A->getValue())
                 .Case("1", "1")
                 .Case("2", "2")
                 .Case("3", "3")
                 .Case("s", "2")
                 .Case("z", "2")
                 .Default("2");
    }
    CmdArgs.push_back(Args.MakeArgString(llvm::Twine("-O") + OOpt));
  } else {
    // If no -O was passed, pass -O0 to cnas -- no opt flag should correspond
    // to no optimizations, but cnas's default is -O3.
    CmdArgs.push_back("-O0");
  }

  std::string NormalizedTriple =
    C.getSingleOffloadToolChain<Action::OFK_Host>()->getTriple().normalize();
  CmdArgs.push_back("--mcpu");
  CmdArgs.push_back(Args.MakeArgString(NormalizedTriple));
  CmdArgs.push_back("--mlu-arch");
  CmdArgs.push_back(Args.MakeArgString(BangArchToString(mlu_arch)));
  CmdArgs.push_back("--output");
  CmdArgs.push_back(Args.MakeArgString(Output.getFilename()));
  for (const auto& II : Inputs) {
    CmdArgs.push_back("--input");
    CmdArgs.push_back(Args.MakeArgString(II.getFilename()));
  }

  // TODO(wangshiyu): Set -fenable-profile to cnas for now, need to
  // differentiate user ldram and profile ldram, and remove setting this flag
  // in the future.
  Arg *ProfileArg = Args.getLastArg(
      options::OPT_fprofile_generate,
      options::OPT_fprofile_generate_EQ,
      options::OPT_fprofile_instr_generate,
      options::OPT_fprofile_instr_generate_EQ);
  if (ProfileArg) {
    CmdArgs.push_back(Args.MakeArgString("-fenable-profile"));
  }

  for (const auto& A : Args.getAllArgValues(options::OPT_Xbang_cnas))
    CmdArgs.push_back(Args.MakeArgString(A));

  if (C.getArgs().hasArg(options::OPT_bang_wram_align64)) {
    CmdArgs.push_back("--wram-align");
    CmdArgs.push_back("64");
  }

  if(Arg *A = Args.getLastArg(options::OPT_wram_align_EQ)) {
    StringRef Val = A->getValue();
    CmdArgs.push_back("--wram-align");
    CmdArgs.push_back(Args.MakeArgString(Val));
  }

  // Set global ieee 754 mode.
  // This is an experimental feature, only used by developers.
  if (Args.getLastArg(options::OPT_fbang_enable_global_ieee_754)) {
    CmdArgs.push_back(Args.MakeArgString("--global-ieee-754=ENABLE"));
  }
  if (Args.getLastArg(options::OPT_fbang_disable_global_ieee_754)) {
    CmdArgs.push_back(Args.MakeArgString("--global-ieee-754=DISABLE"));
  }

  // Set uop control.
  // This is an experimental feature, only used by developers.
  if (Args.getLastArg(options::OPT_fbang_disable_uop_cdma)) {
    CmdArgs.push_back(Args.MakeArgString("--disable-uop-cdma"));
  }
  if (Args.getLastArg(options::OPT_fbang_disable_uop_mdma)) {
    CmdArgs.push_back(Args.MakeArgString("--disable-uop-mdma"));
  }
  if (Args.getLastArg(options::OPT_fbang_disable_uop_gdma)) {
    CmdArgs.push_back(Args.MakeArgString("--disable-uop-gdma"));
  }

  const char *Exec;
  if (Arg *A = Args.getLastArg(options::OPT_cnas_path_EQ))
    Exec = A->getValue();
  else
    Exec = Args.MakeArgString(TC.GetProgramPath("cnas"));

  if (C.getArgs().hasArg(options::OPT_bang_device_only)) {
    CmdArgs.push_back(Args.MakeArgString("--device-only"));
  }
  C.addCommand(std::make_unique<Command>(JA, *this,
      ResponseFileSupport{ResponseFileSupport::RF_Full, llvm::sys::WEM_UTF8,
                          "--options-file"},
      Exec, CmdArgs, Inputs));
}

// All inputs to this linker must be from BangDeviceActions, as we need to look
// at the Inputs' Actions in order to figure out which MLU architecture they
// correspond to.
void MLISA::Linker::ConstructJob(Compilation &C, const JobAction &JA,
                                 const InputInfo &Output,
                                 const InputInfoList &Inputs,
                                 const ArgList &Args,
                                 const char *LinkingOutput) const {
  const auto &TC =
      static_cast<const toolchains::BangToolChain &>(getToolChain());
  assert(TC.getTriple().isMLISA() && "Wrong platform");

  ArgStringList CmdArgs;
  CmdArgs.push_back(Args.MakeArgString("--fatbin"));
  CmdArgs.push_back(Args.MakeArgString("--output"));
  CmdArgs.push_back(Args.MakeArgString(Output.getFilename()));

  Arg* host_device_arg = Args.getLastArg(options::OPT_bang_host_only,
                         options::OPT_bang_device_only,
                         options::OPT_bang_compile_host_device);
  bool device_only = host_device_arg &&
                     host_device_arg->getOption().matches(
                         options::OPT_bang_device_only);
  Arg* bang_fatbin_arg = Args.getLastArg(options::OPT_bang_fatbin_only);
  bool fatbin_only = bang_fatbin_arg &&
                     bang_fatbin_arg->getOption().matches(
                         options::OPT_bang_fatbin_only);

  std::string FatBinCWrapper(Output.getFilename());
  if (!device_only) {
    FatBinCWrapper += ".c";
    CmdArgs.push_back(Args.MakeArgString("--create"));
    CmdArgs.push_back(Args.MakeArgString(FatBinCWrapper));
  } else {
    CmdArgs.push_back(Args.MakeArgString("--device-only"));
  }

  std::string NormalizedTriple =
    C.getSingleOffloadToolChain<Action::OFK_Host>()->getTriple().normalize();

  CmdArgs.push_back("--mcpu");
  CmdArgs.push_back(Args.MakeArgString(NormalizedTriple));

  for (const auto& II : Inputs) {
    CmdArgs.push_back(Args.MakeArgString("--input"));
    CmdArgs.push_back(Args.MakeArgString(II.getFilename()));
  }

  for (const auto& A : Args.getAllArgValues(options::OPT_Xbang_fatbinary))
    CmdArgs.push_back(Args.MakeArgString(A));

  // TODO(zhouxiaoyong): replace cnas with cnlinker
  const char *Exec = Args.MakeArgString(TC.GetProgramPath("cnas"));

  C.addCommand(std::make_unique<Command>(JA, *this,
      ResponseFileSupport{ResponseFileSupport::RF_Full, llvm::sys::WEM_UTF8,
                          "--options-file"},
      Exec, CmdArgs, Inputs));

  if (!device_only && !fatbin_only) {
    // Host cc should compile *.cnfatbin.c to *.cnfatbin
    CmdArgs.clear();
    CmdArgs.push_back(Args.MakeArgString("-c"));
    CmdArgs.push_back(Args.MakeArgString(FatBinCWrapper));
    CmdArgs.push_back(Args.MakeArgString("-o"));
    CmdArgs.push_back(Args.MakeArgString(Output.getFilename()));
    std::string TargetOption = "--target=" + NormalizedTriple;
    CmdArgs.push_back(Args.MakeArgString(TargetOption));
    const char *Extr = Args.MakeArgString(TC.GetProgramPath("cncc"));
    C.addCommand(std::make_unique<Command>(JA, *this,
      ResponseFileSupport{ResponseFileSupport::RF_Full, llvm::sys::WEM_UTF8,
                          "--options-file"},
      Extr, CmdArgs, Output));

    // Clean up tmp *.cnfatbin.c file generated by cnas or cnlinker
    C.addTempFile(Args.MakeArgString(FatBinCWrapper));
  }
}

/// BANG toolchain.  Our assembler is cnas, and our "linker" is fatbinary,
/// which isn't properly a linker but nonetheless performs the step of stitching
/// together object files from the assembler into a single blob.

BangToolChain::BangToolChain(const Driver &D, const llvm::Triple &Triple,
                             const ToolChain &HostTC, const ArgList &Args,
                             const Action::OffloadKind OK)
    : ToolChain(D, Triple, Args), HostTC(HostTC),
      BangInstallation(D, HostTC.getTriple(), Args),
      OK(OK) {
  if (BangInstallation.isValid())
    getProgramPaths().push_back(std::string(BangInstallation.getBinPath()));
  
}

std::string BangToolChain::getInputFilename(const InputInfo &Input) const {
  return ToolChain::getInputFilename(Input);
}

bool BangToolChain::supportsDebugInfoOption(const llvm::opt::Arg *A) const {
  const Option &O = A->getOption();
  return (O.matches(options::OPT_gN_Group) &&
          !O.matches(options::OPT_gmodules)) ||
          O.matches(options::OPT_g_Flag) ||
          O.matches(options::OPT_ggdbN_Group) ||
          O.matches(options::OPT_ggdb) ||
          O.matches(options::OPT_gdwarf) ||
          O.matches(options::OPT_gdwarf_2) ||
          O.matches(options::OPT_gdwarf_3) ||
          O.matches(options::OPT_gdwarf_4) ||
          O.matches(options::OPT_gdwarf_5) ||
          O.matches(options::OPT_gcolumn_info);
}

void BangToolChain::adjustDebugInfoKind(
  codegenoptions::DebugInfoKind &DebugInfoKind,
  const ArgList &Args) const {
  // TODO(libaoliang: add debuginfo
}

void BangToolChain::addClangTargetOptions(
    const llvm::opt::ArgList &DriverArgs,
    llvm::opt::ArgStringList &CC1Args,
    Action::OffloadKind DeviceOffloadingKind) const {
  HostTC.addClangTargetOptions(DriverArgs, CC1Args, DeviceOffloadingKind);

  assert((DeviceOffloadingKind == Action::OFK_OpenMP ||
          DeviceOffloadingKind == Action::OFK_Bang ||
          DeviceOffloadingKind == Action::OFK_SYCL) &&
         "Only OpenMP or BANG or SYCL offloading kinds are supported for CAMBRICON MLUs.");

  if (DeviceOffloadingKind == Action::OFK_Bang) {
    CC1Args.push_back("-fbang-is-device");
    if (DriverArgs.hasArg(options::OPT_noneuwarelib))
      return;
  }

  if (DeviceOffloadingKind == Action::OFK_SYCL) {
    toolchains::SYCLToolChain::AddSYCLIncludeArgs(getDriver(), DriverArgs,
                                                  CC1Args);
  }
  //AddBangIncludeArgs(DriverArgs, CC1Args);

  auto NoLibSpirv = DriverArgs.hasArg(options::OPT_fno_sycl_libspirv,
                                      options::OPT_fsycl_device_only);
  if (DeviceOffloadingKind == Action::OFK_SYCL && !NoLibSpirv) {
    std::string LibSpirvFile;

    if (DriverArgs.hasArg(clang::driver::options::OPT_fsycl_libspirv_path_EQ)) {
      auto ProvidedPath =
          DriverArgs
              .getLastArgValue(
                  clang::driver::options::OPT_fsycl_libspirv_path_EQ)
              .str();
      if (llvm::sys::fs::exists(ProvidedPath))
        LibSpirvFile = ProvidedPath;
    } else {
      SmallVector<StringRef, 8> LibraryPaths;

      // Expected path w/out install.
      SmallString<256> LIBCLCPath("/home/wzy/repos/llvm-mlu/libclc/build/lib/clc");
      LibraryPaths.emplace_back(LIBCLCPath.c_str());

      std::string LibSpirvTargetName = "libspirv-mlisa--.bc";
      for (StringRef LibraryPath : LibraryPaths) {
        SmallString<128> LibSpirvTargetFile(LibraryPath);
        llvm::sys::path::append(LibSpirvTargetFile, LibSpirvTargetName);
        if (llvm::sys::fs::exists(LibSpirvTargetFile)) {
          LibSpirvFile = std::string(LibSpirvTargetFile.str());
          break;
        }
      }
    }

    if (LibSpirvFile.empty()) {
      getDriver().Diag(diag::err_drv_no_sycl_libspirv);
      return;
    }

    CC1Args.push_back("-mlink-builtin-bitcode");
    CC1Args.push_back(DriverArgs.MakeArgString(LibSpirvFile));
  }


  // Enable register allocation in LLVM only when compiling with -g option.
  // TODO(wangshiyu): close register allocation in LLVM.
  if (DriverArgs.getLastArg(options::OPT_g_Group)) {
    CC1Args.push_back("-mllvm");
    CC1Args.push_back("-mlisa-enable-register-allocation=true");
  }

  // If --bang-stack-on-ldram, -fprofile-generate* or -fprofile-instr-generate*
  // is enabled, promote stack to LDRAM.
  Arg *ProfileArg = DriverArgs.getLastArg(
      options::OPT_fprofile_generate,
      options::OPT_fprofile_generate_EQ,
      options::OPT_fprofile_instr_generate,
      options::OPT_fprofile_instr_generate_EQ);

  /*
  if (DriverArgs.hasArg(options::OPT_bang_stack_on_ldram) || ProfileArg) {
    CC1Args.push_back("-bang-auto-vars-on-ldram");
    CC1Args.push_back("-mllvm");
    CC1Args.push_back("-mlisa-enable-stack-promote=false");
  } else if (Arg *A = DriverArgs.getLastArg(options::OPT_O_Group)) {
    // O0 need too much .stack space, so promoted to LDRAM defaultly.
    if (A->getOption().matches(options::OPT_O0)) {
      CC1Args.push_back("-bang-auto-vars-on-ldram");
      CC1Args.push_back("-mllvm");
      CC1Args.push_back("-mlisa-enable-stack-promote=false");
    }
  } else {
    CC1Args.push_back("-bang-auto-vars-on-ldram");
    CC1Args.push_back("-mllvm");
    CC1Args.push_back("-mlisa-enable-stack-promote=false");
  }

  if (DriverArgs.hasArg(options::OPT_bang_device_only)) {
    CC1Args.push_back("-mllvm");
    CC1Args.push_back("-fmlisa-enable-mix-programming=false");
  } else {
    CC1Args.push_back("-mllvm");
    CC1Args.push_back("-fmlisa-enable-mix-programming=true");
  }

  if (DriverArgs.hasArg(options::OPT_fbang_enable_trc_mode)) {
    CC1Args.push_back("-mllvm");
    CC1Args.push_back("-fenable-trc-mode");
  }

  if (DriverArgs.hasArg(options::OPT_fmlu_fast_math)) {
    CC1Args.push_back("-mllvm");
    CC1Args.push_back("-fmlu-fast-math");
  }

  // Pass the function instrumentation option to llvm backend.
  if (DriverArgs.hasArg(options::OPT_fbang_instrument_functions)) {
    CC1Args.push_back("-mllvm");
    CC1Args.push_back("-fbang-instrument-functions");
  }
  */

  // FIXME : For compute_10, the unit for data transport between
  //         nram/wram and ldram/gdram is one line or 32 bytes,
  //         so the common memcpy not aligned to 32 bytes
  //         cannot be lowered as nram_to_gdram, nram_to_ldram, gdram_to_nram,
  //         ldram_to_nram, gdram_to_wram, ldram_to_wram
  //         or even nram_to_nram, nram_to_wram and wram_to_nram intrinsics.
  //
  //         To avoid the backend generates the invalid intrinsics,
  //         we disable the 'LoopIdiomPass' and 'MemCpyOptPass' in llvm so that those
  //         intrinsics will not be generated implicitly during the
  //         optimization.
  /*
  CC1Args.push_back("-mllvm");
  CC1Args.push_back("-disable-loop-idiom");

  CC1Args.push_back("-mllvm");
  CC1Args.push_back("-disable-memcpy-opt=true");
  CC1Args.push_back("-mllvm");
  CC1Args.push_back("-disable-bitcast-for-load=true");

  // Disable the attempt to extend the entire input expression tree to
  // the destination type, because this optimization will emit immediate
  // number that is larger than s48.
  CC1Args.push_back("-mllvm");
  CC1Args.push_back("-disable-sext-entend-to-dst-type=true");

  // Using this option to disable the float constant folding
  CC1Args.push_back("-mllvm");
  CC1Args.push_back("-disable-float-folding");

  CC1Args.push_back("-mllvm");
  CC1Args.push_back("-mlisa-address-alignment=64");
  */
}

void BangToolChain::AddBangIncludeArgs(const ArgList &DriverArgs,
                                       ArgStringList &CC1Args) const {
  // Check our BANG version if we're going to include the BANG headers.
  if (!DriverArgs.hasArg(options::OPT_noneuwareinc) &&
      !DriverArgs.hasArg(options::OPT_no_neuware_version_check)) {
    StringRef Arch = DriverArgs.getLastArgValue(options::OPT_march_EQ);
    //assert(!Arch.empty() && "Must have an explicit MLU arch.");
    if (Arch.empty()) Arch = "mtp_270";
    BangInstallation.CheckBangVersionSupportsArch(StringToBangArch(Arch));
  }
  BangInstallation.AddBangIncludeArgs(DriverArgs, CC1Args);
}

llvm::opt::DerivedArgList *
BangToolChain::TranslateArgs(const llvm::opt::DerivedArgList &Args,
                             StringRef BoundArch,
                             Action::OffloadKind DeviceOffloadKind) const {
  DerivedArgList *DAL =
      HostTC.TranslateArgs(Args, BoundArch, DeviceOffloadKind);
  if (!DAL)
    DAL = new DerivedArgList(Args.getBaseArgs());

  const OptTable &Opts = getDriver().getOpts();

  // For OpenMP device offloading, append derived arguments. Make sure
  // flags are not duplicated.
  // TODO: Append the compute capability.
  if (DeviceOffloadKind == Action::OFK_OpenMP) {
    for (Arg *A : Args){
      bool IsDuplicate = false;
      for (Arg *DALArg : *DAL){
        if (A == DALArg) {
          IsDuplicate = true;
          break;
        }
      }
      if (!IsDuplicate)
        DAL->append(A);
    }
    return DAL;
  }

  for (Arg *A : Args) {
    if (A->getOption().matches(options::OPT_Xarch__)) {
      // Skip this argument unless the architecture matches BoundArch
      if (BoundArch.empty() || A->getValue(0) != BoundArch)
        continue;

      unsigned Index = Args.getBaseArgs().MakeIndex(A->getValue(1));
      unsigned Prev = Index;
      std::unique_ptr<Arg> XarchArg(Opts.ParseOneArg(Args, Index));

      // If the argument parsing failed or more than one argument was
      // consumed, the -Xarch_ argument's parameter tried to consume
      // extra arguments. Emit an error and ignore.
      //
      // We also want to disallow any options which would alter the
      // driver behavior; that isn't going to work in our model. We
      // use isDriverOption() as an approximation, although things
      // like -O4 are going to slip through.
      if (!XarchArg || Index > Prev + 1) {
        getDriver().Diag(diag::err_drv_invalid_Xarch_argument_with_args)
            << A->getAsString(Args);
        continue;
      } else if (XarchArg->getOption().hasFlag(options::NoXarchOption)) {
        getDriver().Diag(diag::err_drv_invalid_Xarch_argument_isdriver)
            << A->getAsString(Args);
        continue;
      }
      XarchArg->setBaseArg(A);
      A = XarchArg.release();
      DAL->AddSynthesizedArg(A);
    }
    DAL->append(A);
  }

  if (!BoundArch.empty()) {
    DAL->eraseArg(options::OPT_march_EQ);
    DAL->AddJoinedArg(nullptr, Opts.getOption(options::OPT_march_EQ), BoundArch);
  }
  return DAL;
}

Tool *BangToolChain::buildBackendCompiler() const {
  return new tools::MLISA::BackendCompiler(*this);
}

Tool *BangToolChain::buildAssembler() const {
  return new tools::MLISA::Assembler(*this);
}

Tool *BangToolChain::buildLinker() const {
  if (OK == Action::OFK_SYCL)
    return new tools::MLISA::SYCLLinker(*this);
  return new tools::MLISA::Linker(*this);
}

Tool *BangToolChain::SelectTool(const JobAction &JA) const {
  if (OK == Action::OFK_SYCL) {
    if (JA.getKind() == Action::LinkJobClass &&
        JA.getType() == types::TY_LLVM_BC) {
      return static_cast<tools::MLISA::SYCLLinker *>(ToolChain::SelectTool(JA))
          ->GetSYCLToolChainLinker();
    }
  }
  return ToolChain::SelectTool(JA);
}

void BangToolChain::addClangWarningOptions(ArgStringList &CC1Args) const {
  HostTC.addClangWarningOptions(CC1Args);
}

ToolChain::CXXStdlibType
BangToolChain::GetCXXStdlibType(const ArgList &Args) const {
  return HostTC.GetCXXStdlibType(Args);
}

void BangToolChain::AddClangSystemIncludeArgs(const ArgList &DriverArgs,
                                              ArgStringList &CC1Args) const {
  HostTC.AddClangSystemIncludeArgs(DriverArgs, CC1Args);
}

void BangToolChain::AddClangCXXStdlibIncludeArgs(const ArgList &Args,
                                                 ArgStringList &CC1Args) const {
  HostTC.AddClangCXXStdlibIncludeArgs(Args, CC1Args);
}

void BangToolChain::AddIAMCUIncludeArgs(const ArgList &Args,
                                        ArgStringList &CC1Args) const {
  HostTC.AddIAMCUIncludeArgs(Args, CC1Args);
}

SanitizerMask BangToolChain::getSupportedSanitizers() const {
  // The BangToolChain only supports sanitizers in the sense that it allows
  // sanitizer arguments on the command line if they are supported by the host
  // toolchain. The BangToolChain will actually ignore any command line
  // arguments for any of these "supported" sanitizers. That means that no
  // sanitization of device code is actually supported at this time.
  //
  // This behavior is necessary because the host and device toolchains
  // invocations often share the command line, so the device toolchain must
  // tolerate flags meant only for the host toolchain.
  return HostTC.getSupportedSanitizers();
}

VersionTuple BangToolChain::computeMSVCVersion(const Driver *D,
                                               const ArgList &Args) const {
  return HostTC.computeMSVCVersion(D, Args);
}
