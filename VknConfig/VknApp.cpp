#include "include/VknApp.hpp"

namespace vkn
{
    VknApp::VknApp() : m_config{&m_engine, &m_infos}, m_cycle{&m_config, &m_engine}
    {
    }

    void VknApp::exit()
    {
        m_engine.shutdown();
    }

    void VknApp::configureWithPreset(std::function<void(VknConfig *, VknEngine *, VknInfos *)> preset)
    {
        preset(&m_config, &m_engine, &m_infos);
        m_configured = true;
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

        VknResult res{"Create window surface."};
        res = glfwCreateWindowSurface(
            m_engine.getObject<VkInstance>(0), window, nullptr,
            m_engine.getVector<VkSurfaceKHR>().getData(1));
    }

    void VknApp::enableValidationLayer()
    {
        VknVector<char> name{};
        for (const char *i = VK_EXT_DEBUG_UTILS_EXTENSION_NAME; i != '\0'; ++i)
            name.append(*i);
        name.append('\0');
        m_config.addInstanceExtension(name);

        std::string name = "VK_LAYER_KHRONOS_validation";
        m_config.addLayer(name);
        m_config.setValidationEnabled();
    }

    void VknApp::cycleEngine()
    {
        m_cycle.wait();
        m_cycle.acquireImage();
        m_cycle.presentImage();
        m_cycle.recordCommandBuffer();
        m_cycle.submitCommandBuffer();
    }
}