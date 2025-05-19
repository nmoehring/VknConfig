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

#include <memory>

#include "VknRenderpass.hpp"
#include "VknPhysicalDevice.hpp"
#include "VknSwapchain.hpp"
#include "VknData.hpp"
#include "VknCommandPool.hpp"

namespace vkn
{
    class VknDevice
    {
    public:
        // Overloads
        VknDevice() = default;
        VknDevice(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        // Features
        VknFeatures features{};

        // Members
        VknSwapchain *addSwapchain(uint32_t swapchainIdx);
        VknRenderpass *addRenderpass(uint32_t newRenderpassIdx);
        VknCommandPool *addCommandPool(uint32_t newCommandPoolIdx);

        // Config
        void createSyncObjects(uint32_t maxFramesInFlight);
        uint32_t findGraphicsQueue();

        // Getters for rendering loop
        void addExtensions(const char *ext[], uint32_t size);

        // Create
        VknResult createDevice();

        // Getters
        VknPhysicalDevice *getPhysicalDevice();
        VknSwapchain *getSwapchain(uint32_t swapchainIdx);
        VknRenderpass *getRenderpass(uint32_t renderpassIdx);
        VknCommandPool *getCommandPool(uint32_t commandPoolIdx);
        VkDevice *getVkDevice();
        VkQueue getGraphicsQueue() const;                                                              // Add getter for graphics queue
        std::vector<VkSemaphore> &getImageAvailableSemaphores() { return m_imageAvailableSemaphores; } // Add getter
        std::vector<VkSemaphore> &getRenderFinishedSemaphores() { return m_renderFinishedSemaphores; } // Add getter
        std::vector<VkFence> &getInFlightFences() { return m_inFlightFences; }                         // Add getter
        VknIdxs &getRelIdxs() { return m_relIdxs; }
        VkSemaphore &getImageAvailableSemaphore(uint32_t frameInFlight);
        VkSemaphore &getRenderFinishedSemaphore(uint32_t frameInFlight);
        VkFence &getFence(uint32_t frameInFlight);

    private:
        // Engine
        VknEngine *m_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Members
        std::list<VknRenderpass> m_renderpasses{};        // List, because elements don't need to be together, refs could be invalidated
        std::list<VknSwapchain> m_swapchains{};           // Doesn't need to change after creation
        std::list<VknPhysicalDevice> m_physicalDevices{}; // List, because elements don't need to be together, refs could be invalidated
        std::list<VknCommandPool> m_commandPools{};
        std::list<VknPhysicalDevice> m_physicalDevices{};

        // Params
        const char *const *m_extensions{nullptr};
        uint32_t m_extensionsSize{0};

        // State
        std::vector<VkSemaphore> m_imageAvailableSemaphores;
        std::vector<VkSemaphore> m_renderFinishedSemaphores;
        std::vector<VkFence> m_inFlightFences;
        bool m_createdVkDevice{false};
        bool m_commandPoolCreated{false};
        bool m_commandBuffersAllocated{false};
        bool m_syncObjectsCreated{false};
        VkQueue m_graphicsQueue = VK_NULL_HANDLE; // Store graphics queue handle
        bool m_filledQueueCreateInfos{false};
    };
}