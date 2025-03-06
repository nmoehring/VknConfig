#include "VknMultisampleState.hpp"
#include "VknInfos.hpp"

namespace vkn
{
    VknMultisampleState::VknMultisampleState()
        : m_deviceIdx{0}, m_renderpassIdx{0}, m_subpassIdx{0}, m_infos{nullptr}, m_placeholder{true}
    {
    }

    VknMultisampleState::VknMultisampleState(uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx,
                                             VknInfos *infos)
        : m_deviceIdx{deviceIdx}, m_renderpassIdx{renderpassIdx}, m_subpassIdx{subpassIdx},
          m_infos{infos}, m_placeholder{false}
    {
    }

    void VknMultisampleState::setRasterizationSamples(VkSampleCountFlagBits rasterizationSamples)
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder Multisample State.");
        m_rasterizationSamples = rasterizationSamples;
    }

    void VknMultisampleState::setSampleShadingEnable(VkBool32 sampleShadingEnable)
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder Multisample State.");
        m_sampleShadingEnable = sampleShadingEnable;
    }

    void VknMultisampleState::setMinSampleShading(float minSampleShading)
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder Multisample State.");
        m_minSampleShading = minSampleShading;
    }

    void VknMultisampleState::setSampleMask(VkSampleMask sampleMask)
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder Multisample State.");
        m_sampleMask = sampleMask;
    }

    void VknMultisampleState::setAlphaToCoverageEnable(VkBool32 alphaToCoverageEnable)
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder Multisample State.");
        m_alphaToCoverageEnable = alphaToCoverageEnable;
    }

    void VknMultisampleState::setAlphaToOneEnable(VkBool32 alphaToOneEnable)
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder Multisample State.");
        m_alphaToOneEnable = alphaToOneEnable;
    }

    void VknMultisampleState::fillMultisampleStateCreateInfo()
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder Multisample State.");
        if (m_filled)
            throw std::runtime_error("Multisample state create info already filled.");
        m_infos->fillMultisampleStateCreateInfo(m_deviceIdx, m_renderpassIdx, m_subpassIdx,
                                                m_minSampleShading, &m_sampleMask,
                                                m_rasterizationSamples, m_sampleShadingEnable,
                                                m_alphaToCoverageEnable, m_alphaToOneEnable);
        m_filled = true;
    }
}