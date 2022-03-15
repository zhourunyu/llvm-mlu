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
#ifndef __BANG_DEVICE_PROFILE_H__
#define __BANG_DEVICE_PROFILE_H__

#ifndef __BANG__
#error "This file is for BANG compilation only."
#endif  // __BANG__

#include <stdint.h>

#if (__BANG_ARCH__ >= 200)

#define IPVK_Last 1

#define CEIL_DIV(x, y) (((x) + (y)-1) / (y))
#define ALIGN(x, y) (CEIL_DIV(x, y) * y)

#define RETURN_IF_OUT_OF_MEMORY(x)                                             \
  {                                                                            \
    if ((x) > PtrProfileDataEnd) {                                             \
      /* mpu does not support printf currently */                              \
      if (__is_ipu()) {                                                        \
        __bang_printf("%s: space reserved is not sufficient\n", __FUNCTION__); \
      }                                                                        \
      return;                                                                  \
    }                                                                          \
  }

#define RETURN_IF_OVERFLOW(x)                                   \
  {                                                             \
    if ((x) <= 0) {                                             \
      /* mpu does not support printf currently */               \
      if (__is_ipu()) {                                         \
        __bang_printf("%s: counters overflow\n", __FUNCTION__); \
      }                                                         \
      return;                                                   \
    }                                                           \
  }

#define INSTR_PROF_RAW_MAGIC_64_LOW \
  ((uint32_t)'o' << 24 | (uint32_t)'f' << 16 | (uint32_t)'r' << 8 | (uint32_t)129)

#define INSTR_PROF_RAW_MAGIC_64_HIGH \
  ((uint32_t)255 << 24 | (uint32_t)'l' << 16 | (uint32_t)'p' << 8 | (uint32_t)'r')

#define EndianSwapLow(val) val
#define EndianSwapHigh(val) 0

// the size of __llvm_profile_header here is the same as the counterpart
// defined in llvm/include/ProfData/InstrProfData.inc. We use two uint32_t
// member to represent a uint64_t member in InstrProfData.inc, since Bang C
// does not support 64bit data type.
struct __llvm_profile_header {
  uint32_t MagicLow;
  uint32_t MagicHigh;
  uint32_t VersionLow;
  uint32_t VersionHigh;
  int32_t DataSizeLow;
  int32_t DataSizeHigh;
  int32_t PaddingBytesBeforeCounterSizeLow;
  int32_t PaddingBytesBeforeCounterSizeHigh;
  int32_t CounterSizeLow;
  int32_t CounterSizeHigh;
  int32_t PaddingBytesAfterCounterSizeLow;
  int32_t PaddingBytesAfterCounterSizeHigh;
  int32_t NamesSizeLow;
  int32_t NamesSizeHigh;
  uint32_t CounterDeltaLow;
  uint32_t CounterDeltaHigh;
  uint32_t NamesDeltaLow;
  uint32_t NamesDeltaHigh;
  uint32_t ValueKindLastLow;
  uint32_t ValueKindLastHigh;
};

// the size of __llvm_profile_data here is the same as the counterpart
// defined in llvm/include/ProfData/InstrProfData.inc. We use two uint32_t
// member to represent a uint64_t member in InstrProfData.inc, since Bang C
// does not support 64bit data type.
struct __llvm_profile_data {
  uint32_t NameRefLow;
  uint32_t NameRefHigh;
  uint32_t FuncHashLow;
  uint32_t FuncHashHigh;
  void *CounterPtr;
  void *FunctionPointer;
  void *Values;
  uint32_t NumCounters;
  uint16_t NumValueSites[IPVK_Last + 1];
};

// split one uint64_t into two uint32_t since MLU does not support uint64_t
// These two pointers point to first 8 Bytes of PGO buffer
__ldram__ uint32_t *PtrPGOSizeLow  = NULL;
__ldram__ uint32_t *PtrPGOSizeHigh = NULL;

// split one uint64_t into two uint32_t since MLU does not support uint64_t
// PGOSize* is the pointee of PtrPGOSizeLow/PtrPGOSizeHigh
__ldram__ uint32_t PGOSizeLow  = 0;
__ldram__ uint32_t PGOSizeHigh = 0;

// split one uint64_t into two uint32_t since MLU does not support uint64_t
// CouterOffset* record the offset of each function's PGO counter
__ldram__ int CounterOffsetLow  = 0;
__ldram__ int CounterOffsetHigh = 0;

