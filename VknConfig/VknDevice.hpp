// I decided to use a Vkn prefix to stand apart from Vk prefixes of the API
// All the structs are in info.hpp

#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

#include "vknInfos.hpp"
#include "VknResult.hpp"
#include "VknPhysicalDevice.hpp"
#include "VknQueueFamily.hpp"

namespace vkn
{
    class VknDevice
    {
    public:
        VknDevice();
        VknDevice(VkPhysicalDevice dev, VknInfos *infos);
        //~VknDevice();
        VknResult createDevice();
        void setPhysicalDevice(VkPhysicalDevice dev) { m_physicalDevice = VknPhysicalDevice(dev); }

        VkPhysicalDeviceProperties getPhysicalDeviceProperties()
        {
            return m_physicalDevice.getProperties();
        }
        std::vector<VknQueueFamily> getQueues() { return m_queues; }

    private:
        VkDevice m_logicalDevice;
        VknPhysicalDevice m_physicalDevice;
        std::vector<VknQueueFamily> m_queues;
        std::vector<VknResult> m_resultArchive;
        VknInfos *m_infos = nullptr;

        // Other utilities
        void archiveResult(VknResult res);

        // Init
        void requestQueueFamilyProperties();
    };
}