#include "common.h"
#include <CL/sycl.hpp>
#include <vector>
#include <math.h>

#define RTOL 1e-5f
#define ATOL 1e-8f

#define BINARY_OP(x, y) (sycl::abs((x) - (y)) <= sycl::max(ATOL, RTOL * sycl::max(sycl::abs(x), sycl::abs(y))))
#include "binary_op.h"

constexpr int N = 1024;

int main() {

    sycl::gpu_selector selector;
    sycl::queue q(selector);
    std::cout << "Running on device: "<< q.get_device().get_info<sycl::info::device::name>() << std::endl;

    std::vector<float> a(N), b(N);
    std::vector<int> c(N), c_host(N);

    initArray(a);
    initArray(b);
    auto startTime = getTime();

    BinaryOp(q, a, b, c);
    auto endTime = getTime();
    std::cout << "Time: " << endTime - startTime << "us" << std::endl;

    for (int i = 0; i < N; i++) {
        c_host[i] = std::abs(a[i] - b[i]) <= std::max(ATOL, RTOL * std::max(a[i], b[i]));
    }
    int ret = compareResult(c, c_host);

    if (ret) {
        std::cout << "Test failed for isclose()." << std::endl;
        return ret;
    }
    std::cout << "Test passed." << std::endl;
    return 0;
}
