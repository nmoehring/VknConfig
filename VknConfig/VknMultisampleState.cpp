#include "VknMultisampleState.hpp"
#include "VknInfos.hpp"

namespace vkn
{
    VknMultisampleState::VknMultisampleState()
        : m_deviceIdx{0}, m_renderPassIdx{0}, m_subpassIdx{0}, m_infos{nullptr}
    {
    }

    VknMultisampleState::VknMultisampleState(uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
                                             VknInfos *infos)
        : m_deviceIdx{deviceIdx}, m_renderPassIdx{renderPassIdx}, m_subpassIdx{subpassIdx},
          m_infos{infos}
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
        m_createInfo = m_infos->fillMultisampleStateCreateInfo(m_deviceIdx, m_renderPassIdx, m_subpassIdx,
                                                               m_minSampleShading, &m_sampleMask,
                                                               m_rasterizationSamples, m_sampleShadingEnable,
                                                               m_alphaToCoverageEnable, m_alphaToOneEnable);
        m_filled = true;
    }
}