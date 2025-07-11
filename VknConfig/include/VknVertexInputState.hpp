/**
 * @file VknVertexInputState.hpp
 * @brief Configures the vertex input state for a graphics pipeline.
 *
 * VknVertexInputState is a hierarchy-bound leaf class within the VknConfig project.
 * It is used by VknPipeline to configure how vertex data is provided to the pipeline.
 * VknVertexInputState depends on VknInfos and VknIdxs.
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
 *                 +-- [VknVertexInputState] ^ / \ <<=== YOU ARE HERE
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

#include <vulkan/vulkan.h>

#include "VknObject.hpp"

namespace vkn
{
    class VknVertexInputState : public VknObject
    {
    public:
        VknVertexInputState() = default;
        VknVertexInputState(VknIdxs relIdxs, VknIdxs absIdxs);

        void fileVertexBindingDescription(uint32_t binding = 0, uint32_t stride = 0,
                                          VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);
        void fileVertexAttributeDescription(uint32_t binding = 0, uint32_t location = 0,
                                            VkFormat format = VK_FORMAT_R32G32_SFLOAT, uint32_t offset = 0);

        void _fileVertexInputStateCreateInfo();

        // Getters
        uint32_t getNumBindings() const { return m_numBindings; }
        uint32_t getNumAttributes() const { return m_numAttributes; }

    private:
        // State
        uint32_t m_numBindings{0};
        uint32_t m_numAttributes{0};
        bool m_attributesfiled{false};
        bool m_bindingsfiled{false};
        bool m_filed{false};
    };
} // namespace vkn