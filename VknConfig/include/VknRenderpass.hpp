/**
 * @file VknRenderpass.hpp
 * @brief Manages a Vulkan render pass and its associated pipelines.
 *
 * VknRenderpass is a hierarchy-bound class within the VknConfig project.
 * It is used by VknDevice to manage a Vulkan render pass and its associated pipelines.
 * VknRenderpass depends on VknEngine, VknInfos, VknPipeline, and VknIdxs.
 * VknRenderpass is a child of VknDevice.
 *
 * Hierarchy Graph:
 * [VknConfig] (Top-Level)
 *     |
 *     +-- [VknDevice]
 *         |
 *         +-- [VknPhysicalDevice]
 *         |   |
 *         |   +-- [VknQueueFamily] ^ / \
 *         |
 *         +-- [VknSwapchain]
 *         |   |
 *         |   +-- [VknImageView] ^ / \
 *         |
 *         +-- [VknRenderpass]  <<=== YOU ARE HERE
 *             |
 *             +-- [VknFramebuffer] ^ / \
 *             |
 *             +-- [VknPipeline]
 *                 |
 *                 +-- [VknVertexInputState] ^ / \
 *                 +-- [VknInputAssemblyState] ^ / \
 *                 +-- [VknMultisampleState] ^ / \
 *                 +-- [VknRasterizationState] ^ / \
 *                 +-- [VknShaderStage] ^ / \
 *                 +-- [VknViewportState] ^ / \
 *
 * [VknEngine] (Free/Top-Level)
 * [VknInfos] (Free/Top-Level)
 * [VknResult] (Free/Top-Level)
 */

#pragma once

#include <vector>

#include "VknPipeline.hpp"
#include "VknFramebuffer.hpp"

namespace vkn
{
    class VknRenderpass
    {
    public:
        // Overloads
        VknRenderpass() = default;
        VknRenderpass(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        // Add
        VknPipeline *addPipeline(uint32_t subpassIdx);
        VknFramebuffer *addFramebuffer(uint32_t framebufferIdx);

        // Config
        void addAttachment(
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
        void addSubpassDependency(
            uint32_t srcSubpass = VK_SUBPASS_EXTERNAL, uint32_t dstSubpass = 0,
            VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VkAccessFlags srcAccessMask = 0,
            VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VkAccessFlags dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
        void addSubpass(
            uint32_t subpassIdx, bool isCompute = false,
            VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            VkSubpassDescriptionFlags flags = 0);

        // Create
        void fillRenderpassCreateInfo(
            VkRenderPassCreateFlags flags = 0); // No flags currently available, no need to fill.
        void createRenderpass();
        void createPipelines();

        // Getters
        VkRenderPass *getVkRenderPass() { return &m_engine->getObject<VkRenderPass>(m_absIdxs.renderpassIdx.value()); }
        VknPipeline *getPipeline(uint32_t idx);
        VknFramebuffer *getFramebuffer(uint32_t idx);
        bool getVkRenderPassCreated() { return m_createdRenderpass; }

    private:
        // Engine
        VknEngine *m_engine{nullptr};
        VknIdxs m_relIdxs{};
        VknIdxs m_absIdxs{};
        VknInfos *m_infos{nullptr};

        // Members
        std::list<VknPipeline> m_pipelines;
        std::list<VknFramebuffer> m_framebuffers{};
        std::vector<std::vector<uint32_t>> m_numAttachRefs{};
        std::vector<uint32_t> m_numPreserveRefs{};

        // Params
        std::vector<VkPipeline> m_rawPipelines; // index should be subpass index

        // State
        bool m_addedDevices{false};
        bool m_createdRenderpass{false};
        bool m_createdPipelines{false};
        bool m_filledColorAttachment{false};

        uint32_t m_numSubpassDeps{0};
        uint32_t m_numAttachments{0};
        uint32_t m_numSubpasses{0};
    };
}