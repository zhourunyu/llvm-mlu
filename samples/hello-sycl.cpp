#include <CL/sycl.hpp>
#include <iostream>
#include <vector>
using namespace sycl;

int main(){
    std::cout<<"hello sycl"<<std::endl;
    const int N = 4;
    const int MAGIC_NUM = 42;
    std::vector<int> vec_num = {3,4,5,6,7,8,9};
    std::cout<<vec_num[0]<<std::endl;

    queue q;
    auto dev = q.get_device();
    std::cout<<"default queue select device info : "<<&dev<<std::endl;
    std::cout<<"default queue select device vendor id : "<<dev.get_info<sycl::info::device::vendor_id>()<<std::endl;
    std::cout<<"default queue select device vendor name : "<<dev.get_info<sycl::info::device::name>()<<std::endl;
    auto devices = dev.get_devices();
    for (auto &device : devices) {
            std::cout << "  queue get device: " << device.get_info<sycl::info::device::name>() << std::endl;
            std::cout << "  queue get device vendor id: " << device.get_info<sycl::info::device::vendor_id>()<< std::endl;
            std::cout << "  queue get device max compute units : " << device.get_info<info::device::max_compute_units>()<< std::endl;
            std::cout << "  queue get device global mem size: " << (device.get_info<info::device::global_mem_size>()/(1024*1024*1024))<< " GB "<< std::endl;
            std::cout << "  queue get device global mem cache size: " <<(device.get_info<info::device::global_mem_cache_size>()/(1024*1024))<<" MB "<< std::endl;
            std::cout << "  queue get device max mem alloc size: " << (device.get_info<info::device::max_mem_alloc_size>()/(1024*1024*1024))<< " GB "<< std::endl;
            std::cout << "  queue get device object address : " << &device << std::endl;                    
    }
    auto ctxt = q.get_context();
 
    auto platforms = sycl::platform::get_platforms();
    for (auto &platform : platforms) {
        std::cout << "Platform name : " << platform.get_info<sycl::info::platform::name>()<< std::endl;
        std::cout << "Platform profile : " << platform.get_info<sycl::info::platform::profile>()<<std::endl;
        std::cout << "Platform version : " << platform.get_info<sycl::info::platform::version>()<<std::endl;
        std::cout << "Platform vendor : " << platform.get_info<sycl::info::platform::vendor>()<<std::endl;
        std::cout << "Platform object address : " << &platform<< std::endl;            
        
        auto devices = platform.get_devices(sycl::info::device_type::gpu);
        for (auto &device : devices) {
            std::cout << "  Device: " << device.get_info<sycl::info::device::name>() << std::endl;
            std::cout << "  Device vendor id: " << device.get_info<sycl::info::device::vendor_id>()<< std::endl;
            std::cout << "  queue get device max compute units : " << device.get_info<info::device::max_compute_units>()<< std::endl;
            std::cout << "  queue get device global mem size: " << (device.get_info<info::device::global_mem_size>()/(1024*1024*1024))<< " GB "<< std::endl;
            std::cout << "  queue get device global mem cache size: " <<(device.get_info<info::device::global_mem_cache_size>()/(1024*1024))<<" MB "<< std::endl;
            std::cout << "  queue get device max mem alloc size: " << (device.get_info<info::device::max_mem_alloc_size>()/(1024*1024*1024))<< " GB "<< std::endl;
            std::cout << "  Device object address : " << &device << std::endl;                    
        }
    }

    return 0;
}