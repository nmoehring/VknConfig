#pragma once

#include "VknWindow.hpp"

namespace vkn
{
    class Android_WindowJNI : public VknWindow
    {
    public:
        // Destructor
        ~Android_WindowJNI() override = default;
        bool init() override;
        bool update() override;                                              // Returns true if app should continue, false to exit
        bool isActive() override;                                            // True if window is active and rendering can occur
        bool isClosed() override;                                            // True if the window has been signaled to close or is demolished
        void *getNativeHandle() const override;                              // To get GLFWwindow* or ANativeWindow*
        void setNativeInterfaceObject(void *nativeInterfaceObject) override; // Initially for Android, but may be other uses?
        void handleAppCmd(int32_t cmd) override;

        // Lifecycle functions
        //  Called from JNI when the Java Surface is created/changed
        void onSurfaceAvailable(ANativeWindow *window);
        // Called from JNI when the Java Surface is destroyed
        void onSurfaceUnavailable();
        // Called from JNI on onPause
        void onAppPause();
        // Called from JNI on onResume
        void onAppResume();

    private:
        ANativeWindow *m_window;
    };
}