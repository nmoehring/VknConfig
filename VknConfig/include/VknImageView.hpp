/**
 * @file VknImageView.hpp
 * @brief Wraps a Vulkan VkImageView object and provides methods for creating and configuring image views.
 *
 * VknImageView is a hierarchy-bound leaf class within the VknConfig project.
 * It is primarily used by VknSwapchain to create image views for the swapchain images.
 * However, it could potentially be used by other classes that need to create image views.
 * VknImageView depends on VknEngine, VknInfos, and VknIdxs.
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
 *         |   +-- [VknImageView] ^ / \ <<=== YOU ARE HERE
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

#include <vulkan/vulkan.h>

#include "VknInfos.hpp"
#include "VknResult.hpp"

namespace vkn
{
    class VknImageView
    {
    public:
        // Overloads
        VknImageView() = default;
        VknImageView(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        // Config
        void setCreateFlags(VkImageViewCreateFlags);
        void setImage(VkImage image);
        void setViewType(VkImageViewType viewType);
        void setFormat(VkFormat format);
        void setComponents(VkComponentMapping components);
        void setSubresourceRange(VkImageSubresourceRange subresourceRange);

        // Create
        void fillImageViewCreateInfo();
        void createImageView();

    private:
        // Engine
        VknEngine *m_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Params
        VkImageViewCreateFlags m_createFlags{0};
        VkImage m_image{};
        VkImageViewType m_viewType{};
        VkFormat m_format{};
        VkComponentMapping m_components{};
        VkImageSubresourceRange m_subresourceRange{};

        // State
        bool m_filledCreateInfo{false};
        bool m_createdImageView{false};
        bool m_setImage{false};
        bool m_setViewType{false};
        bool m_setFormat{false};
    };
}