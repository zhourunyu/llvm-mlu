#ifndef BINARY_OP
#error "BINARY_OP not defined"
#endif

#include <CL/sycl.hpp>
#include <vector>

template <typename Ta, typename Tb, typename Tc>
void BinaryOp(sycl::queue &q, const std::vector<Ta> &a, const std::vector<Tb> &b, std::vector<Tc> &c, std::vector<Ta> &d) {
    sycl::range<1> n_items{a.size()};

    // bufferD在计算中没有用到，但是是必须存在的用于有效的寻址
    sycl::buffer d_buffer(d);
    sycl::buffer a_buffer(a);
    sycl::buffer b_buffer(b);
    sycl::buffer c_buffer(c);

    q.submit([&](sycl::handler &h) {
        sycl::accessor d_accessor(d_buffer, h, sycl::read_only);
        sycl::accessor a_accessor(a_buffer, h, sycl::read_only);
        sycl::accessor b_accessor(b_buffer, h, sycl::read_only);
        sycl::accessor c_accessor(c_buffer, h, sycl::write_only, sycl::no_init);

        h.parallel_for<class mm>(n_items, [=](sycl::id<1> i) {
            c_accessor[i] = BINARY_OP(a_accessor[i], b_accessor[i]);
        });
    });
    q.wait();
}

#undef BINARY_OP
