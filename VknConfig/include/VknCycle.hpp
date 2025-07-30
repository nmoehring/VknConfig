#pragma once
#include <cstdint>   // For UINT32_MAX
#include <algorithm> // For std::min and std::max
#include "VknConfig.hpp"
#include "VknDispatch.hpp"

namespace vkn
{
    class VknCycle
    {
    public:
        // Execution steps
        void wait();
        bool acquireImage();

        void beginFrameRecording();
        void beginGraphicsPassRecording();
        void beginPreComputePassRecording();
        void beginPostComputePassRecording();
        void beginUploadRecording();
        void beginDownloadRecording();

        void endGraphicsPassRecording();
        void endPreComputePassRecording();  // Placeholder for compute pass logic
        void endPostComputePassRecording(); // Placeholder for compute pass logic
        void endDownloadRecording();
        void endUploadRecording();

        bool recordGraphicsPass();
        bool recordPreComputePass();
        bool recordPostComputePass();

        bool uploadData(uint_fast8_t threadBufferIdx, size_t size);
        bool preComputeDownload();
        bool graphicsDownload();
        bool postComputeDownload();

        void submitCommandBuffers();
        bool presentImage();

        // Setup
        void loadBasicConfig(VknConfig *config, VknEngine *engine);
        void loadGraphicsConfig(VknConfig *config, VknEngine *engine);
        void loadComputeConfig(VknConfig *config, VknEngine *engine);
        void setVertexBufferIdx(uint32_t idx) { m_vertexBufferAbsIdx = idx; }
        void setIndexBufferIdx(uint32_t idx) { m_indexBufferAbsIdx = idx; }
        void setUploadData(void *data, size_t size);
        void setUploadBufferType(BufferType type) { m_uploadBufferType = type; }
        void clearSubmitInfo();
        void setDispatchQueue(VknSharedQueue *dispatchQueue) { m_dispatchQueue = dispatchQueue; }
        void transferUploadData(uint32_t threadBufferIdx, uint32_t size);

        // Getters
        VknDevice *getDevice()
        {
            return m_device;
        }

        // Misc
        void setClearColor(float r, float g, float b, float a = 1.0f);
        bool recoverFromSwapchainError();
        void recreateForWindowChange();

    private:
        // Engine
        VknConfig *m_config{nullptr};
        VknEngine *m_engine{nullptr};
        VknSharedQueue *m_dispatchQueue{nullptr};

        // Members
        VknDevice *m_device{nullptr};
        VknSwapchain *m_swapchain{nullptr};                // Assuming swapchain 0
        std::list<VknRenderpass> *m_renderpasses{nullptr}; // Assuming renderpass 0
        VknCommandPool *m_presentPool{nullptr};
        VknCommandPool *m_preComputePool{nullptr};
        VknCommandPool *m_postComputePool{nullptr};
        VknCommandPool *m_uploadPool{nullptr};
        VknCommandPool *m_downloadPool{nullptr};
        uint32_t m_vertexBufferAbsIdx{std::numeric_limits<uint32_t>::max()};
        uint32_t m_indexBufferAbsIdx{std::numeric_limits<uint32_t>::max()};
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
        uint_fast32_t m_currentFrameNum = 0;
        uint_fast32_t m_imageIndex;
        std::vector<VkSemaphore> m_signalSemaphores;
        std::vector<VkFence *> m_imagesInFlight; // Fence for each swapchain image
        VknIdxs m_devRelIdxs;
        uint_fast32_t verticesDrawnLastFrame{0};
        bool m_basicConfigLoaded{false};
        bool m_graphicsConfigLoaded{false};
        bool m_computeConfigLoaded{false};
        VkCommandBuffer *m_currentGraphicsCommandBuffer{nullptr};
        VkCommandBuffer *m_currentPreComputeCommandBuffer{nullptr};
        VkCommandBuffer *m_currentPostComputeCommandBuffer{nullptr};
        VkCommandBuffer *m_currentUploadCommandBuffer{nullptr};
        VkCommandBuffer *m_currentDownloadCommandBuffer{nullptr};
        std::vector<void *> m_uploadData{};
        std::vector<size_t> m_uploadSize{};
        BufferType m_uploadBufferType{BufferType::BUFFER_TYPE_NULL};
        std::vector<uint32_t> m_uploads{};
    };
}