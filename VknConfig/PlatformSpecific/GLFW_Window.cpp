#include <iostream>

#include "include/GLFW_Window.hpp"

namespace vkn
{
    uint_fast32_t GLFW_Window::s_windowCount = 0;

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
            glfwTerminate();
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

    bool GLFW_Window::init()
    {
        if (!glfwVulkanSupported())
            throw std::runtime_error("Vulkan graphics API not supported by any of your devices.");

        if (s_windowCount == 1)
        {
            if (!glfwInit())
                throw std::runtime_error("Problem initializing GLFW.");
            glfwSetErrorCallback(glfwErrorCallback);
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_window = glfwCreateWindow(640, 480, "Window Title", NULL, NULL);

        if (!m_window)
            throw std::runtime_error("Problem creating GLFW window.");
        glfwSetWindowUserPointer(m_window, this);
        glfwSetKeyCallback(m_window, glfwKeyCallback);

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
}