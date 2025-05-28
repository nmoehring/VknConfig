#include "include/VknCycle.hpp"

namespace vkn
{
    void VknCycle::setMaxFramesInFlight(uint_fast32_t maxFramesInFlight)
    {
        MAX_FRAMES_IN_FLIGHT = maxFramesInFlight;
    }

    void VknCycle::setClearColor(float r, float g, float b, float a)
    {
        if (r < 0.0f || r > 1.0f || g < 0.0f || g > 1.0f || b < 0.0f || b > 1.0f || a < 0.0f || a > 1.0f)
            throw std::runtime_error("Invalid clear color value.");
        m_clearColor = {{{r, g, b, a}}};
    }

    bool VknCycle::isWindowMinimized()
    {
        // Prioritize direct window check if available
        if (m_config && m_config->hasGLFWConfig() && m_config->getGLFWwindow())
        {
            m_width = 0;
            m_height = 0;
            glfwGetFramebufferSize(m_config->getGLFWwindow(), &m_width, &m_height);
            if (m_width == 0 || m_height == 0)
                return true;
            // Optionally, also check iconified state, though framebuffer size is usually sufficient for Vulkan
            // if (glfwGetWindowAttrib(m_config->getGLFWwindow(), GLFW_ICONIFIED)) {
            //     return true;
            // }
        }
        else if (m_swapchain) // Fallback to swapchain extent if direct window check not possible
        {
            m_extent = m_swapchain->getActualExtent();
            if (m_extent.width == 0 || m_extent.height == 0)
                return true;
        }
        // If no config or swapchain, or if checks pass, assume not minimized in a way that blocks rendering.
        return false;
    }

    void VknCycle::loadConfig(VknConfig *config, VknEngine *engine)
    {
        m_config = config;
        m_engine = engine;
        m_device = m_config->getDevice(0);
        m_swapchain = m_device->getSwapchain(0);
        m_renderpass = m_device->getRenderpass(0);
        m_pipeline = m_renderpass->getPipeline(0);
        m_commandPool = m_device->getCommandPool(0);
        m_physicalDevice = m_device->getPhysicalDevice();
        if (MAX_FRAMES_IN_FLIGHT == 0)
            MAX_FRAMES_IN_FLIGHT = /*m_imagesInFlight.size() + */ 1u;
        m_device->createSyncObjects(MAX_FRAMES_IN_FLIGHT); // Use 2 frames in flight for a simple demo
        m_imagesInFlight.assign(m_swapchain->getNumImages(), nullptr);
        m_waitSemaphores.push_back(VkSemaphore{});
        m_waitStages.push_back(VkPipelineStageFlags{});
        m_swapchains.push_back(VkSwapchainKHR{});

        // Determine if the pipeline expects vertex inputs
        m_vertexInputState = m_pipeline->getVertexInputState();
        if (m_vertexInputState && (m_vertexInputState->getNumBindings() > 0 || m_vertexInputState->getNumAttributes() > 0))
            m_pipelineExpectsVertexInputs = true;
        else
            m_pipelineExpectsVertexInputs = false;
        m_devRelIdxs = m_device->getRelIdxs();

        m_configLoaded = true;
    }

