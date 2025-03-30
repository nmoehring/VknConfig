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

namespace vkn
{
    class VknFramebuffer
    {
    public:
        // Overloads
        VknFramebuffer() = default;
        VknFramebuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        // Config
        void setDimensions(uint32_t width, uint32_t height);
        void setNumLayers(uint32_t numLayers);
        void setCreateFlags(VkFramebufferCreateFlags createFlags);
        void setAttachments(std::vector<VkImageView> *vkImageViews);

        // Create
        void fillFramebufferCreateInfo();
        void createFramebuffer();

    private:
        // Engine
        VknEngine *m_engine{nullptr};
        VknIdxs m_relIdxs{};
        VknIdxs m_absIdxs{};
        VknInfos *m_infos{nullptr};

        // Params
        std::vector<VkImageView> *m_attachments{};
        uint32_t m_width{800};
        uint32_t m_height{600};
        uint32_t m_numLayers{1};
        VkFramebufferCreateFlags m_createFlags{0};

        // State
        bool m_filledCreateInfo{false};
        bool m_createdFramebuffer{false};
        bool m_attachmentsSet{false};
    };
}