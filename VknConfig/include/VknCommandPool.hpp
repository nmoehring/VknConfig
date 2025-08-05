#pragma once

#include "VknObject.hpp"

namespace vkn
{
    enum CommandBufferType
    {
        UPLOAD_CB = 0,
        DOWNLOAD_CB = 1,
        PRECOMPUTE_CB = 2,
        POSTCOMPUTE_CB = 3,
        GRAPHICS_CB = 4,
        PRESENT_CB = 5,
        NUM_CB_TYPE = 6
    };

    class VknCommandPool : public VknObject
    {
    public:
        // Overrides
        VknCommandPool() = default;
        VknCommandPool(VknIdxs relIdxs, VknIdxs absIdxs);

        // Members
        void createCommandPool(uint32_t queueFamilyIndex);
        uint32_t createCommandBuffers(uint32_t numUniqueBuffers = 1u);

        // Getters
        VkCommandBuffer *getCommandBuffer(uint32_t frameNum, uint32_t imageIdx);

    private:
        // State
        bool m_commandPoolCreated{false};
        bool m_commandBuffersAllocated{false};
        uint32_t m_numTotalCommandBuffers{0};
    };
} // namespace vkn