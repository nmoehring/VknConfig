#include "VknInputAssemblyState.hpp"

namespace vkn
{
    VknInputAssemblyState::VknInputAssemblyState(uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
                                                 VknInfos *infos)
        : m_deviceIdx{deviceIdx}, m_renderPassIdx{renderPassIdx}, m_subpassIdx{subpassIdx},
          m_infos{infos}
    {
    }

    void VknInputAssemblyState::setDetails(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable)
    {
        m_topology = topology;
        m_primitiveRestartEnable = primitiveRestartEnable;
    }

    void VknInputAssemblyState::fillInputAssemblyStateCreateInfo()
    {
        if (m_filled)
            throw std::runtime_error("Input assembly state create info already filled.");
        m_createInfo = m_infos->fillInputAssemblyStateCreateInfo(m_deviceIdx, m_renderPassIdx, m_subpassIdx,
                                                                 m_topology, m_primitiveRestartEnable);
        m_filled = true;
    }

    VkPipelineInputAssemblyStateCreateInfo *VknInputAssemblyState::operator()()
    {
        if (!m_filled)
            throw std::runtime_error("Input assembly state create info not filled.");
        return m_createInfo;
    }
}