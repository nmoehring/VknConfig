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
        VknDevice(VkInstance *instance, VknInfos *infos, VknResultArchive *archive);
        ~VknDevice();
        VknResult createDevice();

        VkPhysicalDeviceProperties getPhysicalDeviceProperties()
        {
            return m_physicalDevice.getProperties();
        }
        std::vector<VknQueueFamily> getQueues() { return m_queues; }
        VknQueueFamily getQueue(int idx) { return m_queues[idx]; }
        VkDevice *getVkDevice() { return &m_logicalDevice; }
        VknPhysicalDevice *getPhysicalDevice() { return &m_physicalDevice; }
        void destroy();

    private:
        VkDevice m_logicalDevice{};
        VknPhysicalDevice m_physicalDevice{};
        std::vector<VknQueueFamily> m_queues{};
        VknResultArchive *m_resultArchive{nullptr};
        VknInfos *m_infos{nullptr};
        bool m_destroyed{false};

        // Other utilities
        void archiveResult(VknResult res);

        // Init
        void requestQueueFamilyProperties();
    };
}