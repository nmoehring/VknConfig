#include "VknRenderPass.hpp"
#include "VknResult.hpp"

namespace vkn
{
    int VknRenderPass::m_subpassCount = 0;

    VknRenderPass::VknRenderPass(VknInfos *infos, VknResultArchive *archive)
        : m_infos{infos}, m_archive{archive}
    {
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
        m_pipelines.push_back(VknPipeline(m_device, m_infos, m_archive, m_subpassCount++));
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
        VknResult res{vkCreateRenderPass(
                          *(m_device->getVkDevice()), &m_createInfo, nullptr, &m_renderPass),
                      "Create renderpass."};
        if (!res.isSuccess())
            throw std::runtime_error(res.toErr("Error creating renderpass."));
        m_archive->store(res);
        m_renderPassCreated = true;
    }

    void VknRenderPass::fillRenderPassCreateInfo(VkRenderPassCreateFlags flags)
    {
        if (!m_deviceAdded)
            throw std::runtime_error("Device not added to renderpass before filling create info.");
        this->collectSubpassDescriptions();
        m_createInfo = m_infos->fillRenderPassCreateInfo(m_attachments, m_subpasses, m_dependencies, flags);
    }

    void VknRenderPass::createSubpassDependency(
        uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask,
        VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStageMask, VkAccessFlags dstAccessMask)
    {
        if (!m_deviceAdded)
            throw std::runtime_error("Device not added to renderpass before creating subpass dependency.");
        m_dependencies.push_back(VkSubpassDependency{});
        m_dependencies.back().srcSubpass = srcSubpass;
        m_dependencies.back().dstSubpass = dstSubpass;
        m_dependencies.back().srcStageMask = srcStageMask;
        m_dependencies.back().srcAccessMask = srcAccessMask;
        m_dependencies.back().dstStageMask = dstStageMask;
        m_dependencies.back().dstAccessMask = dstAccessMask;
    }

    VkAttachmentReference VknRenderPass::createAttachment(
        VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp,
        VkAttachmentStoreOp storeOp, VkAttachmentLoadOp stencilLoadOp,
        VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout,
        VkImageLayout finalLayout, VkImageLayout attachmentRefLayout)
    {
        if (!m_deviceAdded)
            throw std::runtime_error("Device not added to renderpass before creating attachment.");
        m_attachments.push_back(VkAttachmentDescription{});
        m_attachments.back().format = format; // Set to your swapchain image format
        m_attachments.back().samples = samples;
        m_attachments.back().loadOp = loadOp;
        m_attachments.back().storeOp = storeOp;
        m_attachments.back().stencilLoadOp = stencilLoadOp;
        m_attachments.back().stencilStoreOp = stencilStoreOp;
        m_attachments.back().initialLayout = initialLayout;
        m_attachments.back().finalLayout = finalLayout;
        m_attachments.back().flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;

        VkAttachmentReference attachmentRef = {};
        attachmentRef.attachment = m_attachments.size() - 1;
        attachmentRef.layout = attachmentRefLayout;

        return attachmentRef;
    }

    void VknRenderPass::createPipelines()
    {
        if (!m_deviceAdded)
            throw std::runtime_error("Device not added to renderpass before creating pipelines.");
        this->collectRawPipelines();
        VknResult res{vkCreateGraphicsPipelines(
                          *(m_device->getVkDevice()), nullptr, m_pipelineCreateInfos.size(),
                          m_pipelineCreateInfos.data(), nullptr, m_rawPipelines.data()),
                      "Create pipeline."};
        if (!res.isSuccess())
            throw std::runtime_error(res.toErr("Error creating pipeline."));
        m_archive->store(res);
        for (auto pipeline : m_pipelines)
            pipeline.setPipelineCreated();
    }

    void VknRenderPass::collectRawPipelines()
    {
        if (!m_deviceAdded)
            throw std::runtime_error("Device not added to renderpass before collecting raw pipelines.");
        m_pipelineCreateInfos.clear();
        m_rawPipelines.clear();
        for (auto pipeline : m_pipelines)
        {
            m_pipelineCreateInfos.push_back(pipeline.getCreateInfo());
            m_rawPipelines.push_back(pipeline.getVkPipeline());
        }
    }

    void VknRenderPass::collectSubpassDescriptions()
    {
        if (!m_deviceAdded)
            throw std::runtime_error("Device not added to renderpass before collecting subpass descriptions.");
        m_subpasses.clear();
        for (auto pipeline : m_pipelines)
            m_subpasses.push_back(pipeline.getSubpassDescription());
    }
}