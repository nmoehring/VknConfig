#include "include/VknConfig.hpp"

namespace vkn
{
    VknConfig::VknConfig(VknEngine *engine, VknInfos *infos)
        : m_engine{engine}, m_infos{infos}
    {
    }

    VknDevice *VknConfig::addDevice(uint32_t deviceIdx)
    {
        if (!m_createdInstance)
            throw std::runtime_error("Can't add a device until an instance is created.");
        return &addNewVknObject<VknDevice, VkDevice>(
            deviceIdx, m_devices, m_engine, m_relIdxs, m_absIdxs, m_infos);
    }

    void VknConfig::addWindow(GLFWwindow *window)
    {
        uint32_t count;
        const char **extensions = glfwGetRequiredInstanceExtensions(&count);
        if (count == 0 | extensions == nullptr)
            throw std::runtime_error("Problem retrieving Vulkan extensions required for surface creation. There may be a problem with window creation, or only compute is supported.");
        m_window = window;
        if (m_window == nullptr)
            throw std::runtime_error("Window passed to VknConfig is null. There may be a problem with window creation.");
    }

    void VknConfig::enableExtensions(std::vector<std::string> extensions)
    {
        if (m_createdInstance)
            throw std::runtime_error("Can't enable extensions after instance is already created.");
        for (auto &name : extensions)
            m_instanceExtensions.push_back(name);
    }

    void VknConfig::deviceInfo(uint32_t deviceIdx)
    {
        if (m_createdInstance)
            throw std::runtime_error("Can't create a new deviceInfo instance after an instance is already created.");
        this->fillAppInfo(VK_API_VERSION_1_1, "DeviceInfo", "VknConfig");
        this->createInstance();
        this->getDevice(deviceIdx)->createDevice();
    }

    void VknConfig::testNoInputs()
    {
        if (m_createdInstance)
            throw std::runtime_error("Can't create a test instance after an instance is already created.");

        // Shallow Config members
        std::string appName{"NoInputsTest"};
        std::string engineName{"MinVknConfig"};
        this->fillAppInfo(VK_API_VERSION_1_1, appName, engineName);
        const uint32_t instanceExtensionsSize{2};
        const char *instanceExtensions[instanceExtensionsSize] = {
            // VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
            // VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME};
        const uint32_t layersSize{0};
        const char *layers[]{nullptr};
        this->fillInstanceCreateInfo(
            layers, layersSize, instanceExtensions, instanceExtensionsSize);
        this->createInstance();
        this->createWindowSurface(0);

        // Config=>Devices
        auto *device = this->addDevice(0);
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
        device->fillSwapchainCreateInfos();
        swapchain->createSwapchain();
        auto *imageViews = swapchain->createImages();

        // Config=>Device=>Renderpass
        auto *renderpass = device->addRenderpass(0);
        renderpass->addAttachment(0);
        renderpass->addSubpass(0);
        renderpass->createRenderpass();

        // Config=>Device=>Renderpass=>Framebuffer
        std::list<VknFramebuffer> *framebuffers = renderpass->addFramebuffers(
            swapchain->getImageViewStartIdx(), swapchain->getNumImages());
        for (uint32_t i = 0; i < swapchain->getNumImages(); ++i)
        {
            VknFramebuffer *framebuffer = getListElement(i, *framebuffers);
            framebuffer->addSwapchainVkImage(swapchain->getImageViewStartIdx() + i);
            framebuffer->addAttachments();
        }
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

        // Config=>Device=>Pipeline=>PipelineLayout
        auto *layout = pipeline->getLayout();
        layout->createPipelineLayout();

        renderpass->createPipelines();
    }

    void VknConfig::fillAppInfo(uint32_t apiVersion, std::string appName,
                                std::string engineName, VkApplicationInfo *pNext,
                                uint32_t appVersion, uint32_t engineVersion)
    {
        if (m_filledAppInfo)
            throw std::runtime_error("Already filled app info.");
        m_infos->fillAppName(appName);
        m_infos->fillEngineName(engineName);
        m_infos->fillAppInfo(apiVersion, appVersion, engineVersion);
    }

    void VknConfig::fillInstanceCreateInfo(const char *const *enabledLayerNames,
                                           uint32_t enabledLayerNamesSize,
                                           const char *const *enabledExtensionNames,
                                           uint32_t enabledExtensionNamesSize,
                                           VkInstanceCreateFlags flags)
    {
        if (m_filledInstanceCreateInfo)
            throw std::runtime_error("Instance create info already filled.");
        m_infos->fillEnabledLayerNames(enabledLayerNames, enabledLayerNamesSize);
        m_infos->fillInstanceExtensionNames(enabledExtensionNames, enabledExtensionNamesSize);
        m_infos->fillInstanceCreateInfo(flags);
        m_filledInstanceCreateInfo = true;
    }

    VknResult VknConfig::createInstance()
    {
        if (m_createdInstance)
            throw std::runtime_error("Instance already created.");
        if (!m_filledInstanceCreateInfo)
            throw std::runtime_error("Creating instance without filling instance create info.");
        m_engine->push_back(VkInstance{});
        VknResult res{
            vkCreateInstance(
                m_infos->getInstanceCreateInfo(), VK_NULL_HANDLE,
                &m_engine->getObject<VkInstance>(0)),
            "Create instance."};

        m_createdInstance = true;
        return res;
    }

    VknDevice *VknConfig::getDevice(uint32_t deviceIdx)
    {
        return getListElement(deviceIdx, m_devices);
    }

    void VknConfig::createWindowSurface(uint32_t surfaceIdx)
    {
        if (!m_createdInstance)
            throw std::runtime_error("Didn't create instance before trying to create window surface.");
        if (m_window == nullptr)
            throw std::runtime_error("Trying to create a window surface but did not pass a window object to Config.");

        m_engine->push_back(VkSurfaceKHR{});
        VknResult res{"Create window surface."};
        res = glfwCreateWindowSurface(
            m_engine->getObject<VkInstance>(0), m_window, nullptr,
            &m_engine->getObject<VkSurfaceKHR>(surfaceIdx));
    }
}