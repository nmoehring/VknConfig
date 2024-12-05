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
// #include "VknPipeline.hpp"
#include "VknQueueFamily.hpp"
#include "vknInfos.hpp"
#include "VknResult.hpp"

namespace vkn
{
    class VknConfig
    {
    public:
        VknConfig();
        void fillAppInfo(uint32_t apiVersion, std::string appName,
                         std::string engineName,
                         VkApplicationInfo *pNext = nullptr,
                         uint32_t applicationVersion = 0,
                         uint32_t engineVersion = 0);
        VknResult createInstance();
        VknResult createDevice(bool chooseAllAvailable = false);
        VknDevice getDevice() { return m_device; }

    private:
        VknInfos m_infos{};
        VkInstance m_instance;
        VknDevice m_device;
        // VknPipeline m_pipeline{};
        std::vector<VknResult> m_resultArchive;

        VknResult selectPhysicalDevice();

        void archiveResult(VknResult res);
    };
}