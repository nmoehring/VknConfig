#include <iostream>

#include "include/GLFW_Window.hpp"

namespace vkn
{
    uint32_t GLFW_Window::s_windowCount = 0;
    bool GLFW_Window::s_initialized = false;

    GLFW_Window::GLFW_Window()
    {
        ++s_windowCount;
    }

    GLFW_Window::~GLFW_Window()
    {
        this->demolish();
    }

    void GLFW_Window::demolish()
    {
        if (m_windowCreated)
        {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
            m_windowCreated = false;
            m_active = false;
        }
        --s_windowCount;
        if (s_windowCount == 0)
        {
            glfwTerminate();
            s_initialized = false;
        }
    }

    bool GLFW_Window::update()
    {
        if (this->isWindowMinimized())
        {
            glfwWaitEvents();
            this->isWindowMinimized();
        }
        else
            glfwPollEvents();

        if (glfwWindowShouldClose(m_window))
        {
            this->demolish();
            return false; // Exiting
        }

        return true; // Finished update, continuing
    }

    // GLFW error callback
    void GLFW_Window::glfwErrorCallback(int error, const char *description)
    {
        std::cerr << "GLFW Error: " << description << std::endl;
    }

    // GLFW key callback
    void GLFW_Window::glfwKeyCallback(GLFWwindow *windowHandle, int key, int scancode, int action, int mods)
    {
        const bool pressed{action != GLFW_RELEASE};
        if (key == GLFW_KEY_ESCAPE && pressed)
        {
            glfwSetWindowShouldClose(windowHandle, GLFW_TRUE);
        }
    }

    // GLFW framebuffer size callback
    void GLFW_Window::framebufferSizeCallback(GLFWwindow *window, int width, int height)
    {
        auto appWindow = reinterpret_cast<GLFW_Window *>(glfwGetWindowUserPointer(window));
        appWindow->m_width = width;
        appWindow->m_height = height;
        appWindow->m_framebufferResized = true;
    }

    bool GLFW_Window::init()
    {
        if (m_windowCreated)
            throw std::runtime_error("Window already created.");

        if (!s_initialized)
        {
            if (!glfwInit())
                throw std::runtime_error("Problem initializing GLFW.");
            glfwSetErrorCallback(glfwErrorCallback);
            s_initialized = true;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_window = glfwCreateWindow(640, 480, "Window Title", NULL, NULL);

        if (!m_window)
            throw std::runtime_error("Problem creating GLFW window.");
        if (!glfwVulkanSupported())
            throw std::runtime_error("Vulkan graphics API not supported by any of your devices.");
        glfwSetWindowUserPointer(m_window, this);
        glfwSetKeyCallback(m_window, glfwKeyCallback);
        glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
        // glfwSetCursorPosCallback(m_window, glfwCursorPosCallback);
        // glfwSetMouseButtonCallback(m_window, glfwMouseButtonCallback);
        // glfwSetScrollCallback(m_window, glfwScrollCallback);
        // glfwSetCharCallback(m_window, glfwCharCallback);
        // glfwSetDropCallback(m_window, glfwDropCallback);
        // glfwSetJoystickCallback(glfwJoystickCallback);
        // glfwSetMonitorCallback(glfwMonitorCallback)

        m_windowCreated = true;
        m_active = true;
        return true;
    }

    bool GLFW_Window::isWindowMinimized()
    {
        if (!m_windowCreated || !m_window)
        {
            m_active = false;
            return true;
        }
        glfwGetFramebufferSize(m_window, &m_width, &m_height);
        if (m_width == 0 || m_height == 0)
        {
            m_active = false;
            return true;
        }
        else
        {
            m_active = true;
            return false;
        }
    }

    bool GLFW_Window::isActive()
    {
        return !this->isWindowMinimized();
    }

    bool GLFW_Window::isClosed()
    {
        return !m_windowCreated || (m_window && glfwWindowShouldClose(m_window));
    }

    void *GLFW_Window::getNativeHandle() const
    {
        return m_window;
    }

    // Implement pure virtual functions from VknWindow.
    // For GLFW, these methods don't have a direct equivalent to Android's app commands,
    // so they can be no-ops or log a message if called unexpectedly.
    void GLFW_Window::setNativeInterfaceObjectPointer(void *app)
    {
        // No-op for GLFW. This function is primarily for Android's android_app struct.
    }

    void GLFW_Window::handleAppCmd(int32_t cmd)
    {
        // No-op for GLFW. This function is primarily for Android's app command system.
    }
}