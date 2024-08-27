#pragma once

#include <CL/sycl/accessor.hpp>
#include <CL/sycl/backend_types.hpp>
#include <CL/sycl/context.hpp>
#include <CL/sycl/detail/defines.hpp>
#include <CL/sycl/device.hpp>
#include <CL/sycl/event.hpp>
#include <CL/sycl/queue.hpp>

typedef uint64_t CNdev;
typedef struct CNcontext_st *CNcontext;
typedef struct CNqueue_st *CNqueue;
typedef struct CNnotifier_st  *CNnotifier;
typedef struct CNmodule_st *CNmodule;

typedef uint64_t CNaddr;

__SYCL_INLINE_NAMESPACE(cl) {
namespace sycl {

template <> struct interop<backend::ext_oneapi_cnrt, device> { using type = CNdev; };

template <> struct interop<backend::ext_oneapi_cnrt, context> { using type = CNcontext; };

template <> struct interop<backend::ext_oneapi_cnrt, queue> { using type = CNqueue; };

template <> struct interop<backend::ext_oneapi_cnrt, event> { using type = CNnotifier; };

template <> struct interop<backend::ext_oneapi_cnrt, program> { using type = CNmodule; };

template <typename DataT, int Dimensions, access::mode AccessMode>
struct interop<backend::ext_oneapi_cnrt, accessor<DataT, Dimensions, AccessMode,
                                       access::target::global_buffer,
                                       access::placeholder::false_t>> {
  using type = CNaddr;
};

template <typename DataT, int Dimensions, access::mode AccessMode>
struct interop<backend::ext_oneapi_cnrt, accessor<DataT, Dimensions, AccessMode,
                                       access::target::constant_buffer,
                                       access::placeholder::false_t>> {
  using type = CNaddr;
};

template <typename DataT, int Dimensions, typename AllocatorT>
struct interop<backend::ext_oneapi_cnrt, buffer<DataT, Dimensions, AllocatorT>> {
  using type = CNaddr;
};

} // namespace sycl
} // namespace cl

