#include "include/VknCycle.hpp"

namespace vkn
{
    void VknCycle::setClearColor(float r, float g, float b, float a)
    {
        if (r < 0.0f || r > 1.0f || g < 0.0f || g > 1.0f || b < 0.0f || b > 1.0f || a < 0.0f || a > 1.0f)
            throw std::runtime_error("Invalid clear color value.");
        m_clearColor = {{{r, g, b, a}}};
    }

    void VknCycle::loadBasicConfig(VknConfig *config, VknEngine *engine)
    {
        m_config = config;
        m_engine = engine;
        m_device = m_config->getDevice(0);
        if (m_device->getNumVertexBuffers() > 0)
            m_currentVertexBuffer = m_device->getVertexBuffer(0, m_currentFrameNum);
        if (m_device->getNumIndexBuffers() > 0)
            m_currentIndexBuffer = m_device->getIndexBuffer(0, m_currentFrameNum);
        m_uploadPool = m_device->getCommandPool(TRANSFER);
        m_downloadPool = m_device->getCommandPool(TRANSFER);
        m_presentPool = m_device->getCommandPool(PRESENT);
        m_preComputePool = m_device->getCommandPool(COMPUTE);
        m_postComputePool = m_device->getCommandPool(COMPUTE);

        m_physicalDevice = m_device->getPhysicalDevice();

        m_device->createSyncObjects();

        m_waitSemaphores.push_back(VkSemaphore{});
        m_waitStages.push_back(VkPipelineStageFlags{});

        m_uploadData.reserve(VknObject::s_maxFramesInFlight);
        m_uploadSize.reserve(VknObject::s_maxFramesInFlight);

        m_basicConfigLoaded = true;
    }

    void VknCycle::loadGraphicsConfig(VknConfig *config, VknEngine *engine)
    {
        m_swapchain = m_device->getSwapchain();
        m_renderpasses = m_device->getRenderpasses();

        uint32_t actualSwapchainImageCount = m_swapchain->getNumImages();
        if (actualSwapchainImageCount == 0)
            throw std::runtime_error("Swapchain has 0 images in VknCycle::loadConfig. Ensure swapchain is created and has images.");

        m_vkSwapchains.push_back(*m_swapchain->getVkSwapchain());
        m_imagesInFlight.assign(actualSwapchainImageCount, nullptr);

        m_graphicsConfigLoaded = true;
    }

    void VknCycle::loadComputeConfig(VknConfig *config, VknEngine *engine)
    {
        // TODO: consider supporting non-support graphics families
        // TODO: load "compute passes"

        m_computeConfigLoaded = true;
    }

