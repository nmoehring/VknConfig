#include "include/VknImageView.hpp"

namespace vkn
{
    VknImageView::VknImageView(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
    }

    void VknImageView::setCreateFlags(VkImageViewCreateFlags createFlags)
    {
        if (m_filedCreateInfo)
            throw std::runtime_error("Already filed create info, configuring this setting is unnecessary.");

        m_createFlags = createFlags;
    }

    void VknImageView::setImage(uint32_t idx)
    {
        if (m_filedCreateInfo)
            throw std::runtime_error("Already filed create info, configuring this setting is unnecessary.");

        m_vkImageIdx = idx;
        m_setVkImage = true;
    }

    void VknImageView::setImage(VkImage *image)
    {
        if (m_setVkImage)
            throw std::runtime_error("VkImage was set directly. Can't set VknImage.");
        if (m_filedCreateInfo)
            throw std::runtime_error("Already filed create info, configuring this setting is unnecessary.");
        m_image = image;
    }

    void VknImageView::setViewType(VkImageViewType viewType)
    {
        if (m_filedCreateInfo)
            throw std::runtime_error("Already filed create info, configuring this setting is unnecessary.");

        m_viewType = viewType;
    }

    void VknImageView::setFormat(VkFormat format)
    {
        if (m_filedCreateInfo)
            throw std::runtime_error("Already filed create info, configuring this setting is unnecessary.");

        m_format = format;
    }

    void VknImageView::setComponents(VkComponentMapping components)
    {
        if (m_filedCreateInfo)
            throw std::runtime_error("Already filed create info, configuring this setting is unnecessary.");

        m_components = components;
    }

    void VknImageView::setSubresourceRange(VkImageSubresourceRange subresourceRange)
    {
        if (m_filedCreateInfo)
            throw std::runtime_error("Already filed create info, configuring this setting is unnecessary.");

        m_subresourceRange = subresourceRange;
    }

    VkImage *VknImageView::getVkImage()
    {
        if (m_image == nullptr && !m_setVkImage)
            throw std::runtime_error("No image set for VknImageView.");
        if (m_setVkImage)
            return &m_engine->getObject<VkImage>(m_vkImageIdx);
        else
            return m_image;
    }

    void VknImageView::fileImageViewCreateInfo()
    {
        if (m_filedCreateInfo)
            throw std::runtime_error("Image view create info already filed.");
        m_filedCreateInfo = true;
    }

    void VknImageView::demolishImageView()
    {
        vkDestroyImageView(
            m_engine->getObject<VkDevice>(m_absIdxs),
            m_engine->getObject<VkImageView>(m_absIdxs), nullptr);
        m_infos->removeImageViewCreateInfo(m_relIdxs);
        m_filedCreateInfo = false;
        m_createdImageView = false;
        m_setVkImage = false;
    }

    void VknImageView::createImageView()
    {
        if (m_createdImageView)
            throw std::runtime_error("Image view already created.");
        VkImageViewCreateInfo *ci{nullptr};
        ci = m_infos->fileImageViewCreateInfo(m_absIdxs, *this->getVkImage(),
                                              m_viewType, m_format, m_components, m_subresourceRange, m_createFlags);
        VknResult res{
            vkCreateImageView(m_engine->getObject<VkDevice>(m_absIdxs),
                              ci, VK_NULL_HANDLE,
                              &m_engine->getObject<VkImageView>(m_absIdxs)),
            "Create image view."};
        m_createdImageView = true;
    }
}