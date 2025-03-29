#pragma once

#include <vulkan/vulkan.h>

#include "VknInfos.hpp"
#include "VknResult.hpp"

namespace vkn
{
    class VknImageView
    {
    public:
        VknImageView() = default;
        VknImageView(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        void fillCreateFlags(VkImageViewCreateFlags);
        void setImage(VkImage image);
        void setViewType(VkImageViewType viewType);
        void setFormat(VkFormat format);
        void setComponents(VkComponentMapping components);
        void setSubresourceRange(VkImageSubresourceRange subresourceRange);
        void fillImageViewCreateInfo();

        void createImageView();

    private:
        // Engine
        VknEngine *m_engine{nullptr};
        VknIdxs m_relIdxs{};
        VknIdxs m_absIdxs{};
        VknInfos *m_infos{nullptr};

        // Wrapped object
        VkImageView *m_imageView{};

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
    };
}