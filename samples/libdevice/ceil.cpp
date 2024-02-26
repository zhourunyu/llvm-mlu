// 用于测试使用向量加的执行时间

#include <CL/sycl.hpp>
#include <array>
#include <sys/time.h>
// #include <math.h>
using namespace sycl;

constexpr int N = 32;

long long getTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000) + tv.tv_usec;
}

int main() {

    gpu_selector Selector;
    queue Q(Selector);

    float *d = (float *)malloc(sizeof(float) * N);
    float *a = (float *)malloc(sizeof(float) * N);
    float *c = (float *)malloc(sizeof(float) * N);
    float *c_host = (float *)malloc(sizeof(float) * N);

    for (int i = 0; i < N; i++) {
        a[i] = 0.3;
        c[i] = 0.0;
        c_host[i] = 1.0;
        // c_host[i] = M_PI/3;
    }

    cl::sycl::range<1> arr_range{N};

    // bufferD在计算中没有用到，但是是必须存在的用于有效的寻址
    buffer<float, 1> bufferD((float *)d, arr_range);
    buffer<float, 1> bufferA((float *)a, arr_range);
    buffer<float, 1> bufferC((float *)c, arr_range);

    auto startTime = getTime();

    // Submit our job to the queue
    Q.submit([&](cl::sycl::handler &cgh) {
        accessor accessorD(bufferD, cgh, read_only);
        accessor accessorA(bufferA, cgh, read_only);
        //accessor accessorB(bufferB, cgh, read_only);
        accessor accessorC(bufferC, cgh, write_only);
        // Local Accessor for NRAM
        cl::sycl::accessor<float, 1, cl::sycl::access::mode::read_write,
                           cl::sycl::access::target::local>
            localAccA(cl::sycl::range<1>(N), cgh);
        //cl::sycl::accessor<float, 1, cl::sycl::access::mode::read_write,
        //                   cl::sycl::access::target::local>
        //    localAccB(cl::sycl::range<1>(N), cgh);
        //cl::sycl::accessor<float, 1, cl::sycl::access::mode::read_write,
        //                   cl::sycl::access::target::local>
        //    localAccC(cl::sycl::range<1>(N), cgh);

        cgh.parallel_for<class mm>(1, [=](id<1> i) {
            for (int j = 0; j < N; ++j) {
                localAccA[j] = accessorA[j];
                //accessorC[j] = localAccC[j];
		        accessorC[j] = ceil(localAccA[j]);
            }
        });
    });

    host_accessor host_accC(bufferC, read_only);
    std::cout << "Result: " << host_accC[0] << " .. " << host_accC[N - 1] << std::endl;
    // for(int i = 0;i < N;i++){
    //     if(host_accC[i] - c_host[i] > 1e-4 || (host_accC[i] - c_host[i]) > -1e-4){
    //         std::cout<<"Error ! result is expected "<<c_host[i]<<" but get "<<host_accC[i]<<std::endl;
    //     }
    // }
    auto endTime = getTime();
    std::cout << "Time: " << endTime - startTime << std::endl;
    return 0;
}
