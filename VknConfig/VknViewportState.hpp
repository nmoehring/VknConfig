#pragma once

#include "VknInfos.hpp"

namespace vkn
{
    class VknViewportState
    {
    public:
        VknViewportState();
        VknViewportState(uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx,
                         VknInfos *infos);
        VknViewportState(uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx,
                         VknInfos *infos, VkExtent2D *swapchainExtent, const bool *swapchainCreated);

        void fillViewportStateCreateInfo();

        void addViewportWithSwapchain(float x = 0.0f, float y = 0.0f, float minDepth = 0.0f, float maxDepth = 1.0f);
        void addScissorWithSwapchain(VkOffset2D offset = {0, 0});

        void addViewport(float x = 0.0f, float y = 0.0f, float width = 800, float height = 600,
                         float minDepth = 0.0f, float maxDepth = 1.0f);
        void addScissor(VkOffset2D offset = {0, 0}, VkExtent2D extent = {800, 600});

    private:
        uint32_t m_deviceIdx;
        uint32_t m_renderpassIdx;
        uint32_t m_subpassIdx;
        bool m_placeholder;

        VknInfos *m_infos;
        VkExtent2D *m_swapchainExtent;
        const bool *m_swapchainCreated;
        bool m_filled{false};

        VkOffset2D m_defaultOffset{};
        VkExtent2D m_defaultExtent{};
        VkViewport m_defaultViewport{};
        VkRect2D m_defaultScissor{};

        std::vector<VkViewport> m_viewports;
        std::vector<VkRect2D> m_scissors;
    };
}