#pragma once

#include <unordered_map>

#include <vulkan/vulkan.h>

#include "VknResult.hpp"
#include "VknInfos.hpp"
#include "VknVertexInputState.hpp"
#include "VknInputAssemblyState.hpp"
#include "VknMultisampleState.hpp"

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
        VknPipeline(uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx, VkRenderPass *renderPass,
                    VkSubpassDescription *subpass, VkPipeline *pipeline,
                    VkDevice *dev, VknInfos *infos, VknResultArchive *archive, const bool *deviceCreated);
        ~VknPipeline();
        void destroy();

        int createShaderStage(ShaderStage, std::string);
        void createDescriptorSetLayoutBinding(
            uint32_t binding, VkDescriptorType descriptorType, uint32_t descriptorCount,
            VkShaderStageFlags stageFlags, const VkSampler *pImmutableSamplers);
        VkDescriptorSetLayoutCreateInfo *fillDescriptorSetLayoutCreateInfo(
            VkDescriptorSetLayoutCreateFlags flags = 0);
        void createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo *descriptorSetLayoutCreateInfo);
        void createPushConstantRange(VkShaderStageFlags stageFlags = 0, uint32_t offset = 0,
                                     uint32_t size = 0);
        void fillPipelineLayoutCreateInfo(
            VkPipelineLayoutCreateFlags flags = 0);
        void createLayout();

        void fillPipelineCreateInfo(
            VkPipeline basePipelineHandle = VK_NULL_HANDLE, int32_t basePipelineIndex = -1,
            VkPipelineCreateFlags flags = 0);

        VkGraphicsPipelineCreateInfo *getCreateInfo() { return m_createInfo; }
        VkPipeline *getVkPipeline() { return m_pipeline; }
        VkSubpassDescription *getSubpassDescription() { return m_subpass; }
        void setPipelineCreated() { m_pipelineCreated = true; }
        VknVertexInputState *getVertexInputState() { return &m_vertexInputState; }
        VknInputAssemblyState *getInputAssemblyState() { return &m_inputAssemblyState; }
        VknMultisampleState *getMultisampleState() { return &m_multisampleState; }

    private:
        VkDevice *m_device{nullptr};
        VknInfos *m_infos{nullptr};
        VknResultArchive *m_archive{nullptr};
        VkPipeline *m_pipeline{nullptr}; // 1 Subpass per pipeline
        VkRenderPass *m_renderPass{nullptr};
        VkSubpassDescription *m_subpass{nullptr};
        VkGraphicsPipelineCreateInfo *m_createInfo{nullptr};
        std::vector<std::vector<VkAttachmentReference>> *m_attachmentReferences{nullptr};
        std::vector<uint32_t> *m_preserveAttachments{nullptr};
        uint32_t m_deviceIdx;
        uint32_t m_renderPassIdx;
        uint32_t m_subpassIdx;
        const bool *m_deviceCreated{nullptr};

        VknVertexInputState m_vertexInputState{};
        VknInputAssemblyState m_inputAssemblyState{};
        // VknTessellationState m_tessellationState{};
        // VknViewportState m_viewportState{};
        // VknRasterizationState m_rasterizationState{};
        VknMultisampleState m_multisampleState{};
        // VknDepthStencilState m_depthStencilState{};
        // VknColorBlendState m_colorBlendState{};
        // VknDynamicState m_dynamicState{};

        std::vector<VkShaderModule> m_shaderModules{};
        std::vector<VkPipelineShaderStageCreateInfo *> m_shaderStageInfos{};

        std::vector<VkDescriptorSetLayoutBinding> m_bindings{};      //+
        std::vector<VkPushConstantRange> m_pushConstantRanges{};     //=
        std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts{}; // Takes bindings and push constant ranges ^
        VkPipelineLayoutCreateInfo *m_layoutCreateInfo{nullptr};     // Takes an array of descriptor set layouts and an array of push constant ranges ^
        VkPipelineLayout m_layout{};                                 // Takes an array of descriptor set layouts ^

        bool m_destroyed{false};
        bool m_pipelineCreated{false};
        bool m_pipelineLayoutCreated{false};

        int createShaderModule(const std::string filename);
    };
}