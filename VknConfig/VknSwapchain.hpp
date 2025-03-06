#pragma once

#include <vulkan/vulkan.h>

#include "VknInfos.hpp"
#include "VknResult.hpp"
#include "VknImageView.hpp"

namespace vkn
{
    class VknSwapchain
    {
    public:
        VknSwapchain();
        VknSwapchain(uint32_t deviceIdx, uint32_t swapchainIdx, VkDevice *vkDevice, const bool *createdVkDevice,
                     VknInfos *m_infos, VknResultArchive *archive, VkSurfaceKHR surface);
        ~VknSwapchain();
        void destroy();

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

        VkSwapchainKHR *getVkSwapchain();

        void fillSwapchainCreateInfo();
        void createSwapchain();
        void getImages();
        void createImageViews();

    private:
        uint32_t m_deviceIdx;
        uint32_t m_swapchainIdx;
        VknInfos *m_infos;
        VknResultArchive *m_archive;
        VkDevice *m_vkDevice;
        const bool *m_createdVkDevice;
        uint32_t m_imageCount;
        VkExtent2D m_dimensions;
        bool m_placeholder;

        VkSurfaceKHR m_surface{};
        std::vector<VknImageView> m_imageViews{};
        std::vector<VkImage> m_images{};

        bool m_filledCreateInfo{false};
        bool m_destroyed{false};
        bool m_createdSwapchain{false};
        VkSurfaceFormatKHR m_surfaceFormat = VkSurfaceFormatKHR{VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
        uint32_t m_numImageArrayLayers = 1;
        VkImageUsageFlags m_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        VkSharingMode m_sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        VkSurfaceTransformFlagBitsKHR m_preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        VkCompositeAlphaFlagBitsKHR m_compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        VkPresentModeKHR m_presentMode = VK_PRESENT_MODE_FIFO_KHR;
        VkBool32 m_clipped = VK_TRUE;
        VkSwapchainKHR m_oldSwapchain = VK_NULL_HANDLE;

        VkSwapchainKHR m_vkSwapchain = VK_NULL_HANDLE;
    };
}