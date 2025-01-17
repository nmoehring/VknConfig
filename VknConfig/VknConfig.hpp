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
#include "VknPipeline.hpp"
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
        void fillAppInfo(uint32_t apiVersion, std::string appName,
                         std::string engineName,
                         VkApplicationInfo *pNext = nullptr,
                         uint32_t applicationVersion = 0,
                         uint32_t engineVersion = 0);
        VknResult createInstance();
        VknResult createDevice(bool chooseAllAvailable = false);
        std::vector<vkn::VknQueueFamily> getQueueData();
        VknDevice *getDevice() { return &m_device; }
        VknInfos *getInfos() { return &m_infos; }
        VknPipeline *getPipeline() { return &m_pipeline; }

    private:
        VknResultArchive m_resultArchive{};
        VknInfos m_infos{};
        VkInstance m_instance{};
        VknDevice m_device{};
        VknPipeline m_pipeline{&m_device, &m_infos, &m_resultArchive};

        void archiveResult(VknResult res);
    };
}