__ldram__ uint32_t NumCounters = 0;
__ldram__ char *PtrCounter     = NULL;
__ldram__ __llvm_profile_header Header;
__ldram__ char *PtrProfileDataEnd          = NULL;
__ldram__ __llvm_profile_data *PtrData     = NULL;
__ldram__ __llvm_profile_header *PtrHeader = NULL;

__mlu_device__ __mlu_builtin__ void __llvm_profile_register_names_function() {
  void *BaseAddr        = NULL;
  uint32_t BytesPerTask = 0;
  __asm__ volatile(
      "ld.gpr.arg\t%[base], __pgobase, 6;\n\t"
      "ld.gpr.arg\t%[size], __pgosize, 4;\n\t"
      : [size] "+&r"(BytesPerTask), [base] "+&r"(BaseAddr));

  char *TaskAddr = ((char *)BaseAddr) +
                   (BytesPerTask * (__is_ipu() ? taskId : (taskDim + taskId / coreDim)));
  PtrPGOSizeLow     = (uint32_t *)TaskAddr;
  PtrPGOSizeHigh    = (uint32_t *)(TaskAddr + sizeof(uint32_t));
  PtrHeader         = (__llvm_profile_header *)(TaskAddr + sizeof(uint32_t) * 2);
  PtrProfileDataEnd = TaskAddr + BytesPerTask;
  PtrData           = (__llvm_profile_data *)(PtrHeader + 1);
  PtrCounter        = (char *)PtrData;

  RETURN_IF_OUT_OF_MEMORY(TaskAddr + 2 * sizeof(uint32_t) + sizeof(__llvm_profile_header));

  CounterOffsetHigh = 0;
  CounterOffsetLow  = sizeof(__llvm_profile_header);

  PGOSizeLow  = 0;
  PGOSizeHigh = 0;

  // fill 64bit magic with 2 32bit sub-magic
  Header.MagicLow          = INSTR_PROF_RAW_MAGIC_64_LOW;
  Header.MagicHigh         = INSTR_PROF_RAW_MAGIC_64_HIGH;
  Header.VersionLow        = 5;
  Header.VersionHigh       = 0;
  Header.DataSizeLow       = 0;
  Header.DataSizeHigh      = 0;
  Header.PaddingBytesBeforeCounterSizeLow = 0;
  Header.PaddingBytesBeforeCounterSizeHigh = 0;
  Header.CounterSizeLow    = 0;
  Header.CounterSizeHigh   = 0;
  Header.PaddingBytesAfterCounterSizeLow = 0;
  Header.PaddingBytesAfterCounterSizeHigh = 0;
  Header.NamesSizeLow      = 0;
  Header.NamesSizeHigh     = 0;
  Header.CounterDeltaLow   = 0;
  Header.CounterDeltaHigh  = 0;
  Header.NamesDeltaLow     = 0;
  Header.NamesDeltaHigh    = 0;
  Header.ValueKindLastHigh = 0;
  Header.ValueKindLastLow  = IPVK_Last;
}

__mlu_device__ __mlu_builtin__ void __llvm_profile_register_function(void __ldram__ *ProfData) {
  RETURN_IF_OUT_OF_MEMORY(PtrCounter + sizeof(__llvm_profile_data));
  PtrCounter += sizeof(__llvm_profile_data);

  Header.DataSizeLow++;
  RETURN_IF_OVERFLOW(Header.DataSizeLow);

  __llvm_profile_data *data = (__llvm_profile_data *)ProfData;
  NumCounters               = data->NumCounters;
  Header.CounterSizeLow += NumCounters;
  RETURN_IF_OVERFLOW(Header.CounterSizeLow);

  CounterOffsetLow += sizeof(__llvm_profile_data);
}

