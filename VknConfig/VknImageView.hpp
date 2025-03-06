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
        VknImageView(VknResultArchive *archive, VknInfos *infos);

        void fillImageViewCreateInfo();

    private:
        bool m_placeholder;
        VknInfos *m_infos;
        VknResultArchive *m_archive;

        VkImageView m_imageView{};

        VkImageViewCreateFlags flags{0};
        VkImage image{};
        VkImageViewType viewType{};
        VkFormat format{};
        VkComponentMapping components{};
        VkImageSubresourceRange subresourceRange{};

        bool filledCreateInfo{false};
    };
}