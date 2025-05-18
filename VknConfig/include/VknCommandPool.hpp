#pragma once

#include "VknEngine.hpp"
#include "VknInfos.hpp"

namespace vkn
{
    class VknCommandPool
    {
    public:
        // Overrides
        VknCommandPool() = default;
        VknCommandPool(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        // Members
        void createCommandPool(uint32_t queueFamilyIndex);
        void createCommandBuffers(uint32_t numSwapchainImages);

        // Getters
        VkCommandBuffer *getCommandBuffer(uint32_t imageIdx);

    private:
        // Engine
        VknEngine *m_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // State
        bool m_commandPoolCreated{false};
        bool m_commandBuffersAllocated{false};
    };
} // namespace vkn