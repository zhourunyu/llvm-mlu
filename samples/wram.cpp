// 用于测试使用向量加的执行时间

#include <CL/sycl.hpp>
#include <array>
#include <sys/time.h>
#include <iostream>
using namespace std;
using namespace sycl;

constexpr int N = 256;

long long getTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000) + tv.tv_usec;
}

int main() {

    gpu_selector Selector;
    queue Q(Selector);

    // short *f = (short *)malloc(sizeof(short) * N);
    short *d = (short *)malloc(sizeof(short) * N);
    short *a = (short *)malloc(sizeof(short) * N);
    short *b = (short *)malloc(sizeof(short) * N);
    short *c = (short *)malloc(sizeof(short) * N);

    for (int i = 0; i < N; i++) {
        a[i] = 1;
        b[i] = 2;
        c[i] = 0;
    }

    cl::sycl::range<1> arr_range{N};

    // buffer<short, 1> bufferF((short *)f, arr_range);
    buffer<short, 1> bufferD((short *)d, arr_range);
    buffer<short, 1> bufferA((short *)a, arr_range);
    buffer<short, 1> bufferB((short *)b, arr_range);
    buffer<short, 1> bufferC((short *)c, arr_range);

    auto startTime = getTime();

    // Submit our job to the queue
    Q.submit([&](cl::sycl::handler &cgh) {
        accessor accessorC(bufferC, cgh, write_only);
        // AccB is temp 
       cl::sycl::accessor<short, 1, cl::sycl::access::mode::read_write,
                           cl::sycl::access::target::local>
            localAccB(cl::sycl::range<1>(N), cgh);
         
        // Local Accessor for WRAM
        cl::sycl::accessor<short, 1, cl::sycl::access::mode::read_write,
                           cl::sycl::access::target::wram>
            localAccC(cl::sycl::range<1>(N), cgh);

        cl::sycl::accessor<short, 1, cl::sycl::access::mode::read_write,
                            cl::sycl::access::target::local>
            localAccD(cl::sycl::range<1>(N), cgh);

        cgh.parallel_for<class mm>(1, [=](id<1> i) {
            auto bPtr =
                reinterpret_cast<short *>(localAccB.get_pointer().get());
            auto cPtr =
                reinterpret_cast<short *>(localAccC.get_pointer().get());

            auto dPtr =
                reinterpret_cast<short *>(localAccD.get_pointer().get());
            for (int j = 0; j < N; ++j) {
                localAccB[j] = 0;
                localAccD[j] = 4;
            }

            #ifdef __SYCL_DEVICE_ONLY__
            __mlvm_memcpy_nram_to_wram(cPtr, dPtr, N*sizeof(short));
            __mlvm_stream_conv_fix16_fix16_fix16(bPtr, dPtr, cPtr, 32, 32, 1, 1, 1, 1, 1, 256, 32);
            #endif

            for (int j = 0; j < N; ++j) {
                accessorC[j] = localAccB[j];
            }
        });
    });

    host_accessor host_accC(bufferC, read_only);
    for(int i=0; i<N; i++){
        if(host_accC[i] != 0)
		    std::cout<<host_accC[i]<<std::endl;
    }
    //std::cout << "Result: " << host_accC[0] << " .. " << host_accC[N - 1] << std::endl;
    auto endTime = getTime();
    std::cout << "Time: " << endTime - startTime << std::endl;
    return 0;
}