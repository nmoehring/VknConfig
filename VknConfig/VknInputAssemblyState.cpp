#include "include/VknInputAssemblyState.hpp"

namespace vkn
{
    VknInputAssemblyState::VknInputAssemblyState(VknIdxs relIdxs, VknIdxs absIdxs)
        : VknObject(relIdxs, absIdxs)
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
        s_infos.fileInputAssemblyStateCreateInfo(m_relIdxs, m_topology, m_primitiveRestartEnable);
        m_filedCreateInfo = true;
    }
}