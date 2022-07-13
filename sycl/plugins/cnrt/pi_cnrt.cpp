//==---------- pi_cnrt.cpp - CNRT Plugin -----------------------------------==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

/// \file pi_cnrt.cpp
/// Implementation of CNRT Plugin.
///
/// \ingroup sycl_pi_cnrt

#include "pi_cnrt.hpp"
#include <CL/sycl/detail/defines.hpp>
#include <CL/sycl/detail/pi.h>
#include <CL/sycl/detail/pi.hpp>
#include <cstring>
#include <regex>
#include <sstream>
#include <string>

namespace {
std::string getCnrtVersionString() {
  std::stringstream stream;
  unsigned int version;
  cnrtRet_t result = cnrtGetVersion(&version);
  if (result != CNRT_RET_SUCCESS) {
    return "Unknown CNRT Version";
  }
  const unsigned int majorVersion = version / 10000;
  const unsigned int minorVersion = (version % 10000) / 100;
  const unsigned int patchVersion = version % 100;

  stream << "CNRT " << majorVersion << "." << minorVersion << "."
         << patchVersion;
  return stream.str();
}

pi_result map_error(CNresult result) {
  // TODO: more case
  switch (result) {
  case CN_SUCCESS:
    return PI_SUCCESS;
  case CN_OPS_ERROR_NOT_PERMITTED:
    return PI_INVALID_OPERATION;
  case CN_CONTEXT_ERROR_INVALID:
    return PI_INVALID_CONTEXT;
  case CN_ERROR_INVALID_DEVICE:
    return PI_INVALID_DEVICE;
  case CN_ERROR_INVALID_VALUE:
    return PI_INVALID_VALUE;
  case CN_MEMORY_ERROR_OUT_OF_MEMORY:
    return PI_OUT_OF_HOST_MEMORY;
  case CN_INVOKE_ERROR_OUT_OF_RESOURCES:
    return PI_OUT_OF_RESOURCES;
  default:
    return PI_ERROR_UNKNOWN;
  }
}

// Iterates over the event wait list, returns correct pi_result error codes.
// Invokes the callback for the latest event of each queue in the wait list.
// The callback must take a single pi_event argument and return a pi_result.
template <typename Func>
pi_result forLatestEvents(const pi_event *event_wait_list,
                          std::size_t num_events_in_wait_list, Func &&f) {

  if (event_wait_list == nullptr || num_events_in_wait_list == 0) {
    return PI_INVALID_EVENT_WAIT_LIST;
  }

  // Fast path if we only have a single event
  if (num_events_in_wait_list == 1) {
    return f(event_wait_list[0]);
  }

  std::vector<pi_event> events{event_wait_list,
                               event_wait_list + num_events_in_wait_list};
  // Tiered sort creating sublists of streams (smallest value first) in which
  // the corresponding events are sorted into a sequence of newest first.
  std::sort(events.begin(), events.end(), [](pi_event e0, pi_event e1) {
    return e0->get_queue()->queue_ < e1->get_queue()->queue_ ||
           (e0->get_queue()->queue_ == e1->get_queue()->queue_ &&
            e0->get_event_id() > e1->get_event_id());
  });

  bool first = true;
  CNqueue lastSeenQueue = 0;
  for (pi_event event : events) {
    if (!event || (!first && event->get_queue()->queue_ == lastSeenQueue)) {
      continue;
    }

    first = false;
    lastSeenQueue = event->get_queue()->queue_;

    auto result = f(event);
    if (result != PI_SUCCESS) {
      return result;
    }
  }

  return PI_SUCCESS;
}

/// Converts CUDA error into PI error codes, and outputs error information
/// to stderr.
/// If PI_CUDA_ABORT env variable is defined, it aborts directly instead of
/// throwing the error. This is intended for debugging purposes.
/// \return PI_SUCCESS if \param result was CUDA_SUCCESS.
/// \throw pi_error exception (integer) if input was not success.
///
pi_result check_error(CNresult result, const char *function, int line,
                      const char *file) {
  if (result == CN_SUCCESS) {
    return PI_SUCCESS;
  }

  // const char *errorString = cnrtGetErrorStr(result);

  const char *errorString = nullptr;
  const char *errorName = nullptr;
  cnGetErrorName(result, &errorString);
  cnGetErrorString(result, &errorString);
  std::cerr << "\nPI CNRT ERROR:"
            << "\n\tValue:           " << result
            << "\n\tName:            " << errorName
            << "\n\tDescription:     " << errorString
            << "\n\tFunction:        " << function
            << "\n\tSource Location: " << file << ":" << line << "\n"
            << std::endl;

  if (std::getenv("PI_CNRT_ABORT") != nullptr) {
    std::abort();
  }

  throw map_error(result);
}

/// \cond NODOXY
#define PI_CHECK_ERROR(result) check_error(result, __func__, __LINE__, __FILE__)

/// RAII type to guarantee recovering original CUDA context
/// Scoped context is used across all PI CUDA plugin implementation
/// to activate the PI Context on the current thread, matching the
/// CUDA driver semantics where the context used for the CUDA Driver
/// API is the one active on the thread.
/// The implementation tries to avoid replacing the CUcontext if it cans
class ScopedContext {
  pi_context placedContext_;
  CNcontext original_;
  bool needToRecover_;

public:
  ScopedContext(pi_context ctxt) : placedContext_{ctxt}, needToRecover_{false} {

    if (!placedContext_) {
      throw PI_INVALID_CONTEXT;
    }

    CNcontext desired = placedContext_->get();
    PI_CHECK_ERROR(cnCtxGetCurrent(&original_));
    if (original_ != desired) {
      // Sets the desired context as the active one for the thread
      PI_CHECK_ERROR(cnCtxSetCurrent(desired));
      if (original_ == nullptr) {
        // No context is installed on the current thread
        // This is the most common case. We can activate the context in the
        // thread and leave it there until all the PI context referring to the
        // same underlying CUDA context are destroyed. This emulates
        // the behaviour of the CUDA runtime api, and avoids costly context
        // switches. No action is required on this side of the if.
      } else {
        needToRecover_ = true;
      }
    }
  }

  ~ScopedContext() {
    if (needToRecover_) {
      PI_CHECK_ERROR(cnCtxSetCurrent(original_));
    }
  }
};

template <typename T, typename Assign>
pi_result getInfoImpl(size_t param_value_size, void *param_value,
                      size_t *param_value_size_ret, T value, size_t value_size,
                      Assign &&assign_func) {

  if (param_value != nullptr) {

    if (param_value_size < value_size) {
      return PI_INVALID_VALUE;
    }

    assign_func(param_value, value, value_size);
  }

  if (param_value_size_ret != nullptr) {
    *param_value_size_ret = value_size;
  }

  return PI_SUCCESS;
}

template <typename T>
pi_result getInfo(size_t param_value_size, void *param_value,
                  size_t *param_value_size_ret, T value) {

  auto assignment = [](void *param_value, T value, size_t value_size) {
    // Ignore unused parameter
    (void)value_size;

    *static_cast<T *>(param_value) = value;
  };

  return getInfoImpl(param_value_size, param_value, param_value_size_ret, value,
                     sizeof(T), assignment);
}

template <typename T>
pi_result getInfoArray(size_t array_length, size_t param_value_size,
                       void *param_value, size_t *param_value_size_ret,
                       T *value) {
  return getInfoImpl(param_value_size, param_value, param_value_size_ret, value,
                     array_length * sizeof(T), memcpy);
}

template <>
pi_result getInfo<const char *>(size_t param_value_size, void *param_value,
                                size_t *param_value_size_ret,
                                const char *value) {
  return getInfoArray(strlen(value) + 1, param_value_size, param_value,
                      param_value_size_ret, value);
}

/// \endcond

// TODO: guessLocalWorkSize
// Determine local work sizes that result in uniform work groups.
// The default threadsPerBlock only require handling the first work_dim
// dimension.
void guessLocalWorkSize(int *threadsPerBlock, const size_t *global_work_size,
                        const size_t maxThreadsPerBlock[3], pi_kernel kernel) {
  assert(threadsPerBlock != nullptr);
  assert(global_work_size != nullptr);
  assert(kernel != nullptr);

  threadsPerBlock[0] = global_work_size[0];
  return;
}

} // anonymous namespace

/// ------ Error handling, matching OpenCL plugin semantics.
__SYCL_INLINE_NAMESPACE(cl) {
namespace sycl {
namespace detail {
namespace pi {

// Report error and no return (keeps compiler from printing warnings).
// TODO: Probably change that to throw a catchable exception,
//       but for now it is useful to see every failure.
//
[[noreturn]] void die(const char *Message) {
  std::cerr << "pi_die: " << Message << std::endl;
  std::terminate();
}

// Reports error messages
void cuPrint(const char *Message) {
  std::cerr << "pi_print: " << Message << std::endl;
}

void assertion(bool Condition, const char *Message) {
  if (!Condition)
    die(Message);
}

} // namespace pi
} // namespace detail
} // namespace sycl
} // __SYCL_INLINE_NAMESPACE(cl)

//--------------
// PI object implementation

extern "C" {

// Required in a number of functions, so forward declare here
pi_result cnrt_piEnqueueEventsWait(pi_queue command_queue,
                                   pi_uint32 num_events_in_wait_list,
                                   const pi_event *event_wait_list,
                                   pi_event *event);
pi_result cnrt_piEnqueueEventsWaitWithBarrier(pi_queue command_queue,
                                              pi_uint32 num_events_in_wait_list,
                                              const pi_event *event_wait_list,
                                              pi_event *event);
pi_result cnrt_piEventRelease(pi_event event);
pi_result cnrt_piEventRetain(pi_event event);

} // extern "C"

/// \endcond

_pi_event::_pi_event(pi_command_type type, pi_context context, pi_queue queue)
    : commandType_{type}, refCount_{1}, hasBeenWaitedOn_{false},
      isRecorded_{false}, isStarted_{false}, evEnd_{nullptr}, evStart_{nullptr},
      evQueued_{nullptr}, queue_{queue}, context_{context} {

  bool profilingEnabled = queue_->properties_ & PI_QUEUE_PROFILING_ENABLE;

  PI_CHECK_ERROR(cnCreateNotifier(&evEnd_, profilingEnabled
                                               ? CN_NOTIFIER_DEFAULT
                                               : CN_NOTIFIER_DISABLE_TIMING));

  if (profilingEnabled) {
    PI_CHECK_ERROR(cnCreateNotifier(&evQueued_, CN_NOTIFIER_DEFAULT));
    PI_CHECK_ERROR(cnCreateNotifier(&evStart_, CN_NOTIFIER_DEFAULT));
  }

  if (queue_ != nullptr) {
    cnrt_piQueueRetain(queue_);
  }
  cnrt_piContextRetain(context_);
}

_pi_event::~_pi_event() {
  if (queue_ != nullptr) {
    cnrt_piQueueRelease(queue_);
  }
  cnrt_piContextRelease(context_);
}

pi_result _pi_event::start() {
  assert(!is_started());
  pi_result result = PI_SUCCESS;

  try {
    if (queue_->properties_ & PI_QUEUE_PROFILING_ENABLE) {
      // NOTE: This relies on the default stream to be unused.
      result = PI_CHECK_ERROR(cnPlaceNotifier(evQueued_, 0));
      result = PI_CHECK_ERROR(cnPlaceNotifier(evStart_, queue_->get()));
    }
  } catch (pi_result error) {
    result = error;
  }

  isStarted_ = true;
  return result;
}

bool _pi_event::is_completed() const noexcept {
  if (!isRecorded_) {
    return false;
  }
  if (!hasBeenWaitedOn_) {
    const CNresult ret = cnQueryNotifier(evEnd_);
    if (ret != CN_SUCCESS && ret != CN_ERROR_NOT_READY) {
      PI_CHECK_ERROR(ret);
      return false;
    }
    if (ret == CN_ERROR_NOT_READY) {
      return false;
    }
  }
  return true;
}

pi_uint64 _pi_event::get_queued_time() const {
  float miliSeconds = 0.0f;
  assert(is_started());

  PI_CHECK_ERROR(
      cnNotifierElapsedTime(&miliSeconds, context_->evBase_, evQueued_));
  return static_cast<pi_uint64>(miliSeconds * 1.0e6);
}

pi_uint64 _pi_event::get_start_time() const {
  float miliSeconds = 0.0f;
  assert(is_started());

  PI_CHECK_ERROR(
      cnNotifierElapsedTime(&miliSeconds, context_->evBase_, evStart_));
  return static_cast<pi_uint64>(miliSeconds * 1.0e6);
}

pi_uint64 _pi_event::get_end_time() const {
  float miliSeconds = 0.0f;
  assert(is_started() && is_recorded());

  PI_CHECK_ERROR(
      cnNotifierElapsedTime(&miliSeconds, context_->evBase_, evEnd_));
  return static_cast<pi_uint64>(miliSeconds * 1.0e6);
}

pi_result _pi_event::record() {

  if (is_recorded() || !is_started()) {
    return PI_INVALID_EVENT;
  }

  pi_result result = PI_INVALID_OPERATION;

  if (!queue_) {
    return PI_INVALID_QUEUE;
  }

  CNqueue cnQueue = queue_->get();

  try {
    eventId_ = queue_->get_next_event_id();
    if (eventId_ == 0) {
      cl::sycl::detail::pi::die(
          "Unrecoverable program state reached in event identifier overflow");
    }
    result = PI_CHECK_ERROR(cnPlaceNotifier(evEnd_, cnQueue));
    result = PI_SUCCESS;
  } catch (pi_result error) {
    result = error;
  }

  if (result == PI_SUCCESS) {
    isRecorded_ = true;
  }

  return result;
}

pi_result _pi_event::wait() {
  pi_result retErr;
  try {
    retErr = PI_CHECK_ERROR(cnWaitNotifier(evEnd_));
    hasBeenWaitedOn_ = true;
  } catch (pi_result error) {
    retErr = error;
  }

  return retErr;
}

pi_result _pi_event::release() {
  assert(queue_ != nullptr);
  PI_CHECK_ERROR(cnDestroyNotifier(evEnd_));

  if (queue_->properties_ & PI_QUEUE_PROFILING_ENABLE) {
    PI_CHECK_ERROR(cnDestroyNotifier(evQueued_));
    PI_CHECK_ERROR(cnDestroyNotifier(evStart_));
  }

  return PI_SUCCESS;
}

// makes all future work submitted to queue wait for all work captured in event.
pi_result enqueueEventWait(pi_queue queue, pi_event event) {
  // for native events, the cuStreamWaitEvent call is used.
  // This makes all future work submitted to stream wait for all
  // work captured in event.
  return PI_CHECK_ERROR(cnQueueWaitNotifier(queue->get(), event->get()));
}

_pi_program::_pi_program(pi_context ctxt)
    : module_{nullptr}, binary_{},
      binarySizeInBytes_{0}, refCount_{1}, context_{ctxt} {
  cnrt_piContextRetain(context_);
}

_pi_program::~_pi_program() { cnrt_piContextRelease(context_); }

pi_result _pi_program::set_binary(const char *source, size_t length) {
  assert((binary_ == nullptr && binarySizeInBytes_ == 0) &&
         "Re-setting program binary data which has already been set");
  binary_ = source;
  binarySizeInBytes_ = length;
  return PI_SUCCESS;
}

pi_result _pi_program::build_program(const char *build_options) {

  this->buildOptions_ = build_options;

  auto result = PI_CHECK_ERROR(
      cnModuleLoadFatBinary(static_cast<const void *>(binary_), &module_));

  const auto success = (result == PI_SUCCESS);

  buildStatus_ =
      success ? PI_PROGRAM_BUILD_STATUS_SUCCESS : PI_PROGRAM_BUILD_STATUS_ERROR;

  // If no exception, result is correct
  return success ? PI_SUCCESS : PI_BUILD_PROGRAM_FAILURE;
  return PI_SUCCESS;
}

