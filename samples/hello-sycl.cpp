#include <CL/sycl.hpp>
#include <iostream>
#include <vector>
using namespace sycl;

int main()
{
    std::cout << "Hello sycl!" << std::endl;

    auto platforms = sycl::platform::get_platforms();
    for (auto &platform : platforms)
    {
        std::cout << "Platform name : " << platform.get_info<sycl::info::platform::name>() << std::endl;
        std::cout << "Platform profile : " << platform.get_info<sycl::info::platform::profile>() << std::endl;
        std::cout << "Platform version : " << platform.get_info<sycl::info::platform::version>() << std::endl;
        std::cout << "Platform vendor : " << platform.get_info<sycl::info::platform::vendor>() << std::endl;

        auto devices = platform.get_devices(sycl::info::device_type::gpu);
        std::cout << "Number of devices : " << devices.size() << std::endl;
        for (auto &device : devices)
        {
            std::cout << "  Device name: " << device.get_info<sycl::info::device::name>() << std::endl;
            std::cout << "  Max compute units : " << device.get_info<info::device::max_compute_units>() << std::endl;
            std::cout << "  Max work group size: " << device.get_info<info::device::max_work_group_size>() << std::endl;
            std::cout << "  Global mem size: " << (device.get_info<info::device::global_mem_size>() / (1024 * 1024 * 1024)) << " GB " << std::endl;
            std::cout << std::endl;
        }
    }

    return 0;
}