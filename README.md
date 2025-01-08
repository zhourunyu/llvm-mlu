# LLVM-MLU
This project is based on llvm-13 for supporting cambricon mlu

## 使用方法

### 测试环境
- Ubuntu 20.04.3 LTS，内核版本5.4.0-189-generic
- MLU370-x8，驱动版本5.10.22
- CNToolkit 3.8.4
- GCC 8.4.0

### 编译流程

1. 下载代码及依赖
```shell
git clone http://git.xcoresigma.com/open-source/llvm-mlu.git --recurse-submodules
```
2. 设置环境变量
```shell
cd llvm-mlu
source env-cnrt.sh
```

3. 编译llvm
```shell
python3 buildbot/configure.py -t Release --bang
python3 buildbot/compile.py
```
编译成功后，可以使用`sycl-ls`确认sycl是否正确找到MLU设备
```shell
$ sycl-ls
[ext_oneapi_cnrt:0] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.8.0]
[ext_oneapi_cnrt:1] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.8.0]
[ext_oneapi_cnrt:2] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.8.0]
[ext_oneapi_cnrt:3] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.8.0]
[ext_oneapi_cnrt:4] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.8.0]
[ext_oneapi_cnrt:5] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.8.0]
[ext_oneapi_cnrt:6] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.8.0]
[ext_oneapi_cnrt:7] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.8.0]
[ext_oneapi_cnrt:8] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.8.0]
[ext_oneapi_cnrt:9] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.8.0]
[ext_oneapi_cnrt:10] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.8.0]
[ext_oneapi_cnrt:11] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.8.0]
[ext_oneapi_cnrt:12] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.8.0]
[ext_oneapi_cnrt:13] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.8.0]
[ext_oneapi_cnrt:14] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.8.0]
[ext_oneapi_cnrt:15] GPU : Cambricon BANG BACKEND 0.0 [CNRT 6.8.0]
```

4. 编译libclc
```shell
bash libclc/build_mlisa.sh
```

5. 编译样例并测试
```shell
cd samples
mkdir build && cd build
cmake ..
make -j
./hello-sycl
make test
```