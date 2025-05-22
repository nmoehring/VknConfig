#include "include/VknViewportState.hpp"

namespace vkn
{
    VknViewportState::VknViewportState(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
    }

    VknViewportState::VknViewportState(VknEngine *engine, VknIdxs relIdxs,
                                       VknInfos *infos, VkExtent2D *swapchainExtent,
                                       const bool *swapchainCreated)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_infos{infos},
          m_swapchainExtent{swapchainExtent}
    {
    }

    void VknViewportState::_fillViewportStateCreateInfo()
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("ViewportStateCreateInfo already filled.");
        m_infos->fillViewportStateCreateInfo(
            m_relIdxs, &m_viewports, &m_scissors);
        m_filledCreateInfo = true;
    }

    void VknViewportState::addViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
    {
        VkViewport &viewport = m_viewports.append(VkViewport{});
        viewport.x = x;
        viewport.y = y;
        viewport.width = static_cast<float>(width);
        viewport.height = static_cast<float>(height);
        viewport.minDepth = minDepth;
        viewport.maxDepth = maxDepth;
    }

    void VknViewportState::addScissor(VkOffset2D offset, VkExtent2D extent)
    {
        VkRect2D &scissor = m_scissors.append(VkRect2D{});
        scissor.offset = offset;
        scissor.extent = extent;
    }

    void VknViewportState::syncWithSwapchain(VknSwapchain &swapchain)
    {
        VkExtent2D extent = swapchain.getActualExtent();
        this->addScissor(VkOffset2D{0, 0}, extent);
        this->addViewport(0.0f, 0.0f, extent.width, extent.height);
    }
}