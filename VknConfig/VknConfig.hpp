// I decided to use a Vkn prefix to stand apart from Vk prefixes of the API
// All the structs are in info.hpp
// Interface: need to select a physical device,
//           need to select queue families

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

        VknConfig();
        VknConfig(GLFWwindow *window);
        ~VknConfig();
        void destroy();
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
        void addDevice(uint32_t deviceIdx);
        VknResult createInstance();
        void requestQueueFamilies(uint32_t deviceIndex);
        VknResult createRenderpass();
        std::vector<vkn::VknQueueFamily> getQueueData();
        VknInfos *getInfos() { return &m_infos; }
        VkInstance *getInstance() { return &m_instance; }
        VknRenderpass *getRenderpass(uint32_t deviceIdx, uint32_t renderpassIdx);
        void enableExtensions(std::vector<std::string> extensions);
        bool getInstanceCreated() { return m_instanceCreated; }
        void selectQueues(uint32_t deviceIdx, bool chooseAllAvailableQueues = false);
        VknDevice *getDevice(uint32_t deviceIdx);
        void createWindowSurface();

    private:
        VknResultArchive m_resultArchive;
        VknInfos m_infos;
        VkInstance m_instance;
        std::list<VknDevice> m_devices{};                // A vector is fine because all the devices are inserted in one function call
        std::vector<std::string> m_instanceExtensions{}; // Fine, because this list won't need to change
        GLFWwindow *m_window;
        VkSurfaceKHR m_surface;

        bool m_instanceCreated{false};
        bool m_physicalDeviceSelected{false};
        bool m_queueFamiliesRequested{false};
        bool m_queuesSelected{false};
        bool m_destroyed{false};
        bool m_filledInstanceCreateInfo{false};
        uint32_t m_numDevices{0};

        void archiveResult(VknResult res);
    };
}