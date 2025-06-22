#include "include/VknViewportState.hpp"

namespace vkn
{
    VknViewportState::VknViewportState(VknIdxs relIdxs, VknIdxs absIdxs)
        : VknObject(relIdxs, absIdxs)
    {
    }

    void VknViewportState::_fileViewportStateCreateInfo()
    {
        if (m_filedCreateInfo)
            throw std::runtime_error("ViewportStateCreateInfo already filed.");
        s_infos->fileViewportStateCreateInfo(
            m_relIdxs, &m_viewports, &m_scissors);
        m_filedCreateInfo = true;
    }

    void VknViewportState::addViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
    {
        VkViewport &viewport = m_viewports.appendOne(VkViewport{});
        viewport.x = x;
        viewport.y = y;
        viewport.width = static_cast<float>(width);
        viewport.height = static_cast<float>(height);
        viewport.minDepth = minDepth;
        viewport.maxDepth = maxDepth;
    }

    void VknViewportState::addScissor(VkOffset2D offset, VkExtent2D extent)
    {
        VkRect2D &scissor = m_scissors.appendOne(VkRect2D{});
        scissor.offset = offset;
        scissor.extent = extent;
    }

    void VknViewportState::syncWithSwapchain(VknSwapchain &swapchain, uint32_t viewportIdx, uint32_t scissorIdx)
    {
        if (viewportIdx > m_viewports.getSize())
            throw std::runtime_error("Viewport index out of range.");
        if (scissorIdx > m_scissors.getSize())
            throw std::runtime_error("Scissor index out of range.");
        if (viewportIdx == m_viewports.getSize())
            this->addViewport();
        if (scissorIdx == m_scissors.getSize())
            this->addScissor();
        VkViewport &viewport = m_viewports(viewportIdx);
        VkRect2D &scissor = m_scissors(scissorIdx);
        VkExtent2D extent = swapchain.getActualExtent();
        scissor.extent = extent;
        viewport.width = static_cast<float>(extent.width);
        viewport.height = static_cast<float>(extent.height);
    }

    void VknViewportState::removeCreateInfo()
    {
        s_infos->removeViewportStateCreateInfo(m_relIdxs);
        m_filedCreateInfo = false;
    }

    void VknViewportState::removeScissors()
    {
        m_scissors.clear();
    }
    void VknViewportState::removeViewports()
    {
        m_viewports.clear();
    }
}