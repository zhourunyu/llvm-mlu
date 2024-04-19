#include "common.h"
#include <CL/sycl.hpp>
#include <vector>

constexpr auto N = 1024;

int main() {
    sycl::gpu_selector selector;
    sycl::queue q(selector);

    std::vector<float> x_host(N), y(N), y_host(N);
    initArray(x_host);

    auto x = sycl::malloc_device<float>(N, q);
    auto startTime = getTime();

    q.memcpy(x, x_host.data(), N * sizeof(float));
    q.parallel_for<class mm>(N, [=](sycl::id<1> i) {
        x[i] *= 2;
    });
    q.memcpy(y_host.data(), x, N * sizeof(float));
    q.wait();
    auto endTime = getTime();
    std::cout << "Time: " << endTime - startTime << "us" << std::endl;

    for (int i = 0; i < N; i++) {
        y[i] = x_host[i] * 2;
    }
    int ret = compareResult(y_host, y);
    sycl::free(x, q);

    if (ret) {
        std::cout << "Memory test failed!" << std::endl;
        return ret;
    }
    std::cout << "Test passed." << std::endl;
    return 0;
}