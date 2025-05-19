#include "../include/VknConfig.hpp"

using namespace vkn;

VknConfig noInputConfig(VknConfig *config, VknEngine *engine, VknInfos *infos)
{
    // Shallow Config members
    std::string appName{"NoInputsTest"};
    std::string engineName{"MinVknConfig"};
    config->fillAppInfo(VK_API_VERSION_1_1, appName, engineName);
    config->fillInstanceCreateInfo();

    config->createInstance();
    // Setup debug messenger after instance creation
    config->createSurface(0);

    // Config=>Devices
    auto *device = config->addDevice(0);
    VknPhysicalDevice *physDev = device->getPhysicalDevice();
    const uint32_t numExtensions{1};
    const char *deviceExtensions[numExtensions] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    device->addExtensions(deviceExtensions, numExtensions);
    // Config->Device->PhysicalDevice
    physDev->selectPhysicalDevice();
    physDev->requestQueueFamilyProperties();
    physDev->selectQueues(false);
    physDev->fillDeviceQueuePrioritiesDefault();
    device->createDevice();

    // Config=>Device=>Swapchain
    auto *swapchain = device->addSwapchain(0);
    swapchain->setSurface(0);
    swapchain->setImageCount(1);
    swapchain->createSwapchain();
    auto *swapchainImageViews = swapchain->createImages();

    // Config=>Device=>Renderpass
    auto *renderpass = device->addRenderpass(0);
    renderpass->addAttachment(0);
    renderpass->addAttachmentRef(0, 0);
    renderpass->addSubpass(0);
    renderpass->createRenderpass();

    // Config=>Device=>Renderpass=>Framebuffer
    std::list<VknFramebuffer> *framebuffers = renderpass->addFramebuffers(swapchainImageViews);
    renderpass->createFramebuffers();

    // Config=>Device=>Pipeline (subpass creates a pipeline)
    auto *pipeline = renderpass->getPipeline(0);

    // Config=>Device=>Pipeline=>ShaderStage
    VknShaderStage *vertShader = pipeline->addShaderStage(0, vkn::VKN_VERTEX_STAGE, "simple_shader.vert.spv");
    vertShader->createShaderModule();
    vertShader->fillShaderStageCreateInfo();
    VknShaderStage *fragShader = pipeline->addShaderStage(1, vkn::VKN_FRAGMENT_STAGE, "simple_shader.frag.spv");
    fragShader->createShaderModule();
    fragShader->fillShaderStageCreateInfo();

    // Config=>Device=>Pipeline=>[Various Pipeline States]
    vkn::VknVertexInputState *vertexInputState = pipeline->getVertexInputState();
    vertexInputState->fillVertexInputStateCreateInfo();
    vkn::VknInputAssemblyState *inputAssemblyState = pipeline->getInputAssemblyState();
    inputAssemblyState->fillInputAssemblyStateCreateInfo();
    vkn::VknMultisampleState *multisampleState = pipeline->getMultisampleState();
    multisampleState->fillMultisampleStateCreateInfo();
    vkn::VknRasterizationState *rasterizationState = pipeline->getRasterizationState();
    rasterizationState->fillRasterizationStateCreateInfo();
    vkn::VknViewportState *viewportState = pipeline->getViewportState();
    viewportState->addViewport();
    viewportState->addScissor();
    viewportState->fillViewportStateCreateInfo();
    vkn::VknColorBlendState *colorBlendState = pipeline->getColorBlendState();
    colorBlendState->fillColorBlendStateCreateInfo();

    // Config=>Device=>Pipeline=>PipelineLayout
    auto *layout = pipeline->getLayout();
    layout->createPipelineLayout();

    renderpass->createPipelines();

    // Create command pool, command buffers, and sync objects
    VknCommandPool *commandPool = device->addCommandPool(0);
    uint32_t queueFamilyIdx = device->findGraphicsQueue();
    commandPool->createCommandPool(queueFamilyIdx);
    commandPool->createCommandBuffers(swapchain->getNumImages());
    device->createSyncObjects(2); // Use 2 frames in flight for a simple demo
}