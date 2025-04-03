/**
 * @file VknDevice.hpp
 * @brief Manages a Vulkan logical device and its associated resources.
 *
 * VknDevice is a hierarchy-bound class within the VknConfig project.
 * It is responsible for creating and managing a Vulkan logical device,
 * swapchains, and renderpasses.
 * VknDevice depends on VknEngine, VknInfos, VknPhysicalDevice, VknSwapchain, VknRenderpass, and VknIdxs.
 * VknDevice is a child of VknConfig.
 *
 * Hierarchy Graph:
 * [VknConfig] (Top-Level)
 *     |
 *     +-- [VknDevice]  <<=== YOU ARE HERE
 *         |
 *         +-- [VknPhysicalDevice]
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

#include <vector>
#include <memory>

#include "VknRenderpass.hpp"
#include "VknPhysicalDevice.hpp"
#include "VknSwapchain.hpp"
#include "VknCommon.hpp"

namespace vkn
{
    class VknDevice
    {
    public:
        // Overloads
        VknDevice() = default;
        VknDevice(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        // Add
        VknSwapchain *addSwapchain(uint32_t swapchainIdx);
        VknRenderpass *addRenderpass(uint32_t newRenderpassIdx);

        // Config
        void addExtensions(const char *ext[], uint32_t size);

        // Create
        void fillSwapchainCreateInfos();
        VknResult createDevice();

        // Getters
        VknPhysicalDevice *getPhysicalDevice();
        VknSwapchain *getSwapchain(uint32_t swapchainIdx);
        VknRenderpass *getRenderpass(uint32_t renderpassIdx);
        VkDevice *getVkDevice();

    private:
        // Engine
        VknEngine *m_engine;

        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Members
        std::list<VknRenderpass> m_renderpasses{}; // List, because elements don't need to be together, refs could be invalidated
        std::list<VknSwapchain> m_swapchains{};    // Doesn't need to change after creation
        std::list<VknPhysicalDevice> m_physicalDevices{};

        // Params
        const char *const *m_extensions{nullptr};
        uint32_t m_extensionsSize{0};
        VkPhysicalDeviceFeatures *m_features{nullptr};

        // State
        bool m_createdVkDevice{false};
        bool m_filledQueueCreateInfos{false};
    };
}