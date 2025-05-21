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
 *                 +-- [VknPipelineLayout]
 *                 |   |
 *                 |   +-- [VknDescriptorSetLayout]
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
#include <iostream>

#include <vulkan/vulkan.h>
#include "VknQueueFamily.hpp"
#include "VknData.hpp"

namespace vkn
{
    class VknDevice;

    class VknPhysicalDevice
    {
    public:
        friend VknDevice;

        VknPhysicalDevice() = default;
        VknPhysicalDevice(
            VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);
        ~VknPhysicalDevice();

        VkPhysicalDevice *getVkPhysicalDevice();
        VkPhysicalDeviceProperties &getProperties();
        VknVector<VkPhysicalDeviceProperties> &getAllProperties();
        bool getSurfaceSupport(VkSurfaceKHR &surface, uint32_t queueFamilyIdx);
        VkPhysicalDevice *selectPhysicalDevice();
        VkPhysicalDeviceLimits *getLimits();
        void fillDeviceQueuePriorities(uint32_t queueFamilyIdx, VknVector<float> priorities);
        void fillDeviceQueuePrioritiesDefault();
        bool areQueuesSelected() { return m_selectedQueues; }
        bool areQueuePrioritiesFilled();
        void selectQueues(bool chooseAllAvailableQueues);
        int getNumQueueFamilies() { return m_queues.size(); }
        std::list<VknQueueFamily> &getQueues() { return m_queues; }
        VknQueueFamily &getQueue(int idx);

    private:
        // Engine
        VknEngine *m_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Members
        static VknVector<VkPhysicalDeviceProperties> s_properties;
        std::list<VknQueueFamily> m_queues{}; // Vector fine, this shouldn't change.

        // State
        uint32_t m_startAbsIdx{0};
        bool m_selectedPhysicalDevice{false};
        static bool s_enumeratedPhysicalDevices;
        bool m_requestedQueues{false};
        bool m_selectedQueues{false};
        bool m_filledQueueCreateInfos{false};
        bool m_filledQueuePriorities{false};

        void enumeratePhysicalDevices();
        void requestQueueFamilyProperties();
        void fillQueueCreateInfos();
    };
}