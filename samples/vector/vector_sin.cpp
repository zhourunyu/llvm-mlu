#include "common.h"
#include <CL/sycl.hpp>
#include <array>
#include <cmath>

#define VECTOR_OP sycl::ext::mlu::vector_sin
#include "vector_op.h"

constexpr auto N = 1024;

int main() {
    sycl::gpu_selector selector;
    sycl::queue q(selector);
    std::cout << "Running on device: "<< q.get_device().get_info<sycl::info::device::name>() << std::endl;

    std::array<float, N> a, b, b_host;
    initArray(a);

    auto startTime = getTime();
    UnaryOp(q, a, b);
    auto endTime = getTime();
    std::cout << "Time: " << endTime - startTime << "us" << std::endl;

    for (int i = 0; i < N; i++) {
        b_host[i] = std::sin(a[i]);
    }
    int ret = compareResult(b, b_host);
    if (ret) {
        std::cout << "Vector sin test failed!" << std::endl;
        return ret;
    }
    std::cout << "Test passed." << std::endl;
    return 0;
}
