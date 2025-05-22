/**
 * @file VknFramebuffer.hpp
 * @brief Wraps a Vulkan VkFramebuffer object and provides methods for creating and configuring framebuffers.
 *
 * VknFramebuffer is a hierarchy-bound leaf class within the VknConfig project.
 * It is used by VknRenderpass to create framebuffer objects.
 * VknFramebuffer depends on VknEngine, VknInfos, VknIdxs, and VkImageView.
 * It does not have any classes that depend on it.
 *
 * Hierarchy Graph:
 * [VknConfig] (Top-Level)
 *     |
 *     +-- [VknDevice]
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
 *             +-- [VknFramebuffer] ^ / \ <<=== YOU ARE HERE
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

#include <vulkan/vulkan.h>

#include "VknInfos.hpp"
#include "VknResult.hpp"
#include "VknEngine.hpp"
#include "VknData.hpp"
#include "VknFramebuffer.hpp"
#include "VknImage.hpp"
#include "VknImageView.hpp"
#include "VknSwapchain.hpp"

namespace vkn
{
    class VknFramebuffer
    {
    public:
        // Overloads
        VknFramebuffer() = default;
        VknFramebuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        // Members
        void addAttachments();
        void addSwapchainImageView(VknImageView *swapchainImageView);

        // Config
        void setDimensions(uint32_t width, uint32_t height);
        void setDimensions(VknSwapchain &swapchain);
        void setNumLayers(uint32_t numLayers);
        void setCreateFlags(VkFramebufferCreateFlags createFlags);
        void setSwapchainAttachmentDescriptionIndex(uint32_t descriptionIndex);
        void setAttachmentSettings();

        // Create
        void createFramebuffer();
        void createAttachments();
        void demolishFramebuffer();
        void recreateFramebuffer(VknSwapchain &swapchain);

        // Get
        VkFramebuffer *getVkFramebuffer()
        {
            return &m_engine->getObject<VkFramebuffer>(m_absIdxs);
        } // Add getter
        bool isSwapchainImage(uint32_t i);
        VknVectorIterator<VkImageView> getAttachmentImageViews();

    private:
        // Engine
        VknEngine *m_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Members
        std::list<VknImage> m_attachImages{};
        std::list<VknImageView> m_attachViews{};
        VkImage *m_swapchainVkImage{nullptr};
        VknImageView *m_swapchainImageView{nullptr};

        // Params
        uint32_t m_width{640};
        uint32_t m_height{480};
        uint32_t m_numLayers{1};
        VkFramebufferCreateFlags m_createFlags{0};

        // State
        bool m_createdFramebuffer{false};
        bool m_addedAttachments{false};
        bool m_createdAttachments{false};
        bool m_setAttachmentSettings{false};
        uint32_t m_imageStartIdx{0};
        uint32_t m_imageViewStartIdx{0};
        VknInstanceLock<VknFramebuffer> m_instanceLock;
        std::optional<uint32_t> m_swapchainAttachmentDescIndex; // Index of the VkAttachmentDescription for the swapchain
    };
}