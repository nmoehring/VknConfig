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

    void VknRenderpass::addPipeline(uint32_t subpassIdx)
    {
        if (subpassIdx != m_pipelines.size())
            throw std::runtime_error("SubpassIdx passed to addPipeline is invalid. Should be next idx.");
        if (!m_createdRenderpass)
            throw std::runtime_error("Renderpass not created before adding pipeline.");
        m_rawPipelines.push_back(VkPipeline{});
        VknIdxs relIdxs{m_relIdxs};
        relIdxs.subpassIdx = subpassIdx;
        m_pipelines.emplace_back(m_engine, relIdxs, m_absIdxs, m_infos);
    }

    void VknRenderpass::createRenderpass()
    {
        if (m_createdRenderpass)
            throw std::runtime_error("Renderpass already created.");
        this->fillRenderpassCreateInfo();
        VkRenderPassCreateInfo *createInfo{
            m_infos->getRenderpassCreateInfo(m_relIdxs)};
        m_absIdxs.renderpassIdx = m_engine->push_back(VkRenderPass{});
        VknResult res{vkCreateRenderPass(
                          m_engine->getObject<VkDevice>(m_absIdxs.deviceIdx.value()),
                          createInfo, VK_NULL_HANDLE,
                          &m_engine->getObject<VkRenderPass>(m_absIdxs.renderpassIdx.value())),
                      "Create renderpass."};
        m_createdRenderpass = true;
    }

    VknPipeline *VknRenderpass::getPipeline(uint32_t pipelineIdx)
    {
        if (pipelineIdx + 1 > m_pipelines.size())
            throw std::runtime_error("Pipeline index out of range.");
        std::list<VknPipeline>::iterator it = m_pipelines.begin();
        std::advance(it, pipelineIdx);
        return &(*it);
    }

    VknFramebuffer *VknRenderpass::getFramebuffer(uint32_t bufferIdx)
    {
        if (bufferIdx + 1 > m_framebuffers.size())
            throw std::runtime_error("Framebuffer index out of range.");
        std::list<VknFramebuffer>::iterator it = m_framebuffers.begin();
        std::advance(it, bufferIdx);
        return &(*it);
    }

    void VknRenderpass::fillRenderpassCreateInfo(VkRenderPassCreateFlags flags)
    {
        // No flags available, no need to manually fill currently.
        m_infos->fillRenderpassCreateInfo(m_relIdxs, m_numAttachments,
                                          m_numSubpasses, m_numSubpassDeps, flags);
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
        size_t numPipelinesSize = m_engine->getObjectVector<VkPipeline>().size();
        if (numPipelinesSize > std::numeric_limits<uint32_t>::max())
            throw std::runtime_error("Too many pipelines. Max supported: " + std::to_string(std::numeric_limits<uint32_t>::max()));
        uint32_t numPipelines = static_cast<uint32_t>(numPipelinesSize);
        for (auto &pipeline : m_pipelines)
        {
            pipeline.fillPipelineCreateInfo();
            pipeline.setAbsIdx(m_engine->push_back(VkPipeline{}));
        }
        std::vector<VkGraphicsPipelineCreateInfo> *pipelineCreateInfos{
            m_infos->getPipelineCreateInfos(m_relIdxs)};
        VknResult res{vkCreateGraphicsPipelines(
                          m_engine->getObject<VkDevice>(m_absIdxs.deviceIdx.value()),
                          VK_NULL_HANDLE, m_numSubpasses,
                          pipelineCreateInfos->data(), nullptr,
                          &m_engine->getObjectVector<VkPipeline>().data()[numPipelines]),
                      "Create pipeline."};
        m_createdPipelines = true;
    }

    void VknRenderpass::createSubpass(
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