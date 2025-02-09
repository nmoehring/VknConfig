#pragma once

#include <unordered_map>

#include "VknDevice.hpp"

namespace vkn
{
    enum ShaderStage
    {
        VKN_VERTEX_STAGE = VK_SHADER_STAGE_VERTEX_BIT,
        VKN_FRAGMENT_STAGE = VK_SHADER_STAGE_FRAGMENT_BIT
    };

    class VknPipeline
    {
    public:
        VknPipeline();
        VknPipeline(VknDevice *dev, VknInfos *infos, VknResultArchive *archive);
        ~VknPipeline();
        int createShaderStage(ShaderStage, std::string);
        void destroy();

        void createDescriptorSetLayoutBinding(
            uint32_t binding, VkDescriptorType descriptorType, uint32_t descriptorCount,
            VkShaderStageFlags stageFlags, const VkSampler *pImmutableSamplers);
        VkDescriptorSetLayoutCreateInfo &fillDescriptorSetLayoutCreateInfo(
            VkDescriptorSetLayoutCreateFlags flags = VkDescriptorSetLayoutCreateFlags{});
        void createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo &descriptorSetLayoutCreateInfo);
        void createPushConstantRange(VkShaderStageFlags stageFlags = VkShaderStageFlags{}, uint32_t offset = 0,
                                     uint32_t size = 0);
        VkPipelineLayoutCreateInfo fillPipelineLayoutCreateInfo(
            VkPipelineLayoutCreateFlags flags = VkPipelineLayoutCreateFlags{});
        void createLayout(VkPipelineLayoutCreateInfo &pCreateInfo);

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
        void createSubpass(
            VkSubpassDescriptionFlags flags = VkSubpassDescriptionFlags{},
            VkPipelineBindPoint pipelineBindPoint = VkPipelineBindPoint{},
            std::vector<VkAttachmentReference> colorAttachments = std::vector<VkAttachmentReference>{},
            VkAttachmentReference *depthStencilAttachment = nullptr,
            std::vector<VkAttachmentReference> inputAttachments = std::vector<VkAttachmentReference>{},
            std::vector<VkAttachmentReference> resolveAttachments = std::vector<VkAttachmentReference>{},
            std::vector<uint32_t> preserveAttachments = std::vector<uint32_t>{});
        void createSubpassDependency();
        VkRenderPassCreateInfo fillRenderPassCreateInfo(
            VkRenderPassCreateFlags flags = VkRenderPassCreateFlags{});
        VkRenderPass createRenderPass(VkRenderPassCreateInfo *renderPassInfo);

        VkGraphicsPipelineCreateInfo &fillPipelineCreateInfo();
        void createPipeline(VkGraphicsPipelineCreateInfo pipelineCreateInfo);

    private:
        VknDevice *m_device{nullptr};
        VknInfos *m_infos{nullptr};
        VknResultArchive *m_archive{nullptr};
        std::vector<VkGraphicsPipelineCreateInfo> m_pipelineCreateInfos{};
        VkPipeline m_pipeline{};
        std::vector<VkShaderModule> m_shaderModules{};
        std::vector<VkPipelineShaderStageCreateInfo> m_shaderStageInfos{};

        std::vector<VkDescriptorSetLayoutBinding> m_bindings{};
        std::vector<VkPushConstantRange> m_pushConstantRanges{};
        std::vector<VkDescriptorSetLayout> m_setLayouts{};
        VkDescriptorSetLayout m_descriptorSetLayout{};
        VkPipelineLayout m_layout{};

        std::vector<VkAttachmentDescription> m_attachments;
        std::vector<VkSubpassDescription> m_subpasses;
        std::vector<VkSubpassDependency> m_dependencies;
        std::vector<VkRenderPass> m_renderPasses{};

        bool m_destroyed = false;

        int createShaderModule(const std::string &filename);

        void setVertexInput();
        void setInputAssembly();
        void setTessellation();
        void setViewport();
        void setRasterization();
        void setMultisampling();
        void setDepthStencil();
        void setColorBlend();
    };
}