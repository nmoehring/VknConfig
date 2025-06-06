#pragma once

#include <android/native_window.h>
#include <android_native_app_glue.h>
#include <stdexcept> // For std::runtime_error

#include "../../include/VknWindow.hpp"

namespace vkn
{
    class Android_Window : public VknWindow // Needs to be public for polymorphism
    {
    public:
        // Overloads
        Android_Window();
        ~Android_Window();

        bool init() override;
        bool update() override;   // Returns true if app should continue, false to exit
        bool isActive() override; // True if window is active and rendering can occur
        bool isClosed() override; // True if the window has been signaled to close or is demolished
        void *getNativeHandle() const override;

        // Android-specific methods
        void Android_Window::setNativeInterfaceObjectPointer(void *nativeInterfaceObjectPointer) override;
        void handleAppCmd(int32_t cmd) override;
        bool isReady() const;

    private:
        // Members
        // int m_width{0};
        // int m_height{0};
        struct android_app *m_app{nullptr};
        ANativeWindow *m_window{nullptr};

        // State
        bool m_windowCreated{false};
        bool m_active{false}; // True if window is not minimized
        // bool m_readyToRender{false};
        bool m_shouldClose{false};
        // static uint32_t s_windowCount;

        // void demolish();
        // bool isWindowMinimized();
    };
}