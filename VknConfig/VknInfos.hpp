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
            DEVICE_CREATE_INFO = 2
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

        // ============FILL DEVICE INIT INFOS===============
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


        //================FILL PIPELINE INFOS===================
        VkShaderModuleCreateInfo &fillShaderModuleCreateInfo(
            std::vector<char> &code, VkShaderModuleCreateFlags flags = VkShaderModuleCreateFlags{});
        VkWin32SurfaceCreateInfoKHR &fillWin32SurfaceCreateInfo();
        
        VkPipelineShaderStageCreateInfo &fillShaderStageCreateInfo(
            VkShaderModule module, VkShaderStageFlagBits stage,
            std::string entryPointName = "main", VkSpecializationInfo *pSpecializationInfo = nullptr,
            VkPipelineShaderStageCreateFlags flags = VkPipelineShaderStageCreateFlags{});
        VkPipelineVertexInputStateCreateInfo &fillVertexInputStateCreateInfo(
            std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions,
            std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions);
        VkPipelineInputAssemblyStateCreateInfo &fillInputAssemblyStateCreateInfo(
            VkPrimitiveTopology topology = VkPrimitiveTopology{},
            VkBool32 primitiveRestartEnable = VK_FALSE);
        VkPipelineTessellationStateCreateInfo &fillTessellationStateCreateInfo(uint32_t patchControlPoints = 0);
        VkPipelineViewportStateCreateInfo &fillViewportStateCreateInfo(
            std::vector<VkViewport> viewports, std::vector<VkRect2D> scissors);
        VkPipelineRasterizationStateCreateInfo &fillRasterizationStateCreateInfo(
            VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace,
            float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor,
            float lineWidth = 0, VkBool32 depthClampEnable = VK_FALSE,
            VkBool32 rasterizerDiscardEnable = VK_FALSE, VkBool32 depthBiasEnable = VK_FALSE);
        VkPipelineMultisampleStateCreateInfo &fillMultisampleStateCreateInfo(
            float minSampleShading, VkSampleMask *pSampleMask,
            VkSampleCountFlagBits rasterizationSamples = VkSampleCountFlagBits{},
            VkBool32 sampleShadingEnable = VK_FALSE,
            VkBool32 alphaToCoverageEnable = VK_FALSE, VkBool32 alphaToOneEnable = VK_FALSE);
        VkPipelineDepthStencilStateCreateInfo &fillDepthStencilStateCreateInfo(
            VkCompareOp depthCompareOp, VkStencilOpState front, VkStencilOpState back,
            float minDepthBounds, float maxDepthBounds,
            VkPipelineDepthStencilStateCreateFlags flags = VkPipelineDepthStencilStateCreateFlags{},
            VkBool32 depthTestEnable = VK_FALSE,
            VkBool32 depthWriteEnable = VK_FALSE,
            VkBool32 depthBoundsTestEnable = VK_FALSE, VkBool32 stencilTestEnable = VK_FALSE);
        VkPipelineColorBlendStateCreateInfo &fillColorBlendStateCreateInfo(
            VkLogicOp logicOp, std::vector<VkPipelineColorBlendAttachmentState> attachments,
            float blendConstants[4], VkBool32 logicOpEnable = VK_FALSE,
            VkPipelineColorBlendStateCreateFlags flags = VkPipelineColorBlendStateCreateFlags{});
        VkPipelineDynamicStateCreateInfo &fillDynamicStateCreateInfo(std::vector<VkDynamicState> dynamicStates);
        VkGraphicsPipelineCreateInfo &fillGfxPipelineCreateInfo(
            std::vector<VkPipelineShaderStageCreateInfo> &stages,
            VkPipelineLayout layout = VkPipelineLayout{},
            VkRenderPass renderPass = VkRenderPass{}, uint32_t subpass = uint32_t{},
            VkPipeline basePipelineHandle = VkPipeline{}, int32_t basePipelineIndex = int32_t{},
            VkPipelineCreateFlags flags = VkPipelineCreateFlags{},
            VkPipelineVertexInputStateCreateInfo *pVertexInputState = nullptr,
            VkPipelineInputAssemblyStateCreateInfo *pInputAssemblyState = nullptr,
            VkPipelineTessellationStateCreateInfo *pTessellationState = nullptr,
            VkPipelineViewportStateCreateInfo *pViewportState = nullptr,
            VkPipelineRasterizationStateCreateInfo *pRasterizationState = nullptr,
            VkPipelineMultisampleStateCreateInfo *pMultisampleState = nullptr,
            VkPipelineDepthStencilStateCreateInfo *pDepthStencilState = nullptr,
            VkPipelineColorBlendStateCreateInfo *pColorBlendState = nullptr,
            VkPipelineDynamicStateCreateInfo *pDynamicState = nullptr);
        VkPipelineLayoutCreateInfo &fillPipelineLayoutCreateInfo(
            std::vector<VkDescriptorSetLayout> setLayouts = std::vector<VkDescriptorSetLayout>{},
            std::vector<VkPushConstantRange> pushConstantRanges = std::vector<VkPushConstantRange>{},
            VkPipelineLayoutCreateFlags flags = VkPipelineLayoutCreateFlags{});
        VkPipelineCacheCreateInfo &fillPipelineCacheCreateInfo(
            size_t initialDataSize = 0,
            const void *pInitialData = nullptr,
            VkPipelineCacheCreateFlags flags = VkPipelineCacheCreateFlags{});

    private:
        std::string m_appName = "Default App Name";
        std::string m_engineName = "Default Engine Name";
        std::vector<float> m_queuePriorities;

        // Info's
        VkApplicationInfo m_appInfo;
        VkInstanceCreateInfo m_instanceCreateInfo;
        std::vector<VkDeviceQueueCreateInfo> m_queueCreateInfos;
        VkDeviceCreateInfo m_deviceCreateInfo;

        std::vector<VkPipelineLayoutCreateInfo> m_layoutCreateInfos;
        std::vector<VkPipelineCacheCreateInfo> m_cacheCreateInfos;
        std::vector<VkShaderModuleCreateInfo> m_shaderModuleCreateInfos;
        std::vector<VkPipelineShaderStageCreateInfo> m_shaderStageCreateInfos;
        std::vector<VkPipelineVertexInputStateCreateInfo> m_vertexInputStateCreateInfos;
        std::vector<VkPipelineInputAssemblyStateCreateInfo> m_inputAssemblyStateCreateInfos;
        std::vector<VkPipelineTessellationStateCreateInfo> m_tessellationStateCreateInfos;
        std::vector<VkPipelineViewportStateCreateInfo> m_viewportStateCreateInfos;
        std::vector<VkPipelineRasterizationStateCreateInfo> m_rasterizationStateCreateInfos;
        std::vector<VkPipelineMultisampleStateCreateInfo> m_multisampleStateCreateInfos;
        std::vector<VkPipelineDepthStencilStateCreateInfo> m_depthStencilStateCreateInfos;
        std::vector<VkPipelineColorBlendStateCreateInfo> m_colorBlendStateCreateInfos;
        std::vector<VkPipelineDynamicStateCreateInfo> m_dynamicStateCreateInfos;
        std::vector<VkGraphicsPipelineCreateInfo> m_gfxPipelineCreateInfos;

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
