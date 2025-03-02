#include "VknViewportState.hpp"

namespace vkn
{
    VknViewportState::VknViewportState()
        : m_deviceIdx{0}, m_renderPassIdx{0}, m_subpassIdx{0}, m_infos{nullptr}, m_swapchainExtent(nullptr),
          m_swapchainCreated{nullptr}, m_placeholder{true}
    {
    }

    VknViewportState::VknViewportState(uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
                                       VknInfos *infos)
        : m_deviceIdx{deviceIdx}, m_renderPassIdx{renderPassIdx}, m_subpassIdx{subpassIdx},
          m_infos{infos}, m_placeholder{false}
    {
    }

    VknViewportState::VknViewportState(uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
                                       VknInfos *infos, VkExtent2D *swapchainExtent,
                                       const bool *swapchainCreated)
        : m_deviceIdx{deviceIdx}, m_renderPassIdx{renderPassIdx}, m_subpassIdx{subpassIdx},
          m_infos{infos}, m_swapchainExtent{swapchainExtent}, m_swapchainCreated{swapchainCreated},
          m_placeholder{false}
    {
    }

    void VknViewportState::fillViewportStateCreateInfo()
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder VknViewportState.");
        if (m_filled)
            throw std::runtime_error("ViewportStateCreateInfo already filled.");
        m_createInfo = m_infos->fillViewportStateCreateInfo(
            m_deviceIdx, m_renderPassIdx, m_subpassIdx, &m_viewports, &m_scissors);
        m_filled = true;
    }

    void VknViewportState::addViewportWithSwapchain(float x, float y, float minDepth, float maxDepth)
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder VknViewportState.");
        if (!*m_swapchainCreated)
            throw std::runtime_error("Attempting to add a viewport to a viewport state with swapchain values without a swapchain.");
        m_viewports.push_back(VkViewport{});
        VkViewport &viewport = m_viewports.back();
        viewport.x = x;
        viewport.y = y;
        viewport.width = static_cast<float>(m_swapchainExtent->width);
        viewport.height = static_cast<float>(m_swapchainExtent->height);
        viewport.minDepth = minDepth;
        viewport.maxDepth = maxDepth;
    }

    void VknViewportState::addViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder VknViewportState.");
        m_viewports.push_back(VkViewport{});
        VkViewport &viewport = m_viewports.back();
        viewport.x = x;
        viewport.y = y;
        viewport.width = static_cast<float>(width);
        viewport.height = static_cast<float>(height);
        viewport.minDepth = minDepth;
        viewport.maxDepth = maxDepth;
    }

    void VknViewportState::addScissorWithSwapchain(VkOffset2D offset)
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder VknViewportState.");
        if (!*m_swapchainCreated)
            throw std::runtime_error("Attempting to add a scissor to a viewport state with swapchain values without a swapchain.");
        m_scissors.push_back(VkRect2D{});
        VkRect2D &scissor = m_scissors.back();
        scissor.offset = offset;
        scissor.extent = *m_swapchainExtent;
    }

    void VknViewportState::addScissor(VkOffset2D offset, VkExtent2D extent)
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder VknViewportState.");
        m_scissors.push_back(VkRect2D{});
        VkRect2D &scissor = m_scissors.back();
        scissor.offset = offset;
        scissor.extent = extent;
    }
}