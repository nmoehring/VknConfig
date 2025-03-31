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
 *     +-- [VknDevice]
 *         |
 *         +-- [VknPhysicalDevice]  <<=== YOU ARE HERE
 *         |   |
 *         |   +-- [VknQueueFamily] ^ / \
 *         |
 *         +-- [VknSwapchain]
 *         |   |
 *         |   +-- [VknImageView] ^ / \
 *         |
 *         +-- [VknRenderpass]
 *             |
 *             +-- [VknFramebuffer] ^ / \
 *             |
 *             +-- [VknPipeline]
 *                 |
 *                 +-- [VknVertexInputState] ^ / \
 *                 +-- [VknInputAssemblyState] ^ / \
 *                 +-- [VknMultisampleState] ^ / \
 *                 +-- [VknRasterizationState] ^ / \
 *                 +-- [VknShaderStage] ^ / \
 *                 +-- [VknViewportState] ^ / \
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
        VknEngine *m_engine{nullptr}; // external ptr
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos{nullptr}; // external ptr

        // Members
        static std::vector<VkPhysicalDeviceProperties> s_properties;
        std::list<VknQueueFamily> m_queues{}; // Vector fine, this shouldn't change.

        // State
        bool m_selectedPhysicalDevice{false};
        static bool s_enumeratedPhysicalDevices;
        bool m_requestedQueues{false};
        bool m_selectedQueues{false};
        bool m_filledQueueCreateInfos{false};
    };
}