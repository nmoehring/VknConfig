#pragma once

#include <vulkan/vulkan.h>
#include "VknResult.hpp"
#include "VknInfos.hpp"

namespace vkn
{
    class VknPhysicalDevice
    {
    public:
        VknPhysicalDevice();
        VknPhysicalDevice(VknResultArchive *archive, VknInfos *infos, const VkInstance *instance, const bool *instanceCreated);
        VkPhysicalDevice *getVkPhysicalDevice() { return &m_physicalDevice; };
        VkPhysicalDeviceProperties *getProperties() { return &m_properties; };
        bool getSurfaceSupport(VkSurfaceKHR &surface, uint32_t queueFamilyIdx);
        void addInstance(VkInstance *instance);
        VknResult selectPhysicalDevice();
        VkPhysicalDeviceLimits *getLimits();
        bool getPhysicalDeviceSelected() { return m_selectedPhysicalDevice; }

    private:
        VkPhysicalDeviceProperties m_properties{};
        VknResultArchive *m_archive{nullptr};
        VknInfos *m_infos{nullptr};
        const VkInstance *m_instance{nullptr};
        const bool *m_instanceCreated{nullptr};

        VkPhysicalDevice m_physicalDevice{};

        bool m_selectedPhysicalDevice{false};
        void queryProperties();
    };

}