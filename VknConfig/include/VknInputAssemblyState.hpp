#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "VknInfos.hpp"

namespace vkn
{
    class VknInputAssemblyState
    {
    public:
        VknInputAssemblyState() = delete;
        VknInputAssemblyState(
            VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        void setDetails(VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable);
        void fillInputAssemblyStateCreateInfo();

    private:
        // Engine
        VknEngine *m_engine{nullptr};
        VknIdxs m_relIdxs{};
        VknIdxs m_absIdxs{};
        VknInfos *m_infos{nullptr};

        // Params
        VkPrimitiveTopology m_topology{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
        VkBool32 m_primitiveRestartEnable{VK_FALSE};

        // State
        bool m_filled{false};
    };
} // namespace vkn