/// Finds kernel names by searching for entry points in the PTX source, as the
/// CUDA driver API doesn't expose an operation for this.
/// Note: This is currently only being used by the SYCL program class for the
///       has_kernel method, so an alternative would be to move the has_kernel
///       query to PI and use cuModuleGetFunction to check for a kernel.
std::string getKernelNames(pi_program program) {
  std::string source(program->binary_,
                     program->binary_ + program->binarySizeInBytes_);
  std::regex entries_pattern(".entry\\s+([^\\([:s:]]*)");
  std::string names("");
  std::smatch match;
  bool first_match = true;
  while (std::regex_search(source, match, entries_pattern)) {
    assert(match.size() == 2);
    names += first_match ? "" : ";";
    names += match[1]; // Second element is the group.
    source = match.suffix().str();
    first_match = false;
  }
  return names;
}

/// RAII object that calls the reference count release function on the held PI
/// object on destruction.
///
/// The `dismiss` function stops the release from happening on destruction.
template <typename T> class ReleaseGuard {
private:
  T Captive;

  static pi_result callRelease(pi_device Captive) {
    return cnrt_piDeviceRelease(Captive);
  }

  static pi_result callRelease(pi_context Captive) {
    return cnrt_piContextRelease(Captive);
  }

  static pi_result callRelease(pi_mem Captive) {
    return cnrt_piMemRelease(Captive);
  }

  static pi_result callRelease(pi_program Captive) {
    return cnrt_piProgramRelease(Captive);
  }

  static pi_result callRelease(pi_kernel Captive) {
    return cnrt_piKernelRelease(Captive);
  }

  static pi_result callRelease(pi_queue Captive) {
    return cnrt_piQueueRelease(Captive);
  }

  static pi_result callRelease(pi_event Captive) {
    return cnrt_piEventRelease(Captive);
  }

public:
  ReleaseGuard() = delete;
  /// Obj can be `nullptr`.
  explicit ReleaseGuard(T Obj) : Captive(Obj) {}
  ReleaseGuard(ReleaseGuard &&Other) noexcept : Captive(Other.Captive) {
    Other.Captive = nullptr;
  }

  ReleaseGuard(const ReleaseGuard &) = delete;

  /// Calls the related PI object release function if the object held is not
  /// `nullptr` or if `dismiss` has not been called.
  ~ReleaseGuard() {
    if (Captive != nullptr) {
      pi_result ret = callRelease(Captive);
      if (ret != PI_SUCCESS) {
        // A reported CUDA error is either an implementation or an asynchronous
        // CUDA error for which it is unclear if the function that reported it
        // succeeded or not. Either way, the state of the program is compromised
        // and likely unrecoverable.
        cl::sycl::detail::pi::die(
            "Unrecoverable program state reached in cuda_piMemRelease");
      }
    }
  }

  ReleaseGuard &operator=(const ReleaseGuard &) = delete;

  ReleaseGuard &operator=(ReleaseGuard &&Other) {
    Captive = Other.Captive;
    Other.Captive = nullptr;
    return *this;
  }

  /// End the guard and do not release the reference count of the held
  /// PI object.
  void dismiss() { Captive = nullptr; }
};

//-- PI API implementation
extern "C" {

/// Obtains the CUDA platform.
/// There is only one CUDA platform, and contains all devices on the system.
/// Triggers the CUDA Driver initialization (cuInit) the first time, so this
/// must be the first PI API called.
///
pi_result cnrt_piPlatformsGet(pi_uint32 num_entries, pi_platform *platforms,
                              pi_uint32 *num_platforms) {
  try {
    static std::once_flag initFlag;
    static pi_uint32 numPlatforms = 1;
    static _pi_platform platformId;

    if (num_entries == 0 && platforms != nullptr) {
      return PI_INVALID_VALUE;
    }
    if (platforms == nullptr && num_platforms == nullptr) {
      return PI_INVALID_VALUE;
    }

    pi_result err = PI_SUCCESS;

    std::call_once(
        initFlag,
        [](pi_result &err) {
          if (cnInit(0) != CN_SUCCESS) {
            numPlatforms = 0;
            return;
          }
          int numDevices = 0;
          err = PI_CHECK_ERROR(cnDeviceGetCount(&numDevices));
          if (numDevices == 0) {
            numPlatforms = 0;
            return;
          }
          try {
            platformId.devices_.reserve(numDevices);
            for (int i = 0; i < numDevices; ++i) {
              CNdev device;
              err = PI_CHECK_ERROR(cnDeviceGet(&device, i));
              platformId.devices_.emplace_back(
                  new _pi_device{device, &platformId});
            }
          } catch (const std::bad_alloc &) {
            // Signal out-of-memory situation
            platformId.devices_.clear();
            err = PI_OUT_OF_HOST_MEMORY;
          } catch (...) {
            platformId.devices_.clear();
            throw;
          }
        },
        err);

    if (num_platforms != nullptr) {
      *num_platforms = numPlatforms;
    }

    if (platforms != nullptr) {
      *platforms = &platformId;
    }

    return err;
  } catch (pi_result err) {
    return err;
  } catch (...) {
    return PI_OUT_OF_RESOURCES;
  }
}

pi_result cnrt_piPlatformGetInfo(pi_platform platform,
                                 pi_platform_info param_name,
                                 size_t param_value_size, void *param_value,
                                 size_t *param_value_size_ret) {
  assert(platform != nullptr);

  switch (param_name) {
  // TODO: CNRT information
  case PI_PLATFORM_INFO_NAME:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   "Cambricon BANG BACKEND");
  case PI_PLATFORM_INFO_VENDOR:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   "Cambricon Corporation");
  case PI_PLATFORM_INFO_PROFILE:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   "FULL PROFILE");
  case PI_PLATFORM_INFO_VERSION: {
    auto version = getCnrtVersionString();
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   version.c_str());
  }
  case PI_PLATFORM_INFO_EXTENSIONS: {
    return getInfo(param_value_size, param_value, param_value_size_ret, "");
  }
  default:
    __SYCL_PI_HANDLE_UNKNOWN_PARAM_NAME(param_name);
  }
  cl::sycl::detail::pi::die("Platform info request not implemented");
  return {};
}

/// \param devices List of devices available on the system
/// \param num_devices Number of elements in the list of devices
/// Requesting a non-GPU device triggers an error, all PI CUDA devices
/// are GPUs.
///
pi_result cnrt_piDevicesGet(pi_platform platform, pi_device_type device_type,
                            pi_uint32 num_entries, pi_device *devices,
                            pi_uint32 *num_devices) {
  pi_result err = PI_SUCCESS;
  const bool askingForDefault = device_type == PI_DEVICE_TYPE_DEFAULT;
  const bool askingForGPU = device_type & PI_DEVICE_TYPE_GPU;
  const bool returnDevices = askingForDefault || askingForGPU;

  size_t numDevices = returnDevices ? platform->devices_.size() : 0;

  try {
    if (num_devices) {
      *num_devices = numDevices;
    }

    if (returnDevices && devices) {
      for (size_t i = 0; i < std::min(size_t(num_entries), numDevices); ++i) {
        devices[i] = platform->devices_[i].get();
      }
    }

    return err;
  } catch (pi_result err) {
    return err;
  } catch (...) {
    return PI_OUT_OF_RESOURCES;
  }
}

/// \return PI_SUCCESS if the function is executed successfully
/// CUDA devices are always root devices so retain always returns success.
pi_result cnrt_piDeviceRetain(pi_device) { return PI_SUCCESS; }

pi_result cnrt_piContextGetInfo(pi_context context, pi_context_info param_name,
                                size_t param_value_size, void *param_value,
                                size_t *param_value_size_ret) {
  switch (param_name) {
  case PI_CONTEXT_INFO_NUM_DEVICES:
    return getInfo(param_value_size, param_value, param_value_size_ret, 1);
  case PI_CONTEXT_INFO_DEVICES:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   context->get_device());
  case PI_CONTEXT_INFO_REFERENCE_COUNT:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   context->get_reference_count());
  default:
    __SYCL_PI_HANDLE_UNKNOWN_PARAM_NAME(param_name);
  }

  return PI_OUT_OF_RESOURCES;
}

pi_result cnrt_piContextRetain(pi_context context) {
  assert(context != nullptr);
  assert(context->get_reference_count() > 0);

  context->increment_reference_count();
  return PI_SUCCESS;
}

pi_result cnrt_piextContextSetExtendedDeleter(
    pi_context context, pi_context_extended_deleter function, void *user_data) {
  context->set_extended_deleter(function, user_data);
  return PI_SUCCESS;
}

/// Not applicable to CUDA, devices cannot be partitioned.
/// TODO: untie cl_device_partition_property from OpenCL
///
pi_result cnrt_piDevicePartition(pi_device,
                                 const cl_device_partition_property *,
                                 pi_uint32, pi_device *, pi_uint32 *) {
  return {};
}

/// \return If available, the first binary that is PTX
///
pi_result cnrt_piextDeviceSelectBinary(pi_device device,
                                       pi_device_binary *binaries,
                                       pi_uint32 num_binaries,
                                       pi_uint32 *selected_binary) {
  // Ignore unused parameter
  (void)device;

  if (!binaries) {
    cl::sycl::detail::pi::die("No list of device images provided");
  }
  if (num_binaries < 1) {
    cl::sycl::detail::pi::die("No binary images in the list");
  }

  // Look for an image for the NVPTX64 target, and return the first one that is
  // found
  for (pi_uint32 i = 0; i < num_binaries; i++) {
    if (strcmp(binaries[i]->DeviceTargetSpec,
               __SYCL_PI_DEVICE_BINARY_TARGET_MLISA) == 0) {
      *selected_binary = i;
      return PI_SUCCESS;
    }
  }

  // No image can be loaded for the given device
  return PI_INVALID_BINARY;
}

pi_result cnrt_piextGetDeviceFunctionPointer(pi_device, pi_program,
                                             const char *, pi_uint64 *) {
  cl::sycl::detail::pi::die(
      "cnrt_piextGetDeviceFunctionPointer not implemented");
  return {};
}

/// \return PI_SUCCESS always since CUDA devices are always root devices.
///
pi_result cnrt_piDeviceRelease(pi_device) { return PI_SUCCESS; }
/// Obtains the CNRT platform
/// First PI API

