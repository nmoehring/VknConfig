#pragma once

#include <vulkan/vulkan.h>

#include "VknInfos.hpp"
#include "VknResult.hpp"

namespace vkn
{
    class VknImageView
    {
    public:
        VknImageView();
        VknImageView(uint32_t deviceIdx, uint32_t swapchainIdx, uint32_t imageViewIdx, VkDevice *device, const bool *deviceCreated, VknInfos *infos, VknResultArchive *archive, VkImageView *imageView);

        void fillCreateFlags(VkImageViewCreateFlags);
        void setImage(VkImage image);
        void setViewType(VkImageViewType viewType);
        void setFormat(VkFormat format);
        void setComponents(VkComponentMapping components);
        void setSubresourceRange(VkImageSubresourceRange subresourceRange);
        void fillImageViewCreateInfo();

        void createImageView();

    private:
        uint32_t m_deviceIdx;
        uint32_t m_swapchainIdx;
        uint32_t m_imageViewIdx;
        VknInfos *m_infos;
        VknResultArchive *m_archive;
        bool m_placeholder;
        VkDevice *m_vkDevice;
        const bool *m_vkDeviceCreated;

        VkImageView *m_imageView{};

        VkImageViewCreateFlags m_createFlags{0};
        VkImage m_image{};
        VkImageViewType m_viewType{};
        VkFormat m_format{};
        VkComponentMapping m_components{};
        VkImageSubresourceRange m_subresourceRange{};

        bool m_filledCreateInfo{false};
        bool m_createdImageView{false};
    };
}