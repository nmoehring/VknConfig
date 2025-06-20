#include "include/VknPlatforms.hpp"

namespace vkn
{
    ///////////////////////////////////////////////////////////////////////
    std::vector<std::string> getPlatformSpecificExtensions()
    {
        std::vector<std::string> extensionStrings{};

#if defined(_WIN32) || defined(__linux__) || defined(__MAC_OS__) || defined(__UNKNOWN_PLATFORM__)
        uint_fast32_t count{0};
        const char **extensions{nullptr};
        extensions = glfwGetRequiredInstanceExtensions(&count);
        if (extensions)
            for (uint_fast32_t i = 0; i < count; ++i)
                extensionStrings.push_back(extensions[i]);
#elif defined(__ANDROID__)
        extensionStrings.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif
        return extensionStrings;
    }

    /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    VknWindow *getPlatformSpecificWindow()
    {
#if defined(_WIN32) || defined(__linux__) || defined(__MAC_OS__) || defined(__UNKNOWN_PLATFORM__)
        return new GLFW_Window;
#elif defined(__ANDROID__)
        return new Android_Window{};
#endif
    }

    /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////
    // Changed VkInstance* to VkInstance and void* to VknWindow* to match declaration
    void getPlatformSpecificSurface(VkSurfaceKHR *surface, uint32_t surfaceIdx, VkInstance instance, VknWindow *window)
    {
        VknResult res{"Create window surface."};
#if defined(_WIN32) || defined(__linux__) || defined(__MAC_OS__) || defined(__UNKNOWN_PLATFORM__)

        res = glfwCreateWindowSurface(
            instance, static_cast<GLFWwindow *>(window->getNativeHandle()), nullptr, surface);

#elif defined(__ANDROID__)
        VkAndroidSurfaceCreateInfoKHR androidSurfaceCreateInfo{};
        androidSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        androidSurfaceCreateInfo.window = static_cast<ANativeWindow *>(window->getNativeHandle()); // Use getNativeHandle()

        PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR_func =
            (PFN_vkCreateAndroidSurfaceKHR)vkGetInstanceProcAddr(instance, "vkCreateAndroidSurfaceKHR");
        res = vkCreateAndroidSurfaceKHR_func(instance, &androidSurfaceCreateInfo, nullptr, surface);
#endif
        // The VknResult 'res' will evaluate the VkResult from the surface creation call.
    }
}