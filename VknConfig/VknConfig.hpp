// I decided to use a Vkn prefix to stand apart from Vk prefixes of the API
// All the structs are in info.hpp
// Interface: need to select a physical device,
//           need to select queue families

#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <stdexcept>
#include <vulkan/vulkan.h>

#include "VknResult.hpp"
#include "VknInfos.hpp"
#include "VknDevice.hpp"

namespace vkn
{
    class VknConfig
    {
    public:
        void deviceInfo(uint32_t deviceIdx); // Create a simple program with just this call to get some device info

        VknConfig();
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
        VknResult createInstance();
        void selectPhysicalDevice(uint32_t deviceIndex);
        void requestQueueFamilies(uint32_t deviceIndex);
        VknResult createDevice(uint32_t deviceIndex, bool chooseAllAvailable = false);
        VknResult createRenderPass();
        std::vector<vkn::VknQueueFamily> getQueueData();
        VknDevice *getDevice(uint32_t index) { return &(m_devices[index]); }
        VknInfos *getInfos() { return &m_infos; }
        VkInstance *getInstance() { return &m_instance; }
        VknRenderPass *getRenderPass(uint32_t deviceIdx, uint32_t renderPassIdx);
        void enableExtensions(std::vector<std::string> extensions);
        bool getInstanceCreated() { return m_instanceCreated; }
        void selectQueues(uint32_t deviceIdx, bool chooseAllAvailableQueues = false);

    private:
        VknResultArchive m_resultArchive;
        VknInfos m_infos;
        VkInstance m_instance;
        std::vector<VknDevice> m_devices;

        std::vector<std::string> m_instanceExtensions;

        bool m_instanceCreated{false};
        bool m_physicalDeviceSelected{false};
        bool m_queueFamiliesRequested{false};
        bool m_queuesSelected{false};
        bool m_destroyed{false};

        void archiveResult(VknResult res);
    };
}