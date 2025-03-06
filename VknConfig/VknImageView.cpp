#include "VknImageView.hpp"

namespace vkn
{
    VknImageView::VknImageView() : m_placeholder{true}, m_deviceIdx{0}, m_swapchainIdx{0},
                                   m_imageViewIdx{0}, m_vkDevice{VK_NULL_HANDLE},
                                   m_deviceCreated{nullptr}, m_archive{nullptr}, m_infos{nullptr}
    {
    }

    VknImageView::VknImageView(uint32_t deviceIdx, uint32_t swapchainIdx, uint32_t imageViewIdx,
                               VkDevice *device, const bool *deviceCreated,
                               VknResultArchive *archive, VknInfos *infos)
        : m_deviceIdx{deviceIdx}, m_swapchainIdx{swapchainIdx}, m_imageViewIdx{imageViewIdx},
          m_placeholder{false}, m_archive{archive}, m_infos{infos}, m_vkDevice{device},
          m_deviceCreated{deviceCreated}
    {
    }

    void VknImageView::fillCreateFlags(VkImageViewCreateFlags createFlags)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");

        if (m_filledCreateInfo)
            throw std::runtime_error("Already filled create info, configuring this setting is unnecessary.");

        m_createFlags = createFlags;
    }

    void VknImageView::setImage(VkImage image)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");

        if (m_filledCreateInfo)
            throw std::runtime_error("Already filled create info, configuring this setting is unnecessary.");

        m_image = image;
    }

    void VknImageView::setViewType(VkImageViewType viewType)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");

        if (m_filledCreateInfo)
            throw std::runtime_error("Already filled create info, configuring this setting is unnecessary.");

        m_viewType = viewType;
    }

    void VknImageView::setFormat(VkFormat format)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");

        if (m_filledCreateInfo)
            throw std::runtime_error("Already filled create info, configuring this setting is unnecessary.");

        m_format = format;
    }

    void VknImageView::setComponents(VkComponentMapping components)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");

        if (m_filledCreateInfo)
            throw std::runtime_error("Already filled create info, configuring this setting is unnecessary.");

        m_components = components;
    }

    void VknImageView::setSubresourceRange(VkImageSubresourceRange subresourceRange)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");

        if (m_filledCreateInfo)
            throw std::runtime_error("Already filled create info, configuring this setting is unnecessary.");

        m_subresourceRange = subresourceRange;
    }

    void VknImageView::fillImageViewCreateInfo()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Image view create info already filled.");
        m_infos->fillImageViewCreateInfo(m_deviceIdx, m_swapchainIdx, m_imageViewIdx, m_image,
                                         m_viewType, m_format, m_components, m_subresourceRange, m_createFlags);
        m_filledCreateInfo = true;
    }

    void VknImageView::createImageView()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (!m_filledCreateInfo)
            throw std::runtime_error("Trying to create image view before filling create info.");
        if (m_createdImageView)
            throw std::runtime_error("Image view already created.");
        if (!(*m_vkDeviceCreated))
            throw std::runtime_error("Device not created before trying to create image view.");
        VkImageViewCreateInfo *createInfo{m_infos->getImageViewCreateInfo(m_deviceIdx, m_swapchainIdx, m_imageViewIdx)};
        vkCreateImageView(*m_vkDevice, createInfo, VK_NULL_HANDLE, &m_imageView);
        m_createdImageView = true;
    }
}