pi_result cnrt_piDeviceGetInfo(pi_device device, pi_device_info param_name,
                               size_t param_value_size, void *param_value,
                               size_t *param_value_size_ret) {
  static constexpr pi_uint32 max_work_item_dimensions = 3u;

  assert(device != nullptr);

  switch (param_name) {
  case PI_DEVICE_INFO_TYPE: {
    // TODO: GPU??
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   PI_DEVICE_TYPE_GPU);
  }
  case PI_DEVICE_INFO_VENDOR_ID: {
    // TODO: Cambricon ID
    return getInfo(param_value_size, param_value, param_value_size_ret, 4318u);
  }

  case PI_DEVICE_INFO_MAX_COMPUTE_UNITS: {
    int compute_units = 0;
    cl::sycl::detail::pi::assertion(
        cnDeviceGetAttribute(&compute_units,
                             CN_DEVICE_ATTRIBUTE_MAX_CLUSTER_COUNT,
                             device->get()) == CN_SUCCESS);
    cl::sycl::detail::pi::assertion(compute_units >= 0);
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   pi_uint32(compute_units));
  }
  case PI_DEVICE_INFO_MAX_WORK_ITEM_DIMENSIONS: {
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   max_work_item_dimensions);
  }
  case PI_DEVICE_INFO_MAX_WORK_ITEM_SIZES: {
    size_t return_size[max_work_item_dimensions];

    int max_x = 0, max_y = 0, max_z = 0;
    cl::sycl::detail::pi::assertion(
        cnDeviceGetAttribute(&max_x, CN_DEVICE_ATTRIBUTE_MAX_BLOCK_TASK_DIM_X,
                             device->get()) == CN_SUCCESS);
    cl::sycl::detail::pi::assertion(max_x >= 0);

    cl::sycl::detail::pi::assertion(
        cnDeviceGetAttribute(&max_y, CN_DEVICE_ATTRIBUTE_MAX_BLOCK_TASK_DIM_Y,
                             device->get()) == CN_SUCCESS);
    cl::sycl::detail::pi::assertion(max_y >= 0);

    cl::sycl::detail::pi::assertion(
        cnDeviceGetAttribute(&max_z, CN_DEVICE_ATTRIBUTE_MAX_BLOCK_TASK_DIM_Z,
                             device->get()) == CN_SUCCESS);
    cl::sycl::detail::pi::assertion(max_z >= 0);

    return_size[0] = size_t(max_x);
    return_size[1] = size_t(max_y);
    return_size[2] = size_t(max_z);
    return getInfoArray(max_work_item_dimensions, param_value_size, param_value,
                        param_value_size_ret, return_size);
  }

  // TODO: work group int mlu?
  case PI_DEVICE_INFO_MAX_WORK_GROUP_SIZE: {
    int max_work_group_size = 0;
    cl::sycl::detail::pi::assertion(
        cnDeviceGetAttribute(&max_work_group_size,
                             CN_DEVICE_ATTRIBUTE_MAX_CORE_COUNT_PER_CLUSTER,
                             device->get()) == CN_SUCCESS);

    cl::sycl::detail::pi::assertion(max_work_group_size >= 0);

    return getInfo(param_value_size, param_value, param_value_size_ret,
                   size_t(max_work_group_size));
  }
  case PI_DEVICE_INFO_PREFERRED_VECTOR_WIDTH_CHAR: {
    return getInfo(param_value_size, param_value, param_value_size_ret, 1u);
  }
  case PI_DEVICE_INFO_PREFERRED_VECTOR_WIDTH_SHORT: {
    return getInfo(param_value_size, param_value, param_value_size_ret, 1u);
  }
  case PI_DEVICE_INFO_PREFERRED_VECTOR_WIDTH_INT: {
    return getInfo(param_value_size, param_value, param_value_size_ret, 1u);
  }
  case PI_DEVICE_INFO_PREFERRED_VECTOR_WIDTH_LONG: {
    return getInfo(param_value_size, param_value, param_value_size_ret, 1u);
  }
  case PI_DEVICE_INFO_PREFERRED_VECTOR_WIDTH_FLOAT: {
    return getInfo(param_value_size, param_value, param_value_size_ret, 1u);
  }
  case PI_DEVICE_INFO_PREFERRED_VECTOR_WIDTH_DOUBLE: {
    return getInfo(param_value_size, param_value, param_value_size_ret, 1u);
  }
  case PI_DEVICE_INFO_PREFERRED_VECTOR_WIDTH_HALF: {
    return getInfo(param_value_size, param_value, param_value_size_ret, 0u);
  }
  case PI_DEVICE_INFO_NATIVE_VECTOR_WIDTH_CHAR: {
    return getInfo(param_value_size, param_value, param_value_size_ret, 1u);
  }
  case PI_DEVICE_INFO_NATIVE_VECTOR_WIDTH_SHORT: {
    return getInfo(param_value_size, param_value, param_value_size_ret, 1u);
  }
  case PI_DEVICE_INFO_NATIVE_VECTOR_WIDTH_INT: {
    return getInfo(param_value_size, param_value, param_value_size_ret, 1u);
  }
  case PI_DEVICE_INFO_NATIVE_VECTOR_WIDTH_LONG: {
    return getInfo(param_value_size, param_value, param_value_size_ret, 1u);
  }
  case PI_DEVICE_INFO_NATIVE_VECTOR_WIDTH_FLOAT: {
    return getInfo(param_value_size, param_value, param_value_size_ret, 1u);
  }
  case PI_DEVICE_INFO_NATIVE_VECTOR_WIDTH_DOUBLE: {
    return getInfo(param_value_size, param_value, param_value_size_ret, 1u);
  }
  case PI_DEVICE_INFO_NATIVE_VECTOR_WIDTH_HALF: {
    return getInfo(param_value_size, param_value, param_value_size_ret, 0u);
  }

  // TODO[MLU]: how to find max num subgroup of mlu
  case PI_DEVICE_INFO_MAX_NUM_SUB_GROUPS: {
  }

  case PI_DEVICE_INFO_SUB_GROUP_INDEPENDENT_FORWARD_PROGRESS: {
    // Volta provides independent thread scheduling
    // TODO: Revisit for previous generation GPUs
    int major = 0;
    cl::sycl::detail::pi::assertion(
        cnDeviceGetAttribute(&major,
                             CN_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR,
                             device->get()) == CN_SUCCESS);
    // TODO[MLU]: is mlu subgroup indenpendent foward progress
    // bool ifp = (major >= 7);
    bool ifp = false;
    return getInfo(param_value_size, param_value, param_value_size_ret, ifp);
  }
  case PI_DEVICE_INFO_SUB_GROUP_SIZES_INTEL: {
    // NVIDIA devices only support one sub-group size (the warp size)
    int warpSize = 0;
    // cl::sycl::detail::pi::assertion(
    //     cnDeviceGetAttribute(&warpSize, CU_DEVICE_ATTRIBUTE_WARP_SIZE,
    //                          device->get()) == CUDA_SUCCESS);
    // TODO[MLU]: MLU warp size?
    warpSize = 1;
    size_t sizes[1] = {static_cast<size_t>(warpSize)};
    return getInfoArray<size_t>(1, param_value_size, param_value,
                                param_value_size_ret, sizes);
  }

  case PI_DEVICE_INFO_MAX_CLOCK_FREQUENCY: {
    int clock_freq = 0;
    cl::sycl::detail::pi::assertion(
        cnDeviceGetAttribute(&clock_freq,
                             CN_DEVICE_ATTRIBUTE_CLUSTER_CLOCK_RATE,
                             device->get()) == CN_SUCCESS);
    cl::sycl::detail::pi::assertion(clock_freq >= 0);
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   pi_uint32(clock_freq) / 1000u);
  }
  case PI_DEVICE_INFO_ADDRESS_BITS: {
    auto bits = pi_uint32{std::numeric_limits<uintptr_t>::digits};
    return getInfo(param_value_size, param_value, param_value_size_ret, bits);
  }
  case PI_DEVICE_INFO_MAX_MEM_ALLOC_SIZE: {
    // Max size of memory object allocation in bytes.
    // The minimum value is max(min(1024 × 1024 ×
    // 1024, 1/4th of CL_DEVICE_GLOBAL_MEM_SIZE),
    // 32 × 1024 × 1024) for devices that are not of type
    // CL_DEVICE_TYPE_CUSTOM.

    size_t global = 0;
    cl::sycl::detail::pi::assertion(cnDeviceTotalMem(&global, device->get()) ==
                                    CN_SUCCESS);

    auto quarter_global = static_cast<pi_uint32>(global / 4u);

    auto max_alloc = std::max(std::min(1024u * 1024u * 1024u, quarter_global),
                              32u * 1024u * 1024u);

    return getInfo(param_value_size, param_value, param_value_size_ret,
                   pi_uint64{max_alloc});
  }
  case PI_DEVICE_INFO_IMAGE_SUPPORT: {
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   PI_TRUE);
  }
  case PI_DEVICE_INFO_MAX_READ_IMAGE_ARGS: {
    // This call doesn't match to CUDA as it doesn't have images, but instead
    // surfaces and textures. No clear call in the CUDA API to determine this,
    // but some searching found as of SM 2.x 128 are supported.
    return getInfo(param_value_size, param_value, param_value_size_ret, 128u);
  }
  case PI_DEVICE_INFO_MAX_WRITE_IMAGE_ARGS: {
    // This call doesn't match to CUDA as it doesn't have images, but instead
    // surfaces and textures. No clear call in the CUDA API to determine this,
    // but some searching found as of SM 2.x 128 are supported.
    return getInfo(param_value_size, param_value, param_value_size_ret, 128u);
  }
  case PI_DEVICE_INFO_HOST_UNIFIED_MEMORY: {
    // TODO[MLU]: Is MLU device integrated with host memory?
    int is_integrated = 0;
    // cl::sycl::detail::pi::assertion(
    //     cuDeviceGetAttribute(&is_integrated, CU_DEVICE_ATTRIBUTE_INTEGRATED,
    //                          device->get()) == CUDA_SUCCESS);

    cl::sycl::detail::pi::assertion((is_integrated == 0) |
                                    (is_integrated == 1));
    auto result = static_cast<bool>(is_integrated);
    return getInfo(param_value_size, param_value, param_value_size_ret, result);
  }

  case PI_DEVICE_INFO_PROFILING_TIMER_RESOLUTION: {
    // Hard coded to value returned by clinfo for OpenCL 1.2 CUDA | GeForce
    // GTX 1060 3GB
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   size_t{1000u});
  }
  case PI_DEVICE_INFO_ENDIAN_LITTLE: {
    return getInfo(param_value_size, param_value, param_value_size_ret, true);
  }
  case PI_DEVICE_INFO_AVAILABLE: {
    return getInfo(param_value_size, param_value, param_value_size_ret, true);
  }
  case PI_DEVICE_INFO_COMPILER_AVAILABLE: {
    return getInfo(param_value_size, param_value, param_value_size_ret, true);
  }
  case PI_DEVICE_INFO_LINKER_AVAILABLE: {
    return getInfo(param_value_size, param_value, param_value_size_ret, true);
  }
  case PI_DEVICE_INFO_EXECUTION_CAPABILITIES: {
    auto capability = CL_EXEC_KERNEL;
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   capability);
  }
  case PI_DEVICE_INFO_QUEUE_ON_DEVICE_PROPERTIES: {
    // The mandated minimum capability:
    auto capability =
        CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   capability);
  }
  case PI_DEVICE_INFO_QUEUE_ON_HOST_PROPERTIES: {
    // The mandated minimum capability:
    auto capability = CL_QUEUE_PROFILING_ENABLE;
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   capability);
  }
  case PI_DEVICE_INFO_BUILT_IN_KERNELS: {
    // An empty string is returned if no built-in kernels are supported by
    // the device.
    return getInfo(param_value_size, param_value, param_value_size_ret, "");
  }
  case PI_DEVICE_INFO_PLATFORM: {
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   device->get_platform());
  }

  case PI_DEVICE_INFO_NAME: {
    static constexpr size_t MAX_DEVICE_NAME_LENGTH = 256u;
    char name[MAX_DEVICE_NAME_LENGTH];
    cl::sycl::detail::pi::assertion(
        cnDeviceGetName(name, MAX_DEVICE_NAME_LENGTH, device->get()) ==
        CN_SUCCESS);
    return getInfoArray(strlen(name) + 1, param_value_size, param_value,
                        param_value_size_ret, name);
  }
  case PI_DEVICE_INFO_VENDOR: {
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   "Cambricon, Inc");
  }
  case PI_DEVICE_INFO_DRIVER_VERSION: {
    auto version = getCnrtVersionString();
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   version.c_str());
  }
  case PI_DEVICE_INFO_PROFILE: {
    return getInfo(param_value_size, param_value, param_value_size_ret, "CUDA");
  }
  case PI_DEVICE_INFO_REFERENCE_COUNT: {
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   device->get_reference_count());
  }
  case PI_DEVICE_INFO_VERSION: {
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   "PI 0.0");
  }
  case PI_DEVICE_INFO_OPENCL_C_VERSION: {
    return getInfo(param_value_size, param_value, param_value_size_ret, "");
  }
  case PI_DEVICE_INFO_EXTENSIONS: {
    return getInfo(param_value_size, param_value, param_value_size_ret, "");
  }
  case PI_DEVICE_INFO_PRINTF_BUFFER_SIZE: {
    // The minimum value for the FULL profile is 1 MB.
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   size_t{1024u});
  }
  case PI_DEVICE_INFO_PREFERRED_INTEROP_USER_SYNC: {
    return getInfo(param_value_size, param_value, param_value_size_ret, true);
  }
  case PI_DEVICE_INFO_PARENT_DEVICE: {
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   nullptr);
  }
  default:
    __SYCL_PI_HANDLE_UNKNOWN_PARAM_NAME(param_name);
  }
  cl::sycl::detail::pi::die("Device info request not implemented");
  return {};
}

/// Gets the native CUDA handle of a PI device object
///
/// \param[in] device The PI device to get the native CUDA object of.
/// \param[out] nativeHandle Set to the native handle of the PI device object.
///
/// \return PI_SUCCESS
pi_result cnrt_piextDeviceGetNativeHandle(pi_device device,
                                          pi_native_handle *nativeHandle) {
  *nativeHandle = static_cast<pi_native_handle>(device->get());
  return PI_SUCCESS;
}

/// Created a PI device object from a CUDA device handle.
/// TODO: Implement this.
/// NOTE: The created PI object takes ownership of the native handle.
///
/// \param[in] nativeHandle The native handle to create PI device object from.
/// \param[in] platform is the PI platform of the device.
/// \param[out] device Set to the PI device object created from native handle.
///
/// \return TBD
pi_result cnrt_piextDeviceCreateWithNativeHandle(pi_native_handle, pi_platform,
                                                 pi_device *) {
  cl::sycl::detail::pi::die(
      "Creation of PI device from native handle not implemented");
  return {};
}

/* Context APIs */

/// Create a PI CNRT context.
///
/// TODO: Primary CUDA context
///
/// \param[in] properties 0 terminated array of key/id-value combinations. Can
/// be nullptr. Only accepts property key/id
/// \param[in] num_devices Number of devices to create the context for.
/// \param[in] devices Devices to create the context for.
/// \param[in] pfn_notify Callback, currently unused.
/// \param[in] user_data User data for callback.
/// \param[out] retcontext Set to created context on success.
///
/// \return PI_SUCCESS on success, otherwise an error return code.
pi_result cnrt_piContextCreate(const pi_context_properties *properties,
                               pi_uint32 num_devices, const pi_device *devices,
                               void (*pfn_notify)(const char *errinfo,
                                                  const void *private_info,
                                                  size_t cb, void *user_data),
                               void *user_data, pi_context *retcontext) {

  assert(devices != nullptr);
  // TODO: How to implement context callback?
  assert(pfn_notify == nullptr);
  assert(user_data == nullptr);
  assert(num_devices == 1);
  // Need input context
  assert(retcontext != nullptr);
  pi_result errcode_ret = PI_SUCCESS;

  // Parse properties.
  bool property_cuda_primary = false;
  while (properties && (0 != *properties)) {
    // Consume property ID.
    pi_context_properties id = *properties;
    ++properties;
    // Consume property value.
    // TODO: Consume property value
    // pi_context_properties value = *properties;
    ++properties;
    switch (id) {
    default:
      // Unknown property.
      cl::sycl::detail::pi::die(
          "Unknown piContextCreate property in property list");
      return PI_INVALID_VALUE;
    }
  }

  std::unique_ptr<_pi_context> piContextPtr{nullptr};
  try {
    CNcontext current = nullptr;

    // TODO: Primary Context？
    if (property_cuda_primary) {
      // Use the CUDA primary context and assume that we want to use it
      // immediately as we want to forge context switches.
    } else {
      // Create a scoped context.
      CNcontext newContext;
      PI_CHECK_ERROR(cnCtxGetCurrent(&current));
      //  TODO: CU_CTX_MAP_HOST
      errcode_ret = PI_CHECK_ERROR(
          cnCtxCreate(&newContext, CN_CTX_SCHED_SYNC_AUTO, devices[0]->get()));
      piContextPtr = std::unique_ptr<_pi_context>(new _pi_context{
          _pi_context::kind::user_defined, newContext, *devices});
    }

    // Use default stream to record base event counter
    PI_CHECK_ERROR(
        cnCreateNotifier(&piContextPtr->evBase_, CN_NOTIFIER_DEFAULT));
    PI_CHECK_ERROR(cnPlaceNotifier(piContextPtr->evBase_, 0));

    // For non-primary scoped contexts keep the last active on top of the stack
    // as `cuCtxCreate` replaces it implicitly otherwise.
    // Primary contexts are kept on top of the stack, so the previous context
    // is not queried and therefore not recovered.
    if (current != nullptr) {
      PI_CHECK_ERROR(cnCtxSetCurrent(current));
    }

    *retcontext = piContextPtr.release();
  } catch (pi_result err) {
    errcode_ret = err;
  } catch (...) {
    errcode_ret = PI_OUT_OF_RESOURCES;
  }
  return errcode_ret;
}

pi_result cnrt_piContextRelease(pi_context ctxt) {
  assert(ctxt != nullptr);

  if (ctxt->decrement_reference_count() > 0) {
    return PI_SUCCESS;
  }
  ctxt->invoke_extended_deleters();

  std::unique_ptr<_pi_context> context{ctxt};

  PI_CHECK_ERROR(cnDestroyNotifier(context->evBase_));

  if (!ctxt->is_primary()) {
    CNcontext cnCtxt = ctxt->get();
    CNcontext current = nullptr;
    cnCtxGetCurrent(&current);
    if (cnCtxt != current) {
      PI_CHECK_ERROR(cnCtxSetCurrent(cnCtxt));
    }
    PI_CHECK_ERROR(cnCtxSync());
    cnCtxGetCurrent(&current);
    if (cnCtxt == current) {
      // PopCurrent
      PI_CHECK_ERROR(cnCtxSetCurrent(NULL));
    }
    return PI_CHECK_ERROR(cnCtxDestroy(cnCtxt));
  } else {
    // Primary context is not destroyed, but released
    CNdev cuDev = ctxt->get_device()->get();
    cnCtxSetCurrent(NULL);
    // TODO[MLU]: Is cnSharedContextRelease the same as
    // cuDevicePrimaryCtxRelease?
    return PI_CHECK_ERROR(cnSharedContextRelease(cuDev));
  }
}

/// Gets the native CUDA handle of a PI context object
///
/// \param[in] context The PI context to get the native CUDA object of.
/// \param[out] nativeHandle Set to the native handle of the PI context object.
///
/// \return PI_SUCCESS
pi_result cnrt_piextContextGetNativeHandle(pi_context context,
                                           pi_native_handle *nativeHandle) {
  *nativeHandle = reinterpret_cast<pi_native_handle>(context->get());
  return PI_SUCCESS;
}

/// Created a PI context object from a CUDA context handle.
/// TODO: Implement this.
/// NOTE: The created PI object takes ownership of the native handle.
///
/// \param[in] nativeHandle The native handle to create PI context object from.
/// \param[out] context Set to the PI context object created from native handle.
///
/// \return TBD
pi_result cnrt_piextContextCreateWithNativeHandle(pi_native_handle, pi_uint32,
                                                  const pi_device *, bool,
                                                  pi_context *) {
  cl::sycl::detail::pi::die(
      "Creation of PI context from native handle not implemented");
  return {};
}

/// Creates a PI Memory object using a CUDA memory allocation.
/// Can trigger a manual copy depending on the mode.
/// \TODO Implement USE_HOST_PTR using cuHostRegister
///


