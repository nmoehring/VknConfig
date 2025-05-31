#include "include/VknApp.hpp"

namespace vkn
{
    uint32_t VknApp::m_numApps{0};

    VknApp::VknApp() : m_config{&m_engine, &m_infos}, m_cycle{}
    {
        if (m_numApps > 0)
            throw std::runtime_error("Previous VknApp() was not exited via VknApp::exit().");
        else
            ++m_numApps;
    }

    void VknApp::exit()
    {
        m_engine.shutdown();
        m_config.demolish();
        --m_numApps;
    }

    void VknApp::configureWithPreset(std::function<bool(VknConfig *, VknEngine *, VknInfos *)> preset)
    {
        bool readyToRun = preset(&m_config, &m_engine, &m_infos);
        m_configured = true;
        if (readyToRun)
            m_cycle.loadConfig(&m_config, &m_engine);
    }

    void VknApp::addWindow(void *window)
    {
        if (window == nullptr)
            throw std::runtime_error("Window passed to addWindow_GLFW() is null. There may be a problem with window creation.");
        m_config.addWindow(window);
        uint32_t count{0};
        std::vector<std::string> extensionStrings{};
        const char **extensions{nullptr};

#ifdef _WIN32
        extensions = glfwGetRequiredInstanceExtensions(&count);
        for (uint32_t i = 0; i < count; ++i)
            extensionStrings.push_back(extensions[i]);
#elif defined(__ANDROID__)
        extensionStrings.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif

        // After attempting to populate extensionStrings, check if it's empty.
        // For GLFW, if count is 0 or extensions is null, extensionStrings would also be empty.
        // For Android, we explicitly add one.
        if (extensionStrings.empty())
            throw std::runtime_error("Problem retrieving Vulkan extensions required for surface creation. There may be a problem with window creation, or only compute is supported.");
        for (auto &ext : extensionStrings)
            m_config.addInstanceExtension(ext);
    }

    void VknApp::enableValidationLayer()
    {
        std::string debugExtName = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        m_config.addInstanceExtension(debugExtName);

        std::string name = "VK_LAYER_KHRONOS_validation";
        m_config.addLayer(name);
        m_config.setValidationEnabled();
    }

    bool VknApp::cycleEngine()
    {
        if (!m_configured)
            throw std::runtime_error("App not configured before cycling engine.");

        if (!this->waitForWindowEventsAndTestIfMinimized()) // Ensures glfwWaitEvents is called if minimized
            return false;
        m_cycle.wait();
        if (!m_cycle.acquireImage())
            return this->waitForWindowEventsAndTestIfMinimized();
        m_cycle.recordCommandBuffer();
        m_cycle.submitCommandBuffer();
        if (!m_cycle.presentImage())
            return this->waitForWindowEventsAndTestIfMinimized();
        return true;
    }

    bool VknApp::waitForWindowEventsAndTestIfMinimized()
    {
        if (m_cycle.isWindowMinimized())
        {
            if (m_config.hasGLFWConfig())
                glfwWaitEvents();
            else
                throw std::runtime_error("Unknown window configuration!");
            return false;
        }
        return true;
    }
}