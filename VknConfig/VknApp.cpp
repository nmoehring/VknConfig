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

    void VknApp::run()
    {
        bool keepRunning{true};
        if (m_vknWindow && !m_vknWindow->init())
            throw std::runtime_error("VknWindow initialization failed.");
        while (keepRunning)
        {
            keepRunning = m_vknWindow->update();
            if (keepRunning && m_vknWindow->isActive())
                this->cycleEngine();
        }
    }

    void VknApp::exit()
    {
        m_engine.shutdown();
        m_config.demolish();
        if (m_vknWindow)
            delete m_vknWindow;
        m_vknWindow = nullptr;
        --m_numApps;
    }

    void VknApp::configureWithPreset(std::function<bool(VknConfig *, VknEngine *, VknInfos *)> preset)
    {
        bool readyToRun = preset(&m_config, &m_engine, &m_infos);
        m_configured = true;
        if (readyToRun)
            m_cycle.loadConfig(&m_config, &m_engine);
    }

    void VknApp::addWindow()
    {
        m_vknWindow = getPlatformSpecificWindow();

        if (!m_vknWindow) // Should be caught by addWindow if getPlatformSpecificWindow returns null
            throw std::runtime_error("VknWindow was not created by addWindow().");

        if (!m_vknWindow->init())
            throw std::runtime_error("VknWindow initialization failed.");

        if (!m_setPlatformExtensions)
        {
            std::vector<std::string> extensionStrings = getPlatformSpecificExtensions();

            // After attempting to populate extensionStrings, check if it's empty.
            // For GLFW, if count is 0 or extensions is null, extensionStrings would also be empty.
            // For Android, we explicitly add one.
            if (extensionStrings.empty())
                throw std::runtime_error("Problem retrieving Vulkan extensions required for surface creation. There may be a problem with window creation, or only compute is supported.");

            for (auto &ext : extensionStrings)
                m_config.addInstanceExtension(ext);

            m_setPlatformExtensions = true;
            m_config.addWindow(m_vknWindow);
        }
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

        m_cycle.wait();
        if (!m_cycle.acquireImage())
            return false;
        m_cycle.recordCommandBuffer();
        m_cycle.submitCommandBuffer();
        if (!m_cycle.presentImage())
            return false;
        return true;
    }
}