#include <stdexcept>

#include "VknFramebuffer.hpp"

namespace vkn
{
    VknFramebuffer::VknFramebuffer() : m_placeholder{true} {}

    VknFramebuffer::VknFramebuffer(VknResultArchive *archive, VknInfos *infos, VkRenderPass *renderpass)
        : m_archive{archive}, m_infos{infos}
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
        m_infos->fillFramebufferCreateInfo(
            *m_renderpass, m_attachments, m_width, m_height, m_numLayers, m_createFlags);
        m_filledCreateInfo = true;
    }
}