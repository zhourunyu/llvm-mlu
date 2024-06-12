#ifndef VECTOR_OP
#error "VECTOR_OP not defined"
#endif

#include <CL/sycl.hpp>
#include <array>

#define N_CORES 1

template <typename Ta, typename Tb, size_t n>
void UnaryOp(sycl::queue &q, const std::array<Ta, n> &a, std::array<Tb, n> &b) {
    auto a_dev = sycl::malloc_device<Ta>(n, q);
    auto b_dev = sycl::malloc_device<Tb>(n, q);
    q.memcpy(a_dev, a.data(), n * sizeof(Ta));

    q.parallel_for<class mm>(sycl::range<3>{1, 1, N_CORES}, [=](sycl::item<3> item) {
        auto id = item.get_id(2);
        constexpr size_t nram_length = n / N_CORES;
        Ta *a_offset = a_dev + id * nram_length;
        Tb *b_offset = b_dev + id * nram_length;
        auto nram_buffer_a = *sycl::ext::mlu::sycl_nram_memory<Ta[nram_length]>();
        auto nram_buffer_b = *sycl::ext::mlu::sycl_nram_memory<Tb[nram_length]>();
        sycl::ext::mlu::memcpy_gdram2nram(nram_buffer_a, a_offset, nram_length);
        VECTOR_OP(nram_buffer_b, nram_buffer_a, nram_length);
        sycl::ext::mlu::memcpy_nram2gdram(b_offset, nram_buffer_b, nram_length);
    });

    q.memcpy(b.data(), b_dev, n * sizeof(Tb));
    q.wait();
    sycl::free(a_dev, q);
    sycl::free(b_dev, q);
}

template <typename Ta, typename Tb, typename Tc, size_t n>
void BinaryOp(sycl::queue &q, const std::array<Ta, n> &a, const std::array<Tb, n> &b, std::array<Tc, n> &c) {
    auto a_dev = sycl::malloc_device<Ta>(n, q);
    auto b_dev = sycl::malloc_device<Tb>(n, q);
    auto c_dev = sycl::malloc_device<Tc>(n, q);
    q.memcpy(a_dev, a.data(), n * sizeof(Ta));
    q.memcpy(b_dev, b.data(), n * sizeof(Tb));

    q.parallel_for<class mm>(sycl::range<3>{1, 1, N_CORES}, [=](sycl::item<3> item) {
        auto id = item.get_id(2);
        constexpr size_t nram_length = n / N_CORES;
        Ta *a_offset = a_dev + id * nram_length;
        Tb *b_offset = b_dev + id * nram_length;
        Tc *c_offset = c_dev + id * nram_length;
        auto nram_buffer_a = *sycl::ext::mlu::sycl_nram_memory<Ta[nram_length]>();
        auto nram_buffer_b = *sycl::ext::mlu::sycl_nram_memory<Tb[nram_length]>();
        auto nram_buffer_c = *sycl::ext::mlu::sycl_nram_memory<Tc[nram_length]>();
        sycl::ext::mlu::memcpy_gdram2nram(nram_buffer_a, a_offset, nram_length);
        sycl::ext::mlu::memcpy_gdram2nram(nram_buffer_b, b_offset, nram_length);
        VECTOR_OP(nram_buffer_c, nram_buffer_a, nram_buffer_b, nram_length);
        sycl::ext::mlu::memcpy_nram2gdram(c_offset, nram_buffer_c, nram_length);
    });

    q.memcpy(c.data(), c_dev, n * sizeof(Tc));
    q.wait();
    sycl::free(a_dev, q);
    sycl::free(b_dev, q);
    sycl::free(c_dev, q);
}

template <typename Ta, typename Tb, typename Tc, size_t n>
void BinaryOp(sycl::queue &q, const std::array<Ta, n> &a, Tb b, std::array<Tc, n> &c) {
    auto a_dev = sycl::malloc_device<Ta>(n, q);
    auto c_dev = sycl::malloc_device<Tc>(n, q);
    q.memcpy(a_dev, a.data(), n * sizeof(Ta));

    q.parallel_for<class mm>(sycl::range<3>{1, 1, N_CORES}, [=](sycl::item<3> item) {
        auto id = item.get_id(2);
        constexpr size_t nram_length = n / N_CORES;
        Ta *a_offset = a_dev + id * nram_length;
        Tc *c_offset = c_dev + id * nram_length;
        auto nram_buffer_a = *sycl::ext::mlu::sycl_nram_memory<Ta[nram_length]>();
        auto nram_buffer_c = *sycl::ext::mlu::sycl_nram_memory<Tc[nram_length]>();
        sycl::ext::mlu::memcpy_gdram2nram(nram_buffer_a, a_offset, nram_length);
        VECTOR_OP(nram_buffer_c, nram_buffer_a, b, nram_length);
        sycl::ext::mlu::memcpy_nram2gdram(c_offset, nram_buffer_c, nram_length);
    });

    q.memcpy(c.data(), c_dev, n * sizeof(Tc));
    q.wait();
    sycl::free(a_dev, q);
    sycl::free(c_dev, q);
}

template <typename Ta, typename Tb, typename Tc, size_t n>
void BinaryOp(sycl::queue &q, Ta a, const std::array<Tb, n> &b, std::array<Tc, n> &c) {
    auto b_dev = sycl::malloc_device<Tb>(n, q);
    auto c_dev = sycl::malloc_device<Tc>(n, q);
    q.memcpy(b_dev, b.data(), n * sizeof(Tb));

    q.parallel_for<class mm>(sycl::range<3>{1, 1, N_CORES}, [=](sycl::item<3> item) {
        auto id = item.get_id(2);
        constexpr size_t nram_length = n / N_CORES;
        Tb *b_offset = b_dev + id * nram_length;
        Tc *c_offset = c_dev + id * nram_length;
        auto nram_buffer_b = *sycl::ext::mlu::sycl_nram_memory<Tb[nram_length]>();
        auto nram_buffer_c = *sycl::ext::mlu::sycl_nram_memory<Tc[nram_length]>();
        sycl::ext::mlu::memcpy_gdram2nram(nram_buffer_b, b_offset, nram_length);
        VECTOR_OP(nram_buffer_c, a, nram_buffer_b, nram_length);
        sycl::ext::mlu::memcpy_nram2gdram(c_offset, nram_buffer_c, nram_length);
    });

    q.memcpy(c.data(), c_dev, n * sizeof(Tc));
    q.wait();
    sycl::free(b_dev, q);
    sycl::free(c_dev, q);
}

#undef N_CORES
#undef VECTOR_OP
