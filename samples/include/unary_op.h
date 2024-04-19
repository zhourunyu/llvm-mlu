#ifndef UNARY_OP
#error "UNARY_OP not defined"
#endif

#include <CL/sycl.hpp>
#include <vector>

template <typename Ta, typename Tb>
void UnaryOp(sycl::queue &q, const std::vector<Ta> &a, std::vector<Tb> &b) {
    sycl::range<1> n_items{a.size()};

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
