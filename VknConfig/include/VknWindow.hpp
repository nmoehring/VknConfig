#pragma once

namespace vkn
{
    class VknWindow
    {
    public:
        virtual ~VknWindow() = default; // Important for polymorphic deletion
        virtual bool init() = 0;
        virtual bool update() = 0;                 // Returns true if app should continue, false to exit
        virtual bool isActive() = 0;               // True if window is active and rendering can occur
        virtual bool isClosed() = 0;               // True if the window has been signaled to close or is demolished
        virtual void *getNativeHandle() const = 0; // To get GLFWwindow* or ANativeWindow*
    };
}