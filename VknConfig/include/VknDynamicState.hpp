#pragma once

#include <vulkan/vulkan.h>

#include "VknData.hpp"
#include "VknEngine.hpp"
#include "VknInfos.hpp"

namespace vkn
{
    class VknDynamicState
    {
    public:
        // Overloads
        VknDynamicState(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        // Config
        void addDynamicState(VkDynamicState dynamicState);

        // Create
        VkPipelineDynamicStateCreateInfo *_fileDynamicStateCreateInfo();

    private:
        // Engine
        VknEngine *m_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        VknVector<VkDynamicState> m_dynamicStates{};

        // State
        bool m_filedCreateInfo{false};
        bool m_createdDynamicState{false};
    };
}