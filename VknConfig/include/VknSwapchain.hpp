/**
 * @file VknSwapchain.hpp
 * @brief Manages a Vulkan swapchain and its associated image views.
 *
 * VknSwapchain is a hierarchy-bound class within the VknConfig project.
 * It is used by VknDevice to manage a Vulkan swapchain and its associated image views.
 * VknSwapchain depends on VknEngine, VknInfos, VknImageView, and VknIdxs.
 * VknSwapchain is a child of VknDevice.
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
 *         +-- [VknSwapchain]  <<=== YOU ARE HERE
 *         |   |
 *         |   +-- [VknImageView] ^ / \
 *         |
 *         +-- [VknRenderpass]
 *             |
 *             +-- [VknFramebuffer] ^ / \
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

#include "VknImageView.hpp"

namespace vkn
{
    class VknSwapchain
    {
    public:
        // Overloads
        VknSwapchain() = default;
        VknSwapchain(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *m_infos);

        // Add
        void addImageViews();

        // Config
        void setImageCount(uint32_t imageCount);
        void setImageDimensions(uint32_t imageWidth, uint32_t imageHeight);
        void setSurfaceFormat(VkFormat format, VkColorSpaceKHR colorSpace);
        void setNumImageLayers(uint32_t numImageLayers);
        void setUsage(VkImageUsageFlags usage);
        void setSharingMode(VkSharingMode sharingMode);
        void setPreTransform(VkSurfaceTransformFlagBitsKHR preTransform);
        void setCompositeAlpha(VkCompositeAlphaFlagBitsKHR compositeAlpha);
        void setPresentMode(VkPresentModeKHR presentMode);
        void setClipped(bool clipped);
        void setOldSwapchain(VkSwapchainKHR oldSwapchain);
        void setSurface(uint32_t surfaceIdx);

        // Create
        void fillSwapchainCreateInfo();
        void createSwapchain();
        void createImageViews();

        // Getters
        VkSwapchainKHR *getVkSwapchain();
        std::vector<VkImageView> *getVkImageViews();
        void getImages();

    private:
        // Engine
        VknEngine *m_engine{nullptr};
        VknIdxs m_relIdxs{};
        VknIdxs m_absIdxs{};
        VknInfos *m_infos{nullptr};

        // Members
        std::list<VknImageView> m_imageViews{};
        std::vector<VkImage> m_images{};
        std::vector<VkImageView> m_rawImageViews{};

        // Params
        VkExtent2D m_dimensions{VkExtent2D{640, 480}};
        VkSurfaceFormatKHR m_surfaceFormat{VkSurfaceFormatKHR{VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}};
        uint32_t m_numImageArrayLayers{1};
        VkImageUsageFlags m_usage{VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT};
        VkSharingMode m_sharingMode{VK_SHARING_MODE_EXCLUSIVE};
        VkSurfaceTransformFlagBitsKHR m_preTransform{VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR};
        VkCompositeAlphaFlagBitsKHR m_compositeAlpha{VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR};
        VkPresentModeKHR m_presentMode{VK_PRESENT_MODE_FIFO_KHR};
        VkBool32 m_clipped{VK_TRUE};
        VkSwapchainKHR m_oldSwapchain{VK_NULL_HANDLE};
        uint32_t m_imageCount{1};
        std::optional<uint32_t> m_surfaceIdx{};

        // State
        uint32_t m_startAbsIdx{0};
        bool m_filledCreateInfo{false};
        bool m_createdSwapchain{false};
        bool m_setSurface{false};
        bool m_setImageCount{false};

        void resizeImageVectors();
    };
}