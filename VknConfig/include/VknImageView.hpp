/**
 * @file VknImageView.hpp
 * @brief Wraps a Vulkan VkImageView object and provides methods for creating and configuring image views.
 *
 * VknImageView is a hierarchy-bound leaf class within the VknConfig project.
 * It is primarily used by VknSwapchain to create image views for the swapchain images.
 * It requires a VkImage (which can be provided by VknImage or VknSwapchain) to be created.
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
 *                 |   +-- [VknDescriptorSetLayout] ^ / \
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
 * [VknImage] (Free/Top-Level)
 */

#pragma once

#include <vulkan/vulkan.h>

#include "VknImage.hpp"
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
        void setImage(VkImage *image);
        void setImage(VknImage *image);
        void setViewType(VkImageViewType viewType);
        void setFormat(VkFormat format);
        void setComponents(VkComponentMapping components);
        void setSubresourceRange(VkImageSubresourceRange subresourceRange);

        // Create
        void fillImageViewCreateInfo();
        void createImageView();

        // Get
        VkImageView *getImageView();

    private:
        // Engine
        VknEngine *m_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Members
        VkImage *m_vkImage{VK_NULL_HANDLE};
        VknImage *m_image{nullptr};

        // Params
        VkImageViewCreateFlags m_createFlags{0};
        VkImageViewType m_viewType{VK_IMAGE_VIEW_TYPE_2D};
        VkFormat m_format{VK_FORMAT_UNDEFINED};
        VkComponentMapping m_components{// Default identity mapping
                                        VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                                        VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
        VkImageSubresourceRange m_subresourceRange{// Default single color level/layer
                                                   VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

        // State
        bool m_filledCreateInfo{false};
        bool m_createdImageView{false};
        bool m_setVkImage{false};
    };
}