#pragma once

#include <unordered_map>

#include <vulkan/vulkan.h>
#include <list>

#include "VknResult.hpp"
#include "VknInfos.hpp"
#include "VknVertexInputState.hpp"
#include "VknInputAssemblyState.hpp"
#include "VknMultisampleState.hpp"
#include "VknViewportState.hpp"
#include "VknRasterizationState.hpp"
#include "VknShaderStage.hpp"

namespace vkn
{
    class VknPipeline
    {
    public:
        VknPipeline() = delete;
        VknPipeline(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        void createDescriptorSetLayoutBinding(
            uint32_t binding, VkDescriptorType descriptorType, uint32_t descriptorCount,
            VkShaderStageFlags stageFlags, const VkSampler *pImmutableSamplers);
        void fillDescriptorSetLayoutCreateInfo(
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

        void addShaderStage(uint32_t newShaderStageIdx,
                            VknShaderStageType stageType, std::string filename,
                            VkPipelineShaderStageCreateFlags flags = 0);

        VknShaderStage *getShaderStage(uint32_t shaderIdx);
        VkPipeline *getVkPipeline() { return m_pipeline; }
        VknVertexInputState *getVertexInputState() { return &m_vertexInputState; }
        VknInputAssemblyState *getInputAssemblyState() { return &m_inputAssemblyState; }
        VknMultisampleState *getMultisampleState() { return &m_multisampleState; }
        VknRasterizationState *getRasterizationState() { return &m_rasterizationState; }
        VknViewportState *getViewportState() { return &m_viewportState; }

    private:
        // Engine
        VknEngine *m_engine{nullptr};
        VknIdxs m_relIdxs{};
        VknIdxs m_absIdxs{};
        VknInfos *m_infos{nullptr};

        // Wrapped object
        VkPipeline *m_pipeline{}; // 1 Subpass per pipeline

        // TODO: Implement with CC Valueable, if it doesn't initialize the data
        //  Members
        VknVertexInputState m_vertexInputState;
        VknInputAssemblyState m_inputAssemblyState;
        // VknTessellationState m_tessellationState;
        VknViewportState m_viewportState;
        VknRasterizationState m_rasterizationState;
        VknMultisampleState m_multisampleState;
        // VknDepthStencilState m_depthStencilState;
        // VknColorBlendState m_colorBlendState;
        // VknDynamicState m_dynamicState;
        std::vector<VknShaderStage> m_shaderStages{}; // List prevents dangling pointers to elements of changing structure

        // Params
        std::vector<VkDescriptorSetLayoutBinding> m_bindings{};      //+
        std::vector<VkPushConstantRange> m_pushConstantRanges{};     //=
        std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts{}; // Takes bindings and push constant ranges ^
        VkPipelineLayout m_layout{};                                 // Takes an array of descriptor set layouts ^

        // State
        bool m_filledCreateInfo{false};
        bool m_createdPipeline{false};
        bool m_createdPipelineLayout{false};
        uint32_t m_numShaderStages{0};
    };
}