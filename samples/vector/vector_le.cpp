#include "common.h"
#include <CL/sycl.hpp>
#include <array>
#include <cmath>

#define VECTOR_OP sycl::ext::mlu::vector_le
#include "vector_op.h"

constexpr auto N = 1024;

int main() {
    sycl::gpu_selector selector;
    sycl::queue q(selector);
    std::cout << "Running on device: "<< q.get_device().get_info<sycl::info::device::name>() << std::endl;

    std::array<float, N> a, b;
    std::array<bool, N> c, c_host;
    initArray(a);
    initArray(b);

    auto startTime = getTime();
    BinaryOp(q, a, b, c);
    auto endTime = getTime();
    std::cout << "Time: " << endTime - startTime << "us" << std::endl;

    for (int i = 0; i < N; i++) {
        c_host[i] = a[i] <= b[i];
    }
    int ret = compareResult(c, c_host);
    if (ret) {
        std::cout << "Vector less-equal test failed!" << std::endl;
        return ret;
    }
    std::cout << "Test passed." << std::endl;
    return 0;
}