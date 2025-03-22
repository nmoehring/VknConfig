#include "VknConfig.hpp"

namespace vkn
{
    VknConfig::VknConfig(VknEngine *engine, VknInfos *infos, GLFWwindow *window)
        : m_engine{engine}, m_infos{infos}, m_window(window)
    {
    }

    void VknConfig::addDevice(uint32_t deviceIdx)
    {
        if (!m_state.createdInstance)
            throw std::runtime_error("Can't add a device until an instance is created.");
        if (deviceIdx != m_state.numDevices)
            throw std::runtime_error("Device index should equal the current numDevices.");

        m_relIdxs.deviceIdx = deviceIdx;
        m_absIdxs.deviceIdx =
            m_devices.push_back(VknDevice{&m_engine, m_relIdxs, m_absIdxs, &m_infos});
        ++m_state.numDevices;
    }

    void VknConfig::enableExtensions(std::vector<std::string> extensions)
    {
        if (m_state.createdInstance)
            throw std::runtime_error("Can't enable extensions after instance is already created.");
        for (auto &name : extensions)
            m_instanceExtensions.push_back(name);
    }

    void VknConfig::deviceInfo(uint32_t deviceIdx)
    {
        if (m_state.createdInstance)
            throw std::runtime_error("Can't create a new deviceInfo instance after an instance is already created.");
        this->fillAppInfo(VK_API_VERSION_1_1, "DeviceInfo", "VknConfig");
        this->createInstance();
        this->getDevice(deviceIdx)->createDevice();
    }

    void VknConfig::testNoInputs()
    {
        if (m_state.createdInstance)
            throw std::runtime_error("Can't create a test instance after an instance is already created.");
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

        // VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        // VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        // VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME};

        auto device = this->getDevice(0);
        auto physDev = device->getPhysicalDevice();
        const uint32_t numExtensions{1};
        const char *deviceExtensions[numExtensions] = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        device->addExtensions(deviceExtensions, numExtensions);
        physDev->selectPhysicalDevice();
        device->requestQueueFamilyProperties();
        device->selectQueues(false);
        device->fillDeviceQueuePrioritiesDefault();
        device->createDevice();
        device->addRenderpass(0);

        auto renderpass = device->getRenderpass(0);
        renderpass->createAttachment(0);
        renderpass->createSubpass(0);
        renderpass->createRenderpass();

        auto pipeline = renderpass->getPipeline(0);
        pipeline->addShaderStage(0, vkn::VKN_VERTEX_STAGE, "simple_shader.vert.spv");
        pipeline->addShaderStage(1, vkn::VKN_FRAGMENT_STAGE, "simple_shader.frag.spv");
        auto vertShader = pipeline->getShaderStage(0);
        auto fragShader = pipeline->getShaderStage(1);
        vertShader->createShaderStage();
        fragShader->createShaderStage();
        vkn::VknVertexInputState *vertexInputState = pipeline->getVertexInputState();
        // vertexInputState->fillVertexAttributeDescription();
        // vertexInputState->fillVertexBindingDescription();
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
        /*auto inputAssemblyStateCreateInfos{infos->fillInputAssemblyStateCreateInfo()};
        auto tessellationStateCreateInfos{infos->fillTessellationStateCreateInfo()};
        auto viewportStateCreateInfos{infos->fillViewportStateCreateInfo()};
        auto rasterizationStateCreateInfos{infos->fillRasterizationStateCreateInfo()};
        auto multisampleStateCreateInfos{infos->fillMultisampleStateCreateInfo()};
        auto depthStencilStateCreateInfos{infos->fillDepthStencilStateCreateInfo()};
        auto colorBlendStateCreateInfos{infos->fillColorBlendStateCreateInfo()}; */
        pipeline->fillPipelineLayoutCreateInfo();
        pipeline->createLayout();
        /*, layoutCreateInfo, renderpass, subpass, basepipelinehandle,
            basepipelineidx, flags, vertexInputStateCreateInfo, inputAssemblyStateCreateInfo,
            tessellationStateCreateInfo, viewportStateCreateInfo, rasterizationStateCreateInfo,
            multisampleStateCreateInfo, depthStencilStateCreateInfo, colorBlendStateCreateInfo);
        */

        this->createWindowSurface();
        device->addSwapchain(0, &m_surface, 1, 800, 600);
        device->fillSwapchainCreateInfos();
        VknSwapchain *swapchain{device->getSwapchain(0)};
        swapchain->createSwapchain();

        // auto layoutCreateInfo{infos->fillPipelineLayoutCreateInfo()};
        // auto cacheCreateInfos{infos->fillPipelineCacheCreateInfo()};

        renderpass->createPipelines();
    }

    void VknConfig::fillAppInfo(uint32_t apiVersion, std::string appName,
                                std::string engineName, VkApplicationInfo *pNext,
                                uint32_t appVersion, uint32_t engineVersion)
    {
        if (m_state.filledAppInfo)
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
        if (m_state.filledInstanceCreateInfo)
            throw std::runtime_error("Instance create info already filled.");
        m_infos->fillEnabledLayerNames(enabledLayerNames, enabledLayerNamesSize);
        m_infos->fillInstanceExtensionNames(enabledExtensionNames, enabledExtensionNamesSize);
        m_infos->fillInstanceCreateInfo(flags);
        m_state.filledInstanceCreateInfo = true;
    }

    VknResult VknConfig::createInstance()
    {
        if (m_state.createdInstance)
            throw std::runtime_error("Instance already created.");
        if (!m_state.filledInstanceCreateInfo)
            throw std::runtime_error("Creating instance without filling instance create info.");
        VknResult res{vkCreateInstance(m_infos->getInstanceCreateInfo(), VK_NULL_HANDLE, &m_engine->instance),
                      "Create instance."};

        m_state.createdInstance = true;
        return res;
    }

    VknDevice *VknConfig::getDevice(uint32_t deviceIdx)
    {
        return &m_devices[deviceIdx];
    }

    void VknConfig::createWindowSurface(uint32_t surfaceIdx)
    {
        if (!m_state.createdInstance)
            throw std::runtime_error("Didn't create instance before trying to create window surface.");
        if (m_window == nullptr)
            throw std::runtime_error("Trying to create a window surface but did not pass a window object to Config.");
        VknResult res("Create window surface.");
        res = glfwCreateWindowSurface(m_engine->instance, m_window, nullptr,
                                      &m_engine->getElement<VkSurfaceKHR>(surfaceIdx))
    }
}