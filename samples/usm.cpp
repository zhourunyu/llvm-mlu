#include "common.h"
#include <CL/sycl.hpp>
#include <array>

constexpr auto N = 1024;

int main() {
    sycl::gpu_selector selector;
    sycl::queue q(selector);
    std::cout << "Running on device: "<< q.get_device().get_info<sycl::info::device::name>() << std::endl;

    std::array<float, N> a_host, b_host, c_dev, c_host;
    initArray(a_host);
    initArray(b_host);

    auto a = sycl::malloc_device<float>(N, q);
    auto b = sycl::malloc_device<float>(N, q);
    auto c = sycl::malloc_device<float>(N, q);
    std::cout << std::hex;
    std::cout << "a: " << a << std::endl;
    std::cout << "b: " << b << std::endl;
    std::cout << "c: " << c << std::endl;
    std::cout << std::dec;
    auto startTime = getTime();

    q.memcpy(a, a_host.data(), N * sizeof(float));
    q.memcpy(b, b_host.data(), N * sizeof(float));
    q.parallel_for<class mm>(N, [=](sycl::id<1> i) {
        c[i] = a[i] + b[i];
    });
    q.memcpy(c_dev.data(), c, N * sizeof(float));
    q.wait();
    auto endTime = getTime();
    std::cout << "Time: " << endTime - startTime << "us" << std::endl;

    for (int i = 0; i < N; i++) {
        c_host[i] = a_host[i] + b_host[i];
    }
    int ret = compareResult(c_host, c_dev);
    sycl::free(a, q);
    sycl::free(b, q);
    sycl::free(c, q);

    if (ret) {
        std::cout << "USM test failed!" << std::endl;
        return ret;
    }
    std::cout << "Test passed." << std::endl;
    return 0;
}