    void VknCycle::wait()
    {
        if (!m_basicConfigLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");

        // Copy any download data from last frame
        for (VknBuffer *buffer : m_config->getPreComputeBuffers())
            buffer->msgToDownloadData();
        for (VknBuffer *buffer : m_config->getGraphicsBuffers())
            buffer->msgToDownloadData();
        for (VknBuffer *buffer : m_config->getPostComputeBuffers())
            buffer->msgToDownloadData();

        // Clear submit-related vectors
        this->clearSubmitInfo();
        m_currentFrameNum = (m_currentFrameNum + 1) % m_swapchain->getNumImages(); // Move to the next frame

        if (m_device->getNumVertexBuffers() > 0)
            m_currentVertexBuffer = m_device->getVertexBuffer(0, m_currentFrameNum);
        if (m_device->getNumIndexBuffers() > 0)
            m_currentIndexBuffer = m_device->getIndexBuffer(0, m_currentFrameNum);

        // Copy any upload data for this frame
        for (VknBuffer *buffer : m_config->getPreComputeBuffers())
            buffer->waitOnUploadData();
        for (VknBuffer *buffer : m_config->getGraphicsBuffers())
            buffer->waitOnUploadData();

        // 1. Wait for the previous frame to finish
        vkWaitForFences(
            *m_device->getVkDevice(), 1u, &m_device->getFence(m_currentFrameNum), VK_TRUE, m_defaultTimeout);
    }

    bool VknCycle::acquireImage()
    {
        if (!m_graphicsConfigLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");

        this->wait();
        // 2. Acquire an image from the swapchain
        m_acquireResult = vkAcquireNextImageKHR(
            *m_device->getVkDevice(), *m_swapchain->getVkSwapchain(), m_defaultTimeout,
            m_device->getImageAvailableSemaphore(m_currentFrameNum), VK_NULL_HANDLE, &m_imageIndex);

        if (m_acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
        {
            this->recoverFromSwapchainError(); // Skip rendering this frame
            return false;
        }
        else if (m_acquireResult != VK_SUBOPTIMAL_KHR && m_acquireResult != VK_SUCCESS)
            throw std::runtime_error("Failed to acquire swapchain image!");

        // Check if a previous frame is using this image
        if (m_imagesInFlight[m_imageIndex] != nullptr)
            vkWaitForFences(*m_device->getVkDevice(), 1, m_imagesInFlight[m_imageIndex], VK_TRUE, m_defaultTimeout);

        // Mark the image as being in use by this frame
        m_imagesInFlight[m_imageIndex] = &m_device->getFence(m_currentFrameNum);
        return true;
    }

    void VknCycle::beginFrameRecording()
    {
        this->beginUploadRecording();
        this->beginDownloadRecording();
        this->beginPreComputePassRecording();
        this->beginPostComputePassRecording();
        this->beginGraphicsPassRecording();
    }

    void VknCycle::beginGraphicsPassRecording()
    {
        if (m_currentGraphicsCommandBuffer)
            throw std::runtime_error("Graphics command buffer already recording.");
        m_currentGraphicsCommandBuffer = m_presentPool->getCommandBuffer(m_currentFrameNum, 0);
        vkResetCommandBuffer(*m_currentGraphicsCommandBuffer, 0);

        m_beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        m_beginInfo.flags = 0; // Optional: VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        m_resBegin = vkBeginCommandBuffer(*m_currentGraphicsCommandBuffer, &m_beginInfo);
        VknObject::s_recordingGfxCommandBuffer = true;
    }

    void VknCycle::beginPreComputePassRecording()
    {
        if (m_currentPreComputeCommandBuffer)
            throw std::runtime_error("Compute command buffer already recording.");
        m_currentPreComputeCommandBuffer = m_preComputePool->getCommandBuffer(m_currentFrameNum, 0);
        vkResetCommandBuffer(*m_currentPreComputeCommandBuffer, 0);

        m_beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        m_beginInfo.flags = 0; // Optional: VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        m_resBegin = vkBeginCommandBuffer(*m_currentPreComputeCommandBuffer, &m_beginInfo);
        VknObject::s_recordingPreComputeCommandBuffer = true;
    }

    void VknCycle::beginPostComputePassRecording()
    {
        if (m_currentPostComputeCommandBuffer)
            throw std::runtime_error("Compute command buffer already recording.");
        m_currentPostComputeCommandBuffer = m_postComputePool->getCommandBuffer(m_currentFrameNum, 1);
        vkResetCommandBuffer(*m_currentPostComputeCommandBuffer, 0);

        m_beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        m_beginInfo.flags = 0; // Optional: VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        m_resBegin = vkBeginCommandBuffer(*m_currentPostComputeCommandBuffer, &m_beginInfo);
        VknObject::s_recordingPostComputeCommandBuffer = true;
    }

    void VknCycle::beginDownloadRecording()
    {
        if (m_currentDownloadCommandBuffer)
            throw std::runtime_error("Download command buffer already recording.");
        m_currentDownloadCommandBuffer = m_downloadPool->getCommandBuffer(m_currentFrameNum, 1); // Use m_currentFrameNum for download buffers

        vkResetCommandBuffer(*m_currentDownloadCommandBuffer, 0);

        m_beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        m_beginInfo.flags = 0; // Optional: VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        m_resBegin = vkBeginCommandBuffer(*m_currentDownloadCommandBuffer, &m_beginInfo);
        VknObject::s_recordingDownloadCommandBuffer = true;
        VknObject::s_downloadCommandBuffer = m_currentDownloadCommandBuffer;
    }

    void VknCycle::beginUploadRecording()
    {
        if (m_currentUploadCommandBuffer)
            throw std::runtime_error("Upload command buffer already recording.");
        m_currentUploadCommandBuffer = m_uploadPool->getCommandBuffer(m_currentFrameNum, 0); // Use m_currentFrameNum for upload buffers
        vkResetCommandBuffer(*m_currentUploadCommandBuffer, 0);

        m_beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        m_beginInfo.flags = 0; // Optional: VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        m_resBegin = vkBeginCommandBuffer(*m_currentUploadCommandBuffer, &m_beginInfo);
        VknObject::s_recordingUploadCommandBuffer = true;
        VknObject::s_uploadCommandBuffer = m_currentUploadCommandBuffer;
    }

    void VknCycle::endUploadRecording()
    {
        if (!VknObject::s_recordingUploadCommandBuffer)
            throw std::runtime_error("VknCycle::endUploadRecording called when not recording a transfer command buffer. Call beginTransferRecording first.");
        if (!m_currentUploadCommandBuffer)
            return;

        m_resEnd = vkEndCommandBuffer(*m_currentUploadCommandBuffer);
        m_currentUploadCommandBuffer = nullptr;
        VknObject::s_recordingUploadCommandBuffer = false;
        VknObject::s_uploadCommandBuffer = nullptr;
    }

    void VknCycle::endDownloadRecording()
    {
        if (!VknObject::s_recordingDownloadCommandBuffer)
            throw std::runtime_error("VknCycle::endDownloadRecording called when not recording a transfer command buffer. Call beginTransferRecording first.");
        if (!m_currentDownloadCommandBuffer)
            return;

        m_resEnd = vkEndCommandBuffer(*m_currentDownloadCommandBuffer);
        m_currentDownloadCommandBuffer = nullptr;
        VknObject::s_recordingDownloadCommandBuffer = false;
        VknObject::s_downloadCommandBuffer = nullptr;
    }

    void VknCycle::endGraphicsPassRecording()
    {
        if (!VknObject::s_recordingGfxCommandBuffer)
            throw std::runtime_error("VknCycle::endGraphicsPassRecording called when not recording a graphics command buffer. Call beginGraphicsPassRecording first.");
        if (!m_currentGraphicsCommandBuffer)
            return;

        m_resEnd = vkEndCommandBuffer(*m_currentGraphicsCommandBuffer);
        m_currentGraphicsCommandBuffer = nullptr;
        VknObject::s_recordingGfxCommandBuffer = false;
    }

    void VknCycle::endPreComputePassRecording()
    {
        if (!VknObject::s_recordingPreComputeCommandBuffer)
            throw std::runtime_error("VknCycle::endComputePassRecording called when not recording a compute command buffer. Call beginComputePassRecording first.");
        if (!m_currentPreComputeCommandBuffer)
            return;

        m_resEnd = vkEndCommandBuffer(*m_currentPreComputeCommandBuffer);
        m_currentPreComputeCommandBuffer = nullptr;
        VknObject::s_recordingPreComputeCommandBuffer = false;
    }

    void VknCycle::endPostComputePassRecording()
    {
        if (!VknObject::s_recordingPostComputeCommandBuffer)
            throw std::runtime_error("VknCycle::endComputePassRecording called when not recording a compute command buffer. Call beginComputePassRecording first.");
        if (!m_currentPostComputeCommandBuffer)
            return;

        m_resEnd = vkEndCommandBuffer(*m_currentPostComputeCommandBuffer);
        m_currentPostComputeCommandBuffer = nullptr;
        VknObject::s_recordingPostComputeCommandBuffer = false;
    }

    bool VknCycle::recordGraphicsPass()
    {
        if (!m_graphicsConfigLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");
        if (!VknObject::s_recordingGfxCommandBuffer)
            throw std::runtime_error("VknCycle::recordGraphicsPass called when not recording a graphics command buffer. Call beginGraphicsPassRecording first.");

        m_renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        VknRenderpass *renderpass = getListElement(0, *m_renderpasses);
        m_renderPassBeginInfo.renderPass = *renderpass->getVkRenderPass();
        m_renderPassBeginInfo.framebuffer = *renderpass->getFramebuffer(m_imageIndex)->getVkFramebuffer();
        m_renderPassBeginInfo.renderArea.offset = {0, 0};
        m_renderPassBeginInfo.renderArea.extent = m_swapchain->getActualExtent();

        m_renderPassBeginInfo.clearValueCount = 1; // Assuming one color attachment, should be renderpass attachments that have loadOp=VK_ATTACHMENT_LOAD_OP_CLEAR
        m_renderPassBeginInfo.pClearValues = &m_clearColor;

        vkCmdBeginRenderPass(*m_currentGraphicsCommandBuffer, &m_renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Correctly iterate through pipelines: bind, set state, and draw for each one.
        for (VknPipeline &pipeline : *renderpass->getPipelines())
        {
            // 1. Bind the pipeline
            vkCmdBindPipeline(*m_currentGraphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline.getVkPipeline());

            // 2. Set dynamic states for this pipeline
            VknViewportState *viewportState = pipeline.getViewportState();
            if (viewportState)
            {
                vkCmdSetViewport(*m_currentGraphicsCommandBuffer, 0, 1, &viewportState->getVkViewport(0));
                vkCmdSetScissor(*m_currentGraphicsCommandBuffer, 0, 1, &viewportState->getVkScissor(0));
            }

            // 3. Check how to draw for this pipeline
            if (m_currentIndexBuffer)
            {
                // Indexed drawing
                VknVertexInputState *vertexInputState = pipeline.getVertexInputState();
                if (!vertexInputState || vertexInputState->getNumBindings() == 0)
                {
                    throw std::runtime_error("Indexed drawing requested, but no vertex input bindings are configured in the pipeline.");
                }

                VkDeviceSize offset = 0;
                vkCmdBindVertexBuffers(*m_currentGraphicsCommandBuffer, 0, 1, m_currentVertexBuffer->getVkBuffer(), &offset);
                vkCmdBindIndexBuffer(*m_currentGraphicsCommandBuffer, *m_currentIndexBuffer->getVkBuffer(), 0, VK_INDEX_TYPE_UINT32);
                vkCmdDrawIndexed(*m_currentGraphicsCommandBuffer, m_numIndices, 1, 0, 0, 0);
                m_primitivesDrawnLastFrame = m_numIndices / 3;
            }
            else if (m_currentVertexBuffer)
            {
                // Non-indexed drawing from a vertex buffer
                VknVertexInputState *vertexInputState = pipeline.getVertexInputState();
                if (!vertexInputState || vertexInputState->getNumBindings() == 0)
                {
                    throw std::runtime_error("Non-indexed drawing from a buffer was requested, but no vertex input bindings are configured in the pipeline.");
                }

                VkDeviceSize offset = 0;
                vkCmdBindVertexBuffers(*m_currentGraphicsCommandBuffer, 0, 1, m_currentVertexBuffer->getVkBuffer(), &offset);
                vkCmdDraw(*m_currentGraphicsCommandBuffer, m_numVertices, 1, 0, 0);
                m_primitivesDrawnLastFrame = m_numVertices / 3; // Assuming triangles
            }
            else if (pipeline.getNumHardCodedVertices() > 0)
            {
                // This pipeline uses hard-coded vertices in the shader.
                vkCmdDraw(*m_currentGraphicsCommandBuffer, pipeline.getNumHardCodedVertices(), 1, 0, 0);
                m_primitivesDrawnLastFrame = pipeline.getNumHardCodedVertices() / 3; // Assuming triangles
            }
        }

        vkCmdEndRenderPass(*m_currentGraphicsCommandBuffer);
        return true;
    }

    bool VknCycle::recordPreComputePass()
    {
        if (!m_computeConfigLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");
        if (!VknObject::s_recordingGfxCommandBuffer)
            throw std::runtime_error("VknCycle::recordGraphicsPass called when not recording a graphics command buffer. Call beginGraphicsPassRecording first.");

        // TODO: Record compute pipeline binding, descriptor sets, and dispatch calls.
        // TODO: Record a pipeline barrier to ensure compute writes are visible to the graphics pass.
        return true;
    }

    bool VknCycle::recordPostComputePass()
    {
        if (!m_computeConfigLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");
        if (!VknObject::s_recordingPostComputeCommandBuffer)
            throw std::runtime_error("VknCycle::recordPostComputePass called when not recording a post-compute command buffer. Call beginPostComputePassRecording first.");

        // TODO: Record post-compute pipeline binding, descriptor sets, and dispatch calls.
        // TODO: Record a pipeline barrier to ensure post-compute writes are visible to the graphics pass.
        return true;
    }

    void VknCycle::submitCommandBuffers()
    {
        if (!m_basicConfigLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");

        if (m_currentUploadCommandBuffer)
            this->endUploadRecording();
        if (m_currentDownloadCommandBuffer)
            this->endDownloadRecording();
        if (m_currentPreComputeCommandBuffer)
            this->endPreComputePassRecording();
        if (m_currentPostComputeCommandBuffer)
            this->endPostComputePassRecording();
        if (m_currentGraphicsCommandBuffer)
            this->endGraphicsPassRecording();

        // Iterate through the possible command buffer types, submitting buffers as appropriate
        // This assumes you want all the command buffers to run on the same queue... if different queues are needed
        // some extra code will be needed.

        for (uint32_t i = 0; i < CommandBufferType::NUM_CB_TYPE; ++i)
        {
            this->clearSubmitInfo();
            VkQueue *currentQueue = m_device->getQueue(static_cast<QueueType>(i));

            if (i == CommandBufferType::GRAPHICS_CB)
                continue;                               // Graphics queue (as considered separate from PRESENT) is unhandled currently.
            else if (i == CommandBufferType::UPLOAD_CB) // Upload
            {
                currentQueue = m_device->getQueue(TRANSFER, 0);
                m_waitSemaphores[0] = m_device->getImageAvailableSemaphore(m_currentFrameNum);
                m_waitStages[0] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                m_signalSemaphores.push_back(m_device->getUploadsFinishedSemaphore(m_currentFrameNum));
                m_submitInfo.pCommandBuffers = m_currentUploadCommandBuffer;
            }
            else if (i == CommandBufferType::PRECOMPUTE_CB)
            {
                currentQueue = m_device->getQueue(COMPUTE, 0);
                m_waitSemaphores.push_back(m_device->getUploadsFinishedSemaphore(m_currentFrameNum));
                m_waitStages.push_back(VK_PIPELINE_STAGE_TRANSFER_BIT);
                m_signalSemaphores.push_back(m_device->getPreComputeStageFinishedSemaphore(m_currentFrameNum));
                m_submitInfo.pCommandBuffers = m_currentPreComputeCommandBuffer;
            }
            else if (i == CommandBufferType::PRESENT_CB)
            {
                currentQueue = m_device->getQueue(PRESENT, 0);
                m_waitSemaphores.push_back(m_device->getImageAvailableSemaphore(m_currentFrameNum));
                m_waitSemaphores.push_back(m_device->getPreComputeStageFinishedSemaphore(m_currentFrameNum));
                m_signalSemaphores.push_back(m_device->getRenderFinishedSemaphore(m_currentFrameNum));
                m_waitStages.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
                m_waitStages.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
                m_submitInfo.pCommandBuffers = m_currentGraphicsCommandBuffer;
            }
            else if (i == CommandBufferType::POSTCOMPUTE_CB)
            {
                uint_fast32_t familyIdx = m_device->getQueueFamilyIdxByType(COMPUTE);
                uint_fast32_t numQueues = m_physicalDevice->getQueue(familyIdx).getNumAvailable();
                currentQueue = m_device->getQueue(COMPUTE, 1 % numQueues);
                m_waitSemaphores.push_back(m_device->getRenderFinishedSemaphore(m_currentFrameNum));
                m_waitStages.push_back(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
                m_signalSemaphores.push_back(m_device->getPostComputeFinishedSemaphore(m_currentFrameNum));
                m_submitInfo.pCommandBuffers = m_currentPostComputeCommandBuffer;
            }
            else if (i == CommandBufferType::DOWNLOAD_CB) // Download
            {
                // Check number of transfer queues and use remainder operator to try to select the second one if it exists
                uint_fast32_t familyIdx = m_device->getQueueFamilyIdxByType(TRANSFER);
                uint_fast32_t numQueues = m_physicalDevice->getQueue(familyIdx).getNumAvailable();
                currentQueue = m_device->getQueue(TRANSFER, 1 % numQueues);
                m_waitSemaphores.push_back(m_device->getPostComputeFinishedSemaphore(m_currentFrameNum));
                m_waitStages.push_back(VK_PIPELINE_STAGE_TRANSFER_BIT);
                m_signalSemaphores.push_back(m_device->getDownloadsFinishedSemaphore(m_currentFrameNum));
                m_submitInfo.pCommandBuffers = m_currentDownloadCommandBuffer;
            }

            m_submitInfo.waitSemaphoreCount = m_waitSemaphores.size();
            m_submitInfo.pWaitSemaphores = m_waitSemaphores.data();
            m_submitInfo.pWaitDstStageMask = m_waitStages.data();

            // signal when upload is done
            m_submitInfo.signalSemaphoreCount = m_signalSemaphores.size();
            m_submitInfo.pSignalSemaphores = m_signalSemaphores.data();

            // your upload command buffer
            m_submitInfo.commandBufferCount = 1u;

            vkResetFences(*m_device->getVkDevice(), 1, &m_device->getFence(m_currentFrameNum)); // Reset the fence before submitting
            vkQueueSubmit(*currentQueue, 1, &m_submitInfo, VK_NULL_HANDLE);
        }
    }

    bool VknCycle::presentImage()
    {
        if (!m_graphicsConfigLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");

        // --- ADD THIS CHECK ---
        if (m_config && m_config->getWindow() && !m_config->getWindow()->isActive())
        {
            // Window is minimized or not ready, skip presenting.
            return false;
        }
        // --- END ADD ---

        // 5. Present the image
        m_presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        m_presentInfo.pNext = nullptr;

        m_presentInfo.waitSemaphoreCount = 1;
        m_presentInfo.pWaitSemaphores = &m_device->getRenderFinishedSemaphore(m_currentFrameNum);
        m_presentInfo.swapchainCount = 1;
        m_presentInfo.pSwapchains = m_vkSwapchains.data();
        m_presentInfo.pImageIndices = &m_imageIndex;
        m_presentInfo.pResults = nullptr; // Optional: to check results per swapchain

        m_presentResult = vkQueuePresentKHR(*m_device->getQueue(QueueType::PRESENT), &m_presentInfo);

        if (m_presentResult == VK_ERROR_OUT_OF_DATE_KHR || m_presentResult == VK_SUBOPTIMAL_KHR)
            return this->recoverFromSwapchainError();
        else if (m_presentResult != VK_SUCCESS)
            throw std::runtime_error("Failed to present swapchain image!");
        return true;
    }

    bool VknCycle::recoverFromSwapchainError()
    {
        // First, check if the window is minimized using the most reliable method.
        // If it is, we cannot and should not attempt to recreate the swapchain with zero dimensions.
        // Signal that recovery failed *for now*; the calling layer (VknApp)
        // should handle waiting for window events (like un-minimizing).
        if (!m_config->getWindow()->isActive())
            return false;

        vkDeviceWaitIdle(*m_device->getVkDevice());

        // Critical check: Re-query surface capabilities *directly* before attempting swapchain recreation.
        // This ensures we have the absolute latest extent information.

        if (!m_swapchain->getSurfaceIdx().has_value())
            throw std::runtime_error("Swapchain does not have a valid surface index for recovery.");

        // This might not be necessary because minimization doesn't trigger this recovery function anymore
        // I'm not sure about other platforms though, so I'll keep it in mind
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            *(m_physicalDevice->getVkPhysicalDevice()),
            m_engine->getObject<VkSurfaceKHR>(m_swapchain->getSurfaceIdx().value()),
            &m_capabilities);

        if (m_capabilities.currentExtent.width == 0 || m_capabilities.currentExtent.height == 0)
            // Window became (or still is) minimized just before recreation attempt.
            return false; // Signal app to wait for events.

        // If we've reached here, the window is not minimized, and capabilities show non-zero extent.
        this->recreateForWindowChange();
        std::cerr << "Recovered from swapchain error." << std::endl;
        return true;
    }

    void VknCycle::recreateForWindowChange()
    {
        for (auto &renderpass : *m_renderpasses)
            renderpass.demolishFramebuffers();

        m_swapchain->recreateSwapchain();
        m_vkSwapchains[0] = *m_swapchain->getVkSwapchain(); // Update
        m_device->recreateSyncObjects();

        // Reset m_imagesInFlight for the new swapchain
        m_imagesInFlight.assign(m_swapchain->getNumImages(), nullptr);

        for (auto &renderpass : *m_renderpasses)
        {
            for (auto &pipeline : *renderpass.getPipelines())
                pipeline.getViewportState()->syncWithSwapchain(*m_swapchain, 0, 0);
            renderpass.createFramebuffers(*m_swapchain);
        }
    }

    void VknCycle::clearSubmitInfo()
    {
        m_submitInfo = VkSubmitInfo{};
        m_submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        m_submitInfo.pNext = nullptr;
        m_submitInfo.waitSemaphoreCount = 0;
        m_submitInfo.pWaitSemaphores = nullptr;
        m_submitInfo.pWaitDstStageMask = nullptr;
        m_submitInfo.commandBufferCount = 0;
        m_submitInfo.pCommandBuffers = nullptr;
        m_submitInfo.signalSemaphoreCount = 0;
        m_submitInfo.pSignalSemaphores = nullptr;

        m_waitSemaphores.clear();
        m_signalSemaphores.clear();
        m_waitStages.clear();
        m_numIndices = 0;
        m_numVertices = 0;

        for (VknMessage &msg : m_sentMessages[m_currentFrameNum].getDataVector())
        {
            msg.processed.wait(false);
            msg.processed.store(false);
        }
    }

    void VknCycle::setUploadData(void *data, size_t size)
    {
        if (m_uploadData.size() >= VknObject::s_maxFramesInFlight)
            throw std::runtime_error("Exceeded maximum frames in flight for upload data.");
        if (m_uploadData[m_currentFrameNum])
            throw std::runtime_error("Upload data already set for the current frame.");
        m_uploadData[m_currentFrameNum] = data;
        m_uploadSize[m_currentFrameNum] = size;
    }

    bool VknCycle::uploadData()
    {
        if (!m_basicConfigLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");

        for (VknBuffer *buffer : m_config->getPreComputeBuffers())
            buffer->uploadData();
        for (VknBuffer *buffer : m_config->getGraphicsBuffers())
            buffer->uploadData();

        return true;
    }

    void VknCycle::transferUploadData(uint32_t threadBufferIdx, uint32_t size)
    {
        if (!m_basicConfigLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");

        VknMessage &msg = m_sentMessages[m_currentFrameNum].append(VknMessage{});
        msg.type = VknMessageType::VknThreadMessageType_Transfer;
        msg.srcThreadName = VknThreadName::AppThread;
        msg.dstThreadName = VknThreadName::GpuThread;
        msg.dataSize = m_uploads[threadBufferIdx];
        msg.srcDataIndex = threadBufferIdx;
        msg.dstDataIndex = threadBufferIdx;
        VknObject::sendMessage(&msg);
    }

    bool VknCycle::downloadData()
    {
        if (!m_basicConfigLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");

        for (VknBuffer *buffer : m_config->getPreComputeBuffers())
            buffer->downloadData();
        for (VknBuffer *buffer : m_config->getGraphicsBuffers())
            buffer->downloadData();
        for (VknBuffer *buffer : m_config->getPostComputeBuffers())
            buffer->downloadData();

        return true;
    }

} // namespace vkn