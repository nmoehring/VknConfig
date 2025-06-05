#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
// Is it needed?
// #include <GLFW/glfw3native.h>
#include "../../include/VknWindow.hpp"

namespace vkn
{
    class GLFW_Window : public VknWindow // Needs to be public for polymorphism
    {
    public:
        // Overloads
        GLFW_Window();
        ~GLFW_Window();

        bool init() override;
        bool update() override;   // Returns true if app should continue, false to exit
        bool isActive() override; // True if window is active and rendering can occur
        bool isClosed() override; // True if the window has been signaled to close or is demolished
        void *getNativeHandle() const override;
        void setNativeInterfaceObjectPointer(void *nativeInterfaceObjectPointer) override; // Initially for Android, but may be other uses?
        void handleAppCmd(int32_t cmd) override;                                           // Initially for Android, but may be other uses?

    private:
        // Members
        int m_width{0};
        int m_height{0};
        GLFWwindow *m_window{nullptr};

        // State
        bool m_windowCreated{false};
        bool m_active{false}; // True if window is not minimized
        static uint_fast32_t s_windowCount;
        static void glfwErrorCallback(int error, const char *description);                            // Must be static
        static void glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods); // Must be static
        void demolish();

        bool isWindowMinimized();
    };
}