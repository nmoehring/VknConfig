#include "include/VknImageView.hpp"

namespace vkn
{
    VknImageView::VknImageView(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
    }

    void VknImageView::setCreateFlags(VkImageViewCreateFlags createFlags)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Already filled create info, configuring this setting is unnecessary.");

        m_createFlags = createFlags;
    }

    void VknImageView::setImage(VkImage image)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Already filled create info, configuring this setting is unnecessary.");

        m_image = image;
    }

    void VknImageView::setViewType(VkImageViewType viewType)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Already filled create info, configuring this setting is unnecessary.");

        m_viewType = viewType;
    }

    void VknImageView::setFormat(VkFormat format)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Already filled create info, configuring this setting is unnecessary.");

        m_format = format;
    }

    void VknImageView::setComponents(VkComponentMapping components)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Already filled create info, configuring this setting is unnecessary.");

        m_components = components;
    }

    void VknImageView::setSubresourceRange(VkImageSubresourceRange subresourceRange)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Already filled create info, configuring this setting is unnecessary.");

        m_subresourceRange = subresourceRange;
    }

    void VknImageView::fillImageViewCreateInfo()
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Image view create info already filled.");
        m_infos->fillImageViewCreateInfo(m_relIdxs, m_image,
                                         m_viewType, m_format, m_components, m_subresourceRange, m_createFlags);
        m_filledCreateInfo = true;
    }

    void VknImageView::createImageView()
    {
        if (!m_filledCreateInfo)
            throw std::runtime_error("Trying to create image view before filling create info.");
        if (m_createdImageView)
            throw std::runtime_error("Image view already created.");
        VkImageViewCreateInfo *createInfo{m_infos->getImageViewCreateInfo(m_relIdxs)};
        vkCreateImageView(m_engine->getObject<VkDevice>(m_absIdxs),
                          createInfo, VK_NULL_HANDLE,
                          &m_engine->getObject<VkImageView>(m_absIdxs));
        m_createdImageView = true;
    }
}