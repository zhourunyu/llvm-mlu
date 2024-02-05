#include <sycl/sycl.hpp>
#include <array>
#include <math.h>
#include <sys/time.h>
#include <iostream>

constexpr int N = 256;

using namespace std;
using namespace sycl;


long long getTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec*1000000 + tv.tv_usec);
}

int main(){
    gpu_selector Selector;
    queue Q(Selector);

    float *a = (float *)malloc(sizeof(float) * N);
    float *b = (float *)malloc(sizeof(float) * N);
    float *c = (float *)malloc(sizeof(float) * N);
    // float *d = (float *)malloc(sizeof(float) * N);
    float *c_host = (float *)malloc(sizeof(float) * N);

    for(int i = 0;i < N;i++){
        a[i] = 100;b[i] = 1.0f;c_host[i] = 1.000f;
    }

    range<1> arr_range(N);

    buffer<float,1> bufferA((float*)a,arr_range);
    buffer<float,1> bufferB((float*)b,arr_range);
    buffer<float,1> bufferC((float*)c,arr_range);
    // buffer<float,1> bufferD((float*)d,arr_range);

    auto startTime = getTime();

    Q.submit([&](handler &h){
        accessor aA{bufferA,h,read_only};
        accessor aB{bufferB,h,write_only};
        accessor aC{bufferC,h,write_only};
        // accessor aD{bufferD,h,read_only};

        accessor<float,1,access::mode::read_write,access::target::local> localAccA(range<1>(N),h);
        accessor<float,1,access::mode::read_write,access::target::local> localAccB(range<1>(N), h);
        accessor<float,1,access::mode::read_write,access::target::local> localAccC(range<1>(N), h);
        // accessor<float,1,access::mode::read_write,access::target::local> localAccD(range<1>(N), h);
      
        h.parallel_for<class mm>(1,[=](id<1> i) {
            // aC[i] = aA[i] + aB[i];
            auto aPtr =
                reinterpret_cast<float *>(localAccA.get_pointer().get());
            auto bPtr =
                reinterpret_cast<float *>(localAccB.get_pointer().get());
            auto cPtr =
                reinterpret_cast<float *>(localAccC.get_pointer().get()); 
            // auto dPtr =
            //     reinterpret_cast<float *>(localAccD.get_pointer().get());                 
            for (int j = 0; j < N; ++j) {
                localAccA[j] = aA[j];
                // localAccB[j] = aB[j];
            }
            for (int j = 0;j < N;++j){
            #ifdef __SYCL_DEVICE_ONLY__
                __mlvm_stream_active_tanh_f32(bPtr,aPtr,N);
            #endif
            }

            for(int j = 0;j < N;++j){
                aB[j] = localAccB[j];
            }

        });
    });

    host_accessor result{bufferB};

    std::cout << "tanh(100) Result: " << result[0] << " .. " << result[N - 1] << std::endl;
    // for(int i = 0; i<N; i++){
    //     if(abs(c_host[i]-result[i]) >= 1e-5)
	// 	    std::cout<<result[i]<<std::endl;
    // }

    auto endTime = getTime();
    std::cout << "Time : " << endTime - startTime <<" us "<< std::endl;

    free(a);
    free(b);
    free(c);
    // free(d);
    free(c_host);

    return 0;
}