pi_result cnrt_piMemBufferCreate(pi_context context, pi_mem_flags flags,
                                 size_t size, void *host_ptr, pi_mem *ret_mem,
                                 const pi_mem_properties *properties) {
  // Need input memory object
  assert(ret_mem != nullptr);
  assert(properties == nullptr && "no mem properties goes to cuda RT yet");
  // Currently, USE_HOST_PTR is not implemented using host register
  // since this triggers a weird segfault after program ends.
  // Setting this constant to true enables testing that behavior.
  // const bool enableUseHostPtr = false;
  // const bool performInitialCopy =
  //    (flags & PI_MEM_FLAGS_HOST_PTR_COPY) ||
  //    ((flags & PI_MEM_FLAGS_HOST_PTR_USE) && !enableUseHostPtr);
  pi_result retErr = PI_SUCCESS;
  pi_mem retMemObj = nullptr;

  try {
    CNaddr ptr;
    _pi_mem::mem_::buffer_mem_::alloc_mode allocMode =
        _pi_mem::mem_::buffer_mem_::alloc_mode::classic;
    retErr = PI_CHECK_ERROR(cnMalloc(&ptr, size));
    if (flags & PI_MEM_FLAGS_HOST_PTR_COPY) {
      allocMode = _pi_mem::mem_::buffer_mem_::alloc_mode::copy_in;
    }

    if (retErr == PI_SUCCESS) {
      pi_mem parentBuffer = nullptr;

      auto piMemObj = std::unique_ptr<_pi_mem>(
          new _pi_mem{context, parentBuffer, allocMode, ptr, host_ptr, size});
      if (piMemObj != nullptr) {
        retMemObj = piMemObj.release();
      } else {
        retErr = PI_OUT_OF_HOST_MEMORY;
      }
    }
  } catch (pi_result err) {
    retErr = err;
  } catch (...) {
    retErr = PI_OUT_OF_RESOURCES;
  }

  *ret_mem = retMemObj;

  return retErr;
}

/// Decreases the reference count of the Mem object.
/// If this is zero, calls the relevant CUDA Free function
/// \return PI_SUCCESS unless deallocation error
///
pi_result cnrt_piMemRelease(pi_mem memObj) {
  assert((memObj != nullptr) && "PI_INVALID_MEM_OBJECTS");

  pi_result ret = PI_SUCCESS;

  try {

    // Do nothing if there are other references
    if (memObj->decrement_reference_count() > 0) {
      return PI_SUCCESS;
    }

    // make sure memObj is released in case PI_CHECK_ERROR throws
    std::unique_ptr<_pi_mem> uniqueMemObj(memObj);

    if (memObj->is_sub_buffer()) {
      return PI_SUCCESS;
    }

    ScopedContext active(uniqueMemObj->get_context());

    if (memObj->mem_type_ == _pi_mem::mem_type::buffer) {
      switch (uniqueMemObj->mem_.buffer_mem_.allocMode_) {
      case _pi_mem::mem_::buffer_mem_::alloc_mode::copy_in:
      case _pi_mem::mem_::buffer_mem_::alloc_mode::classic:
        ret = PI_CHECK_ERROR(cnFree(uniqueMemObj->mem_.buffer_mem_.ptr_));
        break;
      case _pi_mem::mem_::buffer_mem_::alloc_mode::use_host_ptr:
        cl::sycl::detail::pi::die("This alloc mode not implemented");
        break;
      case _pi_mem::mem_::buffer_mem_::alloc_mode::alloc_host_ptr:
        ret =
            PI_CHECK_ERROR(cnFreeHost(uniqueMemObj->mem_.buffer_mem_.hostPtr_));
      };
    } else if (memObj->mem_type_ == _pi_mem::mem_type::surface) {
      cl::sycl::detail::pi::die("This mem type not implemented");
    }

  } catch (pi_result err) {
    ret = err;
  } catch (...) {
    ret = PI_OUT_OF_RESOURCES;
  }

  if (ret != PI_SUCCESS) {
    // A reported CUDA error is either an implementation or an asynchronous CUDA
    // error for which it is unclear if the function that reported it succeeded
    // or not. Either way, the state of the program is compromised and likely
    // unrecoverable.
    cl::sycl::detail::pi::die(
        "Unrecoverable program state reached in cuda_piMemRelease");
  }

  return PI_SUCCESS;
}

/// Implements a buffer partition in the CUDA backend.
/// A buffer partition (or a sub-buffer, in OpenCL terms) is simply implemented
/// as an offset over an existing CUDA allocation.
///
pi_result cnrt_piMemBufferPartition(pi_mem parent_buffer, pi_mem_flags flags,
                                    pi_buffer_create_type buffer_create_type,
                                    void *buffer_create_info, pi_mem *memObj) {
  assert((parent_buffer != nullptr) && "PI_INVALID_MEM_OBJECT");
  assert(parent_buffer->is_buffer() && "PI_INVALID_MEM_OBJECTS");
  assert(!parent_buffer->is_sub_buffer() && "PI_INVALID_MEM_OBJECT");

  // Default value for flags means PI_MEM_FLAGS_ACCCESS_RW.
  if (flags == 0) {
    flags = PI_MEM_FLAGS_ACCESS_RW;
  }

  assert((flags == PI_MEM_FLAGS_ACCESS_RW) && "PI_INVALID_VALUE");
  assert((buffer_create_type == PI_BUFFER_CREATE_TYPE_REGION) &&
         "PI_INVALID_VALUE");
  assert((buffer_create_info != nullptr) && "PI_INVALID_VALUE");
  assert(memObj != nullptr);

  const auto bufferRegion =
      *reinterpret_cast<pi_buffer_region>(buffer_create_info);
  assert((bufferRegion.size != 0u) && "PI_INVALID_BUFFER_SIZE");

  assert((bufferRegion.origin <= (bufferRegion.origin + bufferRegion.size)) &&
         "Overflow");
  assert(((bufferRegion.origin + bufferRegion.size) <=
          parent_buffer->mem_.buffer_mem_.get_size()) &&
         "PI_INVALID_BUFFER_SIZE");
  // Retained indirectly due to retaining parent buffer below.
  pi_context context = parent_buffer->context_;
  _pi_mem::mem_::buffer_mem_::alloc_mode allocMode =
      _pi_mem::mem_::buffer_mem_::alloc_mode::classic;

  assert(parent_buffer->mem_.buffer_mem_.ptr_ !=
         _pi_mem::mem_::buffer_mem_::native_type{0});
  _pi_mem::mem_::buffer_mem_::native_type ptr =
      parent_buffer->mem_.buffer_mem_.ptr_ + bufferRegion.origin;

  void *hostPtr = nullptr;
  if (parent_buffer->mem_.buffer_mem_.hostPtr_) {
    hostPtr = static_cast<char *>(parent_buffer->mem_.buffer_mem_.hostPtr_) +
              bufferRegion.origin;
  }

  ReleaseGuard<pi_mem> releaseGuard(parent_buffer);

  std::unique_ptr<_pi_mem> retMemObj{nullptr};
  try {
    ScopedContext active(context);

    retMemObj = std::unique_ptr<_pi_mem>{new _pi_mem{
        context, parent_buffer, allocMode, ptr, hostPtr, bufferRegion.size}};
  } catch (pi_result err) {
    *memObj = nullptr;
    return err;
  } catch (...) {
    *memObj = nullptr;
    return PI_OUT_OF_HOST_MEMORY;
  }

  releaseGuard.dismiss();
  *memObj = retMemObj.release();
  return PI_SUCCESS;
}

pi_result cnrt_piMemGetInfo(pi_mem, cl_mem_info, size_t, void *, size_t *) {
  cl::sycl::detail::pi::die("cnrt_piMemGetInfo not implemented");
}

/// Gets the native CUDA handle of a PI mem object
///
/// \param[in] mem The PI mem to get the native CUDA object of.
/// \param[out] nativeHandle Set to the native handle of the PI mem object.
///
/// \return PI_SUCCESS
pi_result cnrt_piextMemGetNativeHandle(pi_mem mem,
                                       pi_native_handle *nativeHandle) {
  *nativeHandle = static_cast<pi_native_handle>(mem->mem_.buffer_mem_.get());
  return PI_SUCCESS;
}

/// Created a PI mem object from a CUDA mem handle.
/// TODO: Implement this.
/// NOTE: The created PI object takes ownership of the native handle.
///
/// \param[in] nativeHandle The native handle to create PI mem object from.
/// \param[out] mem Set to the PI mem object created from native handle.
///
/// \return TBD
pi_result cnrt_piextMemCreateWithNativeHandle(pi_native_handle, pi_mem *) {
  cl::sycl::detail::pi::die(
      "Creation of PI mem from native handle not implemented");
  return {};
}

/// Creates a `pi_queue` object on the CUDA backend.
/// Valid properties
/// * __SYCL_PI_CUDA_USE_DEFAULT_STREAM -> CU_STREAM_DEFAULT
/// * __SYCL_PI_CUDA_SYNC_WITH_DEFAULT -> CU_STREAM_NON_BLOCKING
/// \return Pi queue object mapping to a CUStream
///
pi_result cnrt_piQueueCreate(pi_context context, pi_device device,
                             pi_queue_properties properties, pi_queue *queue) {
  try {
    pi_result err = PI_SUCCESS;

    std::unique_ptr<_pi_queue> queueImpl{nullptr};

    if (context->get_device() != device) {
      *queue = nullptr;
      return PI_INVALID_DEVICE;
    }

    ScopedContext active(context);

    CNqueue cnQueue;
    // unsigned int flags = 0;

    // TODO: some properties

    err = PI_CHECK_ERROR(cnCreateQueue(&cnQueue, 0));
    if (err != PI_SUCCESS) {
      return err;
    }

    queueImpl = std::unique_ptr<_pi_queue>(
        new _pi_queue{cnQueue, context, device, properties});

    *queue = queueImpl.release();

    return PI_SUCCESS;
  } catch (pi_result err) {
    return err;
  } catch (...) {
    return PI_OUT_OF_RESOURCES;
  }
}

pi_result cnrt_piQueueGetInfo(pi_queue command_queue, pi_queue_info param_name,
                              size_t param_value_size, void *param_value,
                              size_t *param_value_size_ret) {
  assert(command_queue != nullptr);

  switch (param_name) {
  case PI_QUEUE_INFO_CONTEXT:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   command_queue->context_);
  case PI_QUEUE_INFO_DEVICE:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   command_queue->device_);
  case PI_QUEUE_INFO_REFERENCE_COUNT:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   command_queue->get_reference_count());
  case PI_QUEUE_INFO_PROPERTIES:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   command_queue->properties_);
  default:
    __SYCL_PI_HANDLE_UNKNOWN_PARAM_NAME(param_name);
  }
  cl::sycl::detail::pi::die("Queue info request not implemented");
  return {};
}

pi_result cnrt_piQueueRetain(pi_queue command_queue) {
  assert(command_queue != nullptr);
  assert(command_queue->get_reference_count() > 0);

  command_queue->increment_reference_count();
  return PI_SUCCESS;
}

pi_result cnrt_piQueueRelease(pi_queue command_queue) {
  assert(command_queue != nullptr);

  if (command_queue->decrement_reference_count() > 0) {
    return PI_SUCCESS;
  }

  try {
    std::unique_ptr<_pi_queue> queueImpl(command_queue);

    ScopedContext active(command_queue->get_context());

    auto queue = queueImpl->queue_;
    PI_CHECK_ERROR(cnQueueSync(queue));
    PI_CHECK_ERROR(cnDestroyQueue(queue));

    return PI_SUCCESS;
  } catch (pi_result err) {
    return err;
  } catch (...) {
    return PI_OUT_OF_RESOURCES;
  }
}

pi_result cnrt_piQueueFinish(pi_queue command_queue) {

  // set default result to a negative result (avoid false-positve tests)
  pi_result result = PI_OUT_OF_HOST_MEMORY;

  try {

    assert(command_queue !=
           nullptr); // need PI_ERROR_INVALID_EXTERNAL_HANDLE error code
    ScopedContext active(command_queue->get_context());
    result = PI_CHECK_ERROR(cnQueueSync(command_queue->queue_));

  } catch (pi_result err) {

    result = err;

  } catch (...) {

    result = PI_OUT_OF_RESOURCES;
  }

  return result;
}

/// Gets the native CUDA handle of a PI queue object
///
/// \param[in] queue The PI queue to get the native CUDA object of.
/// \param[out] nativeHandle Set to the native handle of the PI queue object.
///
/// \return PI_SUCCESS
pi_result cnrt_piextQueueGetNativeHandle(pi_queue queue,
                                         pi_native_handle *nativeHandle) {
  *nativeHandle = reinterpret_cast<pi_native_handle>(queue->get());
  return PI_SUCCESS;
}

/// Created a PI queue object from a CUDA queue handle.
/// TODO: Implement this.
/// NOTE: The created PI object takes ownership of the native handle.
///
/// \param[in] nativeHandle The native handle to create PI queue object from.
/// \param[in] context is the PI context of the queue.
/// \param[out] queue Set to the PI queue object created from native handle.
///
/// \return TBD
pi_result cnrt_piextQueueCreateWithNativeHandle(pi_native_handle, pi_context,
                                                pi_queue *) {
  cl::sycl::detail::pi::die(
      "Creation of PI queue from native handle not implemented");
  return {};
}

pi_result cnrt_piEnqueueMemBufferWrite(pi_queue command_queue, pi_mem buffer,
                                       pi_bool blocking_write, size_t offset,
                                       size_t size, const void *ptr,
                                       pi_uint32 num_events_in_wait_list,
                                       const pi_event *event_wait_list,
                                       pi_event *event) {

  assert(buffer != nullptr);
  assert(command_queue != nullptr);
  pi_result retErr = PI_SUCCESS;
  CNqueue cnQueue = command_queue->get();
  CNaddr devPtr = buffer->mem_.buffer_mem_.get();
  std::unique_ptr<_pi_event> retImplEv{nullptr};

  try {
    ScopedContext active(command_queue->get_context());

    retErr = cnrt_piEnqueueEventsWait(command_queue, num_events_in_wait_list,
                                      event_wait_list, nullptr);

    if (event) {
      retImplEv = std::unique_ptr<_pi_event>(_pi_event::make_native(
          PI_COMMAND_TYPE_MEM_BUFFER_WRITE, command_queue));
      retImplEv->start();
    }

    retErr =
        PI_CHECK_ERROR(cnMemcpyHtoDAsync(devPtr + offset, ptr, size, cnQueue));
    // retErr = PI_CHECK_ERROR(cnMemcpyHtoD(devPtr + offset, ptr, size));

    if (event) {
      retErr = retImplEv->record();
    }

    if (blocking_write) {
      retErr = PI_CHECK_ERROR(cnQueueSync(cnQueue));
    }

    if (event) {
      *event = retImplEv.release();
    }
  } catch (pi_result err) {
    retErr = err;
  }
  return retErr;
}

pi_result cnrt_piEnqueueMemBufferRead(pi_queue command_queue, pi_mem buffer,
                                      pi_bool blocking_read, size_t offset,
                                      size_t size, void *ptr,
                                      pi_uint32 num_events_in_wait_list,
                                      const pi_event *event_wait_list,
                                      pi_event *event) {

  assert(buffer != nullptr);
  assert(command_queue != nullptr);
  pi_result retErr = PI_SUCCESS;
  CNqueue cnQueue = command_queue->get();
  CNaddr devPtr = buffer->mem_.buffer_mem_.get();
  std::unique_ptr<_pi_event> retImplEv{nullptr};

  try {
    ScopedContext active(command_queue->get_context());

    retErr = cnrt_piEnqueueEventsWait(command_queue, num_events_in_wait_list,
                                      event_wait_list, nullptr);

    if (event) {
      retImplEv = std::unique_ptr<_pi_event>(_pi_event::make_native(
          PI_COMMAND_TYPE_MEM_BUFFER_READ, command_queue));
      retImplEv->start();
    }

    retErr =
        PI_CHECK_ERROR(cnMemcpyDtoHAsync(ptr, devPtr + offset, size, cnQueue));

    if (event) {
      retErr = retImplEv->record();
    }

    if (blocking_read) {
      retErr = PI_CHECK_ERROR(cnQueueSync(cnQueue));
    }

    if (event) {
      *event = retImplEv.release();
    }

  } catch (pi_result err) {
    retErr = err;
  }
  return retErr;
}

