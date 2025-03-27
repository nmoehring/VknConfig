#include "../include/VknRasterizationState.hpp"
#include "../include/VknEngine.hpp"

namespace vkn
{
    VknRasterizationState::VknRasterizationState(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
    }

    void VknRasterizationState::fillRasterizationStateCreateInfo()
    {
        if (m_filled)
            throw std::runtime_error("RasterizationStateCreateInfo already filled.");
        m_infos->fillRasterizationStateCreateInfo(
            m_relIdxs, m_polygonMode, m_cullMode, m_frontFace,
            m_depthClampEnable, m_rasterizerDiscardEnable, m_depthBiasEnable, m_lineWidth);

        m_filled = true;
    }

    void VknRasterizationState::setPolygonMode(VkPolygonMode mode)
    {

        m_polygonMode = mode;
    }

    void VknRasterizationState::setCullMode(VkCullModeFlags mode)
    {

        m_cullMode = mode;
    }

    void VknRasterizationState::setFrontFace(VkFrontFace face)
    {
        m_frontFace = face;
    }
    void VknRasterizationState::setDepthClampEnable(bool enable)
    {

        m_depthClampEnable = enable;
    }

    void VknRasterizationState::setRasterizerDiscardEnable(bool enable)
    {

        m_rasterizerDiscardEnable = enable;
    }

    void VknRasterizationState::setDepthBiasEnable(bool enable)
    {

        m_depthBiasEnable = enable;
    }

    void VknRasterizationState::setLineWidth(float width)
    {

        m_lineWidth = width;
    }

    void VknRasterizationState::setDepthBiasConstantFactor(float factor)
    {

        m_depthBiasConstantFactor = factor;
    }

    void VknRasterizationState::setDepthBiasClamp(float clamp)
    {

        m_depthBiasClamp = clamp;
    }

    void VknRasterizationState::setDepthBiasSlopeFactor(float factor)
    {

        m_depthBiasSlopeFactor = factor;
    }
}