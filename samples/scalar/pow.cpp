#include "common.h"
#include <CL/sycl.hpp>
#include <array>
#include <cmath>

#define BINARY_OP sycl::pow
#include "binary_op.h"

constexpr int N = 1024;

int main() {

    sycl::gpu_selector selector;
    sycl::queue q(selector);
    std::cout << "Running on device: "<< q.get_device().get_info<sycl::info::device::name>() << std::endl;

    std::array<float, N> a, b, c, c_host;

    initArray(a, 0.0f, 5.0f);   // float y requires x >= 0
    initArray(b);
    auto startTime = getTime();

    BinaryOp(q, a, b, c);
    auto endTime = getTime();
    std::cout << "Time: " << endTime - startTime << "us" << std::endl;

    for (int i = 0; i < N; i++) {
        c_host[i] = std::pow(a[i], b[i]);
    }
    int ret = compareResult(c, c_host);

    if (ret) {
        std::cout << "Test failed for pow()." << std::endl;
        return ret;
    }
    std::cout << "Test passed." << std::endl;
    return 0;
}
