/**
 * @file VknViewportState.hpp
 * @brief Configures the viewport and scissor state for a graphics pipeline.
 *
 * VknViewportState is a hierarchy-bound leaf class within the VknConfig project.
 * It is used by VknPipeline to configure the viewport and scissor state.
 * VknViewportState depends on VknEngine, VknInfos, VknIdxs, and VkExtent2D.
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
 *                 +-- [VknVertexInputState] ^ / \
 *                 +-- [VknInputAssemblyState] ^ / \
 *                 +-- [VknMultisampleState] ^ / \
 *                 +-- [VknRasterizationState] ^ / \
 *                 +-- [VknShaderStage] ^ / \
 *                 +-- [VknViewportState] ^ / \ <<=== YOU ARE HERE
 *
 * [VknEngine] (Free/Top-Level)
 * [VknInfos] (Free/Top-Level)
 * [VknResult] (Free/Top-Level)
 */

#pragma once

#include "VknInfos.hpp"

namespace vkn
{
    class VknViewportState
    {
    public:
        VknViewportState() = default;
        VknViewportState(
            VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);
        VknViewportState(VknEngine *engine, VknIdxs relIdxs, VknInfos *infos,
                         VkExtent2D *swapchainExtent, const bool *swapchainCreated);

        void fillViewportStateCreateInfo();

        void addViewportWithSwapchain(float x = 0.0f, float y = 0.0f, float minDepth = 0.0f, float maxDepth = 1.0f);
        void addScissorWithSwapchain(VkOffset2D offset = {0, 0});

        void addViewport(float x = 0.0f, float y = 0.0f, float width = 800, float height = 600,
                         float minDepth = 0.0f, float maxDepth = 1.0f);
        void addScissor(VkOffset2D offset = {0, 0}, VkExtent2D extent = {800, 600});

    private:
        // Engine
        VknEngine *m_engine{nullptr};
        VknIdxs m_relIdxs{};
        VknIdxs m_absIdxs{};
        VknInfos *m_infos{nullptr};

        // Params
        VkExtent2D *m_swapchainExtent{nullptr};
        VkOffset2D m_defaultOffset{0, 0};
        VkExtent2D m_defaultExtent{800, 600};
        VkViewport m_defaultViewport{
            .x = 0.0f,
            .y = 0.0f,
            .width = 800.0f,
            .height = 600.0f,
            .minDepth = 0.0f,
            .maxDepth = 1.0f};
        VkRect2D m_defaultScissor{
            .offset = m_defaultOffset,
            .extent = m_defaultExtent};
        std::vector<VkViewport> m_viewports{};
        std::vector<VkRect2D> m_scissors{};

        // State
        bool m_filledCreateInfo{false};
    };
}