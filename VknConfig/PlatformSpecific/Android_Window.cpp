#include "include/Android_Window.hpp"

namespace vkn
{
    // Static callback functions to be registered with android_app_glue
    // These will call member functions on the Android_Window instance

    static void handle_app_cmd_static(struct android_app *app, int32_t cmd)
    {
        Android_Window *androidWindow = static_cast<Android_Window *>(app->userData);
        if (androidWindow)
        {
            androidWindow->handleAppCmd(cmd);
        }
    }

    // We're not handling input in this example, but this is where it would go
    // static int32_t handle_input_event_static(struct android_app* app, AInputEvent* event) {
    //     Android_Window* androidWindow = static_cast<Android_Window*>(app->userData);
    //     if (androidWindow) {
    //         // return androidWindow->handleInputEvent(event);
    //     }
    //     return 0; // 0 if not handled, 1 if handled
    // }

    Android_Window::Android_Window()
    {
        // m_app will be set via setApp()
    }

    Android_Window::~Android_Window()
    {
        // If the app pointer is valid and userData points to this window, clear it
        // to prevent dangling pointers if the app struct outlives this window object.
        if (m_app && m_app->userData == this)
        {
            m_app->userData = nullptr;
            // Callbacks are on the app struct, not cleared here unless necessary
        }
        // ANativeWindow is managed by the system, no direct deletion needed here.
        // It's released when APP_CMD_TERM_WINDOW is handled.
    }

    void Android_Window::setNativeInterfaceObjectPointer(void *nativeInterfaceObjectPointer)
    {
        if (!nativeInterfaceObjectPointer)
            throw std::runtime_error("Android_Window::setApp received a null android_app pointer.");

        m_app = static_cast<struct android_app *>(app);
        // Associate this Android_Window instance with the android_app state
        m_app->userData = this;
        // Register our command handler
        m_app->onAppCmd = handle_app_cmd_static;
        // Register input handler (optional, if you need input)
        // m_app->onInputEvent = handle_input_event_static;
    }

    bool Android_Window::init()
    {
        // Initialization for Android_Window primarily means the android_app
        // pointer is set and callbacks are registered. This is done in setApp().
        // The actual native window becomes available via APP_CMD_INIT_WINDOW.
        if (!m_app)
        {
            // This state should ideally be prevented by application logic
            // (e.g., ensuring setApp is called before init or VknApp handles it).
            return false;
        }
        m_readyToRender = false; // Window is not ready until APP_CMD_INIT_WINDOW
        m_active = false;        // Not active until APP_CMD_GAINED_FOCUS
        m_shouldExit = false;
        return true;
    }

    bool Android_Window::update()
    {
        if (!m_app)
            return false; // Cannot update without app state

        int ident;
        int events;
        struct android_poll_source *source;

        // Process all pending events.
        // Using a timeout of 0 makes it non-blocking.
        // If you want to block until an event, use -1 or a positive timeout.
        while ((ident = ALooper_pollAll(0, nullptr, &events, (void **)&source)) >= 0)
        {
            // Process this event.
            if (source != nullptr)
            {
                source->process(m_app, source);
            }

            // Check if the app is exiting.
            if (m_app->destroyRequested != 0)
            {
                m_shouldExit = true;
                return false; // Signal to VknApp to stop running
            }
        }
        return !m_shouldExit; // Continue running if not explicitly told to exit
    }

    bool Android_Window::isActive()
        // Active means we have focus and a window to render to.
        return m_active
        && m_readyToRender;

    bool Android_Window::isClosed() return m_shouldExit || (m_app && m_app -> destroyRequested != 0);

    void *Android_Window::getNativeHandle() const
        // m_window is ANativeWindow*, set in handleAppCmd
        return m_window;

    void Android_Window::handleAppCmd(int32_t cmd)
    {
        switch (cmd)
        {
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (m_app->window != nullptr)
            {
                m_window = m_app->window;
                m_readyToRender = true;
                // At this point, you can create/recreate your Vulkan surface and swapchain.
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            m_readyToRender = false;
            m_window = nullptr; // The ANativeWindow is no longer valid.
            // Destroy Vulkan surface and swapchain here.
            break;
        case APP_CMD_GAINED_FOCUS:
            m_active = true;
            break;
        case APP_CMD_LOST_FOCUS:
            m_active = false;
            // Consider pausing rendering or reducing frame rate.
            break;
        case APP_CMD_DESTROY:
            m_shouldExit = true;
            break;
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.
            // (Not implemented in this skeleton)
            break;
        case APP_CMD_STOP:
            m_active = false; // App is no longer visible
            break;
        case APP_CMD_RESUME:
        case APP_CMD_START:
            // These often don't require specific window actions unless tied to focus
            break;
        case APP_CMD_PAUSE:
            // App is about to be paused
            break;
            // Handle other commands as needed:
            // APP_CMD_CONFIG_CHANGED, APP_CMD_LOW_MEMORY, etc.
        }
    }

    bool Android_Window::isReady() const
    {
        return m_readyToRender;
    }
}