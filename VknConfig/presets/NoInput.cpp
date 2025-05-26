#include "../include/VknConfig.hpp"

namespace vkn
{
    bool noInputConfig(VknConfig *config, VknEngine *engine, VknInfos *infos)
    {
        // Shallow Config members
        config->setAppName("NoInputsTest");
        config->setEngineName("MinVknConfig");
        config->createInstance();
        config->createSurface(0);

        // Config=>Devices
        auto *device = config->addDevice(0);
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
        renderpass->createRenderpass();
        // Config=>Device=>Renderpass=>Framebuffer
        std::list<VknFramebuffer> *framebuffers = renderpass->addFramebuffers(*swapchain);
        renderpass->createFramebuffers();

        // Config=>Device=>Renderpass=>Pipeline (subpass creates a pipeline)
        auto *pipeline = renderpass->getPipeline(0);
        // Config=>Device=>Renderpass=>Pipeline=>ShaderStage
        VknShaderStage *vertShader = pipeline->addShaderStage(0, vkn::VKN_VERTEX_STAGE, "simple_shader.vert.spv");
        vertShader->createShaderModule();
        VknShaderStage *fragShader = pipeline->addShaderStage(1, vkn::VKN_FRAGMENT_STAGE, "simple_shader.frag.spv");
        fragShader->createShaderModule();
        // Config=>Device=>Renderpass=>Pipeline=>ViewportState
        vkn::VknViewportState *viewportState = pipeline->getViewportState();
        viewportState->syncWithSwapchain(*swapchain);
        // Create the pipeline
        renderpass->createPipelines();

        // Create command pool, command buffers, and sync objects
        VknCommandPool *commandPool = device->addCommandPool(0);
        uint32_t queueFamilyIdx = device->findGraphicsQueue();
        commandPool->createCommandPool(queueFamilyIdx);
        commandPool->createCommandBuffers(swapchain->getNumImages());
        device->createSyncObjects(2); // Use 2 frames in flight for a simple demo

        // Set shader vertices
        config->setNumHardCodedVertices(3);

        return true;
    }
}