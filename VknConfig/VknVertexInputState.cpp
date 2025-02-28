#include "VknVertexInputState.hpp"

namespace vkn
{
    VknVertexInputState::VknVertexInputState(uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
                                             VknInfos *infos)
        : m_deviceIdx{deviceIdx}, m_renderPassIdx{renderPassIdx}, m_subpassIdx{subpassIdx},
          m_infos{infos}
    {
    }

    VkPipelineVertexInputStateCreateInfo *VknVertexInputState::operator()()
    {
        if (!m_filled)
            throw std::runtime_error("Vertex input state not filled before attempting to retrieve.");
        return m_createInfo;
    }

    void VknVertexInputState::fillVertexInputStateCreateInfo()
    {
        m_infos->fillVertexInputStateCreateInfo(m_deviceIdx, m_renderPassIdx, m_subpassIdx);
        m_filled = true;
    }

    void VknVertexInputState::fillVertexAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset)
    {
        if (!(m_vertexBindingDescriptions.size() == m_vertexAttributeDescriptions.size() + 1))
            throw std::runtime_error("Vertex attribute description must be filled after binding description.");
        m_infos->fillVertexInputAttributeDescription(
            m_deviceIdx, m_renderPassIdx, m_subpassIdx, binding, location, format, offset);
    }

    void VknVertexInputState::fillVertexBindingDescription(
        uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)
    {
        m_vertexBindingDescriptions.push_back(m_infos->fillVertexInputBindingDescription(
            m_deviceIdx, m_renderPassIdx, m_subpassIdx, binding, stride, inputRate));
    }
}