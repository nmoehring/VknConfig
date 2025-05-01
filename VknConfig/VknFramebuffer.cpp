#include "include/VknFramebuffer.hpp"

namespace vkn
{

    VknFramebuffer *VknFramebuffer::s_editable{nullptr};

    VknFramebuffer::VknFramebuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
        s_editable = this;
    }

    void VknFramebuffer::setDimensions(uint32_t width, uint32_t height)
    {
        testEditability();
        m_width = width;
        m_height = height;
        if (m_setAttachments)
            this->setAttachmentDimensions(width, height);
    }

    void VknFramebuffer::setNumLayers(uint32_t numLayers)
    {
        testEditability();
        m_numLayers = numLayers;
    }

    void VknFramebuffer::setCreateFlags(VkFramebufferCreateFlags createFlags)
    {
        testEditability();
        m_createFlags = createFlags;
    }

    void VknFramebuffer::createFramebuffer()
    {
        if (m_createdFramebuffer)
            throw std::runtime_error("Framebuffer already created.");
        testEditability();

        m_infos->fillFramebufferCreateInfo(
            m_relIdxs, &m_engine->getObject<VkRenderPass>(m_absIdxs),
            this->getAttachmentImageViews(), m_width, m_height, m_numLayers, m_createFlags);

        VkFramebufferCreateInfo *createInfo =
            m_infos->getFramebufferCreateInfo(m_relIdxs);
        vkCreateFramebuffer(
            m_engine->getObject<VkDevice>(m_absIdxs), createInfo, VK_NULL_HANDLE,
            m_engine->getVector<VkFramebuffer>().getData(1));
        m_createdFramebuffer = true;
    }

    void VknFramebuffer::addAttachments()
    {
        if (m_setAttachments)
            throw std::runtime_error("Attachments already set on framebuffer.");
        testEditability();

        //>attachment_infos
        VknSpace<VkAttachmentReference> *refs = m_infos->getRenderpassAttachmentReferences(
            m_relIdxs);
        VknSpace<VkAttachmentDescription> *descriptions = m_infos->getRenderpassAttachmentDescriptions(
            m_relIdxs);

        m_imageViewStartIdx = m_engine->getVectorSize<VkImageView>();
        for (uint32_t i = 0; i < descriptions->getDataSize(); ++i)
            for (VknSpace<VkAttachmentReference> &subPassSpace : refs->getSubspaceVector())
                for (uint32_t j = 0; j < subPassSpace.getNumSubspaces(); ++j)
                    for (VkAttachmentReference &attachmentRef : subPassSpace[j].getDataVector())
                        if (attachmentRef.attachment == i)
                        {
                            VknImage newImage{addNewVknObject<VknImage, VkImage>(m_attachImages.size(), m_attachImages,
                                                                                 m_engine, m_relIdxs, m_absIdxs, m_infos)};
                            VknImageView newView{addNewVknObject<VknImageView, VkImageView>(m_attachViews.size(), m_attachViews,
                                                                                            m_engine, m_relIdxs, m_absIdxs,
                                                                                            m_infos)};
                            newView.setImage(&newImage);
                            newImage.setExtent({m_width, m_height, 1});
                            newImage.setFormat((*descriptions)(i).format);
                            newView.setFormat((*descriptions)(i).format);
                            newImage.setSamples((*descriptions)(i).samples);
                            switch (j)
                            {
                            case COLOR_ATTACHMENT:
                                newImage.setUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
                                break;
                            case DEPTH_STENCIL_ATTACHMENT:
                                newImage.setUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
                                break;
                            case RESOLVE_ATTACHMENT:
                                newImage.setUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
                                break;
                            case INPUT_ATTACHMENT:
                                newImage.setUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
                                break;
                            case PRESERVE_ATTACHMENT:
                                newImage.setUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
                                break;
                            }
                        }
        m_setAttachments = true;
    }

    void VknFramebuffer::setAttachmentDimensions(uint32_t width, uint32_t height)
    {
        testEditability();
        for (auto &image : m_attachImages)
            image.setExtent({width, height, 1});
    }

    void VknFramebuffer::addSwapchainVkImage(uint32_t engineImageIdx)
    {
        testEditability();
        m_swapchainVkImage = &m_engine->getObject<VkImage>(engineImageIdx);
    }

    VknVectorIterator<VkImageView> VknFramebuffer::getAttachmentImageViews()
    {
        if (!m_setAttachments)
            throw std::runtime_error("Attachments not set before trying to get() them.");
        return m_engine->getVectorSlice<VkImageView>(m_imageViewStartIdx, m_attachImages.size());
    }

    void VknFramebuffer::testEditability()
    {
        if (s_editable != this)
            throw std::runtime_error("Members of a VknPipeline must be added all at once so that they are stored contiguously.");
    }
}