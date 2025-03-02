#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "VknInfos.hpp"

namespace vkn
{
    class VknInputAssemblyState
    {
    public:
        VknInputAssemblyState();
        VknInputAssemblyState(uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
                              VknInfos *infos);
        //~VknInputAssemblyState();

        void setDetails(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable);
        void fillInputAssemblyStateCreateInfo();
        VkPipelineInputAssemblyStateCreateInfo *operator()();

    private:
        uint32_t m_deviceIdx{};
        uint32_t m_renderPassIdx{};
        uint32_t m_subpassIdx{};

        VknInfos *m_infos{nullptr};

        VkPipelineInputAssemblyStateCreateInfo *m_createInfo{VK_NULL_HANDLE};
        VkPrimitiveTopology m_topology{VK_PRIMITIVE_TOPOLOGY_POINT_LIST};
        VkBool32 m_primitiveRestartEnable{VK_FALSE};

        bool m_filled{false};
    };
} // namespace vkn