#include "include/VknDynamicState.hpp"

namespace vkn
{
    VknDynamicState::VknDynamicState(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
        m_dynamicStates.append(VK_DYNAMIC_STATE_VIEWPORT);
        m_dynamicStates.append(VK_DYNAMIC_STATE_SCISSOR);
        /*dynamicStates.append(VK_DYNAMIC_STATE_LINE_WIDTH);
        dynamicStates.append(VK_DYNAMIC_STATE_DEPTH_BIAS);
        dynamicStates.append(VK_DYNAMIC_STATE_DEPTH_BOUNDS);
        dynamicStates.append(VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK);
        dynamicStates.append(VK_DYNAMIC_STATE_STENCIL_WRITE_MASK);
        dynamicStates.append(VK_DYNAMIC_STATE_STENCIL_REFERENCE);
        dynamicStates.append(VK_DYNAMIC_STATE_BLEND_CONSTANTS);
        dynamicStates.append(VK_DYNAMIC_STATE_CULL_MODE);
        dynamicStates.append(VK_DYNAMIC_STATE_FRONT_FACE);
        dynamicStates.append(VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE);
        dynamicStates.append(VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE);
        dynamicStates.append(VK_DYNAMIC_STATE_DEPTH_COMPARE_OP);
        dynamicStates.append(VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE);
        dynamicStates.append(VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE);*/
    }

    void VknDynamicState::addDynamicState(VkDynamicState dynamicState)
    {
        for (int i = 0; i < m_dynamicStates.size(); i++)
        {
            if (m_dynamicStates(i) == dynamicState)
                throw std::runtime_error("Dynamic state has already been added.");
        }
        m_dynamicStates.append(dynamicState);
    }

    VkPipelineDynamicStateCreateInfo *VknDynamicState::fillDynamicStateCreateInfo()
    {
        VkPipelineDynamicStateCreateInfo *result = m_infos->fillDynamicStateCreateInfo(
            m_relIdxs.get<VkDevice>(), m_relIdxs.get<VkRenderPass>(), m_relIdxs.get<VkPipeline>(), m_dynamicStates);
        m_filledCreateInfo = true;
        return result;
    }
}