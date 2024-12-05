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
        //~VknConfig();

        void createDevice(bool chooseAllAvailable = false);
        VknDevice getDevice() { return m_device; }

    private:
        VknInfos m_infos{};
        VkInstance m_instance;
        VknDevice m_device;
        // VknPipeline m_pipeline{};
        std::vector<VknResult> m_resultArchive;

        VknResult createInstance();
        VknResult selectPhysicalDevice();

        void archiveResult(VknResult res);
    };
}