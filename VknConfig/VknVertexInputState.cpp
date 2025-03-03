#include "VknVertexInputState.hpp"

namespace vkn
{
    VknVertexInputState::VknVertexInputState()
        : m_deviceIdx{0}, m_renderPassIdx{0}, m_subpassIdx{0}, m_infos{nullptr}, m_placeholder{true}
    {
    }

    VknVertexInputState::VknVertexInputState(uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
                                             VknInfos *infos)
        : m_deviceIdx{deviceIdx}, m_renderPassIdx{renderPassIdx}, m_subpassIdx{subpassIdx},
          m_infos{infos}, m_placeholder{false}
    {
    }

    void VknVertexInputState::fillVertexInputStateCreateInfo()
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder Vertex Input State.");
        if (m_filled)
            throw std::runtime_error("Vertex input state already filled.");
        m_infos->fillVertexInputStateCreateInfo(m_deviceIdx, m_renderPassIdx, m_subpassIdx);
        m_filled = true;
    }

    void VknVertexInputState::fillVertexAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset)
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder Vertex Input State.");
        if (m_numBindings == 0)
            throw std::runtime_error("Must have a binding description if there are any attribute descriptions.");
        uint32_t attributeIdx = m_numAttributes;
        m_infos->fillVertexInputAttributeDescription(
            m_deviceIdx, m_renderPassIdx, m_subpassIdx, attributeIdx, binding,
            location, format, offset);
        ++m_numAttributes;
    }

    void VknVertexInputState::fillVertexBindingDescription(
        uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)
    {
        if (m_placeholder)
            throw std::runtime_error("Attempting to configure a placeholder Vertex Input State.");
        uint32_t bindIdx = m_numBindings;
        m_infos->fillVertexInputBindingDescription(
            m_deviceIdx, m_renderPassIdx, m_subpassIdx, bindIdx, binding, stride, inputRate);
        ++m_numBindings;
    }
}