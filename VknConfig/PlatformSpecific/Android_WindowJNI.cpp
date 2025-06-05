#include "include/Android_WindowJNI.hpp"
#include <android/log.h> // For logging

#define LOG_TAG_JNI_WINDOW "Android_WindowJNI"
#define LOGI_JNI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG_JNI_WINDOW, __VA_ARGS__)

namespace vkn
{
    // Constructor
    AndroidWindowJNI::AndroidWindowJNI() : m_window(nullptr), m_active(false), m_shouldClose(false)
    {
        LOGI_JNI("AndroidWindowJNI created");
    }

    // Destructor
    AndroidWindowJNI::~AndroidWindowJNI()
    {
        LOGI_JNI("AndroidWindowJNI destroyed");
        // ANativeWindow is managed by the system/JNI bridge, no direct release here
        // unless this destructor is guaranteed to be called when the ANativeWindow is truly gone.
        // Typically, ANativeWindow_release is handled in the JNI layer (native-lib.cpp).
    }

    bool AndroidWindowJNI::init()
    {
        // For JNI, initialization is minimal. The window handle comes via onSurfaceAvailable.
        m_active = false;
        m_shouldClose = false;
        LOGI_JNI("init called");
        return true;
    }

    bool AndroidWindowJNI::update()
    {
        // In JNI mode, this function is called by nativeRender.
        // It doesn't poll events like the NativeActivity version.
        // It should return true if the app is supposed to keep running.
        // The decision to stop rendering is usually managed by the Java side (e.g., onPause).
        // LOGI_JNI("update called, returning %s", !m_shouldClose ? "true" : "false");
        return !m_shouldClose;
    }

    bool AndroidWindowJNI::isActive()
    {
        // Active means we have a valid window and the app is in a state to render (e.g., not paused).
        // LOGI_JNI("isActive called, window: %p, active_state: %d", m_window, m_active);
        return m_window != nullptr && m_active;
    }

    bool AndroidWindowJNI::isClosed()
    {
        // LOGI_JNI("isClosed called, shouldClose: %d", m_shouldClose);
        return m_shouldClose;
    }

    void *AndroidWindowJNI::getNativeHandle() const
    {
        // LOGI_JNI("getNativeHandle called, returning: %p", m_window);
        return m_window;
    }

    void AndroidWindowJNI::setNativeInterfaceObjectPointer(void *nativeInterfaceObjectPointer)
    {
        // For JNI, this is less critical. The ANativeWindow comes via onSurfaceAvailable.
        // This could be used if there was some other global Android object to pass.
        LOGI_JNI("setNativeInterfaceObjectPointer called with: %p", nativeInterfaceObjectPointer);
        // If nativeInterfaceObjectPointer was meant to be ANativeWindow initially:
        // m_window = static_cast<ANativeWindow*>(nativeInterfaceObjectPointer);
    }

    void AndroidWindowJNI::handleAppCmd(int32_t cmd)
    {
        // No-op for JNI mode, as app commands are handled by NativeActivity.
        LOGI_JNI("handleAppCmd called with cmd: %d (No-op for JNI)", cmd);
    }

    // Called from JNI when the Java Surface is created/changed
    void AndroidWindowJNI::onSurfaceAvailable(ANativeWindow *window)
    {
        LOGI_JNI("onSurfaceAvailable, window: %p", window);
        m_window = window;
        if (m_window)
        {                    // If window is now valid, mark as active (if app is also resumed)
            m_active = true; // Assuming onResume will also be called or has been.
        }
        m_shouldClose = false; // If we got a new surface, we are not closing.
    }

    // Called from JNI when the Java Surface is destroyed
    void AndroidWindowJNI::onSurfaceUnavailable()
    {
        LOGI_JNI("onSurfaceUnavailable");
        m_window = nullptr;
        m_active = false; // Cannot be active without a window
    }

    // Called from JNI on onPause
    void AndroidWindowJNI::onAppPause()
    {
        LOGI_JNI("onAppPause");
        m_active = false;
        // You might set m_shouldClose = true here if onPause means the app is exiting,
        // but typically Android can pause and resume.
    }

    // Called from JNI on onResume
    void AndroidWindowJNI::onAppResume()
    {
        LOGI_JNI("onAppResume");
        m_active = true; // App is resuming, allow rendering if window is also available
        m_shouldClose = false;
    }
}