/**
 * @file VknPipeline.hpp
 * @brief Manages a Vulkan graphics pipeline.
 *
 * VknPipeline is a hierarchy-bound class within the VknConfig project.
 * It is used by VknRenderpass to manage a Vulkan graphics pipeline for a specific subpass.
 * It aggregates various pipeline state configuration objects (like VknVertexInputState,
 * VknRasterizationState, etc.), shader stages (VknShaderStage), and the pipeline layout (VknPipelineLayout).
 * VknPipeline depends on VknEngine, VknInfos, VknIdxs, and its constituent state/stage/layout classes.
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
 *                 +-- [VknPipelineLayout]
 *                 |   |
 *                 |   +-- [VknDescriptorSetLayout] ^ / \
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
 * [VknImage] (Free/Top-Level)
 */

#pragma once

#include <unordered_map>

#include <vulkan/vulkan.h>
#include <list>
#include <filesystem>
#include <iterator>

#include "VknObject.hpp"
#include "VknData.hpp"
#include "VknVertexInputState.hpp"
#include "VknInputAssemblyState.hpp"
#include "VknMultisampleState.hpp"
#include "VknViewportState.hpp"
#include "VknRasterizationState.hpp"
#include "VknShaderStage.hpp"
#include "VknColorBlendState.hpp"
#include "VknDynamicState.hpp"
#include "VknPipelineLayout.hpp"

namespace vkn
{
    class VknPipeline : public VknObject
    {
    public:
        // Overloads
        VknPipeline() = default;
        VknPipeline(VknIdxs relIdxs, VknIdxs absIdxs);

        // Vkn Members
        VknShaderStage *addShaderStage(uint32_t newShaderStageIdx,
                                       VknShaderStageType stageType, std::string filename,
                                       VkPipelineShaderStageCreateFlags flags = 0);
        VknPipelineLayout *getLayout();

        // Config
        void setBasePipelineHandle(VkPipeline basePipelineHandle) { m_basePipelineHandle = basePipelineHandle; }
        void setBasePipelineIndex(int32_t basePipelineIndex) { m_basePipelineIndex = basePipelineIndex; }
        void setCreateFlags(VkPipelineCreateFlags createFlags) { m_createFlags = createFlags; }

        // Create
        VkGraphicsPipelineCreateInfo *_filePipelineCreateInfo();

        // Get
        VknShaderStage *getShaderStage(uint32_t shaderIdx);
        std::list<VknShaderStage> *getShaderStages() { return &m_shaderStages; }
        VkPipeline *getVkPipeline() { return &s_engine.getObject<VkPipeline>(m_absIdxs); }
        VknVertexInputState *getVertexInputState() { return &m_vertexInputState.value(); }
        VknInputAssemblyState *getInputAssemblyState() { return &m_inputAssemblyState.value(); }
        VknMultisampleState *getMultisampleState() { return &m_multisampleState.value(); }
        VknRasterizationState *getRasterizationState() { return &m_rasterizationState.value(); }
        VknViewportState *getViewportState() { return &m_viewportState.value(); }
        VknColorBlendState *getColorBlendState() { return &m_colorBlendState.value(); }
        VknDynamicState *getDynamicState() { return &m_dynamicState.value(); }
        VknPipelineLayout *getPipelineLayout() { return &m_layouts.front(); }
        VknIdxs &getRelIdxs() { return m_relIdxs; }
        VknIdxs &getAbsIdxs() { return m_absIdxs; }

    private:
        //  Members
        std::list<VknPipelineLayout> m_layouts{};
        std::optional<VknVertexInputState> m_vertexInputState = std::nullopt;
        std::optional<VknInputAssemblyState> m_inputAssemblyState = std::nullopt;
        // std::optional<VknTessellationState> m_tessellationState = std::nullopt;
        std::optional<VknViewportState> m_viewportState = std::nullopt;
        std::optional<VknRasterizationState> m_rasterizationState = std::nullopt;
        std::optional<VknMultisampleState> m_multisampleState = std::nullopt;
        std::optional<VknColorBlendState> m_colorBlendState = std::nullopt;
        std::optional<VknDynamicState> m_dynamicState = std::nullopt;
        std::list<VknShaderStage> m_shaderStages{}; // List prevents dangling pointers to elements of changing structure

        // Params
        VkPipeline m_basePipelineHandle{VK_NULL_HANDLE};
        int32_t m_basePipelineIndex{-1};
        VkPipelineCreateFlags m_createFlags{0};

        // State
        bool m_createdPipeline{false};
        VknInstanceLock<VknPipeline>
            m_instanceLock;
    };
}