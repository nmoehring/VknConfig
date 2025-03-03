#include "VknRenderPass.hpp"

namespace vkn
{
    VknRenderPass::VknRenderPass(uint32_t deviceIdx, uint32_t renderPassIdx, VknInfos *infos,
                                 VknResultArchive *archive, VkDevice *device, const bool *deviceCreated)
        : m_infos{infos}, m_archive{archive}, m_device{device}, m_deviceIdx{deviceIdx},
          m_renderPassIdx{renderPassIdx}, m_deviceCreated{deviceCreated}
    {

        if (m_device == VK_NULL_HANDLE)
            throw std::runtime_error("m_device pointer is null.");
        if (m_deviceCreated == nullptr)
            throw std::runtime_error("m_deviceCreated pointer is null.");
        m_infos->initRenderPass(deviceIdx, renderPassIdx);
        m_infos->fillRenderPassPtrs(deviceIdx, renderPassIdx, &m_renderPass, deviceCreated);
    }

    VknRenderPass::~VknRenderPass()
    {
        if (!m_destroyed)
            this->destroy();
    }

    void VknRenderPass::destroy()
    {
        if (!m_destroyed)
        {
            if (m_renderPassCreated)
                vkDestroyRenderPass(*m_device, m_renderPass, nullptr);
            for (auto pipeline : m_pipelines)
                pipeline.destroy();
            m_destroyed = true;
        }
    }

    void VknRenderPass::addPipeline()
    {
        m_rawPipelines.push_back(VkPipeline{});
        uint32_t newSubpassIdx = m_pipelines.size();
        m_pipelines.push_back(VknPipeline{m_deviceIdx, m_renderPassIdx, newSubpassIdx, &m_renderPass,
                                          &m_rawPipelines.back(), m_device, m_infos, m_archive, m_deviceCreated});
    }

    void VknRenderPass::createRenderPass()
    {
        if (!m_deviceCreated)
            throw std::runtime_error("Device not created before creating renderpass.");
        this->fillRenderPassCreateInfo();
        VkRenderPassCreateInfo *createInfo{m_infos->getRenderPassCreateInfo(m_deviceIdx, m_renderPassIdx)};
        VknResult res{vkCreateRenderPass(
                          *m_device, createInfo, VK_NULL_HANDLE, &m_renderPass),
                      "Create renderpass."};
        if (!res.isSuccess())
            throw std::runtime_error(res.toErr("Error creating renderpass."));
        m_archive->store(res);
        m_renderPassCreated = true;
    }

    void VknRenderPass::fillRenderPassCreateInfo(VkRenderPassCreateFlags flags)
    {
        // No flags available, no need to manually fill currently.
        m_infos->fillRenderPassCreateInfo(m_deviceIdx, m_renderPassIdx, m_numAttachments,
                                          m_numSubpasses, m_numSubpassDeps, flags);
    }

    void VknRenderPass::createSubpassDependency(
        uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask,
        VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStageMask, VkAccessFlags dstAccessMask)
    {
        uint32_t subpassDepIdx = m_numSubpassDeps++;
        m_infos->fillSubpassDependency(
            srcSubpass, dstSubpass, srcStageMask, srcAccessMask, dstStageMask, dstAccessMask);
    }

    void VknRenderPass::createAttachment(
        uint32_t subpassIdx, VknAttachmentType attachmentType,
        VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp,
        VkAttachmentStoreOp storeOp, VkAttachmentLoadOp stencilLoadOp,
        VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout,
        VkImageLayout finalLayout, VkImageLayout attachmentRefLayout,
        VkAttachmentDescriptionFlags flags)
    {
        uint32_t attachIdx = m_numAttachments++;

        m_infos->fillAttachmentDescription(
            m_deviceIdx, m_renderPassIdx, attachIdx, format, samples, loadOp, storeOp, stencilLoadOp,
            stencilStoreOp, initialLayout, finalLayout, flags);

        uint32_t refIdx{0};
        m_infos->initVectors<uint32_t>(subpassIdx, NUM_ATTACHMENT_TYPES - 1, m_numAttachRefs);
        m_infos->initVectors<uint32_t>(subpassIdx, m_numPreserveRefs);
        if (attachmentType != PRESERVE_ATTACHMENT)
            refIdx = m_numAttachRefs[subpassIdx][attachmentType]++;
        else
            refIdx = m_numPreserveRefs[subpassIdx]++;

        m_infos->fillAttachmentReference(m_deviceIdx, m_renderPassIdx, subpassIdx, refIdx,
                                         attachmentType, attachIdx, attachmentRefLayout);
    }

    void VknRenderPass::createPipelines()
    {
        if (!m_renderPassCreated)
            throw std::runtime_error("Renderpass not created before creating pipelines.");
        for (auto pipeline : m_pipelines)
            pipeline.fillPipelineCreateInfo();
        std::vector<VkGraphicsPipelineCreateInfo> *pipelineCreateInfos{
            m_infos->getPipelineCreateInfos(m_deviceIdx, m_renderPassIdx)};
        VknResult res{vkCreateGraphicsPipelines(
                          *m_device, VK_NULL_HANDLE, pipelineCreateInfos->size(),
                          pipelineCreateInfos->data(), nullptr, m_rawPipelines.data()),
                      "Create pipeline."};
        if (!res.isSuccess())
            throw std::runtime_error(res.toErr("Error creating pipeline."));
        m_archive->store(res);
        for (auto pipeline : m_pipelines)
            pipeline.setPipelineCreated();
    }

    void VknRenderPass::createSubpass(
        uint32_t subpassIdx, VkPipelineBindPoint pipelineBindPoint,
        VkSubpassDescriptionFlags flags)
    {
        if (m_numAttachRefs.size() < subpassIdx + 1)
            throw std::runtime_error("No attachment created before creating subpass.");
        ++m_numSubpasses;
        m_infos->fillSubpassDescription(
            m_numAttachRefs[subpassIdx][COLOR_ATTACHMENT],
            m_numAttachRefs[subpassIdx][INPUT_ATTACHMENT],
            m_numAttachRefs[subpassIdx][RESOLVE_ATTACHMENT],
            m_numAttachRefs[subpassIdx][DEPTH_STENCIL_ATTACHMENT],
            m_numPreserveRefs[subpassIdx], m_deviceIdx, m_renderPassIdx,
            subpassIdx, pipelineBindPoint, flags);
        this->addPipeline();
    }
}