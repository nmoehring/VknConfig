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
        while (keepRunning)
        {
            keepRunning = m_config.getWindow()->update();
            if (keepRunning && m_config.getWindow()->isActive())
                this->cycleEngine();
        }
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