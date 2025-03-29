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
 *     +-- [VknDevice] (Hierarchy-Bound) <<=== YOU ARE HERE
 *         |
 *         +-- [VknPhysicalDevice] (Hierarchy-Bound)
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

#include <vector>
#include <memory>

#include "VknRenderpass.hpp"
#include "VknPhysicalDevice.hpp"
#include "VknSwapchain.hpp"

namespace vkn
{
    class VknDevice
    {
    public:
        VknDevice() = default;
        VknDevice(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        void addExtensions(const char *ext[], uint32_t size);
        VknResult createDevice();

        void setSwapchainDimensions();
        void fillSwapchainCreateInfos();
        void addSwapchain(uint32_t swapchainIdx, VkSurfaceKHR *surface, uint32_t imageCount,
                          uint32_t imageWidth, uint32_t imageHeight);

        void addRenderpass(uint32_t newRenderpassIdx);

        VknPhysicalDevice *getPhysicalDevice();
        VknSwapchain *getSwapchain(uint32_t swapchainIdx);
        VknRenderpass *getRenderpass(uint32_t renderpassIdx);
        VkDevice *getVkDevice();

    private:
        // Engine
        VknEngine *m_engine{nullptr};
        VknIdxs m_relIdxs{};
        VknIdxs m_absIdxs{};
        VknInfos *m_infos{nullptr};

        // Members
        std::list<VknRenderpass> m_renderpasses{}; // List, because elements don't need to be together, refs could be invalidated
        std::list<VknSwapchain> m_swapchains{};    // Doesn't need to change after creation
        VknPhysicalDevice m_physicalDevice;

        // Params
        const char *const *m_extensions{nullptr};
        uint32_t m_extensionsSize{0};
        VkPhysicalDeviceFeatures *m_features{nullptr};

        // State
        bool m_createdVkDevice{false};
        bool m_filledQueueCreateInfos{false};
    };
}