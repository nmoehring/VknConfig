#include "include/VknFramebuffer.hpp"

namespace vkn
{
    VknFramebuffer::VknFramebuffer(VknIdxs relIdxs, VknIdxs absIdxs)
        : VknObject(relIdxs, absIdxs)
    {
    }

    void VknFramebuffer::setDimensions(uint32_t width, uint32_t height)
    {
        m_width = width;
        m_height = height;
    }

    void VknFramebuffer::setDimensions()
    {
        if (!m_setSwapchain)
            throw std::runtime_error("Swapchain not added before setting dimensions from swapchain.");
        auto swapchainExtent = m_swapchain->getActualExtent();
        m_width = swapchainExtent.width;
        m_height = swapchainExtent.height;
    }

    void VknFramebuffer::setSwapchain(VknSwapchain *swapchain)
    {
        m_swapchain = swapchain;
        m_setSwapchain = true;
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
        /*if (!m_setAttachmentSettings)
            this->setAttachmentSettings();*/
        if (!m_createdAttachments)
            this->createAttachments();

        s_infos->fileFramebufferCreateInfo(
            m_relIdxs, &s_engine->getObject<VkRenderPass>(m_absIdxs),
            this->getAttachmentImageViews(), m_width, m_height, m_numLayers, m_createFlags);

        VkFramebufferCreateInfo *createInfo =
            s_infos->getFramebufferCreateInfo(m_relIdxs);
        VknResult res{
            vkCreateFramebuffer(
                s_engine->getObject<VkDevice>(m_absIdxs), createInfo, VK_NULL_HANDLE,
                &s_engine->getObject<VkFramebuffer>(m_absIdxs)),
            "Create framebuffer."};
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

        VknSpace<VkAttachmentReference> *refs = s_infos->getRenderpassAttachmentReferences(
            m_relIdxs);
        VknSpace<VkAttachmentDescription> *descriptions = s_infos->getRenderpassAttachmentDescriptions(
            m_relIdxs);

        if (descriptions->getDataSize() == 0)
        {
            m_addedAttachments = true; // No descriptions, so attachments are "set" (to none)
            return;
        }

        if (!m_recreatingFramebuffer)
            m_imageViewStartIdx = s_engine->addNewVknObjects<VknImageView, VkImageView, VkDevice>(
                descriptions->getDataSize(), m_attachViews, m_relIdxs, m_absIdxs);

        if ((this->hasSwapchainImage() && descriptions->getDataSize() > 1) || !this->hasSwapchainImage())
            // This guard was missing. Without it, new VknImage objects would be allocated on every recreation.
            if (!m_recreatingFramebuffer)
                m_imageStartIdx = s_engine->addNewVknObjects<VknImage, VkImage, VkDevice>(
                    this->hasSwapchainImage() ? descriptions->getDataSize() - 1u : descriptions->getDataSize(),
                    m_attachImages, m_relIdxs, m_absIdxs);

        uint32_t imageIndex = 0;                                   // The index for the m_attachImages list
        for (uint32_t i = 0; i < descriptions->getDataSize(); ++i) // Iterate descriptions
        {
            VknImageView *view = getListElement(i, m_attachViews);

            if (isSwapchainImage(i))
                m_swapchain->initializeSwapchainImageViewFromFramebuffer(view, m_relIdxs.get<VkFramebuffer>());
            else
            {
                // This is an attachment for which we need to create and manage a VknImage
                // (e.g., depth buffer, offscreen color target).
                VknImage *image = getListElement(imageIndex, m_attachImages);
                imageIndex++; // Increment for the next non-swapchain image

                // Configure the newly added VknImage
                image->setExtent({m_width, m_height, 1});
                image->setFormat((*descriptions)(i).format);
                image->setSamples((*descriptions)(i).samples);
                image->setInitialLayout((*descriptions)(i).initialLayout); // Set initial layout
                image->setUsage(0);

                view->setImage(image->getVkImage());
                view->setFormat((*descriptions)(i).format); // Format from the attachment description
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

                image->setUsage(accumulatedUsage);
            } // if isSwapchainImage
        } // for descriptions

        m_addedAttachments = true;
    } // addAttachments()

    bool VknFramebuffer::isSwapchainImage(uint32_t i)
    {
        return m_swapchain &&
               m_swapchainAttachmentDescIndex.has_value() &&
               m_swapchainAttachmentDescIndex.value() == i;
    }

    bool VknFramebuffer::hasSwapchainImage()
    {
        return m_swapchain && m_swapchainAttachmentDescIndex.has_value();
    }

    void VknFramebuffer::setAttachmentSettings()
    {
        uint32_t imageIndex = 0;
        for (uint32_t i = 0; i < m_attachViews.size(); ++i)
        {
            if (!isSwapchainImage(i))
            {
                getListElement(imageIndex, m_attachImages)->setExtent({m_width, m_height, 1});
                imageIndex++;
            }
        }
        m_setAttachmentSettings = true;
    }

    void VknFramebuffer::createAttachments()
    {
        uint32_t imageIndex = 0;
        for (uint32_t i = 0; i < m_attachViews.size(); ++i)
        {
            if (!isSwapchainImage(i))
            {
                // Use the correct, separate index for the m_attachImages list.
                getListElement(imageIndex, m_attachImages)->createImage();
                getListElement(i, m_attachViews)->createImageView();
                imageIndex++;
            }
        }
    }

    VknVectorIterator<VkImageView> VknFramebuffer::getAttachmentImageViews()
    {
        if (!m_addedAttachments)
            throw std::runtime_error("Attachments not set before trying to get() them.");

        if (s_infos->getRenderpassAttachmentDescriptions(m_relIdxs)->getDataSize() == 0)
            return s_engine->getVectorSlice<VkImageView>(m_imageViewStartIdx, 0);
        else
            return s_engine->getVectorSlice<VkImageView>(m_imageViewStartIdx, m_attachViews.size());
    }

    void VknFramebuffer::demolish()
    {
        uint32_t imageIndex = 0;
        for (uint32_t i = 0; i < m_attachViews.size(); ++i)
        {
            getListElement(i, m_attachViews)->demolishImageView();
            if (!isSwapchainImage(i))
            {
                getListElement(imageIndex, m_attachImages)->demolishImage();
                imageIndex++;
            }
        }
        // This should be called once per framebuffer, not inside the attachment loop.
        s_infos->removeFramebufferCreateInfo(m_relIdxs);
        vkDestroyFramebuffer(
            s_engine->getObject<VkDevice>(m_absIdxs), s_engine->getObject<VkFramebuffer>(m_absIdxs), nullptr);

        // Reset state flags to allow for recreation. This makes demolish a true teardown.
        m_addedAttachments = false;
        m_createdFramebuffer = false;
        m_createdAttachments = false;
        m_setAttachmentSettings = false;

        // CRITICAL FIX: Clear the internal lists to prevent them from growing across recreations.
        m_attachViews.clear();
        m_attachImages.clear();
    }

    void VknFramebuffer::recreateFramebuffer()
    {
        m_recreatingFramebuffer = true;
        this->demolish();
        this->setDimensions();
        this->createFramebuffer();
        m_recreatingFramebuffer = false;
    }
}