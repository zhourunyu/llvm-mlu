#include "common.h"
#include <CL/sycl.hpp>
#include <array>
#include <cmath>

#define UNARY_OP sycl::isfinite
#include "unary_op.h"

constexpr int N = 1024;

int main() {

    sycl::gpu_selector selector;
    sycl::queue q(selector);
    std::cout << "Running on device: "<< q.get_device().get_info<sycl::info::device::name>() << std::endl;

    std::array<float, N> a;
    std::array<int, N> b, b_host;

    initArray(a);
    for (auto &i : a) {
        auto p = rand() / (float)RAND_MAX;
        if (p > 0.75)
            i = INFINITY;
        else if (p > 0.5)
            i = -INFINITY;
        else if (p > 0.25)
            i = NAN;
    }
    auto startTime = getTime();

    UnaryOp(q, a, b);
    auto endTime = getTime();
    std::cout << "Time: " << endTime - startTime << "us" << std::endl;

    for (int i = 0; i < N; i++) {
        b_host[i] = std::isfinite(a[i]);
    }
    int ret = compareResult(b, b_host);

    if (ret) {
        std::cout << "Test failed for isfinite()." << std::endl;
        return ret;
    }
    std::cout << "Test passed." << std::endl;
    return 0;
}
