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
 *                 +-- [VknPipelineLayout]
 *                 |   |
 *                 |   +-- [VknDescriptorSetLayout]
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

#include "VknObject.hpp"
#include "VknPipeline.hpp"
#include "VknFramebuffer.hpp"
#include "VknData.hpp"
#include "VknSwapchain.hpp"
#include "VknInfos.hpp"

namespace vkn
{
    class VknRenderpass : public VknObject
    {
    public:
        // Overloads
        VknRenderpass() = default;
        VknRenderpass(VknIdxs relIdxs, VknIdxs absIdxs);

        // Members
        void addSubpass(
            uint32_t subpassIdx, bool isCompute = false,
            VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            VkSubpassDescriptionFlags flags = 0);
        VknFramebuffer *addFramebuffer(uint32_t framebufferIdx);
        std::list<VknFramebuffer> *addFramebuffers(VknSwapchain &swapchain);

        // Config
        void addAttachment(
            uint32_t attachIdx,
            VkFormat format = VK_FORMAT_B8G8R8A8_SRGB,
            VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
            VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VkAttachmentDescriptionFlags flags = 0);
        void addAttachmentRef(
            uint32_t subpassIdx, uint32_t attachIdx,
            VknAttachmentType attachmentType = COLOR_ATTACHMENT,
            VkImageLayout attachmentRefLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        void addSubpassDependency(uint32_t dependencyIdx,
                                  uint32_t srcSubpass = VK_SUBPASS_EXTERNAL, uint32_t dstSubpass = 0,
                                  VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                  VkAccessFlags srcAccessMask = 0,
                                  VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                  VkAccessFlags dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

        // Create
        void createRenderpass();
        void createPipelines();
        void createFramebuffers(VknSwapchain &swapchain);
        void demolishFramebuffers();
        void recreatePipelines(VknSwapchain &swapchain, uint32_t viewportIdx, uint32_t scissorIdx);
        void recreateFramebuffers(VknSwapchain &swapchain);

        // Getters
        VkRenderPass *getVkRenderPass() { return &s_engine->getObject<VkRenderPass>(m_absIdxs); }
        VknPipeline *getPipeline(uint32_t idx);
        std::list<VknPipeline> *getPipelines() { return &m_pipelines; }
        VknFramebuffer *getFramebuffer(uint32_t idx);
        std::list<VknFramebuffer> *getFramebuffers() { return &m_framebuffers; }
        VknIdxs &getRelIdxs() { return m_relIdxs; }

    private:
        // Members
        std::list<VknPipeline> m_pipelines{};
        std::list<VknFramebuffer> m_framebuffers{};
        VknSpace<uint32_t> m_numAttachRefs{1u};  // Subpass > AttachmentType#AttachCount
        VknVector<uint32_t> m_numPreserveRefs{}; // Subpass#PreserveRefCount

        // State
        bool m_addedDevices{false};
        bool m_createdRenderpass{false};
        bool m_createdPipelines{false};
        bool m_filedColorAttachment{false};
        uint32_t m_numSubpassDeps{0};
        uint32_t m_numAttachments{0};
        uint32_t m_numSubpasses{0};
        size_t m_pipelineStartAbsIdx{0};
        bool m_recreatingPipelines{false};
        uint32_t m_framebufferStartPos{0};
        bool m_recreatingFramebuffers{false};
        bool m_addedFramebuffers{false};
        bool m_demolishedVknFramebuffers{true};
        VknInstanceLock<VknRenderpass> m_instanceLock;

        VknPipeline *addPipeline(uint32_t subpassIdx);
    };
}