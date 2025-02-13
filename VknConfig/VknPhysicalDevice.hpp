#pragma once
#include <vulkan/vulkan.h>

#include "VknResult.hpp"
#include "VknInfos.hpp"

namespace vkn
{
    class VknPhysicalDevice
    {
    public:
        VknPhysicalDevice() {}
        VknPhysicalDevice(VknResultArchive *archive, VknInfos *infos);
        VkPhysicalDevice getVkPhysicalDevice() { return m_physicalDevice; };
        VkPhysicalDeviceProperties getProperties() { return m_properties; };
        bool getSurfaceSupport(VkSurfaceKHR &surface, uint32_t queueFamilyIdx);
        void addInstance(VkInstance *instance);
        VknResult selectPhysicalDevice();

    private:
        VkInstance *m_instance{nullptr};
        VkPhysicalDevice m_physicalDevice{};
        VkPhysicalDeviceProperties m_properties{};
        VknResultArchive *m_archive{nullptr};
        VknInfos *m_infos{nullptr};
        bool m_instanceAdded{false};
        void queryProperties() { vkGetPhysicalDeviceProperties(m_physicalDevice, &m_properties); }
    };

}