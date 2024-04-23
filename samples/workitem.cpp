#include "common.h"
#include <CL/sycl.hpp>
#include <vector>

constexpr auto N = 256;
constexpr auto M = N / 4 / 4;

int main() {
    sycl::gpu_selector selector;
    sycl::queue q(selector);
    std::cout << "Running on device: "<< q.get_device().get_info<sycl::info::device::name>() << std::endl;

    std::vector<int> a_host(N), a_dev(N);
    std::fill(a_host.begin(), a_host.end(), 0);

    auto a = sycl::malloc_device<int>(N, q);
    auto startTime = getTime();

    q.memcpy(a, a_host.data(), N * sizeof(int));
    q.parallel_for<class mm>(sycl::nd_range<3>({2, 2, 1}, {1, 1, 1}), [=](sycl::nd_item<3> item) {
        int i = item.get_global_id(0), j = item.get_global_id(1), k = item.get_global_id(2);
        int start = (i + j * 4 + k * 16) * M;
        for (int l = 0; l < M; l++)
            a[start + l] += i + j + k;
    });
    q.memcpy(a_dev.data(), a, N * sizeof(float));
    q.wait();
    auto endTime = getTime();
    std::cout << "Time: " << endTime - startTime << "us" << std::endl;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 1; k++) {
                int start = (i + j * 4 + k * 16) * M;
                for (int l = 0; l < M; l++)
                    a_host[start + l] += i + j + k;
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