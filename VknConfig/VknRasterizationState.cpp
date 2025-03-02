#include "VknRasterizationState.hpp"

namespace vkn
{
    VknRasterizationState::VknRasterizationState()
        : m_deviceIdx{0}, m_renderPassIdx{0}, m_subpassIdx{0}, m_infos{nullptr}
    {
    }

    VknRasterizationState::VknRasterizationState(uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
                                                 VknInfos *infos)
        : m_deviceIdx{deviceIdx}, m_renderPassIdx{renderPassIdx}, m_subpassIdx{subpassIdx},
          m_infos{infos}
    {
    }

    void VknRasterizationState::fillRasterizationStateCreateInfo()
    {
        if (m_filled)
            throw std::runtime_error("RasterizationStateCreateInfo already filled.");
        m_createInfo = m_infos->fillRasterizationStateCreateInfo(
            m_deviceIdx, m_renderPassIdx, m_subpassIdx, m_polygonMode, m_cullMode, m_frontFace,
            m_depthClampEnable, m_rasterizerDiscardEnable, m_depthBiasEnable, m_lineWidth);

        m_filled = true;
    }

    void VknRasterizationState::setPolygonMode(VkPolygonMode mode) { m_polygonMode = mode; }
    void VknRasterizationState::setCullMode(VkCullModeFlags mode) { m_cullMode = mode; }
    void VknRasterizationState::setFrontFace(VkFrontFace face) { m_frontFace = face; }
    void VknRasterizationState::setDepthClampEnable(bool enable) { m_depthClampEnable = enable; }
    void VknRasterizationState::setRasterizerDiscardEnable(bool enable) { m_rasterizerDiscardEnable = enable; }
    void VknRasterizationState::setDepthBiasEnable(bool enable) { m_depthBiasEnable = enable; }
    void VknRasterizationState::setLineWidth(float width) { m_lineWidth = width; }
    void VknRasterizationState::setDepthBiasConstantFactor(float factor) { m_depthBiasConstantFactor = factor; }
    void VknRasterizationState::setDepthBiasClamp(float clamp) { m_depthBiasClamp = clamp; }
    void VknRasterizationState::setDepthBiasSlopeFactor(float factor) { m_depthBiasSlopeFactor = factor; }
}