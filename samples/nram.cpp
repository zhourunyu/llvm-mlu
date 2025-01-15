#include "common.h"
#include <CL/sycl.hpp>

constexpr auto N = 1024;

int main() {
    sycl::gpu_selector selector;
    sycl::queue q(selector);
    std::cout << "Running on device: "<< q.get_device().get_info<sycl::info::device::name>() << std::endl;

    q.parallel_for<class mm>(1, [=](sycl::id<1> idx) {
        auto nram_buffer = *sycl::ext::mlu::sycl_nram_memory<float[N]>();
    });
    q.wait();
    
    std::cout << "Test passed." << std::endl;
    return 0;
}