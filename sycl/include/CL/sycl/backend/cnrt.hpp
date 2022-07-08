#pragma once

#include <CL/sycl/accessor.hpp>
#include <CL/sycl/backend_types.hpp>
#include <CL/sycl/context.hpp>
#include <CL/sycl/detail/defines.hpp>
#include <CL/sycl/device.hpp>
#include <CL/sycl/event.hpp>
#include <CL/sycl/queue.hpp>

// cuda runtime api
// cu driver api
// CN driver api
// cnrt runtime api

typedef uint64_t CNdev;
typedef struct CNcontext_st *CNcontext;
typedef struct CNqueue_st *CNqueue;
typedef struct CNmodule_st *CNmodule;

// typedef uint64_t cnrtDev_t;
// typedef struct cnrtRuntimeContext *cnrtRuntimeContext_t;
// typedef struct CUevent_st *CUevent;  // TODO: find "event" in cndrv, is it notifier?

#if defined(_WIN64) || defined(__LP64__)
typedef unsigned long long MLUaddr;
#else
typedef unsigned int MLUaddr;
#endif

__SYCL_INLINE_NAMESPACE(cl) {
namespace sycl {

template <> struct interop<backend::cnrt, device> { using type = CNdev; };

template <> struct interop<backend::cnrt, context> { using type = CNcontext; };

template <> struct interop<backend::cnrt, queue> { using type = CNqueue; };

// template <> struct interop<backend::cnrt, event> { using type = CUevent; };

template <> struct interop<backend::cnrt, program> { using type = CNmodule; };

template <typename DataT, int Dimensions, access::mode AccessMode>
struct interop<backend::cnrt, accessor<DataT, Dimensions, AccessMode,
                                       access::target::global_buffer,
                                       access::placeholder::false_t>> {
  using type = MLUaddr;
};

template <typename DataT, int Dimensions, access::mode AccessMode>
struct interop<backend::cnrt, accessor<DataT, Dimensions, AccessMode,
                                       access::target::constant_buffer,
                                       access::placeholder::false_t>> {
  using type = MLUaddr;
};

template <typename DataT, int Dimensions, typename AllocatorT>
struct interop<backend::cnrt, buffer<DataT, Dimensions, AllocatorT>> {
  using type = MLUaddr;
};

} // namespace sycl
} // namespace cl

