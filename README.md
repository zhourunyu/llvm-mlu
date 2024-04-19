# LLVM-MLU
This project is based on llvm-13 for supporting cambricon mlu

## 使用方法

### 测试环境
- Ubuntu 20.04，内核版本5.4.0-174-generic
- MLU370-x8，驱动版本5.10.22
- CNToolkit 3.6.1
- GCC 9.4.0

### 编译流程

1. 下载代码及依赖
```shell
git clone http://git.xcoresigma.com/open-source/llvm-mlu.git -b develop --recurse-submodules
```
2. 设置环境变量
```shell
cd llvm-mlu
source env-cnrt.sh
```

3. 编译llvm
```shell
python3 buildbot/configure.py -t Release --bang
python3 buildbot/compile.py -j$(nproc)
```
编译成功后，可以使用`sycl-ls`确认sycl是否正确找到MLU设备
```shell
$ sycl-ls
[cnrt:0] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.6.0]
[cnrt:1] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.6.0]
[cnrt:2] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.6.0]
[cnrt:3] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.6.0]
[cnrt:4] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.6.0]
[cnrt:5] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.6.0]
[cnrt:6] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.6.0]
[cnrt:7] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.6.0]
[cnrt:8] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.6.0]
[cnrt:9] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.6.0]
[cnrt:10] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.6.0]
[cnrt:11] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.6.0]
[cnrt:12] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.6.0]
[cnrt:13] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.6.0]
[cnrt:14] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.6.0]
[cnrt:15] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.6.0]
```

4. 编译libclc
```shell
ln -s -f /usr/local/neuware/lib/llvm-mm/bin/llvm-as build/bin/llvm-as
ln -s -f /usr/local/neuware/lib/llvm-mm/bin/opt build/bin/opt
bash libclc/build_mlisa.sh
```

5. 编译样例并测试
```shell
cd samples
mkdir build && cd build
cmake ..
make -j$(nproc)
./hello-sycl
make test
```