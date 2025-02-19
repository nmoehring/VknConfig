// I decided to use a Vkn prefix to stand apart from Vk prefixes of the API
// All the structs are in info.hpp
// Interface: need to select a physical device,
//           need to select queue families

#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <functional>

#include "VknDevice.hpp"
#include "VknRenderPass.hpp"
#include "VknQueueFamily.hpp"
#include "vknInfos.hpp"
#include "VknResult.hpp"

namespace vkn
{
    class VknConfig
    {
    public:
        void deviceInfo(); // Create a simple program with just this call to get some device info

        VknConfig();
        ~VknConfig();
        void destroy();
        void fillAppInfo(uint32_t apiVersion, std::string appName,
                         std::string engineName,
                         VkApplicationInfo *pNext = nullptr,
                         uint32_t applicationVersion = 0,
                         uint32_t engineVersion = 0);
        void fillInstanceCreateInfo(std::vector<const char *> &enabledLayerNames,
                                    std::vector<const char *> &enabledExtensionNames,
                                    VkInstanceCreateInfo *pNext = nullptr,
                                    VkInstanceCreateFlags flags = 0);
        VknResult createInstance();
        void selectPhysicalDevice();
        void requestQueueFamilies();
        VknResult createDevice(bool chooseAllAvailable = false);
        VknResult createRenderPass();
        std::vector<vkn::VknQueueFamily> getQueueData();
        VknDevice *getDevice() { return &m_device; }
        VknInfos *getInfos() { return &m_infos; }
        VknRenderPass *getRenderPass();
        void enableExtensions(std::vector<std::string> extensions);

    private:
        VknResultArchive m_resultArchive;
        VknInfos m_infos;
        VkInstance m_instance;
        VknDevice m_device;
        VknRenderPass m_renderPass;
        std::vector<std::string> m_instanceExtensions;

        bool m_instanceCreated{false};
        bool m_physicalDeviceSelected{false};
        bool m_queueFamiliesRequested{false};
        bool m_destroyed{false};

        void archiveResult(VknResult res);
    };
}