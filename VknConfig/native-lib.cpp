
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

// For GTest
#include "gtest/gtest.h"

#define LOG_TAG "VknTesterJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static vkn::VknApp g_vknApp;
static bool g_isVulkanInitialized = false;
static ANativeWindow* g_nativeWindow = nullptr;

// Helper to redirect GTest output to Logcat
class LogcatPrinter : public ::testing::EmptyTestEventListener {
    void OnTestProgramStart(const ::testing::UnitTest& /*unit_test*/) override {
        LOGI("GTest: Test program starting.");
    }
    void OnTestProgramEnd(const ::testing::UnitTest& unit_test) override {
        LOGI("GTest: Test program ending. %d tests from %d test suites ran.",
             unit_test.total_test_count(), unit_test.total_test_suite_count());
        LOGI("GTest: PASSED: %d", unit_test.successful_test_count());
        LOGI("GTest: FAILED: %d", unit_test.failed_test_count());
        LOGI("GTest: DISABLED: %d", unit_test.disabled_test_count());
    }
    void OnTestStart(const ::testing::TestInfo& test_info) override {
        LOGI("GTest: [ RUN      ] %s.%s", test_info.test_suite_name(), test_info.name());
    }
    void OnTestEnd(const ::testing::TestInfo& test_info) override {
        if (test_info.result()->Passed()) {
            LOGI("GTest: [       OK ] %s.%s (%lld ms)", test_info.test_suite_name(), test_info.name(), test_info.result()->elapsed_time());
        } else {
            LOGE("GTest: [  FAILED  ] %s.%s (%lld ms)", test_info.test_suite_name(), test_info.name(), test_info.result()->elapsed_time());
            const ::testing::TestPartResultArray& results = test_info.result()->test_part_results();
            for (int i = 0; i < results.size(); ++i) {
                const ::testing::TestPartResult& result = results.GetTestPartResult(i);
                if (result.failed()) {
                    LOGE("GTest: %s:%d: Failure: %s", result.file_name(), result.line_number(), result.summary());
                }
            }
        }
    }
    // You can override other OnTestSuiteStart, OnTestSuiteEnd, etc. if needed.
};


