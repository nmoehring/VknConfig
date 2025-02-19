#include "VknRenderPass.hpp"
#include "VknResult.hpp"
#include "VknInfos.hpp"

namespace vkn
{
    int VknRenderPass::m_subpassCount = 0;

    VknRenderPass::VknRenderPass(VknInfos *infos, VknResultArchive *archive)
        : m_infos{infos}, m_archive{archive}
    {
        m_attachmentRefs = infos->getAllAttachmentReferences();
        m_preserveAttachments = infos->getAllPreserveAttachments();
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
                vkDestroyRenderPass(*(m_device->getVkDevice()), m_renderPass, nullptr);
            for (auto pipeline : m_pipelines)
                pipeline.destroy();
            m_destroyed = true;
        }
    }

    void VknRenderPass::addPipeline()
    {
        if (!m_deviceAdded)
            throw std::runtime_error("Device not added to renderpass before adding pipeline.");
        if (m_subpasses.size() != m_pipelines.size() + 1)
            throw std::runtime_error("Subpass not created before pipeline added.");
        int pipelineIdx = m_pipelines.size();
        m_pipelineCreateInfos.push_back(nullptr);
        m_rawPipelines.push_back(VkPipeline{});
        m_pipelines.push_back(VknPipeline(m_subpasses.back(), &(m_rawPipelines.back()),
                                          m_pipelineCreateInfos.back(), m_device, m_infos,
                                          m_archive, pipelineIdx));
    }

    void VknRenderPass::addDevice(VknDevice *dev)
    {
        if (!dev->vkDeviceCreated())
            throw std::runtime_error("Device not created before adding to renderpass.");
        m_device = dev;
        m_deviceAdded = true;
    }

    void VknRenderPass::createRenderPass()
    {
        if (!m_deviceAdded)
            throw std::runtime_error("Device not added to renderpass before creating VkRenderpass.");
        if (m_pipelines.size() != m_subpasses.size())
            throw std::runtime_error("Number of pipelines != number of subpasses at render pass creation.");
        this->fillRenderPassCreateInfo();
        VknResult res{vkCreateRenderPass(
                          *(m_device->getVkDevice()), m_createInfo, nullptr, &m_renderPass),
                      "Create renderpass."};
        if (!res.isSuccess())
            throw std::runtime_error(res.toErr("Error creating renderpass."));
        m_archive->store(res);
        m_renderPassCreated = true;
    }

    void VknRenderPass::fillRenderPassCreateInfo(VkRenderPassCreateFlags flags)
    {
        // No flags available, no need to manually fill currently.
        if (!m_deviceAdded)
            throw std::runtime_error("Device not added to renderpass before filling create info.");
        m_createInfo = m_infos->fillRenderPassCreateInfo(flags);
    }

    void VknRenderPass::createSubpassDependency(
        uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask,
        VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStageMask, VkAccessFlags dstAccessMask)
    {
        if (!m_deviceAdded)
            throw std::runtime_error("Device not added to renderpass before creating subpass dependency.");
        m_dependencies.push_back(m_infos->fillSubpassDependency(
            srcSubpass, dstSubpass, srcStageMask, srcAccessMask, dstStageMask, dstAccessMask));
    }

    void VknRenderPass::createAttachment(
        uint32_t subpassIdx, VknAttachmentType attachmentType,
        VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp,
        VkAttachmentStoreOp storeOp, VkAttachmentLoadOp stencilLoadOp,
        VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout,
        VkImageLayout finalLayout, VkImageLayout attachmentRefLayout,
        VkAttachmentDescriptionFlags flags)
    {
        if (!m_deviceAdded)
            throw std::runtime_error("Device not added to renderpass before creating attachment.");

        uint32_t attachIdx = m_attachments.size();

        m_attachments.push_back(m_infos->fillAttachmentDescription(
            format, samples, loadOp, storeOp, stencilLoadOp, stencilStoreOp, initialLayout, finalLayout, flags));

        m_infos->fillAttachmentReference(subpassIdx, attachmentType, attachIdx, attachmentRefLayout);
    }

    void VknRenderPass::createPipelines()
    {
        if (!m_deviceAdded)
            throw std::runtime_error("Device not added to renderpass before creating pipelines.");
        VknResult res{vkCreateGraphicsPipelines(
                          *(m_device->getVkDevice()), nullptr, m_pipelineCreateInfos.size(),
                          *(m_pipelineCreateInfos.data()), nullptr, m_rawPipelines.data()),
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
        if (!m_deviceAdded)
            throw std::runtime_error("Device not added before creating subpass.");
        uint32_t subpassIdx = m_subpasses.size();
        m_subpasses.push_back(m_infos->fillSubpassDescription(subpassIdx, pipelineBindPoint, flags));
        this->addPipeline();
    }
}