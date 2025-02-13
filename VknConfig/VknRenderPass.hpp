#pragma once

#include <vector>

#include "VknPipeline.hpp"
#include "VknDevice.hpp"

namespace vkn
{
    class VknRenderPass
    {
    public:
        VknRenderPass() {}
        VknRenderPass(VknInfos *infos, VknResultArchive *archive);
        ~VknRenderPass();
        void destroy();

        VkAttachmentReference createAttachment(
            VkFormat format = VK_FORMAT_B8G8R8A8_SRGB,
            VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
            VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VkImageLayout attachmentRefLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        void createSubpassDependency(
            uint32_t srcSubpass = VK_SUBPASS_EXTERNAL, uint32_t dstSubpass = 0,
            VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VkAccessFlags srcAccessMask = 0,
            VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VkAccessFlags dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
        void fillRenderPassCreateInfo(
            VkRenderPassCreateFlags flags = INT_MAX);
        void createRenderPass();
        void addPipeline();
        void addDevice(VknDevice *dev);
        void createPipelines();
        bool deviceAdded() { return m_deviceAdded; }
        VkRenderPass *getVkRenderPass() { return &m_renderPass; }
        VknPipeline *getPipeline(int idx) { return &(m_pipelines[idx]); }

    private:
        VknDevice *m_device{nullptr};
        VknInfos *m_infos{nullptr};
        VknResultArchive *m_archive{nullptr};
        VkRenderPassCreateInfo m_createInfo{};
        VkRenderPass m_renderPass{};
        static int m_subpassCount;

        std::vector<VkAttachmentDescription> m_attachments;
        std::vector<VkSubpassDependency> m_dependencies;
        std::vector<VkGraphicsPipelineCreateInfo> m_pipelineCreateInfos{};
        std::vector<VkPipeline> m_rawPipelines; // index should be subpass index
        std::vector<VkSubpassDescription> m_subpasses;
        std::vector<VknPipeline> m_pipelines;

        bool m_destroyed{false};
        bool m_deviceAdded{false};
        bool m_renderPassCreated{false};

        void collectSubpassDescriptions();
        void collectRawPipelines();
    };
}