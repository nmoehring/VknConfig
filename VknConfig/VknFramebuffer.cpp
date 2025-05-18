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
            &m_engine->getObject<VkFramebuffer>(m_absIdxs));
        m_createdFramebuffer = true;
    }

    void VknFramebuffer::setSwapchainAttachmentDescriptionIndex(uint32_t descriptionIndex)
    {
        testEditability();
        m_swapchainAttachmentDescIndex = descriptionIndex;
    }

    void VknFramebuffer::addAttachments()
    {
        if (m_setAttachments)
            throw std::runtime_error("Attachments already set on framebuffer.");
        testEditability();

        VknSpace<VkAttachmentReference> *refs = m_infos->getRenderpassAttachmentReferences(
            m_relIdxs);
        VknSpace<VkAttachmentDescription> *descriptions = m_infos->getRenderpassAttachmentDescriptions(
            m_relIdxs);

        if (descriptions->getDataSize() == 0)
        {
            m_setAttachments = true; // No descriptions, so attachments are "set" (to none)
            return;
        }

        m_imageViewStartIdx = m_engine->getVectorSize<VkImageView>();
        for (uint32_t i = 0; i < descriptions->getDataSize(); ++i) // Iterate descriptions
        {
            VknImage *vknImageManagedByThisFramebuffer{nullptr}; // Points to an image in m_attachImages if we create one
            VkImage *targetVkImageHandleForView{nullptr};        // The actual VkImage the VknImageView will wrap

            bool isThisDescriptionTheSwapchainOutput = m_swapchainImageView &&
                                                       m_swapchainAttachmentDescIndex.has_value() &&
                                                       m_swapchainAttachmentDescIndex.value() == i;

            if (isThisDescriptionTheSwapchainOutput)
            {
                targetVkImageHandleForView = m_swapchainImageView->getVkImage();
                // No new VknImage is added to m_attachImages for this description.
            }
            else
            {
                // This is an attachment for which we need to create and manage a VknImage
                // (e.g., depth buffer, offscreen color target).
                vknImageManagedByThisFramebuffer = &m_engine->addNewVknObject<VknImage, VkImage, VkDevice>(
                    m_attachImages.size(), m_attachImages, // Adds to this framebuffer's m_attachImages list
                    m_relIdxs, m_absIdxs, m_infos);

                targetVkImageHandleForView = vknImageManagedByThisFramebuffer->getVkImage();

                // Configure the newly created VknImage
                vknImageManagedByThisFramebuffer->setExtent({m_width, m_height, 1});
                vknImageManagedByThisFramebuffer->setFormat((*descriptions)(i).format);
                vknImageManagedByThisFramebuffer->setSamples((*descriptions)(i).samples);
                vknImageManagedByThisFramebuffer->setInitialLayout((*descriptions)(i).initialLayout); // Set initial layout
                vknImageManagedByThisFramebuffer->setUsage(0);                                        // Initialize usage to 0 for accumulation

                // IMPORTANT: The actual VkImage Vulkan object needs to be created.
                // This could happen here or be deferred. If deferred, ensure it happens before vkCreateFramebuffer.
                // For simplicity, let's assume VknImage::createImage() should be called if it's not in constructor.
                // If VknImage constructor or addNewVknObject handles VkImage creation, this explicit call isn't needed.
                // Based on VknImage.cpp, createImage() is a separate call.
            }

            // Always create a VknImageView for this attachment description 'i'.
            // This view is added to this framebuffer's m_attachViews list.
            VknImageView *view{nullptr};
            if (isThisDescriptionTheSwapchainOutput)
            {
                view = &m_engine->addNewVknObject<VknImageView, VkImageView, VkDevice>(
                    m_attachViews.size(), m_attachViews,
                    m_relIdxs, m_absIdxs, m_infos);

                view->setImage(targetVkImageHandleForView);
                view->setFormat((*descriptions)(i).format); // Format from the attachment description
                // Set other VknImageView properties if necessary (e.g., viewType, components, subresourceRange)
                // Default subresource range in VknImageView is usually fine for color/depth.
            }

            // Accumulate usage flags ONLY for images managed by this framebuffer (i.e., not swapchain images).
            if (vknImageManagedByThisFramebuffer)
            {
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

                vknImageManagedByThisFramebuffer->setUsage(accumulatedUsage);
                if (isThisDescriptionTheSwapchainOutput)
                {
                    vknImageManagedByThisFramebuffer->createImage();
                    vknImageManagedByThisFramebuffer->allocateAndBindMemory(); // Allocate and bind memory for the VknImage
                }
            } // if vknImageManagedByThisFramebuffer

            if (view)
                view->createImageView();
        } // for descriptions

        if (descriptions->getDataSize() > 0)
            m_setAttachments = true;
    } // addAttachments()

    void createAttachments()
    {
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
        if (engineImageIdx >= m_engine->getVectorSize<VkImage>())
            throw std::out_of_range("engineImageIdx out of range for VknEngine's VkImage vector in addSwapchainVkImage.");
        m_swapchainVkImage = &m_engine->getObject<VkImage>(engineImageIdx);
    }

    void VknFramebuffer::addSwapchainImageView(VknImageView *swapchainImageView)
    {
        testEditability();
        m_swapchainImageView = swapchainImageView;
    }

    VknVectorIterator<VkImageView> VknFramebuffer::getAttachmentImageViews()
    {
        if (!m_setAttachments)
            throw std::runtime_error("Attachments not set before trying to get() them.");
        if (m_infos->getRenderpassAttachmentDescriptions(m_relIdxs)->getDataSize() == 0 &&
            m_attachViews.empty())
            return m_engine->getVectorSlice<VkImageView>(m_imageViewStartIdx, 0);
        return m_engine->getVectorSlice<VkImageView>(m_imageViewStartIdx, m_attachViews.size());
    }

    void VknFramebuffer::testEditability()
    {
        if (s_editable != this)
            throw std::runtime_error("Members of a VknFramebuffer must be added all at once so that they are stored contiguously.");
    }
}