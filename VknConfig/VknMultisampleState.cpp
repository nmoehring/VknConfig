#include "VknMultisampleState.hpp"
#include "VknInfos.hpp"

namespace vkn
{
    VknMultisampleState::VknMultisampleState(uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
                                             VknInfos *infos)
        : m_deviceIdx{deviceIdx}, m_renderPassIdx{renderPassIdx}, m_subpassIdx{subpassIdx},
          m_infos{infos}
    {
    }

    void VknMultisampleState::setDetails(VkSampleCountFlagBits rasterizationSamples, VkBool32 sampleShadingEnable,
                                         float minSampleShading, VkSampleMask sampleMask,
                                         VkBool32 alphaToCoverageEnable, VkBool32 alphaToOneEnable)
    {
        m_rasterizationSamples = rasterizationSamples;
        m_sampleShadingEnable = sampleShadingEnable;
        m_minSampleShading = minSampleShading;
        m_sampleMask = sampleMask;
        m_alphaToCoverageEnable = alphaToCoverageEnable;
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