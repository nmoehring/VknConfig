#include "include/VknConfig.hpp"

namespace vkn
{
    // Debug callback function
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData)
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE; // Return VK_FALSE to indicate that the Vulkan call should not be aborted.
    }

    // Helper function to set up the debug messenger create info
    VkDebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional user data
        return createInfo;
    }

    // Helper functions to get extension function pointers
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        return func ? func(instance, pCreateInfo, pAllocator, pDebugMessenger) : VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    VknConfig::VknConfig(VknEngine *engine, VknInfos *infos)
        : m_engine{engine}, m_infos{infos}
    {
    }

    VknDevice *VknConfig::addDevice(uint32_t deviceIdx)
    {
        if (!m_createdInstance)
            throw std::runtime_error("Can't add a device until an instance is created.");
        return &m_engine->addNewVknObject<VknDevice, VkDevice, VkInstance>(
            deviceIdx, m_devices, m_relIdxs, m_absIdxs, m_infos);
    }

    void VknConfig::enableExtensions(VknVector<std::string> extensions)
    {
        if (m_createdInstance)
            throw std::runtime_error("Can't enable extensions after instance is already created.");
        for (auto &name : extensions)
            m_instanceExtensions.append(name);
    }

    void VknConfig::deviceInfo(uint32_t deviceIdx)
    {
        if (m_createdInstance)
            throw std::runtime_error("Can't create a new deviceInfo instance after an instance is already created.");
        this->fillAppInfo(VK_API_VERSION_1_1, "DeviceInfo", "VknConfig");
        this->createInstance();
        this->getDevice(deviceIdx)->createDevice();
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

    void VknConfig::fillInstanceCreateInfo(VkInstanceCreateFlags flags)
    {
        if (m_filledInstanceCreateInfo)
            throw std::runtime_error("Instance create info already filled.");
        m_infos->fillInstanceCreateInfo(flags);
        m_filledInstanceCreateInfo = true;
    }

    void VknConfig::addInstanceExtension(std::string &extension)
    {
        m_infos->addInstanceExtension(extension.c_str(), extension.size());
    }

    void VknConfig::addInstanceExtension(VknVector<char> &chars)
    {
        m_infos->addInstanceExtension(chars.getData(), chars.getSize());
    }

    void VknConfig::addLayer(std::string &layer)
    {
        m_infos->addLayer(layer.c_str(), layer.size());
    }

    void VknConfig::addLayer(VknVector<char> &chars)
    {
        m_infos->addLayer(chars.getData(), chars.getSize());
    }

    VknResult VknConfig::createInstance()
    {
        if (m_createdInstance)
            throw std::runtime_error("Instance already created.");
        if (!m_filledInstanceCreateInfo)
            throw std::runtime_error("Creating instance without filling instance create info.");
        m_engine->addVkInstance(m_relIdxs, m_absIdxs);
        VknResult res{
            vkCreateInstance(
                m_infos->getInstanceCreateInfo(), VK_NULL_HANDLE,
                m_engine->getVector<VkInstance>().getData()),
            "Create instance."};

        if (m_validationLayerAdded)
            this->setupDebugMessenger();

        m_createdInstance = true;
        return res;
    }

    void VknConfig::setupDebugMessenger()
    {
        if (!m_createdInstance)
            throw std::runtime_error("Instance not created before setting up debug messenger.");
        m_engine->addNewObject<VkDebugUtilsMessengerEXT, VkInstance>(m_absIdxs);
        VknResult res{CreateDebugUtilsMessengerEXT(
                          m_engine->getObject<VkInstance>(0), &populateDebugMessengerCreateInfo(),
                          nullptr, &m_engine->getObject<VkDebugUtilsMessengerEXT>(m_absIdxs)),
                      "Create debug messenger"};
    }

    VknDevice *VknConfig::getDevice(uint32_t deviceIdx)
    {
        return getListElement(deviceIdx, m_devices);
    }

    void VknConfig::addWindow_GLFW(GLFWwindow *window)
    {
        m_GLFWwindow = window;
    }

    VkSurfaceKHR *VknConfig::createSurface(uint32_t surfaceIdx)
    {
        if (!m_createdInstance)
            throw std::runtime_error("Didn't create instance before trying to create window surface.");
        if (m_GLFWwindow)
            this->createWindowSurface_GLFW(surfaceIdx);
        else
            throw std::runtime_error("No window configured for VknConfig::createSurface()");
    }

    VkSurfaceKHR *VknConfig::createWindowSurface_GLFW(uint32_t surfaceIdx)
    {
        if (m_GLFWwindow == nullptr)
            throw std::runtime_error("Trying to create a window surface but did not pass a window object to Config.");

        VknResult res{"Create window surface."};
        res = glfwCreateWindowSurface(
            m_engine->getObject<VkInstance>(0), m_GLFWwindow, nullptr,
            m_engine->getVector<VkSurfaceKHR>().getData(1));
    }
    /*
    void VknConfig::addInstanceExtension(std::string extension)
    {
        m_infos->addInstanceExtension(extension);
    }
    */
}