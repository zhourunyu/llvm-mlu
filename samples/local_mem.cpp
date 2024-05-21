#include "common.h"
#include <CL/sycl.hpp>
#include <array>

constexpr auto N = 256;

int main() {
    sycl::gpu_selector selector;
    sycl::queue q(selector);
    std::cout << "Running on device: "<< q.get_device().get_info<sycl::info::device::name>() << std::endl;

    std::array<float, N> a_host;
    std::array<float, 1> b_dev, b_host;
    initArray(a_host);
    b_dev[0] = 0.0f;
    b_host[0] = 0.0f;

    auto a = sycl::malloc_device<float>(N, q);
    auto b = sycl::malloc_device<float>(1, q);
    auto startTime = getTime();

    q.memcpy(a, a_host.data(), N * sizeof(float));
    q.memcpy(b, b_host.data(), 1 * sizeof(float));
    q.parallel_for<class mm>(sycl::nd_range<3>({1, 1, 4}, {1, 1, 4}), [=](sycl::nd_item<3> item) {
        auto group = item.get_group();
        auto id = item.get_local_id(2);
        auto buffer = *sycl::group_local_memory<float[4]>(group);
        float tmp = 0.0f;
        for (int i = 0; i < N / 4; i++) {
            tmp += a[id * (N / 4) + i];
        }
        buffer[id] = tmp;
        sycl::group_barrier(group);
        if (id < 2) {
            buffer[id] += buffer[id + 2];
        }
        sycl::group_barrier(group);
        if (id == 0) {
            b[0] = buffer[0] + buffer[1];
        }
    });
    q.memcpy(b_dev.data(), b, 1 * sizeof(float));
    q.wait();
    auto endTime = getTime();
    std::cout << "Time: " << endTime - startTime << "us" << std::endl;

    for (int i = 0; i < N; i++) {
        b_host[0] += a_host[i];
    }
    int ret = compareResult(b_host, b_dev);
    sycl::free(a, q);
    sycl::free(b, q);

    if (ret) {
        std::cout << "Local mem test failed!" << std::endl;
        return ret;
    }
    std::cout << "Test passed." << std::endl;
    return 0;
}