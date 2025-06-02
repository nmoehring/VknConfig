#pragma once

#include <string>
#include <vector>
#include "VknWindow.hpp"
#include "VknResult.hpp"

#if defined(_WIN32)
#include "../PlatformSpecific/include/GLFW_Window.hpp"

#elif defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR // Corrected: define instead of include
#include "../PlatformSpecific/include/Android_Window.hpp"

#elif defined(__linux__)
#include "../PlatformSpecific/include/GLFW_Window.hpp" // Assuming GLFW_Window is for all desktop GLFW

#elif defined(__APPLE__) && defined(TARGET_OS_MAC)     // Check for macOS specifically
#include <TargetConditionals.h>                        // For TARGET_OS_MAC
#include "../PlatformSpecific/include/GLFW_Window.hpp" // Assuming GLFW_Window is for all desktop GLFW
#define __MAC_OS__

#else
#include "../PlatformSpecific/include/GLFW_Window.hpp"
// Default for other/unknown platforms
#define __UNKNOWN_PLATFORM__
// You might want to #warning "Unrecognized platform, attempting to use GLFW."
#endif

///////////////////////////////////////////////////////////////////////

namespace vkn
{
    std::vector<std::string> getPlatformSpecificExtensions();
    VknWindow *getPlatformSpecificWindow();
    // Changed VkInstance* to VkInstance and void* to VknWindow*
    void getPlatformSpecificSurface(VkSurfaceKHR *surface, uint32_t surfaceIdx, VkInstance instance, VknWindow *window);
}