pi_result cnrt_piEventsWait(pi_uint32 num_events, const pi_event *event_list) {

  try {
    assert(num_events != 0);
    assert(event_list);
    if (num_events == 0) {
      return PI_INVALID_VALUE;
    }

    if (!event_list) {
      return PI_INVALID_EVENT;
    }

    auto context = event_list[0]->get_context();
    ScopedContext active(context);

    auto waitFunc = [context](pi_event event) -> pi_result {
      if (!event) {
        return PI_INVALID_EVENT;
      }

      if (event->get_context() != context) {
        return PI_INVALID_CONTEXT;
      }

      return event->wait();
    };
    return forLatestEvents(event_list, num_events, waitFunc);
  } catch (pi_result err) {
    return err;
  } catch (...) {
    return PI_OUT_OF_RESOURCES;
  }
}

pi_result cnrt_piKernelCreate(pi_program program, const char *kernel_name,
                              pi_kernel *kernel) {
  assert(kernel != nullptr);
  assert(program != nullptr);

  pi_result retErr = PI_SUCCESS;
  std::unique_ptr<_pi_kernel> retKernel{nullptr};

  try {
    ScopedContext active(program->get_context());

    CNkernel cnFunc;
    retErr =
        PI_CHECK_ERROR(cnModuleGetKernel(program->get(), kernel_name, &cnFunc));

    std::string kernel_name_woffset = std::string(kernel_name) + "_with_offset";
    CNkernel cnFuncWithOffsetParam;
    CNresult offsetRes = cnModuleGetKernel(
        program->get(), kernel_name_woffset.c_str(), &cnFuncWithOffsetParam);

    // If there is no kernel with global offset parameter we mark it as missing
    if (offsetRes == CN_ERROR_NOT_FOUND) {
      cnFuncWithOffsetParam = nullptr;
    } else {
      retErr = PI_CHECK_ERROR(offsetRes);
    }

    retKernel = std::unique_ptr<_pi_kernel>(
        new _pi_kernel{cnFunc, cnFuncWithOffsetParam, kernel_name, program,
                       program->get_context()});
  } catch (pi_result err) {
    retErr = err;
  } catch (...) {
    retErr = PI_OUT_OF_HOST_MEMORY;
  }

  *kernel = retKernel.release();
  return retErr;
}

pi_result cnrt_piKernelSetArg(pi_kernel kernel, pi_uint32 arg_index,
                              size_t arg_size, const void *arg_value) {

  assert(kernel != nullptr);
  pi_result retErr = PI_SUCCESS;
  try {
    
    
    int* flag = const_cast<int*>(reinterpret_cast<const int *> (arg_value));
    if (*flag == 101) {
      kernel->set_kernel_local_arg(arg_index, arg_size);
    } else if (*flag == 102) {
      kernel->set_kernel_wram_arg(arg_index, arg_size);
    } else if(arg_value) {
      kernel->set_kernel_arg(arg_index, arg_size, arg_value);
    }
    
    /*
    if (arg_value) {
      kernel->set_kernel_arg(arg_index, arg_size, arg_value);
    } else {
      kernel->set_kernel_local_arg(arg_index, arg_size);
    }
    */
    

  } catch (pi_result err) {
    retErr = err;
  }
  return retErr;
}

pi_result cnrt_piextKernelSetArgMemObj(pi_kernel kernel, pi_uint32 arg_index,
                                       const pi_mem *arg_value) {

  assert(kernel != nullptr);
  assert(arg_value != nullptr);

  pi_result retErr = PI_SUCCESS;
  try {
    pi_mem arg_mem = *arg_value;
    CNaddr cuPtr = arg_mem->mem_.buffer_mem_.get();
    kernel->set_kernel_arg(arg_index, sizeof(CNaddr), (void *)&cuPtr);
  } catch (pi_result err) {
  }
  return retErr;
}

pi_result cnrt_piextKernelSetArgSampler(pi_kernel kernel, pi_uint32 arg_index,
                                        const pi_sampler *arg_value) {

  assert(kernel != nullptr);
  assert(arg_value != nullptr);

  pi_result retErr = PI_SUCCESS;
  try {
    pi_uint32 samplerProps = (*arg_value)->props_;
    kernel->set_kernel_arg(arg_index, sizeof(pi_uint32), (void *)&samplerProps);
  } catch (pi_result err) {
    retErr = err;
  }
  return retErr;
}

pi_result cnrt_piEnqueueKernelLaunch(
    pi_queue command_queue, pi_kernel kernel, pi_uint32 work_dim,
    const size_t *global_work_offset, const size_t *global_work_size,
    const size_t *local_work_size, pi_uint32 num_events_in_wait_list,
    const pi_event *event_wait_list, pi_event *event) {

  // Preconditions
  assert(command_queue != nullptr);
  assert(command_queue->get_context() == kernel->get_context());
  assert(kernel != nullptr);
  assert(global_work_offset != nullptr);
  assert(work_dim > 0);
  assert(work_dim < 4);

  // Set the number of threads per block to the number of threads per warp
  // by default unless user has provided a better number
  // TODO[MLU]: threadsPerBlock in mlu?
  // int threadsPerBlock[3] = {32, 1, 1};
  int threadsPerBlock[3] = {1, 1, 1};
  size_t maxWorkGroupSize = 0u;
  size_t maxThreadsPerBlock[3] = {};
  bool providedLocalWorkGroupSize = (local_work_size != nullptr);

  {
    pi_result retError = cnrt_piDeviceGetInfo(
        command_queue->device_, PI_DEVICE_INFO_MAX_WORK_ITEM_SIZES,
        sizeof(maxThreadsPerBlock), maxThreadsPerBlock, nullptr);
    assert(retError == PI_SUCCESS);
    (void)retError;

    retError = cnrt_piDeviceGetInfo(
        command_queue->device_, PI_DEVICE_INFO_MAX_WORK_GROUP_SIZE,
        sizeof(maxWorkGroupSize), &maxWorkGroupSize, nullptr);
    assert(retError == PI_SUCCESS);

    if (providedLocalWorkGroupSize) {
      auto isValid = [&](int dim) {
        if (local_work_size[dim] > maxThreadsPerBlock[dim])
          return PI_INVALID_WORK_ITEM_SIZE;
        // Checks that local work sizes are a divisor of the global work sizes
        // which includes that the local work sizes are neither larger than the
        // global work sizes and not 0.
        if (0u == local_work_size[dim])
          return PI_INVALID_WORK_GROUP_SIZE;
        if (0u != (global_work_size[dim] % local_work_size[dim]))
          return PI_INVALID_WORK_GROUP_SIZE;
        threadsPerBlock[dim] = static_cast<int>(local_work_size[dim]);
        return PI_SUCCESS;
      };

      for (size_t dim = 0; dim < work_dim; dim++) {
        auto err = isValid(dim);
        if (err != PI_SUCCESS)
          return err;
      }
    } else {
      guessLocalWorkSize(threadsPerBlock, global_work_size, maxThreadsPerBlock,
                         kernel);
    }
  }

  pi_result retError = PI_SUCCESS;
  std::unique_ptr<_pi_event> retImplEv{nullptr};

  try {
    ScopedContext active(command_queue->get_context());
    CNqueue cnQueue = command_queue->get();
    CNkernel cuFunc = kernel->get();

    retError = cnrt_piEnqueueEventsWait(command_queue, num_events_in_wait_list,
                                        event_wait_list, nullptr);

    // Set the implicit global offset parameter if kernel has offset variant
    if (kernel->get_with_offset_parameter()) {
      std::uint32_t cuda_implicit_offset[3] = {0, 0, 0};
      if (global_work_offset) {
        for (size_t i = 0; i < work_dim; i++) {
          cuda_implicit_offset[i] =
              static_cast<std::uint32_t>(global_work_offset[i]);
          if (global_work_offset[i] != 0) {
            cuFunc = kernel->get_with_offset_parameter();
          }
        }
      }
      kernel->set_implicit_offset_arg(sizeof(cuda_implicit_offset),
                                      cuda_implicit_offset);
    }

    // auto argIndices = kernel->get_arg_indices();

    if (event) {
      retImplEv = std::unique_ptr<_pi_event>(_pi_event::make_native(
          PI_COMMAND_TYPE_NDRANGE_KERNEL, command_queue));
      retImplEv->start();
    }

    CNaddr *params = kernel->get_kernel_params();
    void *extra[] = {CN_INVOKE_PARAM_BUFFER_POINTER, (void *)(params),
                     CN_INVOKE_PARAM_BUFFER_SIZE,
                     (void *)(kernel->get_num_args() * sizeof(CNaddr)),
                     CN_INVOKE_PARAM_END};

    auto chooseKernelClass = [](int n) -> KernelClass {
      assert(n > 0);
      // 4 clusters are available
      if (n % 16 == 0)
        return CN_KERNEL_CLASS_UNION4;
      if (n % 8 == 0)
        return CN_KERNEL_CLASS_UNION2;
      if (n % 4 == 0)
        return CN_KERNEL_CLASS_UNION;
      return CN_KERNEL_CLASS_BLOCK;
    };
    KernelClass kc = chooseKernelClass(threadsPerBlock[0]);

    retError = PI_CHECK_ERROR(
        cnInvokeKernel(cuFunc, threadsPerBlock[0], threadsPerBlock[1],
                       threadsPerBlock[2], kc, 0, cnQueue, nullptr, extra));

    kernel->free_kernel_params();
    kernel->clear_local_size();
    if (event) {
      retError = retImplEv->record();
    }

    if (event) {
      *event = retImplEv.release();
    }
  } catch (pi_result err) {
    retError = err;
  }
  return retError;
}

/// \TODO Not implemented
pi_result cnrt_piEnqueueNativeKernel(pi_queue, void (*)(void *), void *, size_t,
                                     pi_uint32, const pi_mem *, const void **,
                                     pi_uint32, const pi_event *, pi_event *) {
  cl::sycl::detail::pi::die("Not implemented in CUDA backend");
  return {};
}

pi_result cnrt_piextKernelCreateWithNativeHandle(pi_native_handle, pi_context,
                                                 bool, pi_kernel *) {
  sycl::detail::pi::die("Unsupported operation");
  return PI_SUCCESS;
}

/// \TODO Not implemented
pi_result cnrt_piMemImageCreate(pi_context context, pi_mem_flags flags,
                                const pi_image_format *image_format,
                                const pi_image_desc *image_desc, void *host_ptr,
                                pi_mem *ret_mem) {
  cl::sycl::detail::pi::die("cnrt_piMemImageCreate not implemented");
  return {};
}

/// \TODO Not implemented
pi_result cnrt_piMemImageGetInfo(pi_mem, pi_image_info, size_t, void *,
                                 size_t *) {
  cl::sycl::detail::pi::die("cuda_piMemImageGetInfo not implemented");
  return {};
}

pi_result cnrt_piMemRetain(pi_mem mem) {
  assert(mem != nullptr);
  assert(mem->get_reference_count() > 0);
  mem->increment_reference_count();
  return PI_SUCCESS;
}

/// Not used as CUDA backend only creates programs from binary.
/// See \ref cuda_piclProgramCreateWithBinary.
///
pi_result cnrt_piclProgramCreateWithSource(pi_context, pi_uint32, const char **,
                                           const size_t *, pi_program *) {
  cl::sycl::detail::pi::cuPrint(
      "cnrt_piclProgramCreateWithSource not implemented");
  return PI_INVALID_OPERATION;
}

/// Loads the images from a PI program into a CUmodule that can be
/// used later on to extract functions (kernels).
/// See \ref _pi_program for implementation details.
///
pi_result cnrt_piProgramBuild(pi_program program, pi_uint32 num_devices,
                              const pi_device *device_list, const char *options,
                              void (*pfn_notify)(pi_program program,
                                                 void *user_data),
                              void *user_data) {

  assert(program != nullptr);
  assert(num_devices == 1 || num_devices == 0);
  assert(device_list != nullptr || num_devices == 0);
  assert(pfn_notify == nullptr);
  assert(user_data == nullptr);
  pi_result retError = PI_SUCCESS;

  try {
    ScopedContext active(program->get_context());

    program->build_program(options);

  } catch (pi_result err) {
    retError = err;
  }
  return retError;
}

/// \TODO Not implemented
pi_result cnrt_piProgramCreate(pi_context, const void *, size_t, pi_program *) {
  cl::sycl::detail::pi::die("cnrt_piProgramCreate not implemented");
  return {};
}

/// Loads images from a list of PTX or CUBIN binaries.
/// Note: No calls to CUDA driver API in this function, only store binaries
/// for later.
///
/// Note: Only supports one device
///
pi_result cnrt_piProgramCreateWithBinary(
    pi_context context, pi_uint32 num_devices, const pi_device *device_list,
    const size_t *lengths, const unsigned char **binaries,
    pi_int32 *binary_status, pi_program *program) {
  // Ignore unused parameter
  (void)binary_status;

  assert(context != nullptr);
  assert(binaries != nullptr);
  assert(program != nullptr);
  assert(device_list != nullptr);
  assert(num_devices == 1 && "CUDA contexts are for a single device");
  assert((context->get_device()->get() == device_list[0]->get()) &&
         "Mismatch between devices context and passed context when creating "
         "program from binary");

  pi_result retError = PI_SUCCESS;

  std::unique_ptr<_pi_program> retProgram{new _pi_program{context}};

  const bool has_length = (lengths != nullptr);
  size_t length = has_length
                      ? lengths[0]
                      : strlen(reinterpret_cast<const char *>(binaries[0])) + 1;

  assert(length != 0);

  retProgram->set_binary(reinterpret_cast<const char *>(binaries[0]), length);

  *program = retProgram.release();

  return retError;
}

pi_result cnrt_piProgramGetInfo(pi_program program, pi_program_info param_name,
                                size_t param_value_size, void *param_value,
                                size_t *param_value_size_ret) {
  assert(program != nullptr);

  switch (param_name) {
  case PI_PROGRAM_INFO_REFERENCE_COUNT:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   program->get_reference_count());
  case PI_PROGRAM_INFO_CONTEXT:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   program->context_);
  case PI_PROGRAM_INFO_NUM_DEVICES:
    return getInfo(param_value_size, param_value, param_value_size_ret, 1u);
  case PI_PROGRAM_INFO_DEVICES:
    return getInfoArray(1, param_value_size, param_value, param_value_size_ret,
                        &program->context_->deviceId_);
  case PI_PROGRAM_INFO_SOURCE:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   program->binary_);
  case PI_PROGRAM_INFO_BINARY_SIZES:
    return getInfoArray(1, param_value_size, param_value, param_value_size_ret,
                        &program->binarySizeInBytes_);
  case PI_PROGRAM_INFO_BINARIES:
    return getInfoArray(1, param_value_size, param_value, param_value_size_ret,
                        &program->binary_);
  case PI_PROGRAM_INFO_KERNEL_NAMES: {
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   getKernelNames(program).c_str());
  }
  default:
    __SYCL_PI_HANDLE_UNKNOWN_PARAM_NAME(param_name);
  }
  cl::sycl::detail::pi::die("Program info request not implemented");
  return {};
}

