/*===-----------------------------------------------------------------------===
 *
 * Copyright (C) [2020] by Cambricon, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *===-----------------------------------------------------------------------===
 */
#ifndef  __BANG_PIPELINE_H__
#define  __BANG_PIPELINE_H__

#ifndef __BANG__
#error "This file is for BANG compilation only."
#endif  // __BANG__

#include <bang_device_types.h>
#include <bang_device_functions.h>

#ifndef __CLANG_BANG_BUILTIN_VARS_H__
#include <__clang_bang_builtin_vars.h>
#endif  // __CLANG_BANG_BUILTIN_VARS_H__

#ifndef __CLANG_BANG_RUNTIME_WRAPPER_H__
#include <__clang_bang_runtime_wrapper.h>
#endif  // __CLANG_BANG_RUNTIME_WRAPPER_H__

namespace bang {

// -----------------------------------------------------------------------------
// BANG C++ language pipeline programming model
// -----------------------------------------------------------------------------
class pipeline {
public:
  uint32_t barrier_id;
  uint32_t barrier_count;
  pipeline(const pipeline&) = delete;
  pipeline(pipeline&&) = delete;
  pipeline& operator=(const pipeline&) = delete;
  pipeline& operator=(pipeline&&) = delete;

  __mlu_func__ __mlu_builtin__ pipeline(uint32_t id);
  __mlu_func__ __mlu_builtin__ void wait_compute();
  __mlu_func__ __mlu_builtin__ void wait_memcpy();
  __mlu_func__ __mlu_builtin__ void wait_copy_nram_to_nram();
  __mlu_func__ __mlu_builtin__ void wait_copy_nram_to_dram();
  __mlu_func__ __mlu_builtin__ void wait_copy_dram_to_nram();
  __mlu_func__ __mlu_builtin__ void wait_copy_wram_to_dram();
  __mlu_func__ __mlu_builtin__ void wait_copy_dram_to_wram();
  __mlu_func__ __mlu_builtin__ void wait_copy_sram_to_dram();
  __mlu_func__ __mlu_builtin__ void wait_copy_dram_to_sram();
  __mlu_func__ __mlu_builtin__ void wait_copy_nram_to_sram();
  __mlu_func__ __mlu_builtin__ void wait_copy_sram_to_nram();
  __mlu_func__ __mlu_builtin__ void wait_copy_wram_to_sram();
  __mlu_func__ __mlu_builtin__ void wait_copy_sram_to_wram();
  __mlu_func__ __mlu_builtin__ void wait_copy_sram_to_sram();
};

__mlu_func__ __mlu_builtin__ pipeline::pipeline(uint32_t id)
  : barrier_id(id), barrier_count(0) {
  if (id > 4) {
    // The supported maximum number of pipeline is 4.
    asm volatile("exit %[status];\n\t"
                 ::[status]"i"(TRAP_BANG_MAXIMUM_PIPELINE));
  }
#if __BANG_ARCH__ < 200
  this->barrier_count = coreDim;
#else
  this->barrier_count = coreDim + 1;
#endif
}

__mlu_func__ __mlu_builtin__ void pipeline::wait_memcpy() {
  if (clusterDim) {
    __sync_cluster();
  } else {
    __sync_copy_dram_to_nram();
    __sync_copy_dram_to_sram();
  }
}

__mlu_func__ __mlu_builtin__ void pipeline::wait_compute() {
  __sync_compute();
}

__mlu_func__ __mlu_builtin__ void pipeline::wait_copy_nram_to_nram() {
  __sync_copy_nram_to_nram();
}

__mlu_func__ __mlu_builtin__ void pipeline::wait_copy_nram_to_dram() {
  __sync_copy_nram_to_dram();
}

__mlu_func__ __mlu_builtin__ void pipeline::wait_copy_dram_to_nram() {
  __sync_copy_dram_to_nram();
}

__mlu_func__ __mlu_builtin__ void pipeline::wait_copy_wram_to_dram() {
  __sync_copy_wram_to_dram();
}

__mlu_func__ __mlu_builtin__ void pipeline::wait_copy_dram_to_wram() {
  __sync_copy_dram_to_wram();
}

__mlu_func__ __mlu_builtin__ void pipeline::wait_copy_sram_to_dram() {
  __sync_copy_sram_to_dram();
}

__mlu_func__ __mlu_builtin__ void pipeline::wait_copy_dram_to_sram() {
  __sync_copy_dram_to_sram();
}

__mlu_func__ __mlu_builtin__ void pipeline::wait_copy_nram_to_sram() {
  __sync_copy_nram_to_sram();
}

__mlu_func__ __mlu_builtin__ void pipeline::wait_copy_sram_to_nram() {
  __sync_copy_sram_to_nram();
}

__mlu_func__ __mlu_builtin__ void pipeline::wait_copy_wram_to_sram() {
  __sync_copy_wram_to_sram();
}

__mlu_func__ __mlu_builtin__ void pipeline::wait_copy_sram_to_wram() {
  __sync_copy_sram_to_wram();
}

__mlu_func__ __mlu_builtin__ void pipeline::wait_copy_sram_to_sram() {
  __sync_copy_sram_to_sram();
}

template <typename __TD, typename __TS>
static __mlu_func__ __mlu_builtin__ void memcpy_async(pipeline &pipe,
                                                      const __TD* dst,
                                                      const __TS* src,
                                                      int size,
                                                      mluMemcpyDirection_t dir) {
  switch (dir) {
  default : {
    // Unsupport memcpy direction.
    asm volatile("exit %[status];\n\t"
                 ::[status]"i"(TRAP_BANG_UNSUPPORTED_MEMCPY_DIR));
    break;
  }
  case GDRAM2NRAM: {
    __mlvm_memcpy_gdram_to_nram_async((void*)dst, (void*)src, size);
  } break;
  case NRAM2GDRAM: {
    __mlvm_memcpy_nram_to_gdram_async((void*)dst, (void*)src, size);
  } break;
  case GDRAM2WRAM: {
    __mlvm_memcpy_gdram_to_wram_async((void*)dst, (void*)src, size);
  } break;
#if 0  // FIXME(zhouxiaoyong)
  case WRAM2GDRAM: {
    __mlvm_memcpy_wram_to_gdram_async((void*)dst, (void*)src, size);
  } break;
#endif
  case NRAM2NRAM: {
    // NOTE: async copy from nram to nram is equal to sync copy nram to nram,
    // as nram is onchip memory and copy instruction belong to compute stream.
    __mlvm_memcpy_nram_to_nram((void*)dst, (void*)src, size);
  } break;
  case NRAM2WRAM: {
    __mlvm_memcpy_nram_to_wram_async((void*)dst, (void*)src, size);
  } break;
  case WRAM2NRAM: {
    __mlvm_memcpy_wram_to_nram_async((void*)dst, (void*)src, size);
  } break;
#if __BANG_ARCH__ >= 200
  case GDRAM2SRAM: {
    __mlvm_memcpy_gdram_to_sram((void*)dst, (void*)src, size);
  } break;
  case SRAM2GDRAM: {
    __mlvm_memcpy_sram_to_gdram((void*)dst, (void*)src, size);
  } break;
  case SRAM2NRAM: {
    __mlvm_memcpy_sram_to_nram_async((void*)dst, (void*)src, size);
  } break;
  case NRAM2SRAM: {
    __mlvm_memcpy_nram_to_sram_async((void*)dst, (void*)src, size);
  } break;
  case SRAM2WRAM: {
    __mlvm_memcpy_sram_to_wram_async((void*)dst, (void*)src, size);
  } break;
  case WRAM2SRAM: {
    __mlvm_memcpy_wram_to_sram_async((void*)dst, (void*)src, size);
  } break;
#endif  // __BANG_ARCH__ >= 200
  }
}

namespace vector {

// -----------------------------------------------------------------------------
// Bang C++ math function: stream vector mul operation
// -----------------------------------------------------------------------------
static __mlu_func__ __mlu_builtin__ void mul(pipeline& pipe,
                                             float* dst,
                                             float* src0,
                                             float* src1,
                                             int elem_count) {
  __mlvm_stream_mul_f32(dst, src0, src1, elem_count);
}

static __mlu_func__ __mlu_builtin__ void mul(pipeline& pipe,
                                             half* dst,
                                             half* src0,
                                             half* src1,
                                             int elem_count) {
  __mlvm_stream_mul_f16(dst, src0, src1, elem_count);
}

static __mlu_func__ __mlu_builtin__ void add(pipeline& pipe,
                                             float* dst,
                                             float* src0,
                                             float* src1,
                                             int elem_count) {
  __mlvm_stream_add_f32(dst, src0, src1, elem_count);
}

static __mlu_func__ __mlu_builtin__ void add(pipeline& pipe,
                                             half* dst,
                                             half* src0,
                                             half* src1,
                                             int elem_count) {
  __mlvm_stream_add_f16(dst, src0, src1, elem_count);
}

static __mlu_func__ __mlu_builtin__ void sub(pipeline& pipe,
                                             float* dst,
                                             float* src0,
                                             float* src1,
                                             int elem_count) {
  __mlvm_stream_sub_f32(dst, src0, src1, elem_count);
}

static __mlu_func__ __mlu_builtin__ void sub(pipeline& pipe,
                                             half* dst,
                                             half* src0,
                                             half* src1,
                                             int elem_count) {
  __mlvm_stream_sub_f16(dst, src0, src1, elem_count);
}

}  // namespace vector

}  // namespace bang

#endif  // __BANG_PIPELINE_H__
