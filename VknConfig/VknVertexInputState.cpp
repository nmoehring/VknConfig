#include "include/VknVertexInputState.hpp"

namespace vkn
{
    VknVertexInputState::VknVertexInputState(VknIdxs relIdxs, VknIdxs absIdxs)
        : VknObject(relIdxs, absIdxs)
    {
    }

    void VknVertexInputState::_fileVertexInputStateCreateInfo()
    {
        if (m_filed)
            throw std::runtime_error("Vertex input state already filed.");
        s_infos.fileVertexInputStateCreateInfo(m_relIdxs, m_numBindings, m_numAttributes);
        m_filed = true;
    }

    void VknVertexInputState::fileVertexAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset)
    {
        if (m_numBindings == 0)
            throw std::runtime_error("Must have a binding description if there are any attribute descriptions.");
        uint32_t attributeIdx = m_numAttributes;
        s_infos.fileVertexInputAttributeDescription(
            m_relIdxs, attributeIdx, binding,
            location, format, offset);
        ++m_numAttributes;
        m_attributesfiled = true;
    }

    void VknVertexInputState::fileVertexBindingDescription(
        uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)
    {
        uint32_t bindIdx = m_numBindings;
        s_infos.fileVertexInputBindingDescription(
            m_relIdxs, bindIdx, binding, stride, inputRate);
        ++m_numBindings;
        m_bindingsfiled = true;
    }
}