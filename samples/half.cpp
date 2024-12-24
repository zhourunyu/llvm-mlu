#include "common.h"
#include <CL/sycl.hpp>
#include <algorithm>

constexpr auto N = 1024;

int main() {
    sycl::gpu_selector selector;
    sycl::queue q(selector);
    std::cout << "Running on device: "<< q.get_device().get_info<sycl::info::device::name>() << std::endl;

    std::array<sycl::half, N> a, b, b_host;
    initArray(a);

    auto a_dev = sycl::malloc_device<sycl::half>(N, q);
    auto b_dev = sycl::malloc_device<sycl::half>(N, q);
    q.memcpy(a_dev, a.data(), N * sizeof(sycl::half));

    q.parallel_for<class mm>(1, [=](sycl::id<1> idx) {
        auto a_nram = *sycl::ext::mlu::sycl_nram_memory<sycl::half[N]>();
        sycl::ext::mlu::memcpy_gdram2nram(a_nram, a_dev, N);
        sycl::ext::mlu::vector_cos(a_nram, a_nram, N);
        sycl::ext::mlu::memcpy_nram2gdram(b_dev, a_nram, N);
    });
    q.memcpy(b.data(), b_dev, N * sizeof(sycl::half));
    q.wait();

    std::transform(a.begin(), a.end(), b_host.begin(), [](auto x) { return std::cos(x); });

    for (int i = 0; i < N; i++) {
        if (b[i] != b_host[i]) {
            std::cout << "Test failed." << std::endl;
            return 1;
        }
    }
    std::cout << "Test passed." << std::endl;
    return 0;
}