#include "VknPhysicalDevice.hpp"
#include "VknResult.hpp"
#include <iostream>

namespace vkn
{
    VknResult VknPhysicalDevice::selectPhysicalDevice(VkInstance *instance)
    {
        uint32_t deviceCount{0};
        std::vector<VkPhysicalDevice> devices;

        VknResult res1{vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr),
                       "Enumerate physical devices."};
        m_archive->store(res1);
        if (deviceCount == 0)
            throw std::runtime_error(res1.toErr("No GPU's supporting Vulkan found."));
        else if (deviceCount > 1)
            std::cerr << "Found more than one GPU supporting Vulkan. Selecting device at index 0." << std::endl;

        devices.resize(deviceCount);
        VknResult res2{vkEnumeratePhysicalDevices(*instance, &deviceCount, devices.data()),
                       "Enum physical devices and store."};
        m_device = devices[0];

        return res2;
    }
}