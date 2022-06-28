// 用于测试使用向量加的执行时间

#include <CL/sycl.hpp>
#include <array>
#include <sys/time.h>
using namespace sycl;

constexpr int M = 10000;
constexpr int N = 1024;

long long getTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000) + tv.tv_usec;
}

int main() {

    gpu_selector Selector;
    queue Q(Selector);

    // float *f = (float *)malloc(sizeof(float) * N);
    float *d = (float *)malloc(sizeof(float) * N);
    float *a = (float *)malloc(sizeof(float) * N);
    float *b = (float *)malloc(sizeof(float) * N);
    float *c = (float *)malloc(sizeof(float) * N);

    for (int i = 0; i < N; i++) {
        a[i] = 1.0;
        b[i] = 2.0;
        c[i] = 0;
    }

    cl::sycl::range<1> arr_range{N};

    // buffer<float, 1> bufferF((float *)f, arr_range);
    buffer<float, 1> bufferD((float *)d, arr_range);
    buffer<float, 1> bufferA((float *)a, arr_range);
    buffer<float, 1> bufferB((float *)b, arr_range);
    buffer<float, 1> bufferC((float *)c, arr_range);

    auto startTime = getTime();

    // Submit our job to the queue
    Q.submit([&](cl::sycl::handler &cgh) {
        accessor accessorD(bufferD, cgh, read_only);
        accessor accessorA(bufferA, cgh, read_only);
        accessor accessorB(bufferB, cgh, read_only);
        accessor accessorC(bufferC, cgh, write_only);
        // Local Accessor for NRAM
        cl::sycl::accessor<float, 1, cl::sycl::access::mode::read_write,
                           cl::sycl::access::target::local>
            localAccA(cl::sycl::range<1>(N), cgh);
        // AccB is temp 
        cl::sycl::accessor<float, 1, cl::sycl::access::mode::read_write,
                           cl::sycl::access::target::local>
            localAccB(cl::sycl::range<1>(N), cgh);
         
        // Local Accessor for WRAM
        cl::sycl::accessor<float, 1, cl::sycl::access::mode::read_write,
                           cl::sycl::access::target::wram>
            localAccC(cl::sycl::range<1>(N), cgh);

        cl::sycl::accessor<float, 1, cl::sycl::access::mode::read_write,
                            cl::sycl::access::target::local>
            localAccD(cl::sycl::range<1>(N), cgh);

        cgh.parallel_for<class mm>(1, [=](id<1> i) {
            auto aPtr =
                reinterpret_cast<float_t *>(localAccA.get_pointer().get());
            auto bPtr =
                reinterpret_cast<float_t *>(localAccB.get_pointer().get());
            auto cPtr =
                reinterpret_cast<float_t *>(localAccC.get_pointer().get());
            auto dPtr =
                reinterpret_cast<float_t *>(localAccD.get_pointer().get());
            for (int j = 0; j < N; ++j) {
                localAccA[j] = accessorA[j];
                localAccB[j] = accessorB[j];
            }

            #ifdef __SYCL_DEVICE_ONLY__
            __mlvm_memcpy_nram_to_wram(cPtr, bPtr, N*sizeof(half));
            __mlvm_stream_conv_f32_f32_f32(dPtr, aPtr, cPtr, 32, 32, 1, 1, 1, 1, 1, 32);
            #endif

            for (int j = 0; j < N; ++j) {
                accessorC[j] = localAccB[j];
            }
        });
    });

    host_accessor host_accC(bufferC, read_only);
    std::cout << "Result: " << host_accC[0] << " .. " << host_accC[N - 1] << std::endl;
    auto endTime = getTime();
    std::cout << "Time: " << endTime - startTime << std::endl;
    return 0;
}
