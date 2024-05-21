//==----- mlu_memory.hpp --- SYCL group local memory extension -----==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#pragma once

#include <CL/__spirv/spirv_vars.hpp>
#include <CL/sycl/detail/defines_elementary.hpp>
#include <CL/sycl/detail/sycl_fe_intrins.hpp>
#include <CL/sycl/detail/type_traits.hpp>
#include <CL/sycl/exception.hpp>
#include <CL/sycl/group.hpp>
#include <CL/sycl/multi_ptr.hpp>

#include <cstdint>
#include <type_traits>
#include <utility>

__SYCL_INLINE_NAMESPACE(cl) {
namespace sycl {
namespace ext {
namespace mlu {

template <typename T, typename... Args>
std::enable_if_t<std::is_trivially_destructible<T>::value,
                 multi_ptr<T, access::address_space::nram_space>>
    __SYCL_ALWAYS_INLINE sycl_nram_memory() {
#ifdef __SYCL_DEVICE_ONLY__
  __attribute__((sycl_nram)) std::uint8_t *AllocatedMem =
      __sycl_allocateNramMemory(sizeof(T), alignof(T));

  return reinterpret_cast<__attribute__((sycl_nram)) T *>(AllocatedMem);
#else
  throw feature_not_supported(
      "SYCL_nram_memory extension is not supported on host device",
      PI_INVALID_OPERATION);
#endif
}

template <typename T, typename... Args>
std::enable_if_t<std::is_trivially_destructible<T>::value,
                 multi_ptr<T, access::address_space::wram_space>>
    __SYCL_ALWAYS_INLINE sycl_wram_memory() {
#ifdef __SYCL_DEVICE_ONLY__
  __attribute__((sycl_wram)) std::uint8_t *AllocatedMem =
      __sycl_allocateWramMemory(sizeof(T), alignof(T));

  return reinterpret_cast<__attribute__((sycl_wram)) T *>(AllocatedMem);
#else
  throw feature_not_supported(
      "SYCL_wram_memory extension is not supported on host device",
      PI_INVALID_OPERATION);
#endif
}

} // namespace mlu
} // namespace ext
} // namespace sycl
} // __SYCL_INLINE_NAMESPACE(cl)