/// Creates a new PI program object that is the outcome of linking all input
/// programs.
/// \TODO Implement linker options, requires mapping of OpenCL to CUDA
///
pi_result cnrt_piProgramLink(pi_context context, pi_uint32 num_devices,
                             const pi_device *device_list, const char *options,
                             pi_uint32 num_input_programs,
                             const pi_program *input_programs,
                             void (*pfn_notify)(pi_program program,
                                                void *user_data),
                             void *user_data, pi_program *ret_program) {

  cl::sycl::detail::pi::die("cnrt_piProgramLink not implemented");
  return {};
}

/// Creates a new program that is the outcome of the compilation of the headers
///  and the program.
/// \TODO Implement asynchronous compilation
///
pi_result cnrt_piProgramCompile(
    pi_program program, pi_uint32 num_devices, const pi_device *device_list,
    const char *options, pi_uint32 num_input_headers,
    const pi_program *input_headers, const char **header_include_names,
    void (*pfn_notify)(pi_program program, void *user_data), void *user_data) {
  // Ignore unused parameters
  (void)header_include_names;
  (void)input_headers;

  assert(program != nullptr);
  assert(num_devices == 1 || num_devices == 0);
  assert(device_list != nullptr || num_devices == 0);
  assert(pfn_notify == nullptr);
  assert(user_data == nullptr);
  assert(num_input_headers == 0);
  pi_result retError = PI_SUCCESS;

  try {
    ScopedContext active(program->get_context());

    program->build_program(options);

  } catch (pi_result err) {
    retError = err;
  }
  return retError;
}

pi_result cnrt_piProgramGetBuildInfo(pi_program program, pi_device device,
                                     cl_program_build_info param_name,
                                     size_t param_value_size, void *param_value,
                                     size_t *param_value_size_ret) {
  // Ignore unused parameter
  (void)device;

  assert(program != nullptr);

  switch (param_name) {
  case PI_PROGRAM_BUILD_INFO_STATUS: {
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   program->buildStatus_);
  }
  case PI_PROGRAM_BUILD_INFO_OPTIONS:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   program->buildOptions_.c_str());
  case PI_PROGRAM_BUILD_INFO_LOG:
    return getInfoArray(program->MAX_LOG_SIZE, param_value_size, param_value,
                        param_value_size_ret, program->infoLog_);
  default:
    __SYCL_PI_HANDLE_UNKNOWN_PARAM_NAME(param_name);
  }
  cl::sycl::detail::pi::die("Program Build info request not implemented");
  return {};
}

pi_result cnrt_piProgramRetain(pi_program program) {
  assert(program != nullptr);
  assert(program->get_reference_count() > 0);
  program->increment_reference_count();
  return PI_SUCCESS;
}

/// Decreases the reference count of a pi_program object.
/// When the reference count reaches 0, it unloads the module from
/// the context.
pi_result cnrt_piProgramRelease(pi_program program) {
  assert(program != nullptr);

  // double delete or someone is messing with the ref count.
  // either way, cannot safely proceed.
  assert(program->get_reference_count() != 0 &&
         "Reference count overflow detected in cuda_piProgramRelease.");
  
  // decrement ref count. If it is 0, delete the program.
  if (program->decrement_reference_count() == 0) {

    std::unique_ptr<_pi_program> program_ptr{program};

    pi_result result = PI_INVALID_PROGRAM;

    try {
      ScopedContext active(program->get_context());
      auto cuModule = program->get();
      result = PI_CHECK_ERROR(cnModuleUnload(cuModule));
    } catch (...) {
      result = PI_OUT_OF_RESOURCES;
    }

    return result;
  }

  return PI_SUCCESS;
}

/// Gets the native CUDA handle of a PI program object
///
/// \param[in] program The PI program to get the native CUDA object of.
/// \param[out] nativeHandle Set to the native handle of the PI program object.
///
/// \return TBD
pi_result cnrt_piextProgramGetNativeHandle(pi_program program,
                                           pi_native_handle *nativeHandle) {
  *nativeHandle = reinterpret_cast<pi_native_handle>(program->get());
  return PI_SUCCESS;
}

/// Created a PI program object from a CUDA program handle.
/// TODO: Implement this.
/// NOTE: The created PI object takes ownership of the native handle.
///
/// \param[in] nativeHandle The native handle to create PI program object from.
/// \param[in] context The PI context of the program.
/// \param[out] program Set to the PI program object created from native handle.
///
/// \return TBD
pi_result cnrt_piextProgramCreateWithNativeHandle(pi_native_handle, pi_context,
                                                  pi_program *) {
  cl::sycl::detail::pi::die(
      "Creation of PI program from native handle not implemented");
  return {};
}

pi_result cnrt_piKernelGetInfo(pi_kernel kernel, pi_kernel_info param_name,
                               size_t param_value_size, void *param_value,
                               size_t *param_value_size_ret) {

  if (kernel != nullptr) {

    switch (param_name) {
    case PI_KERNEL_INFO_FUNCTION_NAME:
      return getInfo(param_value_size, param_value, param_value_size_ret,
                     kernel->get_name());
    case PI_KERNEL_INFO_NUM_ARGS:
      return getInfo(param_value_size, param_value, param_value_size_ret,
                     kernel->get_num_args());
    case PI_KERNEL_INFO_REFERENCE_COUNT:
      return getInfo(param_value_size, param_value, param_value_size_ret,
                     kernel->get_reference_count());
    case PI_KERNEL_INFO_CONTEXT: {
      return getInfo(param_value_size, param_value, param_value_size_ret,
                     kernel->get_context());
    }
    case PI_KERNEL_INFO_PROGRAM: {
      return getInfo(param_value_size, param_value, param_value_size_ret,
                     kernel->get_program());
    }
    case PI_KERNEL_INFO_ATTRIBUTES: {
      return getInfo(param_value_size, param_value, param_value_size_ret, "");
    }
    default: {
      __SYCL_PI_HANDLE_UNKNOWN_PARAM_NAME(param_name);
    }
    }
  }

  return PI_INVALID_KERNEL;
}

pi_result cnrt_piKernelGetGroupInfo(pi_kernel kernel, pi_device device,
                                    pi_kernel_group_info param_name,
                                    size_t param_value_size, void *param_value,
                                    size_t *param_value_size_ret) {

  cl::sycl::detail::pi::die("cnrt_piKernelGetGroupInfo not implemented");
  return {};
}

pi_result cnrt_piKernelGetSubGroupInfo(
    pi_kernel kernel, pi_device device, pi_kernel_sub_group_info param_name,
    size_t input_value_size, const void *input_value, size_t param_value_size,
    void *param_value, size_t *param_value_size_ret) {

  cl::sycl::detail::pi::die("cnrt_piKernelGetSubGroupInfo not implemented");
  return {};
}

pi_result cnrt_piKernelRetain(pi_kernel kernel) {
  assert(kernel != nullptr);
  assert(kernel->get_reference_count() > 0u);

  kernel->increment_reference_count();
  return PI_SUCCESS;
}

pi_result cnrt_piKernelRelease(pi_kernel kernel) {
  assert(kernel != nullptr);

  // double delete or someone is messing with the ref count.
  // either way, cannot safely proceed.
  assert(kernel->get_reference_count() != 0 &&
         "Reference count overflow detected in cuda_piKernelRelease.");

  // decrement ref count. If it is 0, delete the program.
  if (kernel->decrement_reference_count() == 0) {
    // no internal cuda resources to clean up. Just delete it.
    delete kernel;
    return PI_SUCCESS;
  }

  return PI_SUCCESS;
}

// A NOP for the CNRT backend
pi_result cnrt_piKernelSetExecInfo(pi_kernel, pi_kernel_exec_info, size_t,
                                   const void *) {
  return PI_SUCCESS;
}

pi_result cnrt_piextKernelSetArgPointer(pi_kernel kernel, pi_uint32 arg_index,
                                        size_t arg_size,
                                        const void *arg_value) {
  kernel->set_kernel_arg(arg_index, arg_size, arg_value);
  return PI_SUCCESS;
}

//
// Events
//
pi_result cnrt_piEventCreate(pi_context, pi_event *) {
  cl::sycl::detail::pi::die("PI Event Create not implemented in CUDA backend");
}

pi_result cnrt_piEventGetInfo(pi_event event, pi_event_info param_name,
                              size_t param_value_size, void *param_value,
                              size_t *param_value_size_ret) {
  assert(event != nullptr);

  switch (param_name) {
  case PI_EVENT_INFO_COMMAND_QUEUE:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   event->get_queue());
  case PI_EVENT_INFO_COMMAND_TYPE:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   event->get_command_type());
  case PI_EVENT_INFO_REFERENCE_COUNT:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   event->get_reference_count());
  case PI_EVENT_INFO_COMMAND_EXECUTION_STATUS: {
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   static_cast<pi_event_status>(event->get_execution_status()));
  }
  case PI_EVENT_INFO_CONTEXT:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   event->get_context());
  default:
    __SYCL_PI_HANDLE_UNKNOWN_PARAM_NAME(param_name);
  }

  return PI_INVALID_EVENT;
}

/// Obtain profiling information from PI CUDA events
/// \TODO Untie from OpenCL, timings from CUDA are only elapsed time.
pi_result cnrt_piEventGetProfilingInfo(pi_event event,
                                       pi_profiling_info param_name,
                                       size_t param_value_size,
                                       void *param_value,
                                       size_t *param_value_size_ret) {

  assert(event != nullptr);

  pi_queue queue = event->get_queue();
  if (queue == nullptr || !(queue->properties_ & PI_QUEUE_PROFILING_ENABLE)) {
    return PI_PROFILING_INFO_NOT_AVAILABLE;
  }

  switch (param_name) {
  case PI_PROFILING_INFO_COMMAND_QUEUED:
  case PI_PROFILING_INFO_COMMAND_SUBMIT:
    return getInfo<pi_uint64>(param_value_size, param_value,
                              param_value_size_ret, event->get_queued_time());
  case PI_PROFILING_INFO_COMMAND_START:
    return getInfo<pi_uint64>(param_value_size, param_value,
                              param_value_size_ret, event->get_start_time());
  case PI_PROFILING_INFO_COMMAND_END:
    return getInfo<pi_uint64>(param_value_size, param_value,
                              param_value_size_ret, event->get_end_time());
  default:
    __SYCL_PI_HANDLE_UNKNOWN_PARAM_NAME(param_name);
  }
  cl::sycl::detail::pi::die("Event Profiling info request not implemented");
  return {};
}

pi_result cnrt_piEventSetCallback(pi_event, pi_int32, pfn_notify, void *) {
  cl::sycl::detail::pi::die("Event Callback not implemented in CUDA backend");
  return PI_SUCCESS;
}

pi_result cnrt_piEventSetStatus(pi_event, pi_int32) {
  cl::sycl::detail::pi::die("Event Set Status not implemented in CUDA backend");
  return PI_INVALID_VALUE;
}

pi_result cnrt_piEventRetain(pi_event event) {
  assert(event != nullptr);

  const auto refCount = event->increment_reference_count();

  cl::sycl::detail::pi::assertion(
      refCount != 0,
      "Reference count overflow detected in cuda_piEventRetain.");

  return PI_SUCCESS;
}

pi_result cnrt_piEventRelease(pi_event event) {
  assert(event != nullptr);

  // double delete or someone is messing with the ref count.
  // either way, cannot safely proceed.
  cl::sycl::detail::pi::assertion(
      event->get_reference_count() != 0,
      "Reference count overflow detected in cuda_piEventRelease.");

  // decrement ref count. If it is 0, delete the event.
  if (event->decrement_reference_count() == 0) {
    std::unique_ptr<_pi_event> event_ptr{event};
    pi_result result = PI_INVALID_EVENT;
    try {
      ScopedContext active(event->get_context());
      //result = event->release();
      result = PI_SUCCESS;
    } catch (...) {
      result = PI_OUT_OF_RESOURCES;
    }
    return result;
  }

  return PI_SUCCESS;
}

/// Enqueues a wait on the given CUstream for all events.
/// See \ref enqueueEventWait
/// TODO: Add support for multiple streams once the Event class is properly
/// refactored.
///
pi_result cnrt_piEnqueueEventsWait(pi_queue command_queue,
                                   pi_uint32 num_events_in_wait_list,
                                   const pi_event *event_wait_list,
                                   pi_event *event) {
  return cnrt_piEnqueueEventsWaitWithBarrier(
      command_queue, num_events_in_wait_list, event_wait_list, event);
}

/// Enqueues a wait on the given CUstream for all specified events (See
/// \ref enqueueEventWaitWithBarrier.) If the events list is empty, the enqueued
/// wait will wait on all previous events in the queue.
///
/// \param[in] command_queue A valid PI queue.
/// \param[in] num_events_in_wait_list Number of events in event_wait_list.
/// \param[in] event_wait_list Events to wait on.
/// \param[out] event Event for when all events in event_wait_list have finished
/// or, if event_wait_list is empty, when all previous events in the queue have
/// finished.
///
/// \return TBD
pi_result cnrt_piEnqueueEventsWaitWithBarrier(pi_queue command_queue,
                                              pi_uint32 num_events_in_wait_list,
                                              const pi_event *event_wait_list,
                                              pi_event *event) {
  if (!command_queue) {
    return PI_INVALID_QUEUE;
  }

  try {
    ScopedContext active(command_queue->get_context());

    if (event_wait_list) {
      auto result =
          forLatestEvents(event_wait_list, num_events_in_wait_list,
                          [command_queue](pi_event event) -> pi_result {
                            return enqueueEventWait(command_queue, event);
                          });

      if (result != PI_SUCCESS) {
        return result;
      }
    }

    if (event) {
      *event = _pi_event::make_native(PI_COMMAND_TYPE_MARKER, command_queue);
      (*event)->start();
      (*event)->record();
    }

    return PI_SUCCESS;
  } catch (pi_result err) {
    return err;
  } catch (...) {
    return PI_ERROR_UNKNOWN;
  }
}

/// Gets the native CUDA handle of a PI event object
///
/// \param[in] event The PI event to get the native CUDA object of.
/// \param[out] nativeHandle Set to the native handle of the PI event object.
///
/// \return PI_SUCCESS on success. PI_INVALID_EVENT if given a user event.
pi_result cnrt_piextEventGetNativeHandle(pi_event event,
                                         pi_native_handle *nativeHandle) {
  *nativeHandle = reinterpret_cast<pi_native_handle>(event->get());
  return PI_SUCCESS;
}

/// Created a PI event object from a CUDA event handle.
/// TODO: Implement this.
/// NOTE: The created PI object takes ownership of the native handle.
///
/// \param[in] nativeHandle The native handle to create PI event object from.
/// \param[out] event Set to the PI event object created from native handle.
///
/// \return TBD
pi_result cnrt_piextEventCreateWithNativeHandle(pi_native_handle, pi_event *) {
  cl::sycl::detail::pi::die(
      "Creation of PI event from native handle not implemented");
  return {};
}

