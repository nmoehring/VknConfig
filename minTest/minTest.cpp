#include <iostream>
#include <filesystem>

#include "VknConfig/include/VknApp.hpp"
#include "VknConfig/include/VknData.hpp"
#include "VknConfig/include/VknData.hpp"

#if defined(_WIN32) || defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS) || defined(__UNKNOWN_PLATFORM__)
// Desktop main function
int main()
{
    vkn::VknApp info_app{};
    info_app.configureWithPreset(vkn::deviceInfoConfig);
    info_app.exit();

    vkn::VknApp noInputApp{};
    noInputApp.configureWithPreset(vkn::noInputConfig); // Configure before run
    // If validation layers are desired:
    // noInputApp.enableValidationLayer();
    noInputApp.run();
    noInputApp.exit(); // Explicitly call exit

    return EXIT_SUCCESS;
}

#elif defined(PLATFORM_ANDROID) && VKN_NATIVE_ACTIVITY_MODE
// Android main function
void android_main(struct android_app *app)
{
    // Ensure the native app glue isn't stripped.
    app_dummy();

    vkn::VknApp androidTestApp{};

    // To make VknApp work seamlessly on Android with a simple run() call,
    // VknApp needs to be aware of the Android application context (struct android_app*).
    // It's assumed here that VknApp has a method (e.g., setAndroidApp(app) or similar)
    // to receive this context. This method would typically store the 'app' pointer
    // and set up necessary Android callbacks (like app->onAppCmd and app->userData)
    // internally within the VknApp class.

    // Example (assuming such a method exists in VknApp):
    // androidTestApp.setAndroidApp(app);

    // Configure the app instance.
    androidTestApp.configureWithPreset(vkn::noInputConfig);

    androidTestApp.getWindow()->setApp(app);

    // If validation layers are desired (assuming this method is platform-agnostic):
    // androidTestApp.enableValidationLayer();

    // Run the application.
    // On Android, VknApp::run() would internally manage the event loop,
    // process lifecycle events, and handle rendering.
    androidTestApp.run();

    // Clean up and exit when run() returns.
    androidTestApp.exit();
}
#endif