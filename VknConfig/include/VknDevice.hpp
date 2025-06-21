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
#include <vma/vk_mem_alloc.h>

#include "VknObject.hpp"
#include "VknRenderpass.hpp"
#include "VknPhysicalDevice.hpp"
#include "VknSwapchain.hpp"
#include "VknData.hpp"
#include "VknCommandPool.hpp"
#include "VknBuffer.hpp"

namespace vkn
{
    class VknDevice : public VknObject
    {
    public:
        // Overloads
        VknDevice() = default;
        VknDevice(VknIdxs relIdxs, VknIdxs absIdxs);

        // Members
        VknSwapchain *addSwapchain(uint32_t swapchainIdx);
        VknRenderpass *addRenderpass(uint32_t newRenderpassIdx);
        VknCommandPool *addCommandPool(uint32_t newCommandPoolIdx);
        VmaAllocator *addAllocator();
        // Buffer creation methods now return pointers and take VkDeviceSize
        VknVertexBuffer *addVertexBuffer(VkDeviceSize size);
        VknIndexBuffer *addIndexBuffer(VkDeviceSize size);
        VknCpuUniformBuffer *addCpuUniformBuffer(VkDeviceSize size);
        VknGpuUniformBuffer *addGpuUniformBuffer(VkDeviceSize size);
        VknStorageBuffer *addStorageBuffer(VkDeviceSize size);
        VknIndirectBuffer *addIndirectBuffer(VkDeviceSize size);
        VknFeatures *features{nullptr};

        // Config
        void createSyncObjects(uint32_t maxFramesInFlight);
        uint32_t findGraphicsQueue();
        void addExtension(std::string extension);
        void setPresentable(bool presentable) { m_presentable = presentable; }

        // Create
        VknResult createDevice();

        // Getters
        VkQueue *getGraphicsQueue(uint32_t index = 0);
        VknPhysicalDevice *getPhysicalDevice();
        VknSwapchain *getSwapchain(uint32_t swapchainIdx);
        VknRenderpass *getRenderpass(uint32_t renderpassIdx);
        VknCommandPool *getCommandPool(uint32_t commandPoolIdx);
        VkDevice *getVkDevice();
        VkSemaphore &getImageAvailableSemaphores(uint32_t frameInFlight);
        VkSemaphore &getRenderFinishedSemaphores(uint32_t frameInFlight);
        VkFence &getInFlightFences(uint32_t frameInFlight);
        VknIdxs &getRelIdxs() { return m_relIdxs; }
        VkSemaphore &getImageAvailableSemaphore(uint32_t frameInFlight);
        VkSemaphore &getRenderFinishedSemaphore(uint32_t frameInFlight);
        VkFence &getFence(uint32_t frameInFlight);

    private:
        // Members
        std::list<VknRenderpass> m_renderpasses{};
        std::list<VknSwapchain> m_swapchains{};
        std::list<VknPhysicalDevice> m_physicalDevices{};
        std::list<VknCommandPool> m_commandPools{};
        VkQueue m_lastUsedGraphicsQueue{}; // Store graphics queue handle
        std::list<VknVertexBuffer> m_vertexBuffers;
        std::list<VknIndexBuffer> m_indexBuffers;
        std::list<VknCpuUniformBuffer> m_cpuUniformBuffers;
        std::list<VknGpuUniformBuffer> m_gpuUniformBuffers;
        std::list<VknStorageBuffer> m_storageBuffers;
        std::list<VknIndirectBuffer> m_indirectBuffers;

        // Params
        const char *const *m_extensions{nullptr};
        uint32_t m_extensionsSize{0};
        VmaVulkanFunctions m_vmaVulkanFunctions{};

        // State
        bool m_createdVkDevice{false};
        bool m_commandPoolCreated{false};
        bool m_commandBuffersAllocated{false};
        bool m_syncObjectsCreated{false};
        bool m_filedQueueCreateInfos{false};
        VknInstanceLock<VknDevice> m_instanceLock;
        bool m_swapchainExtensionEnabled{false};
        bool m_allocatorAdded{false};
        bool m_addedVmaFunctions{false};
        bool m_presentable{false};

        // For correct sync object retrieval
        uint32_t m_imageAvailableSemaphoreStartIdx{0};
        uint32_t m_renderFinishedSemaphoreStartIdx{0}; // Could combine with IA if strictly interleaved
        uint32_t m_inFlightFenceStartIdx{0};
        uint32_t m_maxFramesInFlightForSyncObjects{0};
    };
}