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
 *     +-- [VknDevice] (Hierarchy-Bound)
 *         |
 *         +-- [VknPhysicalDevice] (Hierarchy-Bound)
 *         |   |
 *         |   +-- [VknQueueFamily] (Hierarchy-Bound Leaf)
 *         |
 *         +-- [VknSwapchain] (Hierarchy-Bound)
 *         |   |
 *         |   +-- [VknImageView] (Hierarchy-Bound Leaf)
 *         |
 *         +-- [VknRenderpass] (Hierarchy-Bound)
 *             |
 *             +-- [VknPipeline] (Hierarchy-Bound)
 *                 |
 *                 +-- [VknVertexInputState] (Hierarchy-Bound Leaf) <<=== YOU ARE HERE
 *                 +-- [VknInputAssemblyState] (Hierarchy-Bound Leaf)
 *                 +-- [VknMultisampleState] (Hierarchy-Bound Leaf)
 *                 +-- [VknRasterizationState] (Hierarchy-Bound Leaf)
 *                 +-- [VknShaderStage] (Hierarchy-Bound Leaf)
 *                 +-- [VknViewportState] (Hierarchy-Bound Leaf)
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
    class VknVertexInputState
    {
    public:
        VknVertexInputState() = default;
        VknVertexInputState(
            VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        void fillVertexBindingDescription(uint32_t binding = 0, uint32_t stride = 0,
                                          VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);
        void fillVertexAttributeDescription(uint32_t binding = 0, uint32_t location = 0,
                                            VkFormat format = VK_FORMAT_R32G32_SFLOAT, uint32_t offset = 0);
        void fillVertexInputStateCreateInfo();

    private:
        // Engine
        VknEngine *m_engine{nullptr};
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos{nullptr};

        // State
        uint32_t m_numBindings{0};
        uint32_t m_numAttributes{0};
        bool m_attributesFilled{false};
        bool m_bindingsFilled{false};
        bool m_filled{false};
    };
} // namespace vkn