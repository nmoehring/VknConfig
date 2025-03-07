#include <stdexcept>

#include "VknFramebuffer.hpp"

namespace vkn
{
    VknFramebuffer::VknFramebuffer()
        : m_placeholder{true}, m_deviceIdx{0}, m_renderpassIdx{0},
          m_framebufferIdx{0}, m_archive{nullptr}, m_infos{nullptr},
          m_renderpass{VK_NULL_HANDLE}
    {
    }

    VknFramebuffer::VknFramebuffer(uint32_t deviceIdx, uint32_t renderpassIdx,
                                   uint32_t framebufferIdx, VknResultArchive *archive,
                                   VknInfos *infos, VkRenderPass *renderpass)
        : m_archive{archive}, m_infos{infos}, m_placeholder{false}, m_deviceIdx{deviceIdx},
          m_renderpassIdx{renderpassIdx}, m_framebufferIdx{framebufferIdx}, m_renderpass{renderpass}
    {
    }

    void VknFramebuffer::fillDimensions(uint32_t width, uint32_t height)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        m_width = width;
        m_height = height;
    }

    void VknFramebuffer::fillNumLayers(uint32_t numLayers)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        m_numLayers = numLayers;
    }

    void VknFramebuffer::setCreateFlags(VkFramebufferCreateFlags createFlags)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        m_createFlags = createFlags;
    }

    void VknFramebuffer::fillFramebufferCreateInfo()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Already filled frame buffer create info.");
        m_infos->fillFramebufferCreateInfo(m_deviceIdx, m_renderpassIdx, m_framebufferIdx,
                                           m_renderpass, m_attachments, m_width, m_height, m_numLayers, m_createFlags);
        m_filledCreateInfo = true;
    }

    void VknFramebuffer::createFramebuffer()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");

        if (!m_filledCreateInfo)
            throw std::runtime_error("Trying to create framebuffer before filling create info.");
        if (m_createdFramebuffer)
            throw std::runtime_error("Framebuffer already created.");
        if (!(*m_vkDeviceCreated))
            throw std::runtime_error("Device not created before trying to create framebuffer.");
        VkFramebufferCreateInfo *createInfo = m_infos->getFramebufferCreateInfo(
            m_deviceIdx, m_renderpassIdx, m_framebufferIdx);
        vkCreateFramebuffer(*m_vkDevice, createInfo, VK_NULL_HANDLE, &m_buffer);
    }

    void VknFramebuffer::setAttachments(std::vector<VkImageView> *vkImageViews)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (m_attachmentsSet)
            throw std::runtime_error("Attachments already set on framebuffer.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Framebuffer create info already filled.");

        m_attachments = vkImageViews;
        m_attachmentsSet = true;
    }
}