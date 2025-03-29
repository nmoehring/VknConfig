/**
 * @file VknPhysicalDevice.hpp
 * @brief Manages a Vulkan physical device and its properties.
 *
 * VknPhysicalDevice is a hierarchy-bound class within the VknConfig project.
 * It is used by VknDevice to manage a Vulkan physical device,
 * enumerate its properties, and select queue families.
 * VknPhysicalDevice depends on VknEngine, VknInfos, VknQueueFamily, and VknIdxs.
 * VknPhysicalDevice is a child of VknDevice.
 *
 * Hierarchy Graph:
 * [VknConfig] (Top-Level)
 *     |
 *     +-- [VknDevice] (Hierarchy-Bound)
 *         |
 *         +-- [VknPhysicalDevice] (Hierarchy-Bound) <<=== YOU ARE HERE
 *         |   |
 *         |   +-- [VknQueueFamily] (Hierarchy-Bound Leaf)
 *         |
 *         +-- [VknSwapchain] (Hierarchy-Bound)
 *         |   |
 *         |   +-- [VknImageView] (Hierarchy-Bound Leaf)
 *         |
 *         +-- [VknRenderpass] (Hierarchy-Bound)
 *             |
 *             +-- [VknPipeline] (Hierarchy-Bound)
 *                 |
 *                 +-- [VknVertexInputState] (Hierarchy-Bound Leaf)
 *                 +-- [VknInputAssemblyState] (Hierarchy-Bound Leaf)
 *                 +-- [VknMultisampleState] (Hierarchy-Bound Leaf)
 *                 +-- [VknRasterizationState] (Hierarchy-Bound Leaf)
 *                 +-- [VknShaderStage] (Hierarchy-Bound Leaf)
 *                 +-- [VknViewportState] (Hierarchy-Bound Leaf)
 *
 * [VknEngine] (Free/Top-Level)
 * [VknInfos] (Free/Top-Level)
 * [VknResult] (Free/Top-Level)
 */

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