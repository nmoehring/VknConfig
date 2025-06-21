#include "include/VknMultisampleState.hpp"

namespace vkn
{
    VknMultisampleState::VknMultisampleState(VknIdxs relIdxs, VknIdxs absIdxs)
        : VknObject(relIdxs, absIdxs)
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

    void VknMultisampleState::_fileMultisampleStateCreateInfo()
    {
        if (m_filed)
            throw std::runtime_error("Multisample state create info already filed.");
        s_infos.fileMultisampleStateCreateInfo(m_relIdxs, m_minSampleShading, &m_sampleMask,
                                               m_rasterizationSamples, m_sampleShadingEnable,
                                               m_alphaToCoverageEnable, m_alphaToOneEnable);
        m_filed = true;
    }
}