#include <CL/sycl.hpp>
#include <array>
using namespace sycl;
constexpr int N = 4096;
constexpr int M = 4096;

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
    std::array<int, N> a;
    for(int i=0; i<N; i++) {
        a[i] = 2;
    }

    std::array<int, M> b, c;
    for (int i=0; i<M; i++) {
    	b[i] = 2;
	c[i] = 0;
    }

    buffer A(a);
    buffer B(b);
    buffer C(c);

  
    Q.submit([&](handler &h) {
         accessor accA(A, h, read_only);
	 accessor accB(B, h, read_only);
         accessor accC(C, h, read_write);
         h.parallel_for<class vector_add>(
           1,
           [=](id<1> i) {
	       for(int j=0; j<M; j++)
	           accC[j] = accA[j] + accB[j];
	 });
     });
      
     auto host_accC = C.get_access<access::mode::read>();
     double ret = 0;
     for(int i=M-1; i>M-10; i--) {
	 //std::cout<<"ele is: "<<host_accC[i]i<<std::endl;
         ret += host_accC[i];
     }
     std::cout<<"result is:"<<ret<<"\n";
     return 0;
    
}