/// Creates a PI sampler object
///
/// \param[in] context The context the sampler is created for.
/// \param[in] sampler_properties The properties for the sampler.
/// \param[out] result_sampler Set to the resulting sampler object.
///
/// \return PI_SUCCESS on success. PI_INVALID_VALUE if given an invalid property
///         or if there is multiple of properties from the same category.
pi_result cnrt_piSamplerCreate(pi_context context,
                               const pi_sampler_properties *sampler_properties,
                               pi_sampler *result_sampler) {
  std::unique_ptr<_pi_sampler> retImplSampl{new _pi_sampler(context)};

  bool propSeen[3] = {false, false, false};
  for (size_t i = 0; sampler_properties[i] != 0; i += 2) {
    switch (sampler_properties[i]) {
    case PI_SAMPLER_PROPERTIES_NORMALIZED_COORDS:
      if (propSeen[0]) {
        return PI_INVALID_VALUE;
      }
      propSeen[0] = true;
      retImplSampl->props_ |= sampler_properties[i + 1];
      break;
    case PI_SAMPLER_PROPERTIES_FILTER_MODE:
      if (propSeen[1]) {
        return PI_INVALID_VALUE;
      }
      propSeen[1] = true;
      retImplSampl->props_ |=
          (sampler_properties[i + 1] - PI_SAMPLER_FILTER_MODE_NEAREST) << 1;
      break;
    case PI_SAMPLER_PROPERTIES_ADDRESSING_MODE:
      if (propSeen[2]) {
        return PI_INVALID_VALUE;
      }
      propSeen[2] = true;
      retImplSampl->props_ |=
          (sampler_properties[i + 1] - PI_SAMPLER_ADDRESSING_MODE_NONE) << 2;
      break;
    default:
      return PI_INVALID_VALUE;
    }
  }

  if (!propSeen[0]) {
    retImplSampl->props_ |= CL_TRUE;
  }
  // Default filter mode to CL_FILTER_NEAREST
  if (!propSeen[2]) {
    retImplSampl->props_ |= (CL_ADDRESS_CLAMP % CL_ADDRESS_NONE) << 2;
  }

  *result_sampler = retImplSampl.release();
  return PI_SUCCESS;
}

/// Gets information from a PI sampler object
///
/// \param[in] sampler The sampler to get the information from.
/// \param[in] param_name The name of the information to get.
/// \param[in] param_value_size The size of the param_value.
/// \param[out] param_value Set to information value.
/// \param[out] param_value_size_ret Set to the size of the information value.
///
/// \return PI_SUCCESS on success.
pi_result cnrt_piSamplerGetInfo(pi_sampler sampler, cl_sampler_info param_name,
                                size_t param_value_size, void *param_value,
                                size_t *param_value_size_ret) {
  assert(sampler != nullptr);

  switch (param_name) {
  case PI_SAMPLER_INFO_REFERENCE_COUNT:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   sampler->get_reference_count());
  case PI_SAMPLER_INFO_CONTEXT:
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   sampler->context_);
  case PI_SAMPLER_INFO_NORMALIZED_COORDS: {
    pi_bool norm_coords_prop = static_cast<pi_bool>(sampler->props_ & 0x1);
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   norm_coords_prop);
  }
  case PI_SAMPLER_INFO_FILTER_MODE: {
    pi_sampler_filter_mode filter_prop = static_cast<pi_sampler_filter_mode>(
        ((sampler->props_ >> 1) & 0x1) + PI_SAMPLER_FILTER_MODE_NEAREST);
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   filter_prop);
  }
  case PI_SAMPLER_INFO_ADDRESSING_MODE: {
    pi_sampler_addressing_mode addressing_prop =
        static_cast<pi_sampler_addressing_mode>(
            (sampler->props_ >> 2) + PI_SAMPLER_ADDRESSING_MODE_NONE);
    return getInfo(param_value_size, param_value, param_value_size_ret,
                   addressing_prop);
  }
  default:
    __SYCL_PI_HANDLE_UNKNOWN_PARAM_NAME(param_name);
  }
  return {};
}

/// Retains a PI sampler object, incrementing its reference count.
///
/// \param[in] sampler The sampler to increment the reference count of.
///
/// \return PI_SUCCESS.
pi_result cnrt_piSamplerRetain(pi_sampler sampler) {
  assert(sampler != nullptr);
  sampler->increment_reference_count();
  return PI_SUCCESS;
}

/// Releases a PI sampler object, decrementing its reference count. If the
/// reference count reaches zero, the sampler object is destroyed.
///
/// \param[in] sampler The sampler to decrement the reference count of.
///
/// \return PI_SUCCESS.
pi_result cnrt_piSamplerRelease(pi_sampler sampler) {
  assert(sampler != nullptr);

  // double delete or someone is messing with the ref count.
  // either way, cannot safely proceed.
  cl::sycl::detail::pi::assertion(
      sampler->get_reference_count() != 0,
      "Reference count overflow detected in cuda_piSamplerRelease.");

  // decrement ref count. If it is 0, delete the sampler.
  if (sampler->decrement_reference_count() == 0) {
    delete sampler;
  }

  return PI_SUCCESS;
}

pi_result cnrt_piEnqueueMemBufferReadRect(
    pi_queue command_queue, pi_mem buffer, pi_bool blocking_read,
    pi_buff_rect_offset buffer_offset, pi_buff_rect_offset host_offset,
    pi_buff_rect_region region, size_t buffer_row_pitch,
    size_t buffer_slice_pitch, size_t host_row_pitch, size_t host_slice_pitch,
    void *ptr, pi_uint32 num_events_in_wait_list,
    const pi_event *event_wait_list, pi_event *event) {

  cl::sycl::detail::pi::die("cnrt_piEnqueueMemBufferReadRect not implemented");
  return {};
}

pi_result cnrt_piEnqueueMemBufferWriteRect(
    pi_queue command_queue, pi_mem buffer, pi_bool blocking_write,
    pi_buff_rect_offset buffer_offset, pi_buff_rect_offset host_offset,
    pi_buff_rect_region region, size_t buffer_row_pitch,
    size_t buffer_slice_pitch, size_t host_row_pitch, size_t host_slice_pitch,
    const void *ptr, pi_uint32 num_events_in_wait_list,
    const pi_event *event_wait_list, pi_event *event) {

  cl::sycl::detail::pi::die("cnrt_piEnqueueMemBufferWriteRect not implemented");
  return {};
}

pi_result cnrt_piEnqueueMemBufferCopy(pi_queue command_queue, pi_mem src_buffer,
                                      pi_mem dst_buffer, size_t src_offset,
                                      size_t dst_offset, size_t size,
                                      pi_uint32 num_events_in_wait_list,
                                      const pi_event *event_wait_list,
                                      pi_event *event) {
  if (!command_queue) {
    return PI_INVALID_QUEUE;
  }

  std::unique_ptr<_pi_event> retImplEv{nullptr};

  try {
    ScopedContext active(command_queue->get_context());

    if (event_wait_list) {
      cnrt_piEnqueueEventsWait(command_queue, num_events_in_wait_list,
                               event_wait_list, nullptr);
    }

    pi_result result;

    if (event) {
      retImplEv = std::unique_ptr<_pi_event>(_pi_event::make_native(
          PI_COMMAND_TYPE_MEM_BUFFER_COPY, command_queue));
      result = retImplEv->start();
    }

    auto stream = command_queue->get();
    auto src = src_buffer->mem_.buffer_mem_.get() + src_offset;
    auto dst = dst_buffer->mem_.buffer_mem_.get() + dst_offset;

    result = PI_CHECK_ERROR(cnMemcpyDtoDAsync(dst, src, size, stream));

    if (event) {
      result = retImplEv->record();
      *event = retImplEv.release();
    }

    return result;
  } catch (pi_result err) {
    return err;
  } catch (...) {
    return PI_ERROR_UNKNOWN;
  }
}

pi_result cnrt_piEnqueueMemBufferCopyRect(
    pi_queue command_queue, pi_mem src_buffer, pi_mem dst_buffer,
    pi_buff_rect_offset src_origin, pi_buff_rect_offset dst_origin,
    pi_buff_rect_region region, size_t src_row_pitch, size_t src_slice_pitch,
    size_t dst_row_pitch, size_t dst_slice_pitch,
    pi_uint32 num_events_in_wait_list, const pi_event *event_wait_list,
    pi_event *event) {

  cl::sycl::detail::pi::die("cnrt_piEnqueueMemBufferCopyRect not implemented");
  return {};
}

pi_result cnrt_piEnqueueMemBufferFill(pi_queue command_queue, pi_mem buffer,
                                      const void *pattern, size_t pattern_size,
                                      size_t offset, size_t size,
                                      pi_uint32 num_events_in_wait_list,
                                      const pi_event *event_wait_list,
                                      pi_event *event) {

  cl::sycl::detail::pi::die("cnrt_piEnqueueMemBufferFill not implemented");
  return {};
}

pi_result cnrt_piEnqueueMemImageRead(
    pi_queue command_queue, pi_mem image, pi_bool blocking_read,
    const size_t *origin, const size_t *region, size_t row_pitch,
    size_t slice_pitch, void *ptr, pi_uint32 num_events_in_wait_list,
    const pi_event *event_wait_list, pi_event *event) {

  cl::sycl::detail::pi::die("cnrt_piEnqueueMemImageRead not implemented");
  return {};
}

pi_result
cnrt_piEnqueueMemImageWrite(pi_queue command_queue, pi_mem image,
                            pi_bool blocking_write, const size_t *origin,
                            const size_t *region, size_t input_row_pitch,
                            size_t input_slice_pitch, const void *ptr,
                            pi_uint32 num_events_in_wait_list,
                            const pi_event *event_wait_list, pi_event *event) {

  cl::sycl::detail::pi::die("cnrt_piEnqueueMemImageWrite not implemented");
  return {};
}

pi_result cnrt_piEnqueueMemImageCopy(pi_queue command_queue, pi_mem src_image,
                                     pi_mem dst_image, const size_t *src_origin,
                                     const size_t *dst_origin,
                                     const size_t *region,
                                     pi_uint32 num_events_in_wait_list,
                                     const pi_event *event_wait_list,
                                     pi_event *event) {

  cl::sycl::detail::pi::die("cnrt_piEnqueueMemImageCopy not implemented");
  return {};
}

/// \TODO Not implemented in CUDA, requires untie from OpenCL
pi_result cnrt_piEnqueueMemImageFill(pi_queue, pi_mem, const void *,
                                     const size_t *, const size_t *, pi_uint32,
                                     const pi_event *, pi_event *) {
  cl::sycl::detail::pi::die("cnrt_piEnqueueMemImageFill not implemented");
  return {};
}

/// Implements mapping on the host using a BufferRead operation.
/// Mapped pointers are stored in the pi_mem object.
/// If the buffer uses pinned host memory a pointer to that memory is returned
/// and no read operation is done.
/// \TODO Untie types from OpenCL
///
pi_result cnrt_piEnqueueMemBufferMap(pi_queue command_queue, pi_mem buffer,
                                     pi_bool blocking_map,
                                     pi_map_flags map_flags, size_t offset,
                                     size_t size,
                                     pi_uint32 num_events_in_wait_list,
                                     const pi_event *event_wait_list,
                                     pi_event *event, void **ret_map) {
  assert(ret_map != nullptr);
  assert(command_queue != nullptr);
  assert(buffer != nullptr);
  assert(buffer->mem_type_ == _pi_mem::mem_type::buffer);

  pi_result ret_err = PI_INVALID_OPERATION;
  const bool is_pinned = buffer->mem_.buffer_mem_.allocMode_ ==
                         _pi_mem::mem_::buffer_mem_::alloc_mode::alloc_host_ptr;

  // Currently no support for overlapping regions
  if (buffer->mem_.buffer_mem_.get_map_ptr() != nullptr) {
    return ret_err;
  }

  // Allocate a pointer in the host to store the mapped information
  auto hostPtr = buffer->mem_.buffer_mem_.map_to_ptr(offset, map_flags);
  *ret_map = buffer->mem_.buffer_mem_.get_map_ptr();
  if (hostPtr) {
    ret_err = PI_SUCCESS;
  }

  if (!is_pinned && ((map_flags & PI_MAP_READ) || (map_flags & PI_MAP_WRITE))) {
    // Pinned host memory is already on host so it doesn't need to be read.
    ret_err = cnrt_piEnqueueMemBufferRead(
        command_queue, buffer, blocking_map, offset, size, hostPtr,
        num_events_in_wait_list, event_wait_list, event);
  } else {
    ScopedContext active(command_queue->get_context());

    if (is_pinned) {
      ret_err = cnrt_piEnqueueEventsWait(command_queue, num_events_in_wait_list,
                                         event_wait_list, nullptr);
    }

    if (event) {
      try {
        *event = _pi_event::make_native(PI_COMMAND_TYPE_MEM_BUFFER_MAP,
                                        command_queue);
        (*event)->start();
        (*event)->record();
      } catch (pi_result error) {
        ret_err = error;
      }
    }
  }

  return ret_err;
}

/// Implements the unmap from the host, using a BufferWrite operation.
/// Requires the mapped pointer to be already registered in the given memobj.
/// If memobj uses pinned host memory, this will not do a write.
///
pi_result cnrt_piEnqueueMemUnmap(pi_queue command_queue, pi_mem memobj,
                                 void *mapped_ptr,
                                 pi_uint32 num_events_in_wait_list,
                                 const pi_event *event_wait_list,
                                 pi_event *event) {
  pi_result ret_err = PI_SUCCESS;

  assert(command_queue != nullptr);
  assert(mapped_ptr != nullptr);
  assert(memobj != nullptr);
  assert(memobj->mem_type_ == _pi_mem::mem_type::buffer);
  assert(memobj->mem_.buffer_mem_.get_map_ptr() != nullptr);
  assert(memobj->mem_.buffer_mem_.get_map_ptr() == mapped_ptr);

  const bool is_pinned = memobj->mem_.buffer_mem_.allocMode_ ==
                         _pi_mem::mem_::buffer_mem_::alloc_mode::alloc_host_ptr;

  if (!is_pinned &&
      ((memobj->mem_.buffer_mem_.get_map_flags() & PI_MAP_WRITE) ||
       (memobj->mem_.buffer_mem_.get_map_flags() &
        PI_MAP_WRITE_INVALIDATE_REGION))) {
    // Pinned host memory is only on host so it doesn't need to be written to.
    ret_err = cnrt_piEnqueueMemBufferWrite(
        command_queue, memobj, true,
        memobj->mem_.buffer_mem_.get_map_offset(mapped_ptr),
        memobj->mem_.buffer_mem_.get_size(), mapped_ptr,
        num_events_in_wait_list, event_wait_list, event);
  } else {
    ScopedContext active(command_queue->get_context());

    if (is_pinned) {
      ret_err = cnrt_piEnqueueEventsWait(command_queue, num_events_in_wait_list,
                                         event_wait_list, nullptr);
    }

    if (event) {
      try {
        *event = _pi_event::make_native(PI_COMMAND_TYPE_MEM_BUFFER_UNMAP,
                                        command_queue);
        (*event)->start();
        (*event)->record();
      } catch (pi_result error) {
        ret_err = error;
      }
    }
  }

  memobj->mem_.buffer_mem_.unmap(mapped_ptr);
  return ret_err;
}

/// USM: Implements USM Host allocations using CUDA Pinned Memory
///
pi_result cnrt_piextUSMHostAlloc(void **result_ptr, pi_context context,
                                 pi_usm_mem_properties *properties, size_t size,
                                 pi_uint32 alignment) {
  assert(result_ptr != nullptr);
  assert(context != nullptr);
  assert(properties == nullptr);
  pi_result result = PI_SUCCESS;
  try {
    ScopedContext active(context);
    result = PI_CHECK_ERROR(cnMallocHost(result_ptr, size));
  } catch (pi_result error) {
    result = error;
  }

  assert(alignment == 0 ||
         (result == PI_SUCCESS &&
          reinterpret_cast<std::uintptr_t>(*result_ptr) % alignment == 0));
  return result;
}

/// USM: Implements USM device allocations using a normal CUDA device pointer
///
pi_result cnrt_piextUSMDeviceAlloc(void **result_ptr, pi_context context,
                                   pi_device device,
                                   pi_usm_mem_properties *properties,
                                   size_t size, pi_uint32 alignment) {
  assert(result_ptr != nullptr);
  assert(context != nullptr);
  assert(device != nullptr);
  assert(properties == nullptr);
  pi_result result = PI_SUCCESS;
  try {
    ScopedContext active(context);
    result = PI_CHECK_ERROR(cnMalloc((CNaddr *)result_ptr, size));
  } catch (pi_result error) {
    result = error;
  }

  assert(alignment == 0 ||
         (result == PI_SUCCESS &&
          reinterpret_cast<std::uintptr_t>(*result_ptr) % alignment == 0));
  return result;
}

