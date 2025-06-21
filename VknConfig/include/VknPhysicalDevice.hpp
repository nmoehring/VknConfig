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
#include "VknObject.hpp"

namespace vkn
{
    class VknDevice;

    class VknPhysicalDevice : public VknObject
    {
    public:
        friend VknDevice;

        // Constructor now takes VkSurfaceKHR for selection
        VknPhysicalDevice() = default;
        VknPhysicalDevice(VknIdxs relIdxs, VknIdxs absIdxs);
        ~VknPhysicalDevice();

        VkPhysicalDevice *getVkPhysicalDevice();
        uint32_t getPhysicalDeviceAbsIdx() { return m_absIdxs.get<VkPhysicalDevice>(); }
        VkPhysicalDeviceProperties &getProperties();
        VknVector<VkPhysicalDeviceProperties> &getAllProperties();
        bool getSurfaceSupport(VkSurfaceKHR surface, uint32_t queueFamilyIdx); // surface passed by value
        VkPhysicalDevice *selectPhysicalDevice();                              // Takes surface for selection
        VkPhysicalDeviceLimits *getLimits();
        void fileDeviceQueuePriorities(uint32_t queueFamilyIdx, VknVector<float> priorities);
        void fileDeviceQueuePrioritiesDefault();
        bool areQueuePrioritiesFiled();
        int getNumQueueFamilies() { return s_queues.size(); }
        std::list<VknQueueFamily> &getQueues() { return s_queues; }
        VknQueueFamily &getQueue(int idx);
        VknIdxs getPhysicalDeviceAbsIdxs() { return m_absIdxs; }
        VkPhysicalDeviceType getGPUType();

    private:
        // Members
        static VknVector<VkPhysicalDeviceProperties> s_properties;
        static std::list<VknQueueFamily> s_queues; // Vector fine, this shouldn't change.
        static VknVector<uint_fast32_t> s_deviceQueuePropStartPos;
        static VknVector<uint_fast32_t> s_numQueueFamilies;

        // State
        bool m_selectedPhysicalDevice{false};
        static bool s_enumeratedPhysicalDevices;
        static bool s_requestedQueues;
        static uint_fast32_t physDevCount;
        bool m_filedQueueCreateInfos{false};
        bool m_filedQueuePriorities{false};

        void enumeratePhysicalDevices();
        void requestQueueFamilyProperties();
        void fileQueueCreateInfos();
    };
}