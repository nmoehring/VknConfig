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
        VknPipeline() {}
        VknPipeline(VknDevice *dev, VknInfos *infos, VknResultArchive *archive, uint32_t index);
        ~VknPipeline();
        void destroy();

        int createShaderStage(ShaderStage, std::string);
        void createDescriptorSetLayoutBinding(
            uint32_t binding, VkDescriptorType descriptorType, uint32_t descriptorCount,
            VkShaderStageFlags stageFlags, const VkSampler *pImmutableSamplers);
        VkDescriptorSetLayoutCreateInfo &fillDescriptorSetLayoutCreateInfo(
            VkDescriptorSetLayoutCreateFlags flags = VkDescriptorSetLayoutCreateFlags{});
        void createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo &descriptorSetLayoutCreateInfo);
        void createPushConstantRange(VkShaderStageFlags stageFlags = VkShaderStageFlags{}, uint32_t offset = 0,
                                     uint32_t size = 0);
        void fillPipelineLayoutCreateInfo(
            VkPipelineLayoutCreateFlags flags = VkPipelineLayoutCreateFlags{});
        void createLayout();
        void createSubpass(
            VkSubpassDescriptionFlags flags = VkSubpassDescriptionFlags{},
            VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            std::vector<VkAttachmentReference> colorAttachments = std::vector<VkAttachmentReference>{},
            VkAttachmentReference *depthStencilAttachment = nullptr,
            std::vector<VkAttachmentReference> inputAttachments = std::vector<VkAttachmentReference>{},
            std::vector<VkAttachmentReference> resolveAttachments = std::vector<VkAttachmentReference>{},
            std::vector<uint32_t> preserveAttachments = std::vector<uint32_t>{});

        void fillPipelineCreateInfo(
            VkRenderPass renderPass,
            VkPipeline basePipelineHandle = VK_NULL_HANDLE, int32_t basePipelineIndex = -1,
            VkPipelineCreateFlags flags = VkPipelineCreateFlags{},
            VkPipelineVertexInputStateCreateInfo *pVertexInputState = nullptr,
            VkPipelineInputAssemblyStateCreateInfo *pInputAssemblyState = nullptr,
            VkPipelineTessellationStateCreateInfo *pTessellationState = nullptr,
            VkPipelineViewportStateCreateInfo *pViewportState = nullptr,
            VkPipelineRasterizationStateCreateInfo *pRasterizationState = nullptr,
            VkPipelineMultisampleStateCreateInfo *pMultisampleState = nullptr,
            VkPipelineDepthStencilStateCreateInfo *pDepthStencilState = nullptr,
            VkPipelineColorBlendStateCreateInfo *pColorBlendState = nullptr,
            VkPipelineDynamicStateCreateInfo *pDynamicState = nullptr);

        VkGraphicsPipelineCreateInfo &getCreateInfo() { return m_createInfo; }
        VkPipeline &getVkPipeline() { return m_pipeline; }
        VkSubpassDescription &getSubpassDescription() { return m_subpass; }
        void setPipelineCreated() { m_pipelineCreated = true; }

    private:
        VknDevice *m_device{nullptr};
        VknInfos *m_infos{nullptr};
        VknResultArchive *m_archive{nullptr};

        VkGraphicsPipelineCreateInfo m_createInfo{};
        VkPipeline m_pipeline{}; // 1 Subpass per pipeline
        VkSubpassDescription m_subpass;

        std::vector<VkShaderModule> m_shaderModules{};
        std::vector<VkPipelineShaderStageCreateInfo> m_shaderStageInfos{};

        std::vector<VkDescriptorSetLayoutBinding> m_bindings{};      //+
        std::vector<VkPushConstantRange> m_pushConstantRanges{};     //=
        std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts{}; // Takes bindings and push constant ranges ^
        VkPipelineLayoutCreateInfo m_layoutCreateInfo{};             // Takes an array of descriptor set layouts and an array of push constant ranges ^
        VkPipelineLayout m_layout{};                                 // Takes an array of descriptor set layouts ^

        bool m_destroyed{false};
        bool m_pipelineCreated{false};
        bool m_pipelineLayoutCreated{false};
        uint32_t m_index;

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