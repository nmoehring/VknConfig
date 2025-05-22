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

    void VknApp::addWindow_GLFW(GLFWwindow *window)
    {
        if (window == nullptr)
            throw std::runtime_error("Window passed to addWindow_GLFW() is null. There may be a problem with window creation.");
        m_config.addWindow_GLFW(window);

        uint32_t count;
        const char **extensions = glfwGetRequiredInstanceExtensions(&count);
        if (count == 0 | extensions == nullptr)
            throw std::runtime_error("Problem retrieving Vulkan extensions required for surface creation. There may be a problem with window creation, or only compute is supported.");
        for (uint32_t i = 0; i < count; i++)
        {
            std::string name = extensions[i];
            m_config.addInstanceExtension(name);
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

    void VknApp::cycleEngine()
    {
        m_cycle.wait();
        m_cycle.acquireImage();
        m_cycle.recordCommandBuffer();
        m_cycle.submitCommandBuffer();
        m_cycle.presentImage();
    }
}