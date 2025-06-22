#include "include/VknRenderpass.hpp"

namespace vkn
{
    VknRenderpass::VknRenderpass(VknIdxs relIdxs, VknIdxs absIdxs)
        : VknObject(relIdxs, absIdxs)
    {
        m_instanceLock = this;
        m_pipelineStartAbsIdx = s_engine->getVectorSize<VkPipeline>();
    }

    VknFramebuffer *VknRenderpass::addFramebuffer(uint32_t framebufferIdx)
    {
        m_instanceLock(this);
        if (framebufferIdx + 1 > m_framebuffers.size())
            throw std::runtime_error("FramebufferIdx invalid. Should be next index.");
        if (!m_createdRenderpass)
            throw std::runtime_error("Renderpass not created before adding framebuffer.");
        return &s_engine->addNewVknObject<VknFramebuffer, VkFramebuffer, VkDevice>(
            framebufferIdx, m_framebuffers, m_relIdxs, m_absIdxs);
    }

    VknPipeline *VknRenderpass::addPipeline(uint32_t subpassIdx)
    {
        m_instanceLock(this);
        if (subpassIdx != m_pipelines.size())
            throw std::runtime_error("SubpassIdx passed to addPipeline is invalid. Should be next idx.");

        return &s_engine->addNewVknObject<VknPipeline, VkPipeline, VkDevice>(
            subpassIdx, m_pipelines, m_relIdxs, m_absIdxs);
    }

    void VknRenderpass::createRenderpass()
    {
        if (m_createdRenderpass)
            throw std::runtime_error("Renderpass already created.");
        VkRenderPassCreateInfo *createInfo = s_infos->fileRenderpassCreateInfo(m_relIdxs, 0); // Flags not used ever
        VknResult res{vkCreateRenderPass(
                          s_engine->getObject<VkDevice>(m_absIdxs),
                          createInfo, VK_NULL_HANDLE,
                          &s_engine->getObject<VkRenderPass>(m_absIdxs)),
                      "Create renderpass."};
        m_createdRenderpass = true;
    }

    VknPipeline *VknRenderpass::getPipeline(uint32_t pipelineIdx)
    {
        return getListElement(pipelineIdx, m_pipelines);
    }

    VknFramebuffer *VknRenderpass::getFramebuffer(uint32_t bufferIdx)
    {
        return getListElement(bufferIdx, m_framebuffers);
    }

    void VknRenderpass::addSubpassDependency(uint32_t dependencyIdx,
                                             uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask,
                                             VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStageMask, VkAccessFlags dstAccessMask)
    {
        if (dependencyIdx != m_numSubpassDeps++)
            throw std::runtime_error("DependencyIdx passed to addSubpassDependency is invalid. Should be next idx.");

        s_infos->fileSubpassDependency(m_relIdxs,
                                       srcSubpass, dstSubpass, srcStageMask, srcAccessMask, dstStageMask, dstAccessMask);
    }

    void VknRenderpass::addAttachment(
        uint32_t attachIdx,
        VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp,
        VkAttachmentStoreOp storeOp, VkAttachmentLoadOp stencilLoadOp,
        VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout,
        VkImageLayout finalLayout,
        VkAttachmentDescriptionFlags flags)
    {

        s_infos->fileAttachmentDescription(
            m_relIdxs, attachIdx, format, samples, loadOp, storeOp, stencilLoadOp,
            stencilStoreOp, initialLayout, finalLayout, flags);
    }

    void VknRenderpass::addAttachmentRef(
        uint32_t subpassIdx, uint32_t attachIdx, VknAttachmentType attachmentType,
        VkImageLayout attachmentRefLayout)
    {
        auto *subpassRefs = s_infos->getSubpassAttachmentReferences(
            m_relIdxs.get<VkDevice>(), m_relIdxs.get<VkRenderPass>(), subpassIdx);
        if (!m_numPreserveRefs.exists(subpassIdx))
            m_numPreserveRefs.insert(subpassIdx, 0u);
        if (m_numAttachRefs[subpassIdx].getDataSize() == 0)
            for (size_t i = 0; i < NUM_ATTACHMENT_TYPES; ++i)
            {
                m_numAttachRefs[subpassIdx].insert(0u, i);
                subpassRefs->getSubspace(i);
            }

        uint32_t refIdx{0};
        if (attachmentType == PRESERVE_ATTACHMENT)
            refIdx = m_numPreserveRefs(subpassIdx)++;
        else
            refIdx = m_numAttachRefs[subpassIdx](attachmentType)++;

        s_infos->fileAttachmentReference(m_relIdxs, subpassIdx, refIdx,
                                         attachmentType, attachIdx, attachmentRefLayout);
        if (attachmentType == COLOR_ATTACHMENT)
            m_filedColorAttachment = true;
    }

