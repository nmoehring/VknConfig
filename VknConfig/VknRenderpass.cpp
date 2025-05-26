#include "include/VknRenderpass.hpp"

namespace vkn
{
    VknRenderpass::VknRenderpass(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
        m_instanceLock = this;
        m_pipelineStartAbsIdx = m_engine->getVectorSize<VkPipeline>();
    }

    VknFramebuffer *VknRenderpass::addFramebuffer(uint32_t framebufferIdx)
    {
        m_instanceLock(this);
        if (framebufferIdx + 1 > m_framebuffers.size())
            throw std::runtime_error("FramebufferIdx invalid. Should be next index.");
        if (!m_createdRenderpass)
            throw std::runtime_error("Renderpass not created before adding framebuffer.");
        return &m_engine->addNewVknObject<VknFramebuffer, VkFramebuffer, VkDevice>(
            framebufferIdx, m_framebuffers, m_relIdxs, m_absIdxs, m_infos);
    }

    VknPipeline *VknRenderpass::addPipeline(uint32_t subpassIdx)
    {
        m_instanceLock(this);
        if (subpassIdx != m_pipelines.size())
            throw std::runtime_error("SubpassIdx passed to addPipeline is invalid. Should be next idx.");

        return &m_engine->addNewVknObject<VknPipeline, VkPipeline, VkDevice>(
            subpassIdx, m_pipelines,
            m_relIdxs, m_absIdxs, m_infos);
    }

    void VknRenderpass::createRenderpass()
    {
        if (m_createdRenderpass)
            throw std::runtime_error("Renderpass already created.");
        VkRenderPassCreateInfo *createInfo = m_infos->fillRenderpassCreateInfo(m_relIdxs, 0); // Flags not used ever
        VknResult res{vkCreateRenderPass(
                          m_engine->getObject<VkDevice>(m_absIdxs),
                          createInfo, VK_NULL_HANDLE,
                          &m_engine->getObject<VkRenderPass>(m_absIdxs)),
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

        m_infos->fillSubpassDependency(m_relIdxs,
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

        m_infos->fillAttachmentDescription(
            m_relIdxs, attachIdx, format, samples, loadOp, storeOp, stencilLoadOp,
            stencilStoreOp, initialLayout, finalLayout, flags);
    }

    void VknRenderpass::addAttachmentRef(
        uint32_t subpassIdx, uint32_t attachIdx, VknAttachmentType attachmentType,
        VkImageLayout attachmentRefLayout)
    {
        auto *subpassRefs = m_infos->getSubpassAttachmentReferences(
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

        m_infos->fillAttachmentReference(m_relIdxs, subpassIdx, refIdx,
                                         attachmentType, attachIdx, attachmentRefLayout);
        if (attachmentType == COLOR_ATTACHMENT)
            m_filledColorAttachment = true;
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
            m_engine->getVectorSlice<VkPipeline>(m_pipelineStartAbsIdx, m_numSubpasses).getData();
        for (auto &pipeline : m_pipelines)
        {
            if (m_recreatingPipelines)
                pipeline.getViewportState()->_fillViewportStateCreateInfo();
            else
            {
                pipeline.getVertexInputState()->_fillVertexInputStateCreateInfo();
                pipeline.getInputAssemblyState()->_fillInputAssemblyStateCreateInfo();
                pipeline.getMultisampleState()->_fillMultisampleStateCreateInfo();
                pipeline.getRasterizationState()->_fillRasterizationStateCreateInfo();
                pipeline.getViewportState()->_fillViewportStateCreateInfo();
                pipeline.getColorBlendState()->_fillColorBlendStateCreateInfo();
                pipeline.getPipelineLayout()->_createPipelineLayout();

                for (auto &shaderstage : *pipeline.getShaderStages())
                    shaderstage._fillShaderStageCreateInfo();
            }
            pipeline._fillPipelineCreateInfo();
        }
        VknSpace<VkGraphicsPipelineCreateInfo> *pipelineCreateInfos{
            m_infos->getPipelineCreateInfos(m_relIdxs)};
        VknResult res{vkCreateGraphicsPipelines(
                          m_engine->getObject<VkDevice>(m_absIdxs),
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
        if (!isCompute && !m_filledColorAttachment)
            throw std::runtime_error("No color attachment created before creating subpass.");
        m_infos->fillSubpassDescription(m_relIdxs, m_numSubpasses++, pipelineBindPoint, flags);
        this->addPipeline(subpassIdx);
    }

    std::list<VknFramebuffer> *VknRenderpass::addFramebuffers(VknSwapchain &swapchain)
    {
        for (uint32_t i = 0; i < swapchain.getNumImages(); ++i)
        {
            m_engine->addNewVknObject<VknFramebuffer, VkFramebuffer, VkDevice>(
                i, m_framebuffers, m_relIdxs, m_absIdxs, m_infos);

            m_framebuffers.back().setSwapchain(&swapchain);
            m_framebuffers.back().setDimensions();
            m_framebuffers.back().setSwapchainAttachmentDescriptionIndex(0);
            m_framebuffers.back().addAttachments();
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

    void VknRenderpass::recreateFramebuffersAndPipelines(VknSwapchain &swapchain)
    {
        m_recreatingPipelines = true;
        for (auto &pipeline : m_pipelines)
        {
            m_infos->removePipelineCreateInfo(pipeline.getRelIdxs());
            vkDestroyPipeline(
                m_engine->getObject<VkDevice>(m_absIdxs),
                m_engine->getObject<VkPipeline>(pipeline.getAbsIdxs()),
                nullptr);
            VknViewportState *viewport = pipeline.getViewportState();
            viewport->removeCreateInfo();
            viewport->removeScissors();
            viewport->removeViewports();
            viewport->syncWithSwapchain(swapchain);
            m_createdPipelines = false;
        }
        this->createPipelines();
        m_recreatingPipelines = false;

        uint32_t numSwapchainImages{swapchain.getNumImages()};
        if (m_framebuffers.size() != numSwapchainImages)
        {
            for (auto &framebuffer : m_framebuffers)
                framebuffer.demolishFramebuffer();
            m_framebuffers.clear();
            this->addFramebuffers(swapchain);
            this->createFramebuffers();
        }
        else
            for (auto &framebuffer : m_framebuffers)
                framebuffer.recreateFramebuffer();
    }
}