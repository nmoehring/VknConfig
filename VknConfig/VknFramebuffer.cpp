#include "include/VknFramebuffer.hpp"

namespace vkn
{
    VknFramebuffer::VknFramebuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
        m_instanceLock = this;
    }

    void VknFramebuffer::setDimensions(uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;
    }

    void VknFramebuffer::setDimensions(VknSwapchain &swapchain)
    {
        auto swapchainExtent = swapchain.getActualExtent();
        m_width = swapchainExtent.width;
        m_height = swapchainExtent.height;
    }

    void VknFramebuffer::setNumLayers(uint32_t numLayers)
    {
        m_numLayers = numLayers;
    }

    void VknFramebuffer::setCreateFlags(VkFramebufferCreateFlags createFlags)
    {
        m_createFlags = createFlags;
    }

    void VknFramebuffer::createFramebuffer()
    {
        if (m_createdFramebuffer)
            throw std::runtime_error("Framebuffer already created.");

        if (!m_addedAttachments)
            this->addAttachments();
        if (!m_setAttachmentSettings)
            this->setAttachmentSettings();
        if (!m_createdAttachments)
            this->createAttachments();

        m_infos->fillFramebufferCreateInfo(
            m_relIdxs, &m_engine->getObject<VkRenderPass>(m_absIdxs),
            this->getAttachmentImageViews(), m_width, m_height, m_numLayers, m_createFlags);

        VkFramebufferCreateInfo *createInfo =
            m_infos->getFramebufferCreateInfo(m_relIdxs);
        vkCreateFramebuffer(
            m_engine->getObject<VkDevice>(m_absIdxs), createInfo, VK_NULL_HANDLE,
            &m_engine->getObject<VkFramebuffer>(m_absIdxs));
        m_createdFramebuffer = true;
    }

    void VknFramebuffer::setSwapchainAttachmentDescriptionIndex(uint32_t descriptionIndex)
    {
        m_swapchainAttachmentDescIndex = descriptionIndex;
    }

    void VknFramebuffer::addAttachments()
    {
        if (m_addedAttachments)
            throw std::runtime_error("Attachments already set on framebuffer.");
        m_instanceLock(this);

        VknSpace<VkAttachmentReference> *refs = m_infos->getRenderpassAttachmentReferences(
            m_relIdxs);
        VknSpace<VkAttachmentDescription> *descriptions = m_infos->getRenderpassAttachmentDescriptions(
            m_relIdxs);

        if (descriptions->getDataSize() == 0)
        {
            m_addedAttachments = true; // No descriptions, so attachments are "set" (to none)
            return;
        }

        m_imageViewStartIdx = m_engine->getVectorSize<VkImageView>();
        for (uint32_t i = 0; i < descriptions->getDataSize(); ++i) // Iterate descriptions
        {
            if (!isSwapchainImage(i))
            {
                // This is an attachment for which we need to create and manage a VknImage
                // (e.g., depth buffer, offscreen color target).
                VknImage &image = m_engine->addNewVknObject<VknImage, VkImage, VkDevice>(
                    m_attachImages.size(), m_attachImages, // Adds to this framebuffer's m_attachImages list
                    m_relIdxs, m_absIdxs, m_infos);

                // Configure the newly created VknImage
                image.setFormat((*descriptions)(i).format);
                image.setSamples((*descriptions)(i).samples);
                image.setInitialLayout((*descriptions)(i).initialLayout); // Set initial layout
                image.setUsage(0);                                        // Initialize usage to 0 for accumulation

                VknImageView &view = m_engine->addNewVknObject<VknImageView, VkImageView, VkDevice>(
                    m_attachViews.size(), m_attachViews,
                    m_relIdxs, m_absIdxs, m_infos);

                view.setImage(image.getVkImage());
                view.setFormat((*descriptions)(i).format); // Format from the attachment description
                // Set other VknImageView properties if necessary (e.g., viewType, components, subresourceRange)
                // Default subresource range in VknImageView is usually fine for color/depth.

                VkImageUsageFlags accumulatedUsage = 0;                                         // Start fresh for this image
                for (VknSpace<VkAttachmentReference> &subPassSpace : refs->getSubspaceVector()) // Iterate subpasses (references)
                {
                    for (uint32_t j = 0; j < subPassSpace.getNumSubspaces(); ++j) // Iterate attachment types (references)
                    {
                        for (VkAttachmentReference &attachmentRef : subPassSpace[j].getDataVector()) // Iterate attachment references
                            if (attachmentRef.attachment == i)
                            {

                                switch (static_cast<VknAttachmentType>(j))
                                {
                                case COLOR_ATTACHMENT:
                                    accumulatedUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                                    break;
                                case DEPTH_STENCIL_ATTACHMENT:
                                    accumulatedUsage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                                    break;
                                case RESOLVE_ATTACHMENT:
                                    accumulatedUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                                    break;
                                case PRESERVE_ATTACHMENT:
                                    break;
                                case INPUT_ATTACHMENT:
                                    accumulatedUsage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
                                    break;
                                default:
                                    throw std::runtime_error("Invalid attachment type.");
                                } // switch

                            } // if (attachmentRef.attachment == i)
                    } // for attachment types (ref)
                } // for subpasses (ref)

                image.setUsage(accumulatedUsage);
            } // if isSwapchainImage
        } // for descriptions

        m_addedAttachments = true;
    } // addAttachments()

    bool VknFramebuffer::isSwapchainImage(uint32_t i)
    {
        return m_swapchainImageView &&
               m_swapchainAttachmentDescIndex.has_value() &&
               m_swapchainAttachmentDescIndex.value() == i;
    }

    void VknFramebuffer::setAttachmentSettings()
    {
        for (uint32_t i = 0; i < m_attachViews.size(); ++i)
        {
            if (!isSwapchainImage(i))
                getListElement(i, m_attachImages)->setExtent({m_width, m_height, 1});
        }
        m_setAttachmentSettings = true;
    }

    void VknFramebuffer::createAttachments()
    {
        for (uint32_t i = 0; i < m_attachViews.size(); ++i)
        {
            getListElement(i, m_attachImages)->createImage();
            getListElement(i, m_attachImages)->allocateAndBindMemory(); // Allocate and bind memory for the VknImage
            getListElement(i, m_attachViews)->createImageView();
        }
    }

    void VknFramebuffer::addSwapchainImageView(VknImageView *swapchainImageView)
    {
        m_swapchainImageView = swapchainImageView;
    }

    VknVectorIterator<VkImageView> VknFramebuffer::getAttachmentImageViews()
    {
        if (!m_addedAttachments)
            throw std::runtime_error("Attachments not set before trying to get() them.");
        if (m_infos->getRenderpassAttachmentDescriptions(m_relIdxs)->getDataSize() == 0 &&
            m_attachViews.empty())
            return m_engine->getVectorSlice<VkImageView>(m_imageViewStartIdx, 0);
        return m_engine->getVectorSlice<VkImageView>(m_imageViewStartIdx, m_attachViews.size());
    }

    void VknFramebuffer::demolishFramebuffer()
    {
        for (uint32_t i = 0; i < m_attachViews.size(); ++i)
        {
            getListElement(i, m_attachViews)->demolishImageView();
            getListElement(i, m_attachImages)->demolishImage();
            getListElement(i, m_attachImages)->deallocateMemory();
        }
        vkDestroyFramebuffer(
            m_engine->getObject<VkDevice>(m_absIdxs), m_engine->getObject<VkFramebuffer>(m_absIdxs), nullptr);
    }

    void VknFramebuffer::recreateFramebuffer(VknSwapchain &swapchain)
    {
        this->demolishFramebuffer();
        m_setAttachmentSettings = false;
        m_createdAttachments = false;
        m_createdFramebuffer = false;
        this->setDimensions(swapchain);
        this->createFramebuffer();
    }
}