// Ensure this matches the Java/Kotlin package and class name
// e.g., com_example_vknconfigtester_NativeBridge_nativeInit
extern "C" JNIEXPORT void JNICALL
Java_com_example_vknconfigtester_NativeBridge_nativeInit(JNIEnv* env, jobject /* this */) {
    if (g_isVulkanInitialized) {
        LOGI("Vulkan already initialized.");
        return;
    }
    try {
        // Use your noInputConfig preset (ensure it's defined and accessible)
        g_vknApp.configureWithPreset(vkn::noInputConfig); // Or another suitable preset for Android
        // The noInputConfig should set up Vulkan instance, physical device, logical device.
        // Swapchain and surface-dependent resources will be created when the surface is available.
        LOGI("VknApp initialized with preset.");
        g_isVulkanInitialized = true;
    } catch (const std::exception& e) {
        LOGE("VknApp nativeInit Error: %s", e.what());
        g_isVulkanInitialized = false;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_vknconfigtester_NativeBridge_nativeSetSurface(JNIEnv* env, jobject /* this */, jobject surface) {
    if (!g_isVulkanInitialized) {
        LOGE("Vulkan not initialized in nativeSetSurface.");
        return;
    }

    if (g_nativeWindow) {
        ANativeWindow_release(g_nativeWindow);
        g_nativeWindow = nullptr;
        LOGI("Previous ANativeWindow released.");
        // TODO: Notify VknApp to clean up old surface resources (swapchain, framebuffers, etc.)
        // This might involve calling a specific method in VknApp or VknConfig to destroy
        // the old VkSurfaceKHR and related objects before creating new ones.
        // For now, VknApp::addWindow might handle recreation if called again.
    }

    if (surface != nullptr) {
        g_nativeWindow = ANativeWindow_fromSurface(env, surface);
        LOGI("ANativeWindow acquired: %p", g_nativeWindow);
        if (g_nativeWindow == nullptr) {
            LOGE("Failed to acquire ANativeWindow from surface.");
            return;
        }
        try {
            // This will call VknConfig::addWindow and VknConfig::createSurface
            g_vknApp.addWindow(g_nativeWindow);
            LOGI("ANativeWindow passed to VknApp, surface should be created.");

            // If your preset didn't fully configure the device/swapchain because it lacked a window,
            // you might need to trigger further configuration steps in VknApp or VknConfig here.
            // For example, if VknApp::configureWithPreset only sets m_configured = true,
            // and VknCycle::loadConfig is only called if readyToRun is true (which might depend on having a window).
            // You might need to re-evaluate or call a method like:
            // g_vknApp.completeConfigurationWithWindow();
            // For now, assuming addWindow and the subsequent cycleEngine calls handle this.

        } catch (const std::exception& e) {
            LOGE("VknApp nativeSetSurface Error: %s", e.what());
            if (g_nativeWindow) {
                ANativeWindow_release(g_nativeWindow);
                g_nativeWindow = nullptr;
            }
        }
    } else {
        LOGI("Surface is null in nativeSetSurface (likely destroyed).");
        // g_nativeWindow is already null or will be handled by nativeSurfaceDestroyed
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_vknconfigtester_NativeBridge_nativeRender(JNIEnv* env, jobject /* this */) {
    if (!g_isVulkanInitialized || g_nativeWindow == nullptr) {
        // LOGI("Vulkan not ready for rendering (init: %d, window: %p)",
        //      g_isVulkanInitialized, g_nativeWindow);
        return;
    }
    try {
        if (!g_vknApp.cycleEngine()) {
            // LOGI("VknApp::cycleEngine returned false (e.g. minimized or out of date swapchain)");
        }
    } catch (const std::exception& e) {
        LOGE("VknApp nativeRender Error: %s", e.what());
        // Consider how to handle critical rendering errors.
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_vknconfigtester_NativeBridge_nativeSurfaceDestroyed(JNIEnv * env, jobject /* this */) {
    LOGI("Native surface destroyed callback.");
    if (g_nativeWindow != nullptr) {
        // TODO: Inform VknApp that the surface is gone so it can clean up Vulkan surface-specific resources
        // e.g., g_vknApp.onSurfaceDestroyed(); // You would implement this in VknApp/VknConfig
        // This method would typically destroy the VkSurfaceKHR, swapchain, image views, framebuffers.
        // It's crucial for robust surface recreation.
        LOGI("Releasing ANativeWindow: %p", g_nativeWindow);
        ANativeWindow_release(g_nativeWindow);
        g_nativeWindow = nullptr;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_vknconfigtester_NativeBridge_nativeCleanup(JNIEnv* env, jobject /* this */) {
    if (g_isVulkanInitialized) {
        LOGI("VknApp nativeCleanup called.");
        // Ensure surface-specific resources are cleaned first if not done in nativeSurfaceDestroyed
        if (g_nativeWindow != nullptr) {
            // g_vknApp.onSurfaceDestroyed(); // If not called before
            ANativeWindow_release(g_nativeWindow);
            g_nativeWindow = nullptr;
        }
        g_vknApp.exit(); // This should clean up all Vulkan resources
        g_isVulkanInitialized = false;
        LOGI("VknApp cleanup finished.");
    }
}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_vknconfigtester_NativeBridge_nativeRunTests(JNIEnv *env, jobject /* this */) {
    LOGI("Attempting to run GTests...");
    // It's important that GTest is initialized only once per process.
    // If this function can be called multiple times, consider a static flag.
    static bool gtest_initialized = false;
    if (!gtest_initialized) {
        // You might need to fake argc and argv for InitGoogleTest on Android
        int argc = 1;
        char appname[] = "VknConfigTests"; // Fake app name
        char* argv[] = { appname, nullptr };
        ::testing::InitGoogleTest(&argc, argv);
        gtest_initialized = true;

        // Add a Logcat printer to see test results in Android Studio's Logcat
        ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
        delete listeners.Release(listeners.default_result_printer()); // Remove default printer
        listeners.Append(new LogcatPrinter); // Add custom Logcat printer
    } else {
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
