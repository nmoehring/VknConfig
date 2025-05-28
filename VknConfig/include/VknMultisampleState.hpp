/**
 * @file VknMultisampleState.hpp
 * @brief Configures the multisample state for a graphics pipeline.
 *
 * VknMultisampleState is a hierarchy-bound leaf class within the VknConfig project.
 * It is used by VknPipeline to configure multisampling.
 * VknMultisampleState depends on VknInfos and VknIdxs.
 * It does not have any classes that depend on it.
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
 *             +-- [VknPipeline]
 *                 |
 *                 +-- [VknPipelineLayout]
 *                 |   |
 *                 |   +-- [VknDescriptorSetLayout]
 *                 |
 *                 +-- [VknVertexInputState] ^ / \
 *                 +-- [VknInputAssemblyState] ^ / \
 *                 +-- [VknMultisampleState] ^ / \ <<=== YOU ARE HERE
 *                 +-- [VknRasterizationState] ^ / \
 *                 +-- [VknShaderStage] ^ / \
 *                 +-- [VknViewportState] ^ / \
 *
 * [VknEngine] (Free/Top-Level)
 * [VknInfos] (Free/Top-Level)
 * [VknResult] (Free/Top-Level)
 */

#pragma once

#include <vulkan/vulkan.h>
#include "VknInfos.hpp"

namespace vkn
{
    class VknMultisampleState
    {
    public:
        VknMultisampleState() = default;
        VknMultisampleState(
            VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        void setRasterizationSamples(VkSampleCountFlagBits rasterizationSamples);
        void setSampleShadingEnable(VkBool32 sampleShadingEnable);
        void setMinSampleShading(float minSampleShading);
        void setSampleMask(VkSampleMask sampleMask);
        void setAlphaToCoverageEnable(VkBool32 alphaToCoverageEnable);
        void setAlphaToOneEnable(VkBool32 alphaToOneEnable);

        void _fillMultisampleStateCreateInfo();

    private:
        // Engine
        VknEngine *m_engine;

        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Params
        VkSampleCountFlagBits m_rasterizationSamples{VK_SAMPLE_COUNT_1_BIT};
        VkBool32 m_sampleShadingEnable{VK_FALSE};
        float m_minSampleShading{0.0f};
        VkSampleMask m_sampleMask{0xFFFFFFFFu};
        VkBool32 m_alphaToCoverageEnable{VK_FALSE};
        VkBool32 m_alphaToOneEnable{VK_FALSE};

        // State
        bool m_filled{false};
    };
}