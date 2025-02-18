#include "common.h"
#include <CL/sycl.hpp>
#include <array>

constexpr size_t N = 1024;

int main() {
    sycl::gpu_selector selector;
    sycl::queue q(selector);
    std::cout << "Running on device: "<< q.get_device().get_info<sycl::info::device::name>() << std::endl;

    std::array<int, N> a_host, a_dev;
    for (int i = 0; i < N; i++) {
        a_host[i] = i;
    }
    auto a = sycl::malloc_device<int>(N, q);

    // method 1: group size is auto determined
    q.parallel_for<class range>(sycl::range<3>(1, 1, 32), [=](sycl::item<3> item) {
        size_t id = item.get_id(2);
        for (size_t i = id * N / 32; i < (id + 1) * N / 32; i++) {
            a[i] = i;
        }
    });
    q.memcpy(a_dev.data(), a, N * sizeof(float)).wait();
    if (compareResult(a_host, a_dev)) {
        std::cout << "Test failed for workitem!" << std::endl;
        return 1;
    }

    // method 2: set group size manually
    q.parallel_for<class nd_range>(sycl::nd_range<3>({1, 1, 16}, {1, 1, 4}), [=](sycl::nd_item<3> item) {
        size_t id = item.get_global_id(2);
        for (size_t i = id * N / 16; i < (id + 1) * N / 16; i++) {
            a[i] = i;
        }
    });
    q.memcpy(a_dev.data(), a, N * sizeof(float)).wait();
    if (compareResult(a_host, a_dev)) {
        std::cout << "Test failed for workitem!" << std::endl;
        return 1;
    }

    sycl::free(a, q);
    std::cout << "Test passed." << std::endl;
    return 0;
}