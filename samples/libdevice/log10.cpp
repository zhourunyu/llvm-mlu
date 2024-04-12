#include "common.h"
#include <CL/sycl.hpp>
#include <vector>
#include <math.h>

#define UNARY_OP sycl::log10
#include "unary_op.h"

constexpr int N = 1024;

int main() {

    sycl::gpu_selector selector;
    sycl::queue q(selector);
    std::cout << "Running on device: "<< q.get_device().get_info<sycl::info::device::name>() << std::endl;

    std::vector<float> a(N), b(N), b_host(N), d(N);

    initArray(a, 0.1f, 5.0f);  // log10(x) is only defined for x > 0
    auto startTime = getTime();

    UnaryOp(q, a, b, d);
    auto endTime = getTime();
    std::cout << "Time: " << endTime - startTime << "us" << std::endl;

    for (int i = 0; i < N; i++) {
        b_host[i] = std::log10(a[i]);
    }
    int ret = compareResult(b, b_host);

    if (ret) {
        std::cout << "Test failed for log10()." << std::endl;
        return ret;
    }
    std::cout << "Test passed." << std::endl;
    return 0;
}
