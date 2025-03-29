#pragma once

#include <list>

#include <vulkan/vulkan.h>
#include "VknQueueFamily.hpp"

namespace vkn
{
    class VknPhysicalDevice
    {
    public:
        VknPhysicalDevice() = default;
        VknPhysicalDevice(
            VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        VkPhysicalDevice *getVkPhysicalDevice();
        VkPhysicalDeviceProperties &getProperties();
        std::vector<VkPhysicalDeviceProperties> &getAllProperties();
        bool getSurfaceSupport(VkSurfaceKHR &surface, uint32_t queueFamilyIdx);
        VknResult enumeratePhysicalDevices();
        VknResult selectPhysicalDevice();
        VkPhysicalDeviceLimits *getLimits();
        void queryProperties();
        void requestQueueFamilyProperties();
        void fillDeviceQueuePriorities(uint32_t queueFamilyIdx, std::vector<float> priorities);
        void fillDeviceQueuePrioritiesDefault();
        void fillQueueCreateInfos();
        bool areQueuesSelected() { return m_selectedQueues; }
        void selectQueues(bool chooseAllAvailableQueues);
        int getNumQueueFamilies() { return m_queues.size(); }
        std::vector<VknQueueFamily> &getQueues() { return m_queues; }
        VknQueueFamily &getQueue(int idx);

    private:
        // Engine
        VknEngine *m_engine; // external ptr
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos; // external ptr

        // Wrapped object
        VkPhysicalDevice m_vkPhysicalDevice{}; // ptr to static list member, won't be invalidated

        // Members
        static std::vector<VkPhysicalDevice> s_physicalDevices; // changes don't invalidate member ptrs
        static std::vector<VkPhysicalDeviceProperties> s_properties;
        // std::vector<VkQueue> m_queues{};
        std::vector<VknQueueFamily> m_queues{}; // Vector fine, this shouldn't change.

        // State
        bool m_selectedPhysicalDevice{false};
        static bool s_enumeratedPhysicalDevices;
        bool m_requestedQueues{false};
        bool m_selectedQueues{false};
        bool m_filledQueueCreateInfos{false};
        static uint32_t s_deviceCount;
    };

}