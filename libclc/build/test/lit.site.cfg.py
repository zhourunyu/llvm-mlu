# Autogenerated from /home/mlx/repos/llvm-mlu/libclc/test/lit.site.cfg.py.in
# Do not edit!

# Allow generated file to be relocatable.
from pathlib import Path
def path(p):
    if not p: return ''
    return str((Path(__file__).parent / p).resolve())


import sys

config.llvm_src_root = ""
config.llvm_obj_root = "/home/mlx/repos/llvm-mlu/build"
config.llvm_tools_dir = "/home/mlx/repos/llvm-mlu/build/./bin"
config.llvm_libs_dir = "/home/mlx/repos/llvm-mlu/build/./lib"
config.llvm_shlib_dir = ""
config.llvm_plugin_ext = ".so"
config.lit_tools_dir = ""
config.host_triple = ""
config.target_triple = "x86_64-unknown-linux-gnu"
config.host_arch = "x86_64"
config.python_executable = ""
config.libclc_root = "/home/mlx/repos/llvm-mlu/libclc"
config.test_run_dir = "/home/mlx/repos/llvm-mlu/libclc/build"

import lit.llvm
lit.llvm.initialize(lit_config, config)

# Let the main config do the real work.
lit_config.load_config(config, "/home/mlx/repos/llvm-mlu/libclc/test/lit.cfg.py")