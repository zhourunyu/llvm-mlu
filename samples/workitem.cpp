#include "common.h"
#include <CL/sycl.hpp>
#include <vector>

constexpr auto N = 256;

int main() {
    sycl::gpu_selector selector;
    sycl::queue q(selector);
    std::cout << "Running on device: "<< q.get_device().get_info<sycl::info::device::name>() << std::endl;

    std::vector<int> a_host(N), a_dev(N);
    std::fill(a_host.begin(), a_host.end(), 0);

    auto a = sycl::malloc_device<int>(N, q);
    auto startTime = getTime();

    q.memcpy(a, a_host.data(), N * sizeof(int));
    q.parallel_for<class mm>(sycl::nd_range<3>({N / 4 / 4, 4, 4}, {1, 1, 4}), [=](sycl::nd_item<3> item) {
        int i = item.get_global_id(2), j = item.get_global_id(1), k = item.get_global_id(0);
        size_t dimX = item.get_global_range(2), dimY = item.get_global_range(1), dimZ = item.get_global_range(0);
        int id = i + j * dimX + k * dimX * dimY;
        a[id] += i + j + k;
    });
    q.memcpy(a_dev.data(), a, N * sizeof(float));
    q.wait();
    auto endTime = getTime();
    std::cout << "Time: " << endTime - startTime << "us" << std::endl;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < N / 4 / 4; k++) {
                int id = i + j * 4 + k * 16;
                a_host[id] += i + j + k;
            }
        }
    }
    int ret = compareResult(a_host, a_dev);
    
    sycl::free(a, q);
    if (ret) {
        std::cout << "Test failed for workitem!" << std::endl;
        return ret;
    }

    std::cout << "Test passed." << std::endl;
    return 0;
}