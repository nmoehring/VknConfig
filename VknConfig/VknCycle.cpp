#include "include/VknCycle.hpp"

namespace vkn
{

    void VknCycle::loadConfig(VknConfig *config, VknEngine *engine)
    {
        m_config = config;
        m_engine = engine;
        m_device = m_config->getDevice(0);
        m_swapchain = m_device->getSwapchain(0);
        m_renderpass = m_device->getRenderpass(0);
        m_pipeline = m_renderpass->getPipeline(0);
        m_commandPool = m_device->getCommandPool(0);
        for (uint32_t i = 0; i < m_swapchain->getNumImages(); ++i)
            m_imagesInFlight.push_back(nullptr);

        // Determine if the pipeline expects vertex inputs
        VknVertexInputState *vertexInputState = m_pipeline->getVertexInputState();
        if (vertexInputState && (vertexInputState->getNumBindings() > 0 || vertexInputState->getNumAttributes() > 0))
        {
            m_pipelineExpectsVertexInputs = true;
        }
        else
        {
            m_pipelineExpectsVertexInputs = false;
        }
        m_devRelIdxs = m_device->getRelIdxs();
    }

    void VknCycle::wait()
    {
        // 1. Wait for the previous frame to finish
        vkWaitForFences(
            *m_device->getVkDevice(), 1u, &m_device->getFence(m_currentFrame), VK_TRUE, uint64_t(0) - 1u);

        //*device->getVkDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
    }

    void VknCycle::acquireImage()
    {
        // 2. Acquire an image from the swapchain
        VkResult acquireResult = vkAcquireNextImageKHR(
            *m_device->getVkDevice(), *m_swapchain->getVkSwapchain(), uint64_t(0) - 1u,
            m_device->getImageAvailableSemaphore(m_currentFrame), VK_NULL_HANDLE, &m_imageIndex);

        if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
        {
            // Handle swapchain recreation (more complex, skip for this minimal demo)
            std::cerr << "Swapchain out of date, recreation needed (not implemented in demo)." << std::endl;
            return; // Skip rendering this frame
        }
        else if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed to acquire swapchain image!");
        }

        // Check if a previous frame is using this image
        if (m_imagesInFlight[m_imageIndex] != nullptr)
        {
            vkWaitForFences(*m_device->getVkDevice(), 1, m_imagesInFlight[m_imageIndex], VK_TRUE, uint64_t(0) - 1u);
        }
        // Mark the image as being in use by this frame
        m_imagesInFlight[m_imageIndex] = &m_device->getFence(m_currentFrame);
    }

    void VknCycle::recordCommandBuffer()
    {
        // 3. Record the command buffer
        m_currentCommandBuffer = m_commandPool->getCommandBuffer(m_imageIndex);
        vkResetCommandBuffer(*m_currentCommandBuffer, 0); // Optional: VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT allows this

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional: VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT

        VknResult resBegin{vkBeginCommandBuffer(*m_currentCommandBuffer, &beginInfo), "Begin command buffer"};

        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = *m_renderpass->getVkRenderPass();
        // Need to get the correct framebuffer for this image index
        // Assuming framebuffers are created in the same order as swapchain images
        renderPassBeginInfo.framebuffer = *m_renderpass->getFramebuffer(m_imageIndex)->getVkFramebuffer(); // Need getVkFramebuffer on VknFramebuffer
        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = m_swapchain->getActualExtent(); // Use actual swapchain extent

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}}; // Black clear color
        renderPassBeginInfo.clearValueCount = 1;                // Assuming one color attachment
        renderPassBeginInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(*m_currentCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

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

        VknResult resEnd{vkEndCommandBuffer(*m_currentCommandBuffer), "End command buffer"};
    }

    void VknCycle::submitCommandBuffer()
    {
        // 4. Submit the command buffer
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {m_device->getImageAvailableSemaphore(m_currentFrame)};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = m_currentCommandBuffer;

        m_signalSemaphores.push_back(m_device->getRenderFinishedSemaphore(m_currentFrame));
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = m_signalSemaphores.data();

        vkResetFences(*m_device->getVkDevice(), 1, &m_device->getFence(m_currentFrame)); // Reset the fence before submitting
        VknResult resSubmit{
            vkQueueSubmit(*m_device->getGraphicsQueue(), 1, &submitInfo, m_device->getFence(m_currentFrame)),
            "Submit command buffer"};
    }

    void VknCycle::presentImage()
    {
        // 5. Present the image
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = m_signalSemaphores.data();

        VkSwapchainKHR swapchains[] = {*m_swapchain->getVkSwapchain()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &m_imageIndex;
        presentInfo.pResults = nullptr; // Optional: to check results per swapchain

        VkResult presentResult = vkQueuePresentKHR(*m_device->getGraphicsQueue(), &presentInfo);

        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR)
        {
            // Handle swapchain recreation (not implemented in demo)
            std::cerr << "Swapchain out of date or suboptimal, recreation needed (not implemented in demo)." << std::endl;
        }
        else if (presentResult != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to present swapchain image!");
        }

        m_signalSemaphores.clear();

        // Move to the next frame
        m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

} // namespace vkn