    void VknCycle::wait()
    {
        if (!m_configLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");
        // 1. Wait for the previous frame to finish
        vkWaitForFences(
            *m_device->getVkDevice(), 1u, &m_device->getFence(m_currentFrame), VK_TRUE, m_defaultTimeout);

        //*device->getVkDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
    }

    bool VknCycle::acquireImage()
    {
        if (!m_configLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");
        // 2. Acquire an image from the swapchain
        m_acquireResult = vkAcquireNextImageKHR(
            *m_device->getVkDevice(), *m_swapchain->getVkSwapchain(), m_defaultTimeout,
            m_device->getImageAvailableSemaphore(m_currentFrame), VK_NULL_HANDLE, &m_imageIndex);

        if (m_acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
            return this->recoverFromSwapchainError(); // Skip rendering this frame
        else if (m_acquireResult == VK_SUBOPTIMAL_KHR)
            if (this->isWindowMinimized())
                return false;
            else if (m_acquireResult != VK_SUCCESS && m_acquireResult != VK_SUBOPTIMAL_KHR)
                throw std::runtime_error("Failed to acquire swapchain image!");

        // Check if a previous frame is using this image
        if (m_imagesInFlight[m_imageIndex] != nullptr)
            vkWaitForFences(*m_device->getVkDevice(), 1, m_imagesInFlight[m_imageIndex], VK_TRUE, m_defaultTimeout);

        // Mark the image as being in use by this frame
        m_imagesInFlight[m_imageIndex] = &m_device->getFence(m_currentFrame);
        return true;
    }

    void VknCycle::recordCommandBuffer()
    {
        if (!m_configLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");
        // 3. Record the command buffer
        m_currentCommandBuffer = m_commandPool->getCommandBuffer(m_imageIndex);
        vkResetCommandBuffer(*m_currentCommandBuffer, 0); // Optional: VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT allows this

        m_beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        m_beginInfo.flags = 0; // Optional: VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT

        m_resBegin = vkBeginCommandBuffer(*m_currentCommandBuffer, &m_beginInfo);

        m_renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        m_renderPassBeginInfo.renderPass = *m_renderpass->getVkRenderPass();
        m_renderPassBeginInfo.framebuffer = *m_renderpass->getFramebuffer(m_imageIndex)->getVkFramebuffer(); // Need getVkFramebuffer on VknFramebuffer
        m_renderPassBeginInfo.renderArea.offset = {0, 0};
        m_renderPassBeginInfo.renderArea.extent = m_swapchain->getActualExtent(); // Use actual swapchain extent

        m_renderPassBeginInfo.clearValueCount = 1; // Assuming one color attachment, should be renderpass attachments that have loadOp=VK_ATTACHMENT_LOAD_OP_CLEAR
        m_renderPassBeginInfo.pClearValues = &m_clearColor;

        vkCmdBeginRenderPass(*m_currentCommandBuffer, &m_renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdSetViewport(*m_currentCommandBuffer, 0, 1, &m_pipeline->getViewportState()->getVkViewport(0));
        vkCmdSetScissor(*m_currentCommandBuffer, 0, 1, &m_pipeline->getViewportState()->getVkScissor(0));
        vkCmdBindPipeline(*m_currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *m_pipeline->getVkPipeline());

        if (m_pipelineExpectsVertexInputs)
        {
            // TODO: This is where you would bind your vertex buffers
            // vkCmdBindVertexBuffers(*m_currentCommandBuffer, ...);
            // And then draw based on the count from those buffers or an index buffer
            // vkCmdDraw(*m_currentCommandBuffer, vertexCountFromBuffer, 1, 0, 0);
            throw std::runtime_error("Pipeline expects vertex inputs, but VknCycle input binding is not yet implemented.");
        }
        else // No vertex inputs expected, draw hardcoded vertices (e.g., a triangle)
            vkCmdDraw(*m_currentCommandBuffer, m_config->getNumHardCodedVertices(), 1, 0, 0);

        vkCmdEndRenderPass(*m_currentCommandBuffer);

        m_resEnd = vkEndCommandBuffer(*m_currentCommandBuffer);
    }

    void VknCycle::submitCommandBuffer()
    {
        if (!m_configLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");
        // 4. Submit the command buffer
        m_submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        m_waitSemaphores[0] = m_device->getImageAvailableSemaphore(m_currentFrame);
        m_waitStages[0] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        m_submitInfo.waitSemaphoreCount = 1;
        m_submitInfo.pWaitSemaphores = m_waitSemaphores.data();
        m_submitInfo.pWaitDstStageMask = m_waitStages.data();

        m_submitInfo.commandBufferCount = 1;
        m_submitInfo.pCommandBuffers = m_currentCommandBuffer;

        m_signalSemaphores.push_back(m_device->getRenderFinishedSemaphore(m_currentFrame));
        m_submitInfo.signalSemaphoreCount = 1;
        m_submitInfo.pSignalSemaphores = m_signalSemaphores.data();

        vkResetFences(*m_device->getVkDevice(), 1, &m_device->getFence(m_currentFrame)); // Reset the fence before submitting
        m_resSubmit = vkQueueSubmit(*m_device->getGraphicsQueue(), 1, &m_submitInfo, m_device->getFence(m_currentFrame));
    }

    bool VknCycle::presentImage()
    {
        if (!m_configLoaded)
            throw std::runtime_error("Can't execute VknCycle steps before a config is loaded.");
        // 5. Present the image
        m_presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        m_presentInfo.waitSemaphoreCount = 1;
        m_presentInfo.pWaitSemaphores = m_signalSemaphores.data();

        m_swapchains[0] = *m_swapchain->getVkSwapchain();
        m_presentInfo.swapchainCount = 1;
        m_presentInfo.pSwapchains = m_swapchains.data();
        m_presentInfo.pImageIndices = &m_imageIndex;
        m_presentInfo.pResults = nullptr; // Optional: to check results per swapchain

        m_presentResult = vkQueuePresentKHR(*m_device->getGraphicsQueue(), &m_presentInfo);
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
        if (this->isWindowMinimized())
            return false;

        vkDeviceWaitIdle(*m_device->getVkDevice());

        // Critical check: Re-query surface capabilities *directly* before attempting swapchain recreation.
        // This ensures we have the absolute latest extent information.

        if (!m_swapchain->getSurfaceIdx().has_value())
            throw std::runtime_error("Swapchain does not have a valid surface index for recovery.");

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            *(m_physicalDevice->getVkPhysicalDevice()),
            m_engine->getObject<VkSurfaceKHR>(m_swapchain->getSurfaceIdx().value()),
            &m_capabilities);

        if (m_capabilities.currentExtent.width == 0 || m_capabilities.currentExtent.height == 0)
            // Window became (or still is) minimized just before recreation attempt.
            return false; // Signal app to wait for events.

        // If we've reached here, the window is not minimized, and capabilities show non-zero extent.
        m_swapchain->recreateSwapchain();
        m_pipeline->getViewportState()->syncWithSwapchain(*m_swapchain, 0, 0);
        m_renderpass->recreateFramebuffers(*m_swapchain);
        std::cerr << "Recovered from swapchain error." << std::endl;
        return true;
    }

} // namespace vkn