#include "include/VknMultisampleState.hpp"

namespace vkn
{
    VknMultisampleState::VknMultisampleState(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
    }

    void VknMultisampleState::setRasterizationSamples(VkSampleCountFlagBits rasterizationSamples)
    {
        m_rasterizationSamples = rasterizationSamples;
    }

    void VknMultisampleState::setSampleShadingEnable(VkBool32 sampleShadingEnable)
    {
        m_sampleShadingEnable = sampleShadingEnable;
    }

    void VknMultisampleState::setMinSampleShading(float minSampleShading)
    {
        m_minSampleShading = minSampleShading;
    }

    void VknMultisampleState::setSampleMask(VkSampleMask sampleMask)
    {
        m_sampleMask = sampleMask;
    }

    void VknMultisampleState::setAlphaToCoverageEnable(VkBool32 alphaToCoverageEnable)
    {
        m_alphaToCoverageEnable = alphaToCoverageEnable;
    }

    void VknMultisampleState::setAlphaToOneEnable(VkBool32 alphaToOneEnable)
    {
        m_alphaToOneEnable = alphaToOneEnable;
    }

    void VknMultisampleState::fillMultisampleStateCreateInfo()
    {
        if (m_filled)
            throw std::runtime_error("Multisample state create info already filled.");
        m_infos->fillMultisampleStateCreateInfo(m_relIdxs, m_minSampleShading, &m_sampleMask,
                                                m_rasterizationSamples, m_sampleShadingEnable,
                                                m_alphaToCoverageEnable, m_alphaToOneEnable);
        m_filled = true;
    }
}