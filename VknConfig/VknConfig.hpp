// I decided to use a Vkn prefix to stand apart from Vk prefixes of the API
// All the structs are in info.hpp

#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

#include "vknInfos.hpp"
#include "VknResult.hpp"

namespace vkn
{
    class VknConfig
    {
    public:
        VknConfig();
        ~VknConfig();

    private:
        VknInfos m_infos{};

        VkInstance m_instance;
        std::vector<VkPhysicalDevice> m_physicalDevices;
        int m_idxSelectedPhysicalDevice;
        VkQueueFamilyProperties m_queueFamilyProperties;
        VkDevice m_device;
        std::vector<VknResult> m_resultArchive;

        void archiveResult(VknResult res);
        void runVkFunction(std::function<VknResult()> func, std::string errMsg);

        VknResult createInstance();
        VknResult selectPhysicalDevice();
        VknResult getQueueFamilyProperties();
        VknResult createDevice();
    };
}