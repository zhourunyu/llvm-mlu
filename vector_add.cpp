#include <CL/sycl.hpp>
#include <array>
using namespace sycl;
constexpr int N = 256;

int main() {

    gpu_selector Selector;
    queue Q(Selector);
/*
    try {
    cl::sycl::queue Queue(Selector);
    cl::sycl::device Device(Selector);
  } catch (cl::sycl::invalid_parameter_error &E) {
    std::cout << E.what() << std::endl;
  }
*/  
    std::array<int, N> a,b,c;
    for(int i=0; i<N; i++) {
        a[i] = b[i] = 1;
        c[i] = 0;
    }

    buffer A{a};
    buffer B{b};
    buffer C{c};

  
    Q.submit([&](handler &h) {
         accessor accA(A, h, read_only);
         accessor accB(B, h, read_only);
         accessor accC(C, h, write_only);
         h.parallel_for<class vector_add>(
           N,
           [=](id<1> i) {accC[i] = accB[i] + accA[i];});
     });
      
     host_accessor host_accC(C, read_only);
     double ret = 0;
     for(int i=0; i<N; i++) {
         ret += host_accC[i];
     }
     std::cout<<"result is:"<<ret<<"\n";
     return 0;
    
}

