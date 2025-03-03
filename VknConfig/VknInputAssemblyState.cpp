#include "VknInputAssemblyState.hpp"

namespace vkn
{
    VknInputAssemblyState::VknInputAssemblyState()
        : m_deviceIdx{0}, m_renderPassIdx{0}, m_subpassIdx{0}, m_placeholder{true}, m_infos{nullptr}
    {
    }

    VknInputAssemblyState::VknInputAssemblyState(uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
                                                 VknInfos *infos)
        : m_deviceIdx{deviceIdx}, m_renderPassIdx{renderPassIdx}, m_subpassIdx{subpassIdx},
          m_infos{infos}, m_placeholder{false}
    {
    }

    void VknInputAssemblyState::setDetails(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable)
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder InputAssemblyState.");
        m_topology = topology;
        m_primitiveRestartEnable = primitiveRestartEnable;
    }

    void VknInputAssemblyState::fillInputAssemblyStateCreateInfo()
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder InputAssemblyState.");
        if (m_filled)
            throw std::runtime_error("Input assembly state create info already filled.");
        m_infos->fillInputAssemblyStateCreateInfo(m_deviceIdx, m_renderPassIdx, m_subpassIdx,
                                                  m_topology, m_primitiveRestartEnable);
        m_filled = true;
    }
}