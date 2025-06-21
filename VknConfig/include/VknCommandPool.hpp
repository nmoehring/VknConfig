#pragma once

#include "VknObject.hpp"

namespace vkn
{
    class VknCommandPool : public VknObject
    {
    public:
        // Overrides
        VknCommandPool() = default;
        VknCommandPool(VknIdxs relIdxs, VknIdxs absIdxs);

        // Members
        void createCommandPool(uint32_t queueFamilyIndex);
        void createCommandBuffers(uint32_t numSwapchainImages);

        // Getters
        VkCommandBuffer *getCommandBuffer(uint32_t imageIdx);

    private:
        // State
        bool m_commandPoolCreated{false};
        bool m_commandBuffersAllocated{false};
    };
} // namespace vkn