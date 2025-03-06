#include "VknRasterizationState.hpp"

namespace vkn
{
    VknRasterizationState::VknRasterizationState()
        : m_deviceIdx{0}, m_renderpassIdx{0}, m_subpassIdx{0}, m_infos{nullptr}, m_placeholder{true}
    {
    }

    VknRasterizationState::VknRasterizationState(uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx,
                                                 VknInfos *infos)
        : m_deviceIdx{deviceIdx}, m_renderpassIdx{renderpassIdx}, m_subpassIdx{subpassIdx},
          m_infos{infos}, m_placeholder{false}
    {
    }

    void VknRasterizationState::fillRasterizationStateCreateInfo()
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder rasterization state.");
        if (m_filled)
            throw std::runtime_error("RasterizationStateCreateInfo already filled.");
        m_infos->fillRasterizationStateCreateInfo(
            m_deviceIdx, m_renderpassIdx, m_subpassIdx, m_polygonMode, m_cullMode, m_frontFace,
            m_depthClampEnable, m_rasterizerDiscardEnable, m_depthBiasEnable, m_lineWidth);

        m_filled = true;
    }

    void VknRasterizationState::setPolygonMode(VkPolygonMode mode)
    {

        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder rasterization state.");
        m_polygonMode = mode;
    }

    void VknRasterizationState::setCullMode(VkCullModeFlags mode)
    {

        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder rasterization state.");
        m_cullMode = mode;
    }

    void VknRasterizationState::setFrontFace(VkFrontFace face)
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder rasterization state.");
        m_frontFace = face;
    }
    void VknRasterizationState::setDepthClampEnable(bool enable)
    {

        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder rasterization state.");
        m_depthClampEnable = enable;
    }

    void VknRasterizationState::setRasterizerDiscardEnable(bool enable)
    {

        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder rasterization state.");
        m_rasterizerDiscardEnable = enable;
    }

    void VknRasterizationState::setDepthBiasEnable(bool enable)
    {

        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder rasterization state.");
        m_depthBiasEnable = enable;
    }

    void VknRasterizationState::setLineWidth(float width)
    {

        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder rasterization state.");
        m_lineWidth = width;
    }

    void VknRasterizationState::setDepthBiasConstantFactor(float factor)
    {

        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder rasterization state.");
        m_depthBiasConstantFactor = factor;
    }

    void VknRasterizationState::setDepthBiasClamp(float clamp)
    {

        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder rasterization state.");
        m_depthBiasClamp = clamp;
    }

    void VknRasterizationState::setDepthBiasSlopeFactor(float factor)
    {

        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder rasterization state.");
        m_depthBiasSlopeFactor = factor;
    }
}