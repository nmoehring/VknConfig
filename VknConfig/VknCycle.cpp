#include "include/VknCycle.hpp"
#include <algorithm> // For std::min and std::max
#include <cstdint>   // For uint32_t, uint32_t

namespace vkn
{
    void VknCycle::setMaxFramesInFlight(uint32_t maxFramesInFlight)
    {
        MAX_FRAMES_IN_FLIGHT = maxFramesInFlight;
    }

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
        m_transferPool = m_device->getCommandPool(TRANSFER);
        m_physicalDevice = m_device->getPhysicalDevice();

        m_device->createSyncObjects(MAX_FRAMES_IN_FLIGHT); // Use 2 frames in flight for a simple demo

        m_waitSemaphores.push_back(VkSemaphore{});
        m_waitStages.push_back(VkPipelineStageFlags{});

        m_basicConfigLoaded = true;
    }

    void VknCycle::loadGraphicsConfig(VknConfig *config, VknEngine *engine)
    {
        m_swapchain = m_device->getSwapchain(0);
        m_presentPool = m_device->getCommandPool(PRESENT); // ToDo: consider supporting multiple families returned
        m_renderpasses = m_device->getRenderpasses();

        // Determine MAX_FRAMES_IN_FLIGHT based on user setting and available swapchain images.
        uint32_t actualSwapchainImageCount = m_swapchain->getNumImages();
        if (actualSwapchainImageCount == 0)
            throw std::runtime_error("Swapchain has 0 images in VknCycle::loadConfig. Ensure swapchain is created and has images.");

        m_vkSwapchains.push_back(*m_swapchain->getVkSwapchain());

        MAX_FRAMES_IN_FLIGHT = actualSwapchainImageCount;
        m_imagesInFlight.assign(actualSwapchainImageCount, nullptr);

        m_graphicsConfigLoaded = true;
    }

    void VknCycle::loadComputeConfig(VknConfig *config, VknEngine *engine)
    {
        m_computePool = m_device->getCommandPool(COMPUTE); // TODO: consider supporting non-support graphics families
        // TODO: load "compute passes"

        m_computeConfigLoaded = true;
    }

    void VknCycle::wait()
    {
        if (!m_basicConfigLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");
        // 1. Wait for the previous frame to finish
        vkWaitForFences(
            *m_device->getVkDevice(), 1u, &m_device->getFence(m_currentFrame), VK_TRUE, m_defaultTimeout);

        //*device->getVkDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
    }

    bool VknCycle::acquireImage()
    {
        if (!m_graphicsConfigLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");
        // 2. Acquire an image from the swapchain
        m_acquireResult = vkAcquireNextImageKHR(
            *m_device->getVkDevice(), *m_swapchain->getVkSwapchain(), m_defaultTimeout,
            m_device->getImageAvailableSemaphore(m_currentFrame), VK_NULL_HANDLE, &m_imageIndex);

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
        m_imagesInFlight[m_imageIndex] = &m_device->getFence(m_currentFrame);
        return true;
    }

    void VknCycle::beginFrameRecording()
    {
        m_commandBuffersToSubmit.clear();
    }

    void VknCycle::uploadData()
    {
    }

    void VknCycle::recordGraphicsPass(uint_fast8_t renderpassIdx)
    {
        if (!m_graphicsConfigLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");

        VkCommandBuffer commandBuffer = *m_presentPool->getCommandBuffer(m_imageIndex);
        vkResetCommandBuffer(commandBuffer, 0);

        m_beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        m_beginInfo.flags = 0; // Optional: VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        m_resBegin = vkBeginCommandBuffer(commandBuffer, &m_beginInfo);

        m_renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        VknRenderpass *renderpass = getListElement(renderpassIdx, *m_renderpasses);
        VknPipeline *firstPipeline = renderpass->getPipeline(0);
        m_renderPassBeginInfo.renderPass = *renderpass->getVkRenderPass();
        m_renderPassBeginInfo.framebuffer = *renderpass->getFramebuffer(m_imageIndex)->getVkFramebuffer();
        m_renderPassBeginInfo.renderArea.offset = {0, 0};
        m_renderPassBeginInfo.renderArea.extent = m_swapchain->getActualExtent();

        m_renderPassBeginInfo.clearValueCount = 1; // Assuming one color attachment, should be renderpass attachments that have loadOp=VK_ATTACHMENT_LOAD_OP_CLEAR
        m_renderPassBeginInfo.pClearValues = &m_clearColor;

        vkCmdBeginRenderPass(commandBuffer, &m_renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Correctly iterate through pipelines: bind, set state, and draw for each one.
        for (VknPipeline &pipeline : *renderpass->getPipelines())
        {
            // 1. Bind the pipeline
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline.getVkPipeline());

            // 2. Set dynamic states for this pipeline
            VknViewportState *viewportState = pipeline.getViewportState();
            if (viewportState)
            {
                vkCmdSetViewport(commandBuffer, 0, 1, &viewportState->getVkViewport(0));
                vkCmdSetScissor(commandBuffer, 0, 1, &viewportState->getVkScissor(0));
            }

            // 3. Check how to draw for this pipeline
            VknVertexInputState *vertexInputState = pipeline.getVertexInputState();
            if (vertexInputState && (vertexInputState->getNumBindings() > 0 || vertexInputState->getNumAttributes() > 0))
            {
                // This pipeline expects vertex buffers to be bound.
                throw std::runtime_error("Pipeline expects vertex inputs, but VknCycle input binding is not yet implemented.");
            }
            else if (pipeline.getNumHardCodedVertices() > 0)
            {
                // This pipeline uses hard-coded vertices in the shader.
                vkCmdDraw(commandBuffer, pipeline.getNumHardCodedVertices(), 1, 0, 0);
            }
        }

        vkCmdEndRenderPass(commandBuffer);

        m_resEnd = vkEndCommandBuffer(commandBuffer);
        m_commandBuffersToSubmit.push_back(commandBuffer);
    }

    void VknCycle::recordComputePass(uint_fast8_t computePassIdx)
    {
        if (!m_computeConfigLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");
        VkCommandBuffer commandBuffer = *m_computePool->getCommandBuffer(m_currentFrame); // Use m_currentFrame for compute buffers
        vkResetCommandBuffer(commandBuffer, 0);

        m_beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        m_beginInfo.flags = 0;
        vkBeginCommandBuffer(commandBuffer, &m_beginInfo);

        // TODO: Record compute pipeline binding, descriptor sets, and dispatch calls.
        // TODO: Record a pipeline barrier to ensure compute writes are visible to the graphics pass.

        vkEndCommandBuffer(commandBuffer);
        m_commandBuffersToSubmit.push_back(commandBuffer);
    }

    void VknCycle::submitCommandBuffer()
    {
        if (!m_basicConfigLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");
        // 4. Submit the command buffer
        m_submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        m_waitSemaphores[0] = m_device->getImageAvailableSemaphore(m_currentFrame);
        m_waitStages[0] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        m_submitInfo.waitSemaphoreCount = 1;
        m_submitInfo.pWaitSemaphores = m_waitSemaphores.data();
        m_submitInfo.pWaitDstStageMask = m_waitStages.data();

        m_submitInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffersToSubmit.size());
        m_submitInfo.pCommandBuffers = m_commandBuffersToSubmit.data();

        m_signalSemaphores.push_back(m_device->getRenderFinishedSemaphore(m_currentFrame));
        m_submitInfo.signalSemaphoreCount = 1;
        m_submitInfo.pSignalSemaphores = m_signalSemaphores.data();

        vkResetFences(*m_device->getVkDevice(), 1, &m_device->getFence(m_currentFrame)); // Reset the fence before submitting

        // The queue we submit to must match the queue family of the command pool
        // from which the command buffers were allocated.
        // For the simple case where we only record a graphics pass, it comes from the PRESENT pool.
        // A more complex engine would need to track which queue each command buffer belongs to
        // and potentially perform multiple submissions.
        QueueType submissionQueue = m_graphicsConfigLoaded ? PRESENT : COMPUTE;
        m_resSubmit = vkQueueSubmit(*m_device->getQueue(submissionQueue), 1, &m_submitInfo, m_device->getFence(m_currentFrame));
    }

    void VknCycle::downloadData()
    {
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

        m_presentInfo.waitSemaphoreCount = 1;
        m_presentInfo.pWaitSemaphores = m_signalSemaphores.data();

        m_presentInfo.swapchainCount = 1;
        m_presentInfo.pSwapchains = m_vkSwapchains.data();
        m_presentInfo.pImageIndices = &m_imageIndex;
        m_presentInfo.pResults = nullptr; // Optional: to check results per swapchain

        m_presentResult = vkQueuePresentKHR(*m_device->getQueue(QueueType::PRESENT), &m_presentInfo);
        m_signalSemaphores.clear();
        m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT; // Move to the next frame

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
        m_swapchain->recreateSwapchain();

        // Reset m_imagesInFlight for the new swapchain
        m_imagesInFlight.assign(m_swapchain->getNumImages(), nullptr);

        for (auto &renderpass : *m_renderpasses)
        {
            for (auto &pipeline : *renderpass.getPipelines())
                pipeline.getViewportState()->syncWithSwapchain(*m_swapchain, 0, 0);
            renderpass.recreateFramebuffers(*m_swapchain);
        }
    }

} // namespace vkn