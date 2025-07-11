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
#include "VknObject.hpp"

namespace vkn
{
    class VknInputAssemblyState : public VknObject
    {
    public:
        VknInputAssemblyState() = default;
        VknInputAssemblyState(VknIdxs relIdxs, VknIdxs absIdxs);

        void setDetails(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable);
        void _fileInputAssemblyStateCreateInfo();

    private:
        // Params
        VkPrimitiveTopology m_topology{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
        VkBool32 m_primitiveRestartEnable{VK_FALSE};

        // State
        bool m_filedCreateInfo{false};
    };
} // namespace vkn