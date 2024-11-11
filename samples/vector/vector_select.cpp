#include "common.h"
#include <CL/sycl.hpp>
#include <array>
#include <cmath>

#define N_CORES 1

template <typename T, size_t n>
void SelectOp(sycl::queue &q, const std::array<bool, n> &a, const std::array<T, n> &b, const std::array<T, n> &c, std::array<T, n> &d) {
    auto a_dev = sycl::malloc_device<bool>(n, q);
    auto b_dev = sycl::malloc_device<T>(n, q);
    auto c_dev = sycl::malloc_device<T>(n, q);
    auto d_dev = sycl::malloc_device<T>(n, q);
    q.memcpy(a_dev, a.data(), n * sizeof(bool));
    q.memcpy(b_dev, b.data(), n * sizeof(T));
    q.memcpy(c_dev, c.data(), n * sizeof(T));

    q.parallel_for<class mm>(sycl::range<3>{1, 1, N_CORES}, [=](sycl::item<3> item) {
        auto id = item.get_id(2);
        constexpr size_t nram_length = n / N_CORES;
        bool *a_offset = a_dev + id * nram_length;
        T *b_offset = b_dev + id * nram_length;
        T *c_offset = c_dev + id * nram_length;
        T *d_offset = d_dev + id * nram_length;
        auto nram_buffer_a = *sycl::ext::mlu::sycl_nram_memory<bool[nram_length]>();
        auto nram_buffer_b = *sycl::ext::mlu::sycl_nram_memory<T[nram_length]>();
        auto nram_buffer_c = *sycl::ext::mlu::sycl_nram_memory<T[nram_length]>();
        auto nram_buffer_d = *sycl::ext::mlu::sycl_nram_memory<T[nram_length]>();
        sycl::ext::mlu::memcpy_gdram2nram(nram_buffer_a, a_offset, nram_length);
        sycl::ext::mlu::memcpy_gdram2nram(nram_buffer_b, b_offset, nram_length);
        sycl::ext::mlu::memcpy_gdram2nram(nram_buffer_c, c_offset, nram_length);
        // dst cannot overlap with src
        sycl::ext::mlu::vector_select(nram_buffer_d, nram_buffer_a, nram_buffer_b, nram_buffer_c, nram_length);
        sycl::ext::mlu::memcpy_nram2gdram(d_offset, nram_buffer_d, nram_length);
    });

    q.memcpy(d.data(), d_dev, n * sizeof(T));
    q.wait();
    sycl::free(a_dev, q);
    sycl::free(b_dev, q);
    sycl::free(c_dev, q);
    sycl::free(d_dev, q);
}

template <typename T, size_t n>
void SelectOp(sycl::queue &q, const std::array<bool, n> &a, T b, const std::array<T, n> &c, std::array<T, n> &d) {
    auto a_dev = sycl::malloc_device<bool>(n, q);
    auto c_dev = sycl::malloc_device<T>(n, q);
    auto d_dev = sycl::malloc_device<T>(n, q);
    q.memcpy(a_dev, a.data(), n * sizeof(bool));
    q.memcpy(c_dev, c.data(), n * sizeof(T));

    q.parallel_for<class mm>(sycl::range<3>{1, 1, N_CORES}, [=](sycl::item<3> item) {
        auto id = item.get_id(2);
        constexpr size_t nram_length = n / N_CORES;
        bool *a_offset = a_dev + id * nram_length;
        T *c_offset = c_dev + id * nram_length;
        T *d_offset = d_dev + id * nram_length;
        auto nram_buffer_a = *sycl::ext::mlu::sycl_nram_memory<bool[nram_length]>();
        auto nram_buffer_c = *sycl::ext::mlu::sycl_nram_memory<T[nram_length]>();
        auto nram_buffer_d = *sycl::ext::mlu::sycl_nram_memory<T[nram_length]>();
        sycl::ext::mlu::memcpy_gdram2nram(nram_buffer_a, a_offset, nram_length);
        sycl::ext::mlu::memcpy_gdram2nram(nram_buffer_c, c_offset, nram_length);
        // dst cannot overlap with src
        sycl::ext::mlu::vector_select(nram_buffer_d, nram_buffer_a, b, nram_buffer_c, nram_length);
        sycl::ext::mlu::memcpy_nram2gdram(d_offset, nram_buffer_d, nram_length);
    });

    q.memcpy(d.data(), d_dev, n * sizeof(T));
    q.wait();
    sycl::free(a_dev, q);
    sycl::free(c_dev, q);
    sycl::free(d_dev, q);
}

template <typename T, size_t n>
void SelectOp(sycl::queue &q, const std::array<bool, n> &a, const std::array<T, n> &b, T c, std::array<T, n> &d) {
    auto a_dev = sycl::malloc_device<bool>(n, q);
    auto b_dev = sycl::malloc_device<T>(n, q);
    auto d_dev = sycl::malloc_device<T>(n, q);
    q.memcpy(a_dev, a.data(), n * sizeof(bool));
    q.memcpy(b_dev, b.data(), n * sizeof(T));

    q.parallel_for<class mm>(sycl::range<3>{1, 1, N_CORES}, [=](sycl::item<3> item) {
        auto id = item.get_id(2);
        constexpr size_t nram_length = n / N_CORES;
        bool *a_offset = a_dev + id * nram_length;
        T *b_offset = b_dev + id * nram_length;
        T *d_offset = d_dev + id * nram_length;
        auto nram_buffer_a = *sycl::ext::mlu::sycl_nram_memory<bool[nram_length]>();
        auto nram_buffer_b = *sycl::ext::mlu::sycl_nram_memory<T[nram_length]>();
        auto nram_buffer_d = *sycl::ext::mlu::sycl_nram_memory<T[nram_length]>();
        sycl::ext::mlu::memcpy_gdram2nram(nram_buffer_a, a_offset, nram_length);
        sycl::ext::mlu::memcpy_gdram2nram(nram_buffer_b, b_offset, nram_length);
        // dst cannot overlap with src
        sycl::ext::mlu::vector_select(nram_buffer_d, nram_buffer_a, nram_buffer_b, c, nram_length);
        sycl::ext::mlu::memcpy_nram2gdram(d_offset, nram_buffer_d, nram_length);
    });

    q.memcpy(d.data(), d_dev, n * sizeof(T));
    q.wait();
    sycl::free(a_dev, q);
    sycl::free(b_dev, q);
    sycl::free(d_dev, q);
}

constexpr auto N = 1024;

int main() {
    sycl::gpu_selector selector;
    sycl::queue q(selector);
    std::cout << "Running on device: "<< q.get_device().get_info<sycl::info::device::name>() << std::endl;

    std::array<bool, N> a;
    std::array<float, N> b, c, d, d_host;
    initArray(b);
    initArray(c);
    for (auto &i : a)
        i = rand() % 2;

    auto startTime = getTime();
    SelectOp(q, a, b, c, d);
    auto endTime = getTime();
    std::cout << "Time: " << endTime - startTime << "us" << std::endl;

    for (int i = 0; i < N; i++) {
        d_host[i] = a[i] ? b[i] : c[i];
    }
    int ret = compareResult(d, d_host);
    if (ret) {
        std::cout << "Vector select test failed!" << std::endl;
        return ret;
    }
    std::cout << "Test passed." << std::endl;
    return 0;
}
