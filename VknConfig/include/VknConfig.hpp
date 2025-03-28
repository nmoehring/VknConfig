// Contains instance, devices, and surfaces

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN

#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <stdexcept>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "VknDevice.hpp"

namespace vkn
{
    class VknConfig
    {
    public:
        void deviceInfo(uint32_t deviceIdx); // Create a simple program with just this call to get some device info
        void testNoInputs();

        // No default constructor, no empty object
        VknConfig() = delete;
        VknConfig(VknEngine *engine, VknInfos *infos, GLFWwindow *window);

        // Prevent copy/move constructors and assignment operators
        VknConfig(const VknConfig &) = delete;
        VknConfig &operator=(const VknConfig &) = delete;
        VknConfig(VknConfig &&) = delete;
        VknConfig &operator=(VknConfig &&) = delete;

        void fillAppInfo(uint32_t apiVersion, std::string appName,
                         std::string engineName,
                         VkApplicationInfo *pNext = nullptr,
                         uint32_t applicationVersion = 0,
                         uint32_t engineVersion = 0);
        void fillInstanceCreateInfo(const char *const *enabledLayerNames,
                                    uint32_t enabledLayerNamesSize,
                                    const char *const *enabledExtensionNames,
                                    uint32_t enabledExtensionNamesSize,
                                    VkInstanceCreateFlags flags = 0);
        void enableExtensions(std::vector<std::string> extensions);
        VknResult createInstance();

        void addDevice(uint32_t deviceIdx);
        VkInstance *getInstance() { return &m_engine->getInstance(); }
        bool getInstanceCreated() { return m_createdInstance; }
        VknDevice *getDevice(uint32_t deviceIdx);
        void createWindowSurface(uint32_t surfaceIdx);

    private:
        // Engine
        VknEngine *m_engine{nullptr};
        VknIdxs m_relIdxs{};
        VknIdxs m_absIdxs{};
        VknInfos *m_infos{nullptr};

        // Params
        std::vector<std::string> m_instanceExtensions{}; // Fine, because this list won't need to change
        GLFWwindow *m_window{nullptr};

        // Members
        std::vector<VknDevice> m_devices;

        // State
        bool m_selectedQueues{false};
        bool m_filledInstanceCreateInfo{false};
        bool m_createdInstance{false};
        bool m_filledAppInfo{false};
        uint32_t m_numDevices{0};
    };
}