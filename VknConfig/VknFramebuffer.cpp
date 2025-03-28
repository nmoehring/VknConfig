#include <stdexcept>

#include "include/VknFramebuffer.hpp"

namespace vkn
{
    VknFramebuffer::VknFramebuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
    }

    void VknFramebuffer::fillDimensions(uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;
    }

    void VknFramebuffer::fillNumLayers(uint32_t numLayers)
    {
        m_numLayers = numLayers;
    }

    void VknFramebuffer::setCreateFlags(VkFramebufferCreateFlags createFlags)
    {
        m_createFlags = createFlags;
    }

    void VknFramebuffer::fillFramebufferCreateInfo()
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Already filled frame buffer create info.");
        m_infos->fillFramebufferCreateInfo(
            m_relIdxs, &m_engine->getObject<VkRenderPass>(m_absIdxs.renderpassIdx.value()), m_attachments,
            m_width, m_height, m_numLayers, m_createFlags);
        m_filledCreateInfo = true;
    }

    void VknFramebuffer::createFramebuffer()
    {
        if (!m_filledCreateInfo)
            throw std::runtime_error("Trying to create framebuffer before filling create info.");
        if (m_createdFramebuffer)
            throw std::runtime_error("Framebuffer already created.");
        VkFramebufferCreateInfo *createInfo =
            m_infos->getFramebufferCreateInfo(m_relIdxs);
        m_absIdxs.framebufferIdx = m_engine->push_back(VkFramebuffer{});
        vkCreateFramebuffer(
            m_engine->getObject<VkDevice>(m_absIdxs.deviceIdx.value()), createInfo, VK_NULL_HANDLE,
            &m_engine->getObject<VkFramebuffer>(m_absIdxs.framebufferIdx.value()));
        m_createdFramebuffer = true;
    }

    void VknFramebuffer::setAttachments(std::vector<VkImageView> *vkImageViews)
    {
        if (m_attachmentsSet)
            throw std::runtime_error("Attachments already set on framebuffer.");
        if (m_filledCreateInfo)
            throw std::runtime_error("Framebuffer create info already filled.");

        m_attachments = vkImageViews;
        m_attachmentsSet = true;
    }
}