#include "include/VknApp.hpp"

namespace vkn
{
    uint32_t VknApp::m_numApps{0};

    VknApp::VknApp() : m_config{}, m_cycle{}
    {
        m_engine = m_config.getEngine();
        m_infos = m_config.getInfos();
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
        m_engine->shutdown();
        m_config.demolish();
        --m_numApps;
    }

    void VknApp::configureWithPreset(std::function<bool(VknConfig &)> preset)
    {
        m_readyToRun = preset(m_config);
        if (m_readyToRun)
            m_cycle.loadBasicConfig(&m_config, m_engine);
        if (m_config.isRenderingGraphics())
            m_cycle.loadGraphicsConfig(&m_config, m_engine);
        if (m_config.isComputing())
            m_cycle.loadComputeConfig(&m_config, m_engine);
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
        if (!m_readyToRun)
            throw std::runtime_error("App Cycle not configured before being run.");

        m_cycle.wait();
        if (!m_cycle.acquireImage())
            return false;

        // This is the new, more flexible recording flow.
        // You first begin recording, then record all the passes you need for this frame.
        m_cycle.beginFrameRecording();
        if (m_config.isComputing())
            m_cycle.recordComputePass(0); // Record compute work first
        if (m_config.isRenderingGraphics())
            m_cycle.recordGraphicsPass(0); // Then record graphics work

        m_cycle.submitCommandBuffer();
        if (!m_cycle.presentImage())
            return false;
        return true;
    }
}