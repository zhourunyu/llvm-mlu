#ifndef UNARY_OP
#error "UNARY_OP not defined"
#endif

#include <CL/sycl.hpp>
#include <array>

template <typename Ta, typename Tb, size_t n>
void UnaryOp(sycl::queue &q, const std::array<Ta, n> &a, std::array<Tb, n> &b) {
    sycl::range<1> n_items{n};

    sycl::buffer a_buffer(a);
    sycl::buffer b_buffer(b);

    q.submit([&](sycl::handler &h) {
        sycl::accessor a_accessor(a_buffer, h, sycl::read_only);
        sycl::accessor b_accessor(b_buffer, h, sycl::write_only, sycl::no_init);

        h.parallel_for<class mm>(n_items, [=](sycl::id<1> i) {
            b_accessor[i] = UNARY_OP(a_accessor[i]);
        });
    });
    q.wait();
}

#undef UNARY_OP
