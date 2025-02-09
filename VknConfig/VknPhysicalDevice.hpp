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
        VknPhysicalDevice(VkInstance *instance, VknResultArchive *archive, VknInfos *infos) : m_archive{archive}
        {
            m_infos = infos;
            VknResult res;
            if (!(res = selectPhysicalDevice(instance)).isSuccess())
                throw std::runtime_error(res.toErr("Failed to get physical devices."));
            m_archive->store(res);
            queryProperties();
        }
        VkPhysicalDevice getVkPhysicalDevice() { return m_physicalDevice; };
        VkPhysicalDeviceProperties getProperties() { return m_properties; };
        bool getSurfaceSupport(VkSurfaceKHR &surface, uint32_t queueFamilyIdx);

    private:
        VkPhysicalDevice m_physicalDevice{};
        VkPhysicalDeviceProperties m_properties{};
        VknResultArchive *m_archive{nullptr};
        VknInfos *m_infos{nullptr};
        VknResult selectPhysicalDevice(VkInstance *instance);
        void queryProperties() { vkGetPhysicalDeviceProperties(m_physicalDevice, &m_properties); }
    };

}