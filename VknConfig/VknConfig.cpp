#include "include/VknConfig.hpp"
#include "include/VknPlatforms.hpp"

namespace vkn
{
    VknConfig::VknConfig(VknEngine *engine, VknInfos *infos)
        : m_engine{engine}, m_infos{infos}
    {
#ifdef _WIN32
        m_hasGlfwWindow = true;
#elif defined(__ANDROID__)
        m_hasAndroidWindow = true;
#endif
    }

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
    VkDebugUtilsMessengerCreateInfoEXT &VknConfig::populateDebugMessengerCreateInfo()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional user data
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        return m_infos->setDebugMessengerCreateInfo(createInfo);
    }

    // Helper functions to get extension function pointers
    VkResult VknConfig::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
    {
        return ((PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"))(
            instance, pCreateInfo, pAllocator, pDebugMessenger);
    }

    VknDevice *VknConfig::addDevice(uint32_t deviceIdx)
    {
        if (!m_createdInstance)
            throw std::runtime_error("Can't add a device until an instance is created.");
        return &m_engine->addNewVknObject<VknDevice, VkDevice, VkInstance>(
            deviceIdx, m_devices, m_relIdxs, m_absIdxs, m_infos);
    }

    void VknConfig::fillAppInfo()
    {
        if (m_filledAppInfo)
            throw std::runtime_error("Already filled app info.");
        m_infos->fillAppName(m_appName);
        m_infos->fillEngineName(m_engineName);
        m_infos->fillAppInfo(m_apiVersion, m_appVersion, m_engineVersion);
    }

    void VknConfig::fillInstanceCreateInfo()
    {
        if (m_filledInstanceCreateInfo)
            throw std::runtime_error("Instance create info already filled.");
        m_infos->fillInstanceCreateInfo(m_flags);
        m_filledInstanceCreateInfo = true;
    }

    void VknConfig::addInstanceExtension(std::string &extension)
    {
        if (m_createdInstance)
            throw std::runtime_error("Can't enable extensions after instance is already created.");
        m_infos->addInstanceExtension(extension);
    }

    void VknConfig::addLayer(std::string &layer)
    {
        if (m_createdInstance)
            throw std::runtime_error("Can't enable layers after instance is already created.");
        m_infos->addLayer(layer);
    }

    VknResult VknConfig::createInstance()
    {
        if (m_createdInstance)
            throw std::runtime_error("Instance already created.");
        this->fillAppInfo();
        this->fillInstanceCreateInfo();
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
        VknResult res{createDebugUtilsMessengerEXT(
                          m_engine->getObject<VkInstance>(0), &populateDebugMessengerCreateInfo(),
                          nullptr, &m_engine->getObject<VkDebugUtilsMessengerEXT>(m_absIdxs)),
                      "Create debug messenger"};
    }

    VknDevice *VknConfig::getDevice(uint32_t deviceIdx)
    {
        return getListElement(deviceIdx, m_devices);
    }

    void VknConfig::addWindow(VknWindow *window)
    {
        m_window = window;
    }

    VkSurfaceKHR *VknConfig::createSurface(uint32_t surfaceIdx)
    {

        if (!m_window)
            throw std::runtime_error("No window configured for VknConfig::createSurface()");
        if (!m_createdInstance)
            throw std::runtime_error("Didn't create instance before trying to create window surface.");

        VkSurfaceKHR *surface = &m_engine->addNewObject<VkSurfaceKHR, VkInstance>(m_absIdxs);
        VknResult res{"Create window surface."};

        getPlatformSpecificSurface(
            surface, surfaceIdx, m_engine->getObject<VkInstance>(0), m_window);

        return surface;
    }

    void VknConfig::setAppName(std::string appName)
    {
        m_appName = appName;
    }

    void VknConfig::setEngineName(std::string engineName)
    {
        m_engineName = engineName;
    }

    void VknConfig::setApiVersion(unsigned int apiVersion)
    {
        m_apiVersion = apiVersion;
    }

    void VknConfig::demolish()
    {
        m_devices.clear();
    }
}