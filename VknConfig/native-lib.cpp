
/*
#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_nathanielmoehring_vknconfig_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject ) { // this
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
*/

// In app/src/main/cpp/native-lib.cpp
#include <jni.h>
#include <string>
#include <vector>
#include <android/native_window_jni.h> // For ANativeWindow
#include <android/log.h>

// Adjust path if VknApp.hpp is not directly in VknConfig/include
#include "include/VknApp.hpp"

#if GTEST_ENABLED
// For GTest
#include "gtest/gtest.h"
#endif

#define LOG_TAG "VknConfigTestJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static vkn::VknApp g_vknApp;
static vkn::VknConfig &g_vknConfigRef = g_vknApp.getConfig(); // Use a reference
static vkn::VknCycle &g_vknCycleRef = g_vknApp.getCycle();    // Use a reference
static bool g_isVulkanConfigured = false;
static bool g_isNativeWindowSet = false;
static ANativeWindow *g_nativeWindow = nullptr;

#if GTEST_ENABLED
// Helper to redirect GTest output to Logcat
class LogcatPrinter : public ::testing::EmptyTestEventListener
{
    void OnTestProgramStart(const ::testing::UnitTest & /*unit_test*/) override
    {
        LOGI("GTest: Test program starting.");
    }
    void OnTestProgramEnd(const ::testing::UnitTest &unit_test) override
    {
        LOGI("GTest: Test program ending. %d tests from %d test suites ran.",
             unit_test.total_test_count(), unit_test.total_test_suite_count());
        LOGI("GTest: PASSED: %d", unit_test.successful_test_count());
        LOGI("GTest: FAILED: %d", unit_test.failed_test_count());
        LOGI("GTest: DISABLED: %d", unit_test.disabled_test_count());
    }
    void OnTestStart(const ::testing::TestInfo &test_info) override
    {
        LOGI("GTest: [ RUN      ] %s.%s", test_info.test_suite_name(), test_info.name());
    }
    void OnTestEnd(const ::testing::TestInfo &test_info) override
    {
        if (test_info.result()->Passed())
        {
            LOGI("GTest: [       OK ] %s.%s (%lld ms)", test_info.test_suite_name(), test_info.name(), test_info.result()->elapsed_time());
        }
        else
        {
            LOGE("GTest: [  FAILED  ] %s.%s (%lld ms)", test_info.test_suite_name(), test_info.name(), test_info.result()->elapsed_time());
            const ::testing::TestPartResultArray &results = test_info.result()->test_part_results();
            for (int i = 0; i < results.size(); ++i)
            {
                const ::testing::TestPartResult &result = results.GetTestPartResult(i);
                if (result.failed())
                {
                    LOGE("GTest: %s:%d: Failure: %s", result.file_name(), result.line_number(), result.summary());
                }
            }
        }
    }
    // You can override other OnTestSuiteStart, OnTestSuiteEnd, etc. if needed.
};
#endif // VKNCONFIG_WITH_GTEST

static vkn::AndroidWindowJNI *getJniWindowInstance()
{
    if (!g_vknConfig.getWindow())
        return nullptr;
    // This assumes VKN_NATIVE_ACTIVITY_MODE is 0 for this build
    return static_cast<vkn::AndroidWindowJNI *>(g_vknApp.getWindow());
}

