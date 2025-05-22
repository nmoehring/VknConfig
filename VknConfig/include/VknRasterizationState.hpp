/**
 * @file VknRasterizationState.hpp
 * @brief Configures the rasterization state for a graphics pipeline.
 *
 * VknRasterizationState is a hierarchy-bound leaf class within the VknConfig project.
 * It is used by VknPipeline to configure rasterization.
 * VknRasterizationState depends on VknInfos and VknIdxs.
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
 *                 +-- [VknMultisampleState] ^ / \
 *                 +-- [VknRasterizationState] ^ / \ <<=== YOU ARE HERE
 *                 +-- [VknShaderStage] ^ / \
 *                 +-- [VknViewportState] ^ / \
 *
 * [VknEngine] (Free/Top-Level)
 * [VknInfos] (Free/Top-Level)
 * [VknResult] (Free/Top-Level)
 */

#include <vulkan/vulkan.h>

#include "VknData.hpp"
#include "VknInfos.hpp"

namespace vkn
{
    class VknRasterizationState
    {
    public:
        VknRasterizationState() = default;
        VknRasterizationState(
            VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        void setPolygonMode(VkPolygonMode mode);
        void setCullMode(VkCullModeFlags mode);
        void setFrontFace(VkFrontFace face);
        void setDepthClampEnable(bool enable);
        void setRasterizerDiscardEnable(bool enable);
        void setDepthBiasEnable(bool enable);
        void setLineWidth(float width);
        void setDepthBiasConstantFactor(float factor);
        void setDepthBiasClamp(float clamp);
        void setDepthBiasSlopeFactor(float factor);

        void _fillRasterizationStateCreateInfo();

    private:
        // Engine
        VknEngine *m_engine;

        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Members
        VkPolygonMode m_polygonMode{VK_POLYGON_MODE_FILL};
        VkCullModeFlags m_cullMode{VK_CULL_MODE_BACK_BIT};
        VkFrontFace m_frontFace{VK_FRONT_FACE_CLOCKWISE};
        float m_lineWidth{1.0};
        VkBool32 m_depthClampEnable{VK_FALSE};
        VkBool32 m_rasterizerDiscardEnable{VK_FALSE};
        VkBool32 m_depthBiasEnable{VK_FALSE};
        float m_depthBiasConstantFactor{0};
        float m_depthBiasClamp{0};
        float m_depthBiasSlopeFactor{0};

        // State
        bool m_filled{false};
    };
}