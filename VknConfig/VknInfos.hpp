// I just don't want to look at all these structs every time I look at my code.

#pragma once
#include <string>
#include <vector>
#include <iostream>

#include <vulkan/vulkan.h>

namespace vkn
{
    class VknInfos
    {
    public:
        enum checkFillFunctions
        {
            APP_INFO = 0,
            DEVICE_QUEUE_CREATE_INFO = 1,
            DEVICE_CREATE_INFO
        };

        VknInfos();
        //~VknInfos();
        void setNumQueueFamilies(int num)
        {
            for (int i = 1; i < num; ++i)
            {
                m_queueCreateInfos.push_back(this->getDefaultDeviceQueueCreateInfo());
                m_queuePriorities.push_back(1.0f);
            }
        };

        // getters
        VkApplicationInfo *getAppInfo()
        {
            if (this->checkFill(APP_INFO))
                return &m_appInfo;
            else
                return nullptr;
        };
        VkInstanceCreateInfo *getInstanceCreateInfo() { return &m_instanceCreateInfo; };
        VkDeviceQueueCreateInfo *getDeviceQueueCreateInfo(int index)
        {
            if (this->checkFill(DEVICE_QUEUE_CREATE_INFO))
                return &(m_queueCreateInfos[index]);
            else
                return nullptr;
        };
        VkDeviceCreateInfo *getDeviceCreateInfo()
        {
            if (this->checkFill(DEVICE_CREATE_INFO))
                return &m_deviceCreateInfo;
            else
                return nullptr;
        };

        // Fill
        bool checkFill(checkFillFunctions functionName);
        void fillDefaultInfos();

        void fillAppInfo(uint32_t apiVersion, std::string appName,
                         std::string engineName,
                         VkApplicationInfo *pNext = nullptr,
                         uint32_t applicationVersion = 0,
                         uint32_t engineVersion = 0);
        void fillInstanceCreateInfo(VkInstanceCreateInfo *pNext = nullptr,
                                    VkInstanceCreateFlags flags = 0,
                                    std::vector<std::string> enabledLayerNames = std::vector<std::string>{},
                                    std::vector<std::string> enabledExtensionNames = std::vector<std::string>{});
        void fillDeviceQueueCreateInfo(uint32_t queueFamilyIdx, uint32_t queueCount,
                                       VkApplicationInfo *pNext = nullptr,
                                       VkDeviceQueueCreateFlags flags = INT_MAX,
                                       float queuePriorities = -1);
        void fillDeviceCreateInfo();

    private:
        std::string m_appName = "Default App Name";
        std::string m_engineName = "Default Engine Name";
        std::vector<float> m_queuePriorities;

        // Info's
        VkApplicationInfo m_appInfo;
        VkInstanceCreateInfo m_instanceCreateInfo;
        std::vector<VkDeviceQueueCreateInfo> m_queueCreateInfos;
        VkDeviceCreateInfo m_deviceCreateInfo;

        std::vector<VkGraphicsPipelineCreateInfo> m_gfxPipelineCreateInfos;
        VkPipelineShaderStageCreateInfo m_shaderStageCreateInfo;
        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
        VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo;
        VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;

        // Defaults
        VkApplicationInfo getDefaultAppInfo();
        VkInstanceCreateInfo getDefaultInstanceCreateInfo();
        VkDeviceQueueCreateInfo getDefaultDeviceQueueCreateInfo();
        VkDeviceCreateInfo getDefaultDeviceCreateInfo();

        // Required fill checklist
        bool m_filledAppInfo{false};
        bool m_filledDeviceQueueCreateInfo{false};
        bool m_filledDeviceCreateInfo{false};
    };
}