__mlu_device__ __mlu_builtin__ void __llvm_profile_unregister_function(void __ldram__ *ProfData) {
  // Step 1: copy data to GDRAM
  __memcpy(PtrData, ProfData, sizeof(__llvm_profile_data), LDRAM2GDRAM);

  // Step 2: update counterPtr in GDRAM
  __memcpy((char *)PtrData + 16, &CounterOffsetLow, sizeof(uint32_t), LDRAM2GDRAM);
  __memcpy((char *)PtrData + 20, &CounterOffsetHigh, sizeof(uint32_t), LDRAM2GDRAM);

  // Step 3: increment PtrData
  PtrData++;

  // Step 4: copy counter to GDRAM
  __llvm_profile_data *data = (__llvm_profile_data *)ProfData;
  NumCounters               = data->NumCounters;

  uint32_t ActualSize = NumCounters * 2 * sizeof(uint32_t);
  RETURN_IF_OUT_OF_MEMORY(PtrCounter + ActualSize);

  __memcpy(PtrCounter, data->CounterPtr, ActualSize, LDRAM2GDRAM);

  // Step 5: increment PtrCounter and counter_offset
  PtrCounter += ActualSize;
  CounterOffsetLow += ActualSize;
}

__mlu_device__ __mlu_builtin__ void __llvm_profile_unregister_names_function(void *names, uint32_t len) {
  // Step 1: calculate counter offset
  Header.CounterDeltaLow =
      sizeof(__llvm_profile_header) + Header.DataSizeLow * sizeof(__llvm_profile_data);

  // Step 2: calculate names offset
  Header.NamesDeltaLow = Header.CounterDeltaLow + 2 * sizeof(uint32_t) * Header.CounterSizeLow;

  // Step 3: store PGO length to GDRAM
  uint32_t AlignedLength = ALIGN(len, 2 * sizeof(uint32_t));
  PGOSizeLow             = EndianSwapLow(Header.NamesDeltaLow + AlignedLength);
  __memcpy(PtrPGOSizeLow, &PGOSizeLow, sizeof(uint32_t), LDRAM2GDRAM);
  PGOSizeHigh = EndianSwapHigh(Header.NamesDeltaLow + AlignedLength);
  __memcpy(PtrPGOSizeHigh, &PGOSizeHigh, sizeof(uint32_t), LDRAM2GDRAM);

  // Step 4: store name size
  Header.NamesSizeLow = len;

  // Step 5: copy header to GDRAM
  RETURN_IF_OUT_OF_MEMORY((char *)(PtrHeader + 1));

  Header.VersionHigh       = EndianSwapHigh(Header.VersionLow);
  Header.VersionLow        = EndianSwapLow(Header.VersionLow);
  Header.DataSizeHigh      = EndianSwapHigh(Header.DataSizeLow);
  Header.DataSizeLow       = EndianSwapLow(Header.DataSizeLow);
  Header.PaddingBytesBeforeCounterSizeLow = 0;
  Header.PaddingBytesBeforeCounterSizeHigh = 0;
  Header.CounterSizeHigh   = EndianSwapHigh(Header.CounterSizeLow);
  Header.CounterSizeLow    = EndianSwapLow(Header.CounterSizeLow);
  Header.PaddingBytesAfterCounterSizeLow = 0;
  Header.PaddingBytesAfterCounterSizeHigh = 0;
  Header.NamesSizeHigh     = EndianSwapHigh(Header.NamesSizeLow);
  Header.NamesSizeLow      = EndianSwapLow(Header.NamesSizeLow);
  Header.CounterDeltaHigh  = EndianSwapHigh(Header.CounterDeltaLow);
  Header.CounterDeltaLow   = EndianSwapLow(Header.CounterDeltaLow);
  Header.NamesDeltaHigh    = EndianSwapHigh(Header.NamesDeltaLow);
  Header.NamesDeltaLow     = EndianSwapLow(Header.NamesDeltaLow);
  Header.ValueKindLastHigh = EndianSwapHigh(Header.ValueKindLastLow);
  Header.ValueKindLastLow  = EndianSwapLow(Header.ValueKindLastLow);

  __memcpy(PtrHeader, &Header, sizeof(__llvm_profile_header), LDRAM2GDRAM);

  // Step 6: copy names to GDRAM
  char *PtrName = PtrCounter;
  RETURN_IF_OUT_OF_MEMORY(PtrName + len);
  __memcpy(PtrName, names, len, LDRAM2GDRAM);
}

#undef IPVK_Last
#undef CEIL_DIV
#undef ALIGN
#undef RETURN_IF_OUT_OF_MEMORY
#undef RETURN_IF_OVERFLOW
#undef INSTR_PROF_RAW_MAGIC_64_LOW
#undef INSTR_PROF_RAW_MAGIC_64_HIGH
#undef EndianSwapLow
#undef EndianSwapHigh

#endif  // BANG_ARCH >= 200

#endif  // __BANG_DEVICE_PROFILE_H__
