#include "VknRenderpass.hpp"

namespace vkn
{
    VknRenderpass::VknRenderpass() : m_deviceIdx{0}, m_renderpassIdx{0}, m_archive{nullptr},
                                     m_infos{nullptr}, m_device{nullptr}, m_deviceCreated{nullptr}, m_placeholder{true}
    {
    }

    VknRenderpass::VknRenderpass(uint32_t deviceIdx, uint32_t renderpassIdx, VknInfos *infos,
                                 VknResultArchive *archive, VkDevice *device, const bool *deviceCreated)
        : m_infos{infos}, m_archive{archive}, m_device{device}, m_deviceIdx{deviceIdx},
          m_renderpassIdx{renderpassIdx}, m_deviceCreated{deviceCreated}, m_placeholder{false}
    {

        if (m_device == VK_NULL_HANDLE)
            throw std::runtime_error("m_device pointer is null.");
        if (m_deviceCreated == nullptr)
            throw std::runtime_error("m_deviceCreated pointer is null.");
        m_infos->initRenderpass(deviceIdx, renderpassIdx);
        m_infos->fillRenderpassPtrs(deviceIdx, renderpassIdx, &m_renderpass, deviceCreated);
    }

    VknRenderpass::~VknRenderpass()
    {
        if (!m_destroyed && !m_placeholder)
            this->destroy();
    }

    void VknRenderpass::destroy()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to destroy a placeholder object.");
        if (m_destroyed)
            throw std::runtime_error("Renderpass object already destroyed.");

        if (m_renderpassCreated)
            vkDestroyRenderpass(*m_device, m_renderpass, nullptr);
        for (auto &pipeline : m_pipelines)
            pipeline.destroy();
        m_destroyed = true;
        std::cout << "VknRenderpass DESTROYED." << std::endl;
    }

    void VknRenderpass::addPipeline()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        m_rawPipelines.push_back(VkPipeline{});
        uint32_t newSubpassIdx = m_pipelines.size();
        m_pipelines.emplace_back(m_deviceIdx, m_renderpassIdx, newSubpassIdx, &m_renderpass,
                                 &m_rawPipelines.back(), m_device, m_infos, m_archive, m_deviceCreated);
    }

    void VknRenderpass::createRenderpass()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (!m_deviceCreated)
            throw std::runtime_error("Device not created before creating renderpass.");
        this->fillRenderpassCreateInfo();
        VkRenderPassCreateInfo *createInfo{m_infos->getRenderpassCreateInfo(m_deviceIdx, m_renderpassIdx)};
        VknResult res{vkCreateRenderpass(
                          *m_device, createInfo, VK_NULL_HANDLE, &m_renderpass),
                      "Create renderpass."};
        if (!res.isSuccess())
            throw std::runtime_error(res.toErr("Error creating renderpass."));
        m_archive->store(res);
        m_renderpassCreated = true;
    }

    VknPipeline *VknRenderpass::getPipeline(uint32_t pipelineIdx)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (pipelineIdx != m_numPipelines)
            throw std::runtime_error("Pipeline index out of range.");
        std::list<VknPipeline>::iterator it = m_pipelines.begin();
        std::advance(it, pipelineIdx);
        return &(*it);
    }

    void VknRenderpass::fillRenderpassCreateInfo(VkRenderPassCreateFlags flags)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        // No flags available, no need to manually fill currently.
        m_infos->fillRenderpassCreateInfo(m_deviceIdx, m_renderpassIdx, m_numAttachments,
                                          m_numSubpasses, m_numSubpassDeps, flags);
    }

    void VknRenderpass::createSubpassDependency(
        uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask,
        VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStageMask, VkAccessFlags dstAccessMask)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        uint32_t subpassDepIdx = m_numSubpassDeps++;
        m_infos->fillSubpassDependency(
            srcSubpass, dstSubpass, srcStageMask, srcAccessMask, dstStageMask, dstAccessMask);
    }

    void VknRenderpass::createAttachment(
        uint32_t subpassIdx, VknAttachmentType attachmentType,
        VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp,
        VkAttachmentStoreOp storeOp, VkAttachmentLoadOp stencilLoadOp,
        VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout,
        VkImageLayout finalLayout, VkImageLayout attachmentRefLayout,
        VkAttachmentDescriptionFlags flags)
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        uint32_t attachIdx = m_numAttachments++;

        m_infos->fillAttachmentDescription(
            m_deviceIdx, m_renderpassIdx, attachIdx, format, samples, loadOp, storeOp, stencilLoadOp,
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

        m_infos->fillAttachmentReference(m_deviceIdx, m_renderpassIdx, subpassIdx, refIdx,
                                         attachmentType, attachIdx, attachmentRefLayout);
    }

    void VknRenderpass::createPipelines()
    {
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (!m_renderpassCreated)
            throw std::runtime_error("Renderpass not created before creating pipelines.");
        for (auto &pipeline : m_pipelines)
            pipeline.fillPipelineCreateInfo();
        std::vector<VkGraphicsPipelineCreateInfo> *pipelineCreateInfos{
            m_infos->getPipelineCreateInfos(m_deviceIdx, m_renderpassIdx)};
        VknResult res{vkCreateGraphicsPipelines(
                          *m_device, VK_NULL_HANDLE, m_numSubpasses,
                          pipelineCreateInfos->data(), nullptr, m_rawPipelines.data()),
                      "Create pipeline."};
        if (!res.isSuccess())
            throw std::runtime_error(res.toErr("Error creating pipeline."));
        m_archive->store(res);
        for (auto &pipeline : m_pipelines)
            pipeline.setPipelineCreated();
    }

    void VknRenderpass::createSubpass(
        uint32_t subpassIdx, bool isCompute, VkPipelineBindPoint pipelineBindPoint,
        VkSubpassDescriptionFlags flags)
    {
        if (subpassIdx != m_numSubpasses)
            throw std::runtime_error("SubpassIdx is invalid.");
        if (m_placeholder)
            throw std::runtime_error("Trying to configure a placeholder object.");
        if (!isCompute && !m_filledColorAttachment)
            throw std::runtime_error("No attachment created before creating subpass.");
        m_infos->fillSubpassDescription(
            m_numAttachRefs[subpassIdx][COLOR_ATTACHMENT],
            m_numAttachRefs[subpassIdx][INPUT_ATTACHMENT],
            m_numAttachRefs[subpassIdx][RESOLVE_ATTACHMENT],
            m_numAttachRefs[subpassIdx][DEPTH_STENCIL_ATTACHMENT],
            m_numPreserveRefs[subpassIdx], m_deviceIdx, m_renderpassIdx,
            m_numSubpasses++, pipelineBindPoint, flags);
        this->addPipeline();
    }
}