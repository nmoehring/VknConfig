/**
 * @file VknInputAssemblyState.hpp
 * @brief Configures the input assembly state for a graphics pipeline.
 *
 * VknInputAssemblyState is a hierarchy-bound leaf class within the VknConfig project.
 * It is used by VknPipeline to configure how vertex data is assembled into primitives.
 * VknInputAssemblyState depends on VknInfos and VknIdxs.
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
 *                 +-- [VknInputAssemblyState] ^ / \ <<=== YOU ARE HERE
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

#include <vulkan/vulkan.h>
#include <vector>

#include "VknInfos.hpp"

namespace vkn
{
    class VknInputAssemblyState
    {
    public:
        VknInputAssemblyState() = default;
        VknInputAssemblyState(
            VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        void setDetails(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable);
        void fillInputAssemblyStateCreateInfo();

    private:
        // Engine
        VknEngine *m_engine;

        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Params
        VkPrimitiveTopology m_topology{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
        VkBool32 m_primitiveRestartEnable{VK_FALSE};

        // State
        bool m_filledCreateInfo{false};
    };
} // namespace vkn