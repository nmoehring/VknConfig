#include "../include/VknConfig.hpp"

namespace vkn
{
    bool noInputConfig(VknConfig &config)
    {
        // Shallow Config members
        config.setAppName("NoInputsTest");
        config.setEngineName("MinVknConfig");
        config.addWindow();
        config.createInstance();
        config.createSurface(0);

        // Config=>Devices
        auto *device = config.addDevice(0);
        device->addExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        // Config->Device->PhysicalDevice
        device->createDevice();

        // Config=>Device=>Swapchain
        auto *swapchain = device->addSwapchain(0);
        swapchain->createSwapchain();

        // Config=>Device=>Renderpass
        auto *renderpass = device->addRenderpass(0);
        renderpass->addAttachment(0);
        renderpass->addAttachmentRef(0, 0);
        renderpass->addSubpass(0);
        // Dependency to transition layout for rendering (clear/draw)
        renderpass->addSubpassDependency(
            0,                                             // dependencyIdx
            VK_SUBPASS_EXTERNAL,                           // srcSubpass
            0,                                             // dstSubpass (our only subpass)
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // srcStageMask (wait for previous color ops or top of pipe)
            0,                                             // srcAccessMask (no access needed if coming from UNDEFINED)
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask (transition for color output)
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT           // dstAccessMask (allow writing to color attachment)
        );
        // Dependency to transition layout for presentation
        renderpass->addSubpassDependency(
            1,                                             // dependencyIdx
            0,                                             // srcSubpass (our only subpass)
            VK_SUBPASS_EXTERNAL,                           // dstSubpass
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // srcStageMask (after we're done writing)
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,          // srcAccessMask (ensure writes are finished)
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,          // dstStageMask (presentation engine will pick it up)
            0                                              // dstAccessMask (presentation engine handles its own access)
        );
        renderpass->createRenderpass();
        // Config=>Device=>Renderpass=>Framebuffer
        std::list<VknFramebuffer> *framebuffers = renderpass->addFramebuffers(*swapchain);
        renderpass->createFramebuffers(*swapchain);

        // Config=>Device=>Renderpass=>Pipeline (subpass creates a pipeline)
        auto *pipeline = renderpass->getPipeline(0);
        pipeline->getRasterizationState()->setCullMode(VK_CULL_MODE_NONE); // Temporarily disable culling
        // Config=>Device=>Renderpass=>Pipeline=>ShaderStage
        VknShaderStage *vertShader = pipeline->addShaderStage(0, vkn::VKN_VERTEX_STAGE, "triangle.vert.spv");
        vertShader->createShaderModule();
        VknShaderStage *fragShader = pipeline->addShaderStage(1, vkn::VKN_FRAGMENT_STAGE, "triangle.frag.spv");
        fragShader->createShaderModule();
        // Config=>Device=>Renderpass=>Pipeline=>ViewportState
        vkn::VknViewportState *viewportState = pipeline->getViewportState();
        viewportState->syncWithSwapchain(*swapchain, 0, 0);
        //  Create the pipeline
        renderpass->createPipelines();

        // Create command pool, command buffers, and sync objects
        device->addCommandPools();
        VknCommandPool *commandPool = device->getCommandPool(QueueType::PRESENT);
        commandPool->createCommandBuffers(swapchain->getNumImages());

        // Set shader vertices
        pipeline->setNumHardCodedVertices(3);

        // Return true - ready to render
        return true;
    }
}