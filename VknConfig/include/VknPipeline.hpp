/**
 * @file VknPipeline.hpp
 * @brief Manages a Vulkan graphics pipeline.
 *
 * VknPipeline is a hierarchy-bound class within the VknConfig project.
 * It is used by VknRenderpass to manage a Vulkan graphics pipeline.
 * VknPipeline depends on VknEngine, VknInfos, VknVertexInputState, VknInputAssemblyState,
 * VknMultisampleState, VknRasterizationState, VknShaderStage, and VknIdxs.
 * VknPipeline is a child of VknRenderpass.
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
 *         +-- [VknRenderpass]
 *             |
 *             +-- [VknFramebuffer] ^ / \
 *             |
 *             +-- [VknPipeline]  <<=== YOU ARE HERE
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

#include <unordered_map>

#include <vulkan/vulkan.h>
#include <list>

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
        // Overloads
        VknPipeline() = default;
        VknPipeline(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        // Add Vkn Member
        VknShaderStage *addShaderStage(uint32_t newShaderStageIdx,
                                       VknShaderStageType stageType, std::string filename,
                                       VkPipelineShaderStageCreateFlags flags = 0);

        // Config
        void addDescriptorSetLayoutBinding(
            uint32_t binding, VkDescriptorType descriptorType, uint32_t descriptorCount,
            VkShaderStageFlags stageFlags, const VkSampler *pImmutableSamplers);
        void addPushConstantRange(VkShaderStageFlags stageFlags = 0, uint32_t offset = 0,
                                  uint32_t size = 0);
        void setBasePipelineHandle(VkPipeline basePipelineHandle) { m_basePipelineHandle = basePipelineHandle; }
        void setBasePipelineIndex(int32_t basePipelineIndex) { m_basePipelineIndex = basePipelineIndex; }
        void setCreateFlags(VkPipelineCreateFlags createFlags) { m_createFlags = createFlags; }
        void setLayoutCreateFlags(VkPipelineLayoutCreateFlags layoutCreateFlags) { m_layoutCreateFlags = layoutCreateFlags; }

        // Create

        void _fillPipelineCreateInfo();
        void createLayout();
        void createDescriptorSetLayout();

        // Get
        VknShaderStage *getShaderStage(uint32_t shaderIdx);
        VkPipeline *getVkPipeline() { return &m_engine->getObject<VkPipeline>(m_absIdxs.get<VkPipeline>()); }
        VknVertexInputState *getVertexInputState() { return &m_vertexInputState.value(); }
        VknInputAssemblyState *getInputAssemblyState() { return &m_inputAssemblyState.value(); }
        VknMultisampleState *getMultisampleState() { return &m_multisampleState.value(); }
        VknRasterizationState *getRasterizationState() { return &m_rasterizationState.value(); }
        VknViewportState *getViewportState() { return &m_viewportState.value(); }

    private:
        // Engine
        VknEngine *m_engine{nullptr};
        VknIdxs m_relIdxs{};
        VknIdxs m_absIdxs{};
        VknInfos *m_infos{nullptr};

        //  Members
        std::optional<VknVertexInputState> m_vertexInputState = std::nullopt;
        std::optional<VknInputAssemblyState> m_inputAssemblyState = std::nullopt;
        // std::optional<VknTessellationState> m_tessellationState = std::nullopt;
        std::optional<VknViewportState> m_viewportState = std::nullopt;
        std::optional<VknRasterizationState> m_rasterizationState = std::nullopt;
        std::optional<VknMultisampleState> m_multisampleState = std::nullopt;
        // std::optional<VknDepthStencilState> m_depthStencilState = std::nullopt;
        // std::optional<VknColorBlendState> m_colorBlendState = std::nullopt;
        // std::optional<VknDynamicState> m_dynamicState = std::nullopt;
        std::list<VknShaderStage> m_shaderStages{}; // List prevents dangling pointers to elements of changing structure

        // Params
        std::vector<VkDescriptorSetLayoutBinding> m_bindings{};      //+
        std::vector<VkPushConstantRange> m_pushConstantRanges{};     //=
        std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts{}; // Takes bindings and push constant ranges ^
        VkPipelineLayout m_layout{};                                 // Takes an array of descriptor set layouts ^
        VkPipeline m_basePipelineHandle{VK_NULL_HANDLE};
        int32_t m_basePipelineIndex{-1};
        VkPipelineCreateFlags m_createFlags{0};
        VkPipelineLayoutCreateFlags m_layoutCreateFlags{0};
        VkDescriptorSetLayoutCreateFlags m_descriptorSetLayoutCreateFlags{0};

        // State
        bool m_createdPipeline{false};
        bool m_createdPipelineLayout{false};
        uint32_t m_numShaderStages{0};
        std::vector<uint32_t> m_descriptorSetLayoutIdxs{};
        std::vector<uint32_t> m_pipelineLayoutIdxs{};
    };
}