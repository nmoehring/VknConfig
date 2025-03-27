#pragma once

#include <vector>

#include "VknPipeline.hpp"
#include "VknFramebuffer.hpp"
#include "VknEngine.hpp"

namespace vkn
{
    class VknRenderpass
    {
    public:
        VknRenderpass() = delete;
        VknRenderpass(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        void createAttachment(
            uint32_t subpassIdx,
            VknAttachmentType attachmentType = COLOR_ATTACHMENT,
            VkFormat format = VK_FORMAT_B8G8R8A8_SRGB,
            VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
            VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VkImageLayout initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VkImageLayout attachmentRefLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            VkAttachmentDescriptionFlags flags = 0);
        void createSubpassDependency(
            uint32_t srcSubpass = VK_SUBPASS_EXTERNAL, uint32_t dstSubpass = 0,
            VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VkAccessFlags srcAccessMask = 0,
            VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VkAccessFlags dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
        void createSubpass(
            uint32_t subpassIdx, bool isCompute = false,
            VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            VkSubpassDescriptionFlags flags = 0);
        void createRenderpass();
        void createPipelines();
        VkRenderPass *getVkRenderPass() { return m_renderpass; }
        VknPipeline *getPipeline(uint32_t idx);
        bool getVkRenderPassCreated() { return createdRenderpass; }

    private:
        // Engine
        VknEngine *m_engine{nullptr};
        VknIdxs m_relIdxs{};
        VknIdxs m_absIdxs{};
        VknInfos *m_infos{nullptr};

        // Wrapped object
        VkRenderPass *m_renderpass{};

        // Members
        std::list<VknPipeline> m_pipelines;
        std::vector<VknFramebuffer> m_framebuffers{};
        std::vector<std::vector<uint32_t>> m_numAttachRefs{};
        std::vector<uint32_t> m_numPreserveRefs{};

        // Params
        std::vector<VkPipeline> m_rawPipelines; // index should be subpass index

        // State
        bool addedDevices{false};
        bool createdRenderpass{false};
        bool createdPipelines{false};
        bool filledColorAttachment{false};
        bool placeholder;

        uint32_t m_numSubpassDeps{0};
        uint32_t m_numAttachments{0};
        uint32_t m_numSubpasses{0};
        uint32_t m_numPipelines{0};

        void addPipeline();
        void fillRenderpassCreateInfo(
            VkRenderPassCreateFlags flags = 0); // No flags currently available, no need to fill.
    };
}