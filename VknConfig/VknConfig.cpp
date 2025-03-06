#include "VknConfig.hpp"

namespace vkn
{
    VknConfig::VknConfig()
        : m_resultArchive(VknResultArchive{}), m_infos(VknInfos{}),
          m_window(nullptr)
    {
        this->addDevice(0);
    }

    VknConfig::VknConfig(GLFWwindow *window)
        : m_resultArchive(VknResultArchive{}), m_infos(VknInfos{}),
          m_window(window)
    {
        this->addDevice(0);
    }

    VknConfig::~VknConfig()
    {
        if (!m_destroyed)
            this->destroy();
    }

    void VknConfig::destroy()
    {
        if (m_destroyed)
            throw std::runtime_error("Config object already destroyed.");
        for (auto &device : m_devices)
            device.destroy();
        if (m_instanceCreated)
            vkDestroyInstance(m_instance, nullptr);
        m_destroyed = true;
        std::cout << "VknConfig DESTROYED." << std::endl;
    }

    void VknConfig::addDevice(uint32_t deviceIdx)
    {
        if (deviceIdx != m_numDevices)
            throw std::runtime_error("Device index should equal the current numDevices.");
        m_devices.emplace_back(m_numDevices++, &m_infos, &m_resultArchive, &m_instance, &m_instanceCreated);
    }

    void VknConfig::enableExtensions(std::vector<std::string> extensions)
    {
        for (auto &name : extensions)
            m_instanceExtensions.push_back(name);
    }

    void VknConfig::deviceInfo(uint32_t deviceIdx)
    {
        this->fillAppInfo(VK_API_VERSION_1_1, "DeviceInfo", "VknConfig");
        this->createInstance();
        this->getDevice(deviceIdx)->createDevice();
    }

    void VknConfig::testNoInputs()
    {
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
        device->addSwapchain(0, m_surface, 1, 800, 600);
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
        m_infos.fillAppName(appName);
        m_infos.fillEngineName(engineName);
        m_infos.fillAppInfo(apiVersion, appVersion, engineVersion);
    }

    void VknConfig::fillInstanceCreateInfo(const char *const *enabledLayerNames,
                                           uint32_t enabledLayerNamesSize,
                                           const char *const *enabledExtensionNames,
                                           uint32_t enabledExtensionNamesSize,
                                           VkInstanceCreateFlags flags)
    {
        if (m_filledInstanceCreateInfo)
            throw std::runtime_error("Instance create info already filled.");
        m_infos.fillEnabledLayerNames(enabledLayerNames, enabledLayerNamesSize);
        m_infos.fillInstanceExtensionNames(enabledExtensionNames, enabledExtensionNamesSize);
        m_infos.fillInstanceCreateInfo(flags);
        m_filledInstanceCreateInfo = true;
    }

    VknResult VknConfig::createInstance()
    {
        if (m_instanceCreated)
            throw std::runtime_error("Instance already created.");
        if (!m_filledInstanceCreateInfo)
            throw std::runtime_error("Creating instance without filling instance create info.");
        VknResult res{vkCreateInstance(m_infos.getInstanceCreateInfo(), VK_NULL_HANDLE, &m_instance),
                      "Create instance."};
        if (!(res.isSuccess()))
            throw std::runtime_error(res.toErr("Error creating instance."));
        m_resultArchive.store(res);

        m_instanceCreated = true;
        return res;
    }

    VknRenderpass *VknConfig::getRenderpass(uint32_t deviceIdx, uint32_t renderpassIdx)
    {
        VknRenderpass *renderpass{this->getDevice(deviceIdx)->getRenderpass(renderpassIdx)};
        if (!(renderpass->getVkRenderPassCreated()))
            throw std::runtime_error("Renderpass not created before getting renderpass.");

        return renderpass;
    }

    void VknConfig::archiveResult(VknResult res)
    {
        m_resultArchive.store(res);
    }

    VknDevice *VknConfig::getDevice(uint32_t deviceIdx)
    {
        if (deviceIdx >= m_numDevices)
            throw std::runtime_error("Device index out of range.");
        std::list<VknDevice>::iterator it = m_devices.begin();
        std::advance(it, deviceIdx);
        return &(*it);
    }

    void VknConfig::createWindowSurface()
    {
        if (!m_instanceCreated)
            throw std::runtime_error("Didn't create instance before trying to create window surface.");
        if (m_window == nullptr)
            throw std::runtime_error("Trying to create a window surface but did not pass a window object to Config.");
        if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
            throw std::runtime_error("Failed to create window surface!");
    }
}