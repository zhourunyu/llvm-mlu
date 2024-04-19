#include "common.h"
#include <CL/sycl.hpp>
#include <vector>
#include <math.h>

#define BINARY_OP(x, y) ((x) * (y))
#include "binary_op.h"

constexpr int N = 1024;

int main() {

    sycl::gpu_selector selector;
    sycl::queue q(selector);
    std::cout << "Running on device: "<< q.get_device().get_info<sycl::info::device::name>() << std::endl;

    std::vector<float> a(N), b(N), c(N), c_host(N);

    initArray(a);
    initArray(b);
    auto startTime = getTime();

    BinaryOp(q, a, b, c);
    auto endTime = getTime();
    std::cout << "Time: " << endTime - startTime << "us" << std::endl;

    for (int i = 0; i < N; i++) {
        c_host[i] = a[i] * b[i];
    }
    int ret = compareResult(c, c_host);

    if (ret) {
        std::cout << "Test failed for mul()." << std::endl;
        return ret;
    }
    std::cout << "Test passed." << std::endl;
    return 0;
}
