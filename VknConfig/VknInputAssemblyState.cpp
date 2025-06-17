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

    void VknInputAssemblyState::_fileInputAssemblyStateCreateInfo()
    {
        if (m_filedCreateInfo)
            throw std::runtime_error("Input assembly state create info already filed.");
        m_infos->fileInputAssemblyStateCreateInfo(m_relIdxs, m_topology, m_primitiveRestartEnable);
        m_filedCreateInfo = true;
    }
}