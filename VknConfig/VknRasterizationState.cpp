#include "include/VknRasterizationState.hpp"

namespace vkn
{
    VknRasterizationState::VknRasterizationState(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
    }

    void VknRasterizationState::_fileRasterizationStateCreateInfo()
    {
        if (m_filed)
            throw std::runtime_error("RasterizationStateCreateInfo already filed.");
        m_infos->fileRasterizationStateCreateInfo(
            m_relIdxs, m_polygonMode, m_cullMode, m_frontFace, m_depthBiasConstantFactor,
            m_depthBiasClamp, m_depthBiasSlopeFactor, m_lineWidth,
            m_depthClampEnable, m_rasterizerDiscardEnable, m_depthBiasEnable);

        m_filed = true;
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