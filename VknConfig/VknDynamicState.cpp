#include "include/VknDynamicState.hpp"

namespace vkn
{
    VknDynamicState::VknDynamicState(VknIdxs relIdxs, VknIdxs absIdxs) : VknObject(relIdxs, absIdxs)
    {
        m_dynamicStates.appendOne(VK_DYNAMIC_STATE_VIEWPORT);
        m_dynamicStates.appendOne(VK_DYNAMIC_STATE_SCISSOR);
        /*dynamicStates.appendOne(VK_DYNAMIC_STATE_LINE_WIDTH);
        dynamicStates.appendOne(VK_DYNAMIC_STATE_DEPTH_BIAS);
        dynamicStates.appendOne(VK_DYNAMIC_STATE_DEPTH_BOUNDS);
        dynamicStates.appendOne(VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK);
        dynamicStates.appendOne(VK_DYNAMIC_STATE_STENCIL_WRITE_MASK);
        dynamicStates.appendOne(VK_DYNAMIC_STATE_STENCIL_REFERENCE);
        dynamicStates.appendOne(VK_DYNAMIC_STATE_BLEND_CONSTANTS);
        dynamicStates.appendOne(VK_DYNAMIC_STATE_CULL_MODE);
        dynamicStates.appendOne(VK_DYNAMIC_STATE_FRONT_FACE);
        dynamicStates.appendOne(VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE);
        dynamicStates.appendOne(VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE);
        dynamicStates.appendOne(VK_DYNAMIC_STATE_DEPTH_COMPARE_OP);
        dynamicStates.appendOne(VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE);
        dynamicStates.appendOne(VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE);*/
    }

    void VknDynamicState::addDynamicState(VkDynamicState dynamicState)
    {
        for (int i = 0; i < m_dynamicStates.size(); i++)
        {
            if (m_dynamicStates(i) == dynamicState)
                throw std::runtime_error("Dynamic state has already been added.");
        }
        m_dynamicStates.appendOne(dynamicState);
    }

    VkPipelineDynamicStateCreateInfo *VknDynamicState::_fileDynamicStateCreateInfo()
    {
        VkPipelineDynamicStateCreateInfo *result = s_infos->fileDynamicStateCreateInfo(
            m_relIdxs.get<VkDevice>(), m_relIdxs.get<VkRenderPass>(), m_relIdxs.get<VkPipeline>(), m_dynamicStates);
        m_filedCreateInfo = true;
        return result;
    }
}