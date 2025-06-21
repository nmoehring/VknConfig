#pragma once

#include <vulkan/vulkan.h>

#include "VknObject.hpp"
#include "VknData.hpp"

namespace vkn
{
    class VknDynamicState : public VknObject
    {
    public:
        // Overloads
        VknDynamicState(VknIdxs relIdxs, VknIdxs absIdxs);

        // Config
        void addDynamicState(VkDynamicState dynamicState);

        // Create
        VkPipelineDynamicStateCreateInfo *_fileDynamicStateCreateInfo();

    private:
        // dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        VknVector<VkDynamicState> m_dynamicStates{};

        // State
        bool m_filedCreateInfo{false};
        bool m_createdDynamicState{false};
    };
}