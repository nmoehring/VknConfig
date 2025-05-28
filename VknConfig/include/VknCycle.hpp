#pragma once
#include "VknConfig.hpp"
#include "VknEngine.hpp"

namespace vkn
{
    class VknCycle
    {
    public:
        // Execution steps
        bool isWindowMinimized();
        void wait();
        bool acquireImage();
        void recordCommandBuffer();
        void submitCommandBuffer();
        bool presentImage();

        // Setup
        void loadConfig(VknConfig *config, VknEngine *engine);
        bool recoverFromSwapchainError();
        void setMaxFramesInFlight(uint_fast32_t maxFramesInFlight);
        void setClearColor(float r, float g, float b, float a = 1.0f);

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
        VknPhysicalDevice *m_physicalDevice{nullptr};
        VkSurfaceCapabilitiesKHR m_capabilities{};

        // Params
        uint_fast32_t MAX_FRAMES_IN_FLIGHT = 0; // Should match the value used in createSyncObjects
        uint_fast32_t m_defaultTimeout = uint_fast64_t(0) - 1u;
        int m_width{0};
        int m_height{0};
        VkExtent2D m_extent{0, 0};
        VknVertexInputState *m_vertexInputState{nullptr};
        VkResult m_acquireResult{VK_SUCCESS};
        VkCommandBufferBeginInfo m_beginInfo{};
        VknResult m_resBegin{"Begin command buffer."};
        VknResult m_resEnd{"End command buffer."};
        VkRenderPassBeginInfo m_renderPassBeginInfo{};
        VkClearValue m_clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        VkSubmitInfo m_submitInfo{};
        std::vector<VkSemaphore> m_waitSemaphores{};
        std::vector<VkPipelineStageFlags> m_waitStages{};
        VknResult m_resSubmit{"Submit command buffer."};
        VkPresentInfoKHR m_presentInfo{};
        std::vector<VkSwapchainKHR> m_swapchains{};
        VkResult m_presentResult{};

        // State
        uint_fast32_t m_currentFrame = 0;
        uint_fast32_t m_imageIndex;
        std::vector<VkSemaphore> m_signalSemaphores;
        std::vector<VkFence *> m_imagesInFlight; // Fence for each swapchain image
        bool m_pipelineExpectsVertexInputs{false};
        VknIdxs m_devRelIdxs;
        uint_fast32_t verticesDrawnLastFrame{0};
        bool m_configLoaded{false};
    };
}