extern "C" JNIEXPORT void JNICALL
Java_com_nathanielmoehring_vknconfig_NativeBridge_nativeConfig(JNIEnv *env, jobject /* this */)
{
    LOGI("nativeConfig called.");
    if (!g_isNativeWindowSet)
    {
        LOGI("Native window not set before Vulkan configuration.");
        return;
    }
    else if (g_isVulkanConfigured)
    {
        LOGI("Vulkan already initialized.");
        return;
    }

    try
    {
        // Use your noInputConfig preset (ensure it's defined and accessible)
        g_vknApp.configureWithPreset(vkn::noInputConfig); // Or another suitable preset for Android
        // This preset should call VknConfig::addWindow(), which in turn calls
        // Android_WindowJNI::setNativeInterfaceObjectPointer with the ANativeWindow*
        // previously set by VknConfig::setNativeWindow (which was called from nativeSetSurface).
        // Then, the preset calls VknConfig::createSurface() which uses the handle from Android_WindowJNI.
        // The noInputConfig should set up Vulkan instance, physical device, logical device.
        // Swapchain and surface-dependent resources will be created when the surface is available.
        LOGI("VknApp initialized with preset.");
        g_isVulkanConfigured = true;
    }
    catch (const std::exception &e)
    {
        LOGE("VknApp nativeInit Error: %s", e.what());
        g_isVulkanConfigured = false;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_nathanielmoehring_vknconfig_NativeBridge_nativeSetSurface(JNIEnv *env, jobject /* this */, jobject surface)
{
    LOGI("nativeSetSurface called with surface: %p", surface);
    if (!g_isVulkanConfigured)
    {
        LOGE("Vulkan not initialized in nativeSetSurface.");
        return;
    }

    if (g_nativeWindow)
    {
        // Existing window is being replaced or destroyed
        ANativeWindow_release(g_nativeWindow);
        g_nativeWindow = nullptr;
        LOGI("Previous ANativeWindow released.");
        g_isNativeWindowSet = false;
        if (auto *jniWindow = getJniWindowInstance())
        {
            jniWindow->onSurfaceUnavailable(); // Inform the window object
        }
    }

    if (surface != nullptr)
    {
        g_nativeWindow = ANativeWindow_fromSurface(env, surface);
        LOGI("ANativeWindow acquired: %p", g_nativeWindow);
        if (g_nativeWindow == nullptr)
        {
            LOGE("Failed to acquire ANativeWindow from surface.");
            return;
        }
        try
        {
            if (auto *jniWindow = getJniWindowInstance())
            {
                // If VknApp and its window object already exist (e.g., nativeConfig was called),
                // directly inform the JNI window instance.
                jniWindow->onSurfaceAvailable(g_nativeWindow);
                g_isNativeWindowSet = true;
            }
            LOGI("ANativeWindow set in VknConfig and/or AndroidWindowJNI.");

            if (g_isVulkanConfigured)
                g_vknCycleRef.recreateForWindowChange();
        }
        catch (const std::exception &e)
        {
            LOGE("VknApp nativeSetSurface Error: %s", e.what());
            if (g_nativeWindow)
            {
                ANativeWindow_release(g_nativeWindow);
                g_nativeWindow = nullptr;
            }
        }
    }
    else
    {
        // Surface is null, meaning it's being destroyed
        if (auto *jniWindow = getJniWindowInstance())
            jniWindow->onSurfaceUnavailable();
        LOGI("Surface is null in nativeSetSurface (likely destroyed).");
        g_vknConfigRef.setNativeWindow(nullptr); // Clear it in VknConfig too
        // g_nativeWindow is already null or will be handled by nativeSurfaceDestroyed
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_nathanielmoehring_vknconfig_NativeBridge_nativeRender(JNIEnv *env, jobject /* this */)
{
    if (!g_isVulkanConfigured || !g_vknApp.getWindow() || !g_vknApp.getWindow()->isActive())
    {
        // LOGI("Vulkan not ready for rendering (init: %d, window: %p)",
        //      g_isVulkanConfigured, g_nativeWindow);
        return;
    }
    try
    {
        if (!g_vknApp.cycleEngine())
        {
            // LOGI("VknApp::cycleEngine returned false (e.g. minimized or out of date swapchain)");
        }
    }
    catch (const std::exception &e)
    {
        LOGE("VknApp nativeRender Error: %s", e.what());
        // Consider how to handle critical rendering errors.
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_nathanielmoehring_vknconfig_NativeBridge_nativeSurfaceDestroyed(JNIEnv *env, jobject /* this */)
{
    LOGI("Native surface destroyed callback.");
    if (g_nativeWindow != nullptr)
    {
        if (auto *jniWindow = getJniWindowInstance())
            jniWindow->onSurfaceUnavailable();

        LOGI("Releasing ANativeWindow: %p", g_nativeWindow);
        ANativeWindow_release(g_nativeWindow);
        g_nativeWindow = nullptr;
        g_isNativeWindowSet = false;
        g_vknConfigRef.setNativeWindow(nullptr);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_nathanielmoehring_vknconfig_NativeBridge_nativePause(JNIEnv *env, jobject /* this */)
{
    LOGI("nativePause called.");
    if (auto *jniWindow = getJniWindowInstance())
    {
        jniWindow->onAppPause();
    }
}
extern "C" JNIEXPORT void JNICALL
Java_com_nathanielmoehring_vknconfig_NativeBridge_nativeResume(JNIEnv *env, jobject /* this */)
{
    LOGI("nativeResume called.");
    if (auto *jniWindow = getJniWindowInstance())
    {
        jniWindow->onAppResume();
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_nathanielmoehring_vknconfig_NativeBridge_nativeCleanup(JNIEnv *env, jobject /* this */)
{
    if (g_isVulkanConfigured)
    {
        LOGI("VknApp nativeCleanup called.");
        // Ensure ANativeWindow is released if it hasn't been already
        if (g_nativeWindow != nullptr)
        {
            // The call to g_vknApp.exit() should handle destroying the VkSurfaceKHR
            // and other Vulkan resources.
            ANativeWindow_release(g_nativeWindow);
            g_nativeWindow = nullptr;
            g_isNativeWindowSet = false;
        }
        g_vknApp.exit(); // This should clean up all Vulkan resources
        g_isVulkanConfigured = false;
        // Reset VknConfig's native window pointer as well
        // g_vknConfigRef.setNativeWindow(nullptr); // VknApp::exit() should lead to VknConfig::demolish()
        // which should clear its VknWindow pointer.
        LOGI("VknApp cleanup finished.");
    }
}

#if GTEST_ENABLED
extern "C" JNIEXPORT jint JNICALL
Java_com_nathanielmoehring_vknconfig_NativeBridge_nativeRunTests(JNIEnv *env, jobject /* this */)
{
    LOGI("Attempting to run GTests...");
    // It's important that GTest is initialized only once per process.
    // If this function can be called multiple times, consider a static flag.
    static bool gtest_initialized = false;
    if (!gtest_initialized)
    {
        // You might need to fake argc and argv for InitGoogleTest on Android
        int argc = 1;
        char appname[] = "VknConfigTests"; // Fake app name
        char *argv[] = {appname, nullptr};
        ::testing::InitGoogleTest(&argc, argv);
        gtest_initialized = true;

        // Add a Logcat printer to see test results in Android Studio's Logcat
        ::testing::TestEventListeners &listeners = ::testing::UnitTest::GetInstance()->listeners();
        delete listeners.Release(listeners.default_result_printer()); // Remove default printer
        listeners.Append(new LogcatPrinter);                          // Add custom Logcat printer
    }
    else
    {
        LOGI("GTest already initialized.");
    }

    // Ensure any previous test run's state is cleared if necessary.
    // This might involve re-initializing parts of your VknConfig or specific test fixtures
    // if they leave global state. For simple unit tests, this might not be an issue.

    LOGI("Running all GTests...");
    int result = RUN_ALL_TESTS(); // This will output to Logcat via LogcatPrinter
    LOGI("GTests finished with result code: %d", result);
    return result; // 0 for success
}
#else
// Provide a stub if GTest is not available, so JNI linkage doesn't break
extern "C" JNIEXPORT jint JNICALL
Java_com_nathanielmoehring_vknconfig_NativeBridge_nativeRunTests(JNIEnv *env, jobject /* this */)
{
    LOGE("GTest is not compiled into this build. nativeRunTests is a stub.");
    return -1; // Indicate GTest was not run
}
#endif // VKNCONFIG_WITH_GTEST
