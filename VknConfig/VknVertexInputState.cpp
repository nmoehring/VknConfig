#include "include/VknVertexInputState.hpp"

namespace vkn
{
    VknVertexInputState::VknVertexInputState(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_infos{infos}
    {
    }

    void VknVertexInputState::_fillVertexInputStateCreateInfo()
    {
        if (m_filled)
            throw std::runtime_error("Vertex input state already filled.");
        m_infos->fillVertexInputStateCreateInfo(m_relIdxs, m_numBindings, m_numAttributes);
        m_filled = true;
    }

    void VknVertexInputState::fillVertexAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset)
    {
        if (m_numBindings == 0)
            throw std::runtime_error("Must have a binding description if there are any attribute descriptions.");
        uint32_t attributeIdx = m_numAttributes;
        m_infos->fillVertexInputAttributeDescription(
            m_relIdxs, attributeIdx, binding,
            location, format, offset);
        ++m_numAttributes;
        m_attributesFilled = true;
    }

    void VknVertexInputState::fillVertexBindingDescription(
        uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)
    {
        uint32_t bindIdx = m_numBindings;
        m_infos->fillVertexInputBindingDescription(
            m_relIdxs, bindIdx, binding, stride, inputRate);
        ++m_numBindings;
        m_bindingsFilled = true;
    }
}