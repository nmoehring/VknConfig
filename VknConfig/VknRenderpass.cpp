#include "include/VknRenderpass.hpp"

namespace vkn
{
    VknRenderpass::VknRenderpass(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
        // Move to create function
        m_infos->initRenderpass(relIdxs);
    }

    VknFramebuffer *VknRenderpass::addFramebuffer(uint32_t framebufferIdx)
    {
        if (framebufferIdx + 1 > m_framebuffers.size())
            throw std::runtime_error("FramebufferIdx invalid. Should be next index.");
        if (!m_createdRenderpass)
            throw std::runtime_error("Renderpass not created before adding framebuffer.");
        return &addNewVknObject<VknFramebuffer, VkFramebuffer>(
            framebufferIdx, m_framebuffers, m_engine, m_relIdxs, m_absIdxs, m_infos);
    }

    VknPipeline *VknRenderpass::addPipeline(uint32_t subpassIdx)
    {
        if (subpassIdx != m_pipelines.size())
            throw std::runtime_error("SubpassIdx passed to addPipeline is invalid. Should be next idx.");

        if (m_pipelineStartIdx == 3123123123)
            m_pipelineStartIdx = m_engine->getVectorSize<VkPipeline>();
        return &addNewVknObject<VknPipeline, VkPipeline>(subpassIdx, m_pipelines,
                                                         m_engine, m_relIdxs, m_absIdxs, m_infos);
    }

    void VknRenderpass::createRenderpass()
    {
        if (m_createdRenderpass)
            throw std::runtime_error("Renderpass already created.");
        VkRenderPassCreateInfo *createInfo = m_infos->fillRenderpassCreateInfo(m_relIdxs, m_numAttachments,
                                                                               m_numSubpasses, m_numSubpassDeps, 0); // Flags not used ever
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

    void VknRenderpass::addSubpassDependency(
        uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask,
        VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStageMask, VkAccessFlags dstAccessMask)
    {
        uint32_t subpassDepIdx = m_numSubpassDeps++;
        m_infos->fillSubpassDependency(
            srcSubpass, dstSubpass, srcStageMask, srcAccessMask, dstStageMask, dstAccessMask);
    }

    void VknRenderpass::addAttachment(
        uint32_t subpassIdx, VknAttachmentType attachmentType,
        VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp,
        VkAttachmentStoreOp storeOp, VkAttachmentLoadOp stencilLoadOp,
        VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout,
        VkImageLayout finalLayout, VkImageLayout attachmentRefLayout,
        VkAttachmentDescriptionFlags flags)
    {
        uint32_t attachIdx = m_numAttachments++;

        m_infos->fillAttachmentDescription(
            m_relIdxs, attachIdx, format, samples, loadOp, storeOp, stencilLoadOp,
            stencilStoreOp, initialLayout, finalLayout, flags);

        uint32_t refIdx{0};
        m_infos->initVectors<uint32_t>(subpassIdx, NUM_ATTACHMENT_TYPES - 1, m_numAttachRefs);
        m_infos->initVectors<uint32_t>(subpassIdx, m_numPreserveRefs);
        if (attachmentType != PRESERVE_ATTACHMENT)
            refIdx = m_numAttachRefs[subpassIdx][attachmentType]++;
        else
            refIdx = m_numPreserveRefs[subpassIdx]++;

        if (attachmentType == COLOR_ATTACHMENT)
            m_filledColorAttachment = true;

        m_infos->fillAttachmentReference(m_relIdxs, subpassIdx, refIdx,
                                         attachmentType, attachIdx, attachmentRefLayout);
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

        std::span<VkPipeline> pipelines{
            m_engine->getVectorSlice<VkPipeline>(
                m_pipelineStartIdx, m_pipelines.size())};
        for (auto &pipeline : m_pipelines)
            pipeline._fillPipelineCreateInfo();
        std::vector<VkGraphicsPipelineCreateInfo> *pipelineCreateInfos{
            m_infos->getPipelineCreateInfos(m_relIdxs)};
        VknResult res{vkCreateGraphicsPipelines(
                          m_engine->getObject<VkDevice>(m_absIdxs),
                          VK_NULL_HANDLE, m_numSubpasses,
                          pipelineCreateInfos->data(), nullptr,
                          pipelines.data()),
                      "Create pipeline."};
        m_createdPipelines = true;
    }

    void VknRenderpass::addSubpass(
        uint32_t subpassIdx, bool isCompute, VkPipelineBindPoint pipelineBindPoint,
        VkSubpassDescriptionFlags flags)
    {
        if (!isCompute && !m_filledColorAttachment)
            throw std::runtime_error("No attachment created before creating subpass.");
        m_infos->fillSubpassDescription(
            m_numAttachRefs[subpassIdx][COLOR_ATTACHMENT],
            m_numAttachRefs[subpassIdx][INPUT_ATTACHMENT],
            m_numAttachRefs[subpassIdx][RESOLVE_ATTACHMENT],
            m_numAttachRefs[subpassIdx][DEPTH_STENCIL_ATTACHMENT],
            m_numPreserveRefs[subpassIdx], m_relIdxs,
            m_numSubpasses++, pipelineBindPoint, flags);
        this->addPipeline(subpassIdx);
    }
}