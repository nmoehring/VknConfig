#include "include/VknInputAssemblyState.hpp"

namespace vkn
{
    VknInputAssemblyState::VknInputAssemblyState(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
    }

    void VknInputAssemblyState::setDetails(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable)
    {
        m_topology = topology;
        m_primitiveRestartEnable = primitiveRestartEnable;
    }

    void VknInputAssemblyState::_fillInputAssemblyStateCreateInfo()
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Input assembly state create info already filled.");
        m_infos->fillInputAssemblyStateCreateInfo(m_relIdxs, m_topology, m_primitiveRestartEnable);
        m_filledCreateInfo = true;
    }
}