    void VknRenderpass::createPipelines()
    {
        if (!m_createdRenderpass)
            throw std::runtime_error("Renderpass not created before creating pipelines.");
        if (m_createdPipelines)
            throw std::runtime_error("Pipelines already created.");
        for (auto &pipeline : m_pipelines)
            for (auto &shaderStage : *pipeline.getShaderStages())
                if (!shaderStage.isShaderModuleCreated())
                    throw std::runtime_error("Shader module in shader stage not created before pipelines created.");

        VkPipeline *vkPipelines =
            s_engine->getVectorSlice<VkPipeline>(m_pipelineStartAbsIdx, m_numSubpasses).getData();
        for (auto &pipeline : m_pipelines)
        {
            if (m_recreatingPipelines)
                pipeline.getViewportState()->_fileViewportStateCreateInfo();
            else
            {
                pipeline.getVertexInputState()->_fileVertexInputStateCreateInfo();
                pipeline.getInputAssemblyState()->_fileInputAssemblyStateCreateInfo();
                pipeline.getMultisampleState()->_fileMultisampleStateCreateInfo();
                pipeline.getRasterizationState()->_fileRasterizationStateCreateInfo();
                pipeline.getViewportState()->_fileViewportStateCreateInfo();
                pipeline.getColorBlendState()->_fileColorBlendStateCreateInfo();
                pipeline.getDynamicState()->_fileDynamicStateCreateInfo();
                pipeline.getPipelineLayout()->_createPipelineLayout();
                for (auto &shaderstage : *pipeline.getShaderStages())
                    shaderstage._fileShaderStageCreateInfo();
            }
            pipeline._filePipelineCreateInfo();
        }
        VknSpace<VkGraphicsPipelineCreateInfo> *pipelineCreateInfos{
            s_infos->getPipelineCreateInfos(m_relIdxs)};
        VknResult res{vkCreateGraphicsPipelines(
                          s_engine->getObject<VkDevice>(m_absIdxs),
                          VK_NULL_HANDLE, m_numSubpasses,
                          pipelineCreateInfos->getData(), nullptr,
                          vkPipelines),
                      "Create pipeline."};
        m_createdPipelines = true;
    }

    void VknRenderpass::addSubpass(
        uint32_t subpassIdx, bool isCompute, VkPipelineBindPoint pipelineBindPoint,
        VkSubpassDescriptionFlags flags)
    {
        // Check for color attachment *before* trying to access counts
        if (!isCompute && !m_filedColorAttachment)
            throw std::runtime_error("No color attachment created before creating subpass.");
        s_infos->fileSubpassDescription(m_relIdxs, m_numSubpasses++, pipelineBindPoint, flags);
        this->addPipeline(subpassIdx);
    }

    std::list<VknFramebuffer> *VknRenderpass::addFramebuffers(VknSwapchain &swapchain)
    {
        m_framebufferStartPos = s_engine->addNewVknObjects<VknFramebuffer, VkFramebuffer, VkDevice>(
            swapchain.getNumImages(), m_framebuffers, m_relIdxs, m_absIdxs);
        for (uint32_t i = 0; i < swapchain.getNumImages(); ++i)
        {
            VknFramebuffer *listElement = getListElement(i, m_framebuffers);
            listElement->setSwapchain(&swapchain);
            listElement->setDimensions();
            listElement->setSwapchainAttachmentDescriptionIndex(0);
            listElement->addAttachments();
        }
        return &m_framebuffers;
    }

    void VknRenderpass::createFramebuffers()
    {
        if (m_framebuffers.size() == 0)
            throw std::runtime_error("No framebuffers to create.");
        for (auto &framebuffer : m_framebuffers)
            framebuffer.createFramebuffer();
    }

    void VknRenderpass::recreatePipelines(VknSwapchain &swapchain, uint32_t viewportIdx, uint32_t scissorIdx)
    {
        m_recreatingPipelines = true;
        for (auto &pipeline : m_pipelines)
        {
            s_infos->removePipelineCreateInfo(pipeline.getRelIdxs());
            vkDestroyPipeline(
                s_engine->getObject<VkDevice>(m_absIdxs),
                s_engine->getObject<VkPipeline>(pipeline.getAbsIdxs()),
                nullptr);
            VknViewportState *viewport = pipeline.getViewportState();
            viewport->removeCreateInfo();
            viewport->removeScissors();
            viewport->removeViewports();
            viewport->syncWithSwapchain(swapchain, viewportIdx, scissorIdx);
            m_createdPipelines = false;
        }
        this->createPipelines();
        m_recreatingPipelines = false;
    }

    void VknRenderpass::recreateFramebuffers(VknSwapchain &swapchain)
    {
        uint32_t numSwapchainImages{swapchain.getNumImages()};
        if (m_framebuffers.size() != numSwapchainImages)
        {
            s_engine->demolishVknObjects<VknFramebuffer, VkFramebuffer, VkDevice>(
                m_framebufferStartPos, numSwapchainImages, m_framebuffers);
            m_framebuffers.clear();
            this->addFramebuffers(swapchain);
            this->createFramebuffers();
        }
        else
            for (auto &framebuffer : m_framebuffers)
                framebuffer.recreateFramebuffer();
    }
}