#pragma once
#include "VknConfig.hpp"
#include "VknEngine.hpp"

namespace vkn
{
    class VknCycle
    {
    public:
        // Execution steps
        void wait();
        void acquireImage();
        void recordCommandBuffer();
        void submitCommandBuffer();
        void presentImage();

        // Setup
        void loadConfig(VknConfig *config, VknEngine *engine);
        void recoverFromSwapchainError();

    private:
        // Engine
        VknConfig *m_config{nullptr};
        VknEngine *m_engine{nullptr};

        // Members
        VknDevice *m_device{nullptr};
        VknSwapchain *m_swapchain{nullptr};   // Assuming swapchain 0
        VknRenderpass *m_renderpass{nullptr}; // Assuming renderpass 0
        VknPipeline *m_pipeline{nullptr};     // Assuming pipeline 0 for subpass 0
        VknCommandPool *m_commandPool{nullptr};
        VkCommandBuffer *m_currentCommandBuffer{nullptr};

        // Params
        const int MAX_FRAMES_IN_FLIGHT = 2; // Should match the value used in createSyncObjects

        // State
        uint32_t m_currentFrame = 0;
        uint32_t m_imageIndex;
        std::vector<VkSemaphore> m_signalSemaphores;
        std::vector<VkFence *> m_imagesInFlight{nullptr}; // Fence for each swapchain image
        bool m_pipelineExpectsVertexInputs{false};
        VknIdxs m_devRelIdxs;
        uint32_t verticesDrawnLastFrame{0};
    };
}