/// USM: Implements USM Shared allocations using CUDA Managed Memory
///
pi_result cnrt_piextUSMSharedAlloc(void **result_ptr, pi_context context,
                                   pi_device device,
                                   pi_usm_mem_properties *properties,
                                   size_t size, pi_uint32 alignment) {

  cl::sycl::detail::pi::die("cnrt_piextUSMSharedAlloc not implemented");
  return {};
}

/// USM: Frees the given USM pointer associated with the context.
///
pi_result cnrt_piextUSMFree(pi_context context, void *ptr) {
  cl::sycl::detail::pi::die("cnrt_piextUSMFree not implemented");
  return {};
}

pi_result cnrt_piextUSMEnqueueMemset(pi_queue queue, void *ptr, pi_int32 value,
                                     size_t count,
                                     pi_uint32 num_events_in_waitlist,
                                     const pi_event *events_waitlist,
                                     pi_event *event) {
  assert(queue != nullptr);
  assert(ptr != nullptr);
  CNqueue cnQueue = queue->get();
  pi_result result = PI_SUCCESS;
  std::unique_ptr<_pi_event> event_ptr{nullptr};

  try {
    ScopedContext active(queue->get_context());
    result = cnrt_piEnqueueEventsWait(queue, num_events_in_waitlist,
                                      events_waitlist, nullptr);
    if (event) {
      event_ptr = std::unique_ptr<_pi_event>(
          _pi_event::make_native(PI_COMMAND_TYPE_MEM_BUFFER_FILL, queue));
      event_ptr->start();
    }
    result = PI_CHECK_ERROR(cnMemsetD8Async(
        (CNaddr)ptr, (unsigned char)value & 0xFF, count, cnQueue));
    if (event) {
      result = event_ptr->record();
      *event = event_ptr.release();
    }
  } catch (pi_result err) {
    result = err;
  }
  return result;
}

pi_result cnrt_piextUSMEnqueueMemcpy(pi_queue queue, pi_bool blocking,
                                     void *dst_ptr, const void *src_ptr,
                                     size_t size,
                                     pi_uint32 num_events_in_waitlist,
                                     const pi_event *events_waitlist,
                                     pi_event *event) {
  assert(queue != nullptr);
  assert(dst_ptr != nullptr);
  assert(src_ptr != nullptr);
  CNqueue cnQueue = queue->get();
  pi_result result = PI_SUCCESS;
  std::unique_ptr<_pi_event> event_ptr{nullptr};

  try {
    ScopedContext active(queue->get_context());
    result = cnrt_piEnqueueEventsWait(queue, num_events_in_waitlist,
                                      events_waitlist, nullptr);
    if (event) {
      event_ptr = std::unique_ptr<_pi_event>(
          _pi_event::make_native(PI_COMMAND_TYPE_MEM_BUFFER_COPY, queue));
      event_ptr->start();
    }
    result = PI_CHECK_ERROR(
        cnMemcpyAsync((CNaddr)dst_ptr, (CNaddr)src_ptr, size, cnQueue));
    if (event) {
      result = event_ptr->record();
    }
    if (blocking) {
      result = PI_CHECK_ERROR(cnQueueSync(cnQueue));
    }
    if (event) {
      *event = event_ptr.release();
    }
  } catch (pi_result err) {
    result = err;
  }
  return result;
}

pi_result cnrt_piextUSMEnqueuePrefetch(pi_queue queue, const void *ptr,
                                       size_t size,
                                       pi_usm_migration_flags flags,
                                       pi_uint32 num_events_in_waitlist,
                                       const pi_event *events_waitlist,
                                       pi_event *event) {

  cl::sycl::detail::pi::die("cnrt_piextUSMEnqueuePrefetch not implemented");
  return {};
}

/// USM: memadvise API to govern behavior of automatic migration mechanisms
pi_result cnrt_piextUSMEnqueueMemAdvise(pi_queue queue, const void *ptr,
                                        size_t length, pi_mem_advice advice,
                                        pi_event *event) {

  cl::sycl::detail::pi::die("cnrt_piextUSMEnqueueMemAdvise not implemented");
  return {};
}

/// API to query information about USM allocated pointers
/// Valid Queries:
///   PI_MEM_ALLOC_TYPE returns host/device/shared pi_host_usm value
///   PI_MEM_ALLOC_BASE_PTR returns the base ptr of an allocation if
///                         the queried pointer fell inside an allocation.
///                         Result must fit in void *
///   PI_MEM_ALLOC_SIZE returns how big the queried pointer's
///                     allocation is in bytes. Result is a size_t.
///   PI_MEM_ALLOC_DEVICE returns the pi_device this was allocated against
///
/// \param context is the pi_context
/// \param ptr is the pointer to query
/// \param param_name is the type of query to perform
/// \param param_value_size is the size of the result in bytes
/// \param param_value is the result
/// \param param_value_ret is how many bytes were written
pi_result cnrt_piextUSMGetMemAllocInfo(pi_context context, const void *ptr,
                                       pi_mem_info param_name,
                                       size_t param_value_size,
                                       void *param_value,
                                       size_t *param_value_size_ret) {

  cl::sycl::detail::pi::die("cnrt_piextUSMGetMemAllocInfo not implemented");
  return {};
}

// This API is called by Sycl RT to notify the end of the plugin lifetime.
// TODO: add a global variable lifetime management code here (see
// pi_level_zero.cpp for reference) Currently this is just a NOOP.
pi_result cnrt_piTearDown(void *) { return PI_SUCCESS; }

const char SupportedVersion[] = _PI_H_VERSION_STRING;

pi_result piPluginInit(pi_plugin *PluginInit) {
  int CompareVersions = strcmp(PluginInit->PiVersion, SupportedVersion);
  if (CompareVersions < 0) {
    // PI interface supports lower version of PI.
    // TODO: Take appropriate actions.
    return PI_INVALID_OPERATION;
  }

  // PI interface supports higher version or the same version.
  strncpy(PluginInit->PluginVersion, SupportedVersion, 4);

  // Set whole function table to zero to make it easier to detect if
  // functions are not set up below.
  std::memset(&(PluginInit->PiFunctionTable), 0,
              sizeof(PluginInit->PiFunctionTable));

// Forward calls to CNRT.
#define _PI_CL(pi_api, cnrt_api)                                               \
  (PluginInit->PiFunctionTable).pi_api = (decltype(&::pi_api))(&cnrt_api);

  // Platform
  _PI_CL(piPlatformsGet, cnrt_piPlatformsGet)
  _PI_CL(piPlatformGetInfo, cnrt_piPlatformGetInfo)
  // Device
  _PI_CL(piDevicesGet, cnrt_piDevicesGet)
  _PI_CL(piDeviceGetInfo, cnrt_piDeviceGetInfo)
  _PI_CL(piDevicePartition, cnrt_piDevicePartition)
  _PI_CL(piDeviceRetain, cnrt_piDeviceRetain)
  _PI_CL(piDeviceRelease, cnrt_piDeviceRelease)
  _PI_CL(piextDeviceSelectBinary, cnrt_piextDeviceSelectBinary)
  _PI_CL(piextGetDeviceFunctionPointer, cnrt_piextGetDeviceFunctionPointer)
  _PI_CL(piextDeviceGetNativeHandle, cnrt_piextDeviceGetNativeHandle)
  _PI_CL(piextDeviceCreateWithNativeHandle,
         cnrt_piextDeviceCreateWithNativeHandle)
  // Context
  _PI_CL(piextContextSetExtendedDeleter, cnrt_piextContextSetExtendedDeleter)
  _PI_CL(piContextCreate, cnrt_piContextCreate)
  _PI_CL(piContextGetInfo, cnrt_piContextGetInfo)
  _PI_CL(piContextRetain, cnrt_piContextRetain)
  _PI_CL(piContextRelease, cnrt_piContextRelease)
  _PI_CL(piextContextGetNativeHandle, cnrt_piextContextGetNativeHandle)
  _PI_CL(piextContextCreateWithNativeHandle,
         cnrt_piextContextCreateWithNativeHandle)
  // Queue
  _PI_CL(piQueueCreate, cnrt_piQueueCreate)
  _PI_CL(piQueueGetInfo, cnrt_piQueueGetInfo)
  _PI_CL(piQueueFinish, cnrt_piQueueFinish)
  _PI_CL(piQueueRetain, cnrt_piQueueRetain)
  _PI_CL(piQueueRelease, cnrt_piQueueRelease)
  _PI_CL(piextQueueGetNativeHandle, cnrt_piextQueueGetNativeHandle)
  _PI_CL(piextQueueCreateWithNativeHandle,
         cnrt_piextQueueCreateWithNativeHandle)
  // Memory
  _PI_CL(piMemBufferCreate, cnrt_piMemBufferCreate)
  _PI_CL(piMemImageCreate, cnrt_piMemImageCreate)
  _PI_CL(piMemGetInfo, cnrt_piMemGetInfo)
  _PI_CL(piMemImageGetInfo, cnrt_piMemImageGetInfo)
  _PI_CL(piMemRetain, cnrt_piMemRetain)
  _PI_CL(piMemRelease, cnrt_piMemRelease)
  _PI_CL(piMemBufferPartition, cnrt_piMemBufferPartition)
  _PI_CL(piextMemGetNativeHandle, cnrt_piextMemGetNativeHandle)
  _PI_CL(piextMemCreateWithNativeHandle, cnrt_piextMemCreateWithNativeHandle)
  // Program
  _PI_CL(piProgramCreate, cnrt_piProgramCreate)
  _PI_CL(piclProgramCreateWithSource, cnrt_piclProgramCreateWithSource)
  _PI_CL(piProgramCreateWithBinary, cnrt_piProgramCreateWithBinary)
  _PI_CL(piProgramGetInfo, cnrt_piProgramGetInfo)
  _PI_CL(piProgramCompile, cnrt_piProgramCompile)
  _PI_CL(piProgramBuild, cnrt_piProgramBuild)
  _PI_CL(piProgramLink, cnrt_piProgramLink)
  _PI_CL(piProgramGetBuildInfo, cnrt_piProgramGetBuildInfo)
  _PI_CL(piProgramRetain, cnrt_piProgramRetain)
  _PI_CL(piProgramRelease, cnrt_piProgramRelease)
  _PI_CL(piextProgramGetNativeHandle, cnrt_piextProgramGetNativeHandle)
  _PI_CL(piextProgramCreateWithNativeHandle,
         cnrt_piextProgramCreateWithNativeHandle)
  // Kernel
  _PI_CL(piKernelCreate, cnrt_piKernelCreate)
  _PI_CL(piKernelSetArg, cnrt_piKernelSetArg)
  _PI_CL(piKernelGetInfo, cnrt_piKernelGetInfo)
  _PI_CL(piKernelGetGroupInfo, cnrt_piKernelGetGroupInfo)
  _PI_CL(piKernelGetSubGroupInfo, cnrt_piKernelGetSubGroupInfo)
  _PI_CL(piKernelRetain, cnrt_piKernelRetain)
  _PI_CL(piKernelRelease, cnrt_piKernelRelease)
  _PI_CL(piKernelSetExecInfo, cnrt_piKernelSetExecInfo)
  _PI_CL(piextKernelSetArgPointer, cnrt_piextKernelSetArgPointer)
  _PI_CL(piextKernelCreateWithNativeHandle,
         cnrt_piextKernelCreateWithNativeHandle)
  // Event
  _PI_CL(piEventCreate, cnrt_piEventCreate)
  _PI_CL(piEventGetInfo, cnrt_piEventGetInfo)
  _PI_CL(piEventGetProfilingInfo, cnrt_piEventGetProfilingInfo)
  _PI_CL(piEventsWait, cnrt_piEventsWait)
  _PI_CL(piEventSetCallback, cnrt_piEventSetCallback)
  _PI_CL(piEventSetStatus, cnrt_piEventSetStatus)
  _PI_CL(piEventRetain, cnrt_piEventRetain)
  _PI_CL(piEventRelease, cnrt_piEventRelease)
  _PI_CL(piextEventGetNativeHandle, cnrt_piextEventGetNativeHandle)
  _PI_CL(piextEventCreateWithNativeHandle,
         cnrt_piextEventCreateWithNativeHandle)
  // Sampler
  _PI_CL(piSamplerCreate, cnrt_piSamplerCreate)
  _PI_CL(piSamplerGetInfo, cnrt_piSamplerGetInfo)
  _PI_CL(piSamplerRetain, cnrt_piSamplerRetain)
  _PI_CL(piSamplerRelease, cnrt_piSamplerRelease)
  // Queue commands
  _PI_CL(piEnqueueKernelLaunch, cnrt_piEnqueueKernelLaunch)
  _PI_CL(piEnqueueNativeKernel, cnrt_piEnqueueNativeKernel)
  _PI_CL(piEnqueueEventsWait, cnrt_piEnqueueEventsWait)
  _PI_CL(piEnqueueEventsWaitWithBarrier, cnrt_piEnqueueEventsWaitWithBarrier)
  _PI_CL(piEnqueueMemBufferRead, cnrt_piEnqueueMemBufferRead)
  _PI_CL(piEnqueueMemBufferReadRect, cnrt_piEnqueueMemBufferReadRect)
  _PI_CL(piEnqueueMemBufferWrite, cnrt_piEnqueueMemBufferWrite)
  _PI_CL(piEnqueueMemBufferWriteRect, cnrt_piEnqueueMemBufferWriteRect)
  _PI_CL(piEnqueueMemBufferCopy, cnrt_piEnqueueMemBufferCopy)
  _PI_CL(piEnqueueMemBufferCopyRect, cnrt_piEnqueueMemBufferCopyRect)
  _PI_CL(piEnqueueMemBufferFill, cnrt_piEnqueueMemBufferFill)
  _PI_CL(piEnqueueMemImageRead, cnrt_piEnqueueMemImageRead)
  _PI_CL(piEnqueueMemImageWrite, cnrt_piEnqueueMemImageWrite)
  _PI_CL(piEnqueueMemImageCopy, cnrt_piEnqueueMemImageCopy)
  _PI_CL(piEnqueueMemImageFill, cnrt_piEnqueueMemImageFill)
  _PI_CL(piEnqueueMemBufferMap, cnrt_piEnqueueMemBufferMap)
  _PI_CL(piEnqueueMemUnmap, cnrt_piEnqueueMemUnmap)
  // USM
  _PI_CL(piextUSMHostAlloc, cnrt_piextUSMHostAlloc)
  _PI_CL(piextUSMDeviceAlloc, cnrt_piextUSMDeviceAlloc)
  _PI_CL(piextUSMSharedAlloc, cnrt_piextUSMSharedAlloc)
  _PI_CL(piextUSMFree, cnrt_piextUSMFree)
  _PI_CL(piextUSMEnqueueMemset, cnrt_piextUSMEnqueueMemset)
  _PI_CL(piextUSMEnqueueMemcpy, cnrt_piextUSMEnqueueMemcpy)
  _PI_CL(piextUSMEnqueuePrefetch, cnrt_piextUSMEnqueuePrefetch)
  _PI_CL(piextUSMEnqueueMemAdvise, cnrt_piextUSMEnqueueMemAdvise)
  _PI_CL(piextUSMGetMemAllocInfo, cnrt_piextUSMGetMemAllocInfo)

  _PI_CL(piextKernelSetArgMemObj, cnrt_piextKernelSetArgMemObj)
  _PI_CL(piextKernelSetArgSampler, cnrt_piextKernelSetArgSampler)
  _PI_CL(piTearDown, cnrt_piTearDown)

#undef _PI_CL

  return PI_SUCCESS;
}

} // extern "C"
