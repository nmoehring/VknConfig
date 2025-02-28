#include "VknRenderPass.hpp"

namespace vkn
{
    int VknRenderPass::m_subpassCount = 0;

    VknRenderPass::VknRenderPass(uint32_t deviceIdx, uint32_t renderPassIdx, VknInfos *infos,
                                 VknResultArchive *archive, VkDevice *device, const bool *deviceCreated)
        : m_infos{infos}, m_archive{archive}, m_device{device}, m_deviceIdx{deviceIdx},
          m_renderPassIdx{renderPassIdx}, m_deviceCreated{deviceCreated}
    {
        m_infos->initRenderPass(deviceIdx, renderPassIdx);
        m_attachmentRefs = infos->getRenderPassAttachmentReferences(m_deviceIdx, m_renderPassIdx);
        m_preserveAttachments = infos->getRenderPassPreserveAttachments(m_deviceIdx, m_renderPassIdx);
        m_pipelineCreateInfos = infos->getPipelineCreateInfos();
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
        if (m_subpasses.size() != m_pipelines.size() + 1)
            throw std::runtime_error("Subpass not created before pipeline added.");
        m_rawPipelines.push_back(VkPipeline{});
        uint32_t newSubpassIdx = m_pipelines.size();
        m_pipelines.push_back(VknPipeline{m_deviceIdx, m_renderPassIdx, newSubpassIdx, &m_renderPass,
                                          m_subpasses[newSubpassIdx], &(m_rawPipelines.back()),
                                          m_device, m_infos, m_archive, m_deviceCreated});
    }

    void VknRenderPass::createRenderPass()
    {
        if (!m_deviceCreated)
            throw std::runtime_error("Device not created before creating renderpass.");
        if (m_pipelines.size() != m_subpasses.size())
            throw std::runtime_error("Number of pipelines != number of subpasses at renderpass creation.");
        this->fillRenderPassCreateInfo();
        VknResult res{vkCreateRenderPass(
                          *m_device, m_createInfo, VK_NULL_HANDLE, &m_renderPass),
                      "Create renderpass."};
        if (!res.isSuccess())
            throw std::runtime_error(res.toErr("Error creating renderpass."));
        m_archive->store(res);
        m_renderPassCreated = true;
    }

    void VknRenderPass::fillRenderPassCreateInfo(VkRenderPassCreateFlags flags)
    {
        // No flags available, no need to manually fill currently.
        m_createInfo = m_infos->fillRenderPassCreateInfo(flags);
    }

    void VknRenderPass::createSubpassDependency(
        uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask,
        VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStageMask, VkAccessFlags dstAccessMask)
    {
        m_dependencies.push_back(m_infos->fillSubpassDependency(
            srcSubpass, dstSubpass, srcStageMask, srcAccessMask, dstStageMask, dstAccessMask));
    }

    void VknRenderPass::createAttachment(
        VknAttachmentType attachmentType,
        VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp,
        VkAttachmentStoreOp storeOp, VkAttachmentLoadOp stencilLoadOp,
        VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout,
        VkImageLayout finalLayout, VkImageLayout attachmentRefLayout,
        VkAttachmentDescriptionFlags flags)
    {

        uint32_t subpassIdx = m_subpasses.size();
        uint32_t attachIdx = m_attachments.size();

        m_attachments.push_back(m_infos->fillAttachmentDescription(
            m_deviceIdx, m_renderPassIdx, format, samples, loadOp, storeOp, stencilLoadOp,
            stencilStoreOp, initialLayout, finalLayout, flags));

        m_infos->fillAttachmentReference(m_deviceIdx, m_renderPassIdx, subpassIdx, attachmentType, attachIdx, attachmentRefLayout);
    }

    void VknRenderPass::createPipelines()
    {
        if (!m_renderPassCreated)
            throw std::runtime_error("Renderpass not created before creating pipelines.");
        if (m_pipelines.size() > m_pipelineCreateInfos->size())
            throw std::runtime_error("Not all pipeline create infos filled before calling createPipelines().");
        if (m_pipelineCreateInfos->size() > m_subpasses.size())
            throw std::runtime_error("Not all subpass descriptions filled before calling createPipelines().");
        VknResult res{vkCreateGraphicsPipelines(
                          *m_device, VK_NULL_HANDLE, m_pipelineCreateInfos->size(),
                          m_pipelineCreateInfos->data(), nullptr, m_rawPipelines.data()),
                      "Create pipeline."};
        if (!res.isSuccess())
            throw std::runtime_error(res.toErr("Error creating pipeline."));
        m_archive->store(res);
        for (auto pipeline : m_pipelines)
            pipeline.setPipelineCreated();
    }

    void VknRenderPass::createSubpass(
        VkPipelineBindPoint pipelineBindPoint,
        VkSubpassDescriptionFlags flags)
    {
        uint32_t subpassIdx = m_subpasses.size();
        m_subpasses.push_back(m_infos->fillSubpassDescription(m_deviceIdx, m_renderPassIdx, pipelineBindPoint, flags));
        this->addPipeline();
    }
}