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
        VkExtent2D *m_swapchainExtent{};
        VkOffset2D m_defaultOffset{};
        VkExtent2D m_defaultExtent{};
        VkViewport m_defaultViewport{};
        VkRect2D m_defaultScissor{};
        std::vector<VkViewport> m_viewports{};
        std::vector<VkRect2D> m_scissors{};

        // State
        bool m_filled{false};
    };
}