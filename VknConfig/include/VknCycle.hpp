#pragma once
#include <cstdint> // For UINT32_MAX
#include "VknConfig.hpp"

namespace vkn
{
    class VknCycle
    {
    public:
        // Execution steps
        void wait();
        bool acquireImage();
        void beginFrameRecording();
        void recordGraphicsPass(uint_fast8_t renderpassIdx);
        void recordComputePass(uint_fast8_t computePassIdx); // Placeholder for compute pass logic
        void uploadData();
        void downloadData();
        void submitCommandBuffer();
        bool presentImage();

        // Setup
        void loadBasicConfig(VknConfig *config, VknEngine *engine);
        void loadGraphicsConfig(VknConfig *config, VknEngine *engine);
        void loadComputeConfig(VknConfig *config, VknEngine *engine);
        // Misc
        void setClearColor(float r, float g, float b, float a = 1.0f);
        bool recoverFromSwapchainError();
        void recreateForWindowChange();

    private:
        // Engine
        VknConfig *m_config{nullptr};
        VknEngine *m_engine{nullptr};

        // Members
        VknDevice *m_device{nullptr};
        VknSwapchain *m_swapchain{nullptr};                // Assuming swapchain 0
        std::list<VknRenderpass> *m_renderpasses{nullptr}; // Assuming renderpass 0
        VknCommandPool *m_presentPool{nullptr};
        VknCommandPool *m_computePool{nullptr};
        VknCommandPool *m_transferPool{nullptr};
        std::vector<VkCommandBuffer> m_commandBuffersToSubmit;
        VknPhysicalDevice *m_physicalDevice{nullptr};
        VkSurfaceCapabilitiesKHR m_capabilities{};

        // Params
        uint_fast64_t m_defaultTimeout = UINT64_MAX; // For vkAcquireNextImageKHR, timeout is uint64_t
        int m_width{0};
        int m_height{0};
        VkExtent2D m_extent{0, 0};
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
        std::vector<VkSwapchainKHR> m_vkSwapchains{};
        VkResult m_presentResult{};

        // State
        uint_fast32_t m_currentFrame = 0;
        uint_fast32_t m_imageIndex;
        std::vector<VkSemaphore> m_signalSemaphores;
        std::vector<VkFence *> m_imagesInFlight; // Fence for each swapchain image
        VknIdxs m_devRelIdxs;
        uint_fast32_t verticesDrawnLastFrame{0};
        bool m_basicConfigLoaded{false};
        bool m_graphicsConfigLoaded{false};
        bool m_computeConfigLoaded{false};
    };
}