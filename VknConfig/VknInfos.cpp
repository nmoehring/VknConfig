#include <stdexcept>
#include <limits>

#include "VknInfos.hpp"
#include <vulkan/vulkan.h>

namespace vkn
{
    VknInfos::VknInfos()
    {
        m_queuePriorities.push_back(1.0f);
        fillDefaultInfos();
    }

    void VknInfos::fillDefaultInfos()
    {
        m_appInfo = this->getDefaultAppInfo();
        m_instanceCreateInfo = this->getDefaultInstanceCreateInfo();
        m_queueCreateInfos.push_back(this->getDefaultDeviceQueueCreateInfo());
        m_deviceCreateInfo = this->getDefaultDeviceCreateInfo();
    }

    VkApplicationInfo VknInfos::getDefaultAppInfo()
    {
        VkApplicationInfo info;
        info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        info.pNext = nullptr;
        info.pApplicationName = m_appName.c_str();
        info.applicationVersion = 0;
        info.pEngineName = m_engineName.c_str();
        info.engineVersion = 0;
        // Vulkan 1.0 only compatible with 1.0 (maybe stick with 1.1 at least)
        info.apiVersion = VK_API_VERSION_1_1;
        return info;
    }

    VkInstanceCreateInfo VknInfos::getDefaultInstanceCreateInfo()
    {
        VkInstanceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = VkInstanceCreateFlagBits{};
        info.pApplicationInfo = &m_appInfo;
        info.enabledLayerCount = 0;
        info.ppEnabledLayerNames = nullptr;
        info.enabledExtensionCount = 0;
        info.ppEnabledExtensionNames = nullptr;
        return info;
    }

    VkDeviceQueueCreateInfo VknInfos::getDefaultDeviceQueueCreateInfo()
    {
        VkDeviceQueueCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = VkDeviceQueueCreateFlags{}; // Only flag is a protected memory bit, for a queue family that supports it
        info.queueFamilyIndex = 0;
        info.queueCount = 1;
        info.pQueuePriorities = &(m_queuePriorities[0]);
        return info;
    }

    VkDeviceCreateInfo VknInfos::getDefaultDeviceCreateInfo()
    {
        VkDeviceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0; // flags reserved, must = 0
        info.queueCreateInfoCount = 1;
        info.pQueueCreateInfos = nullptr;
        // enabledLayerCount is deprecated and should not be used
        info.enabledLayerCount = 0; // ignored, value doesn't matter
        // ppEnabledLayerNames is deprecated and should not be used
        info.ppEnabledLayerNames = nullptr; // ignored, value doesn't matter
        info.enabledExtensionCount = 0;
        info.ppEnabledExtensionNames = nullptr;
        info.pEnabledFeatures = nullptr;
        return info;
    }

    VkGraphicsPipelineCreateInfo &VknInfos::fillGfxPipelineCreateInfo(
        std::vector<VkPipelineShaderStageCreateInfo> &stages,
        VkPipelineLayout layout, VkRenderPass renderPass, uint32_t subpass,
        VkPipeline basePipelineHandle, int32_t basePipelineIndex,
        VkPipelineCreateFlags flags,
        VkPipelineVertexInputStateCreateInfo *pVertexInputState,
        VkPipelineInputAssemblyStateCreateInfo *pInputAssemblyState,
        VkPipelineTessellationStateCreateInfo *pTessellationState,
        VkPipelineViewportStateCreateInfo *pViewportState,
        VkPipelineRasterizationStateCreateInfo *pRasterizationState,
        VkPipelineMultisampleStateCreateInfo *pMultisampleState,
        VkPipelineDepthStencilStateCreateInfo *pDepthStencilState,
        VkPipelineColorBlendStateCreateInfo *pColorBlendState,
        VkPipelineDynamicStateCreateInfo *pDynamicState)
    {
        m_gfxPipelineCreateInfos.push_back(VkGraphicsPipelineCreateInfo{});
        VkGraphicsPipelineCreateInfo &info = m_gfxPipelineCreateInfos.back();
        info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        info.stageCount = stages.size();                // Need fill
        info.pStages = stages.data();                   // Need fill (other struct)
        info.pVertexInputState = pVertexInputState;     // Can be null if VK_DYNAMIC_STATE_VERTEX_INPUT_EXT
                                                        // set in pDynamicState below. Ignored if mesh shader stage
                                                        // included in pStages.
        info.pInputAssemblyState = pInputAssemblyState; // Can be null if VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE
                                                        // and VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY are set in pDynamicState,
                                                        // below and dynamicPrimitiveTopologyUnrestricted is VK_TRUE in the
                                                        // VkPhysicalDeviceExtendedDynamicState3PropertiesEXT struct,
                                                        // which requires the VK_EXT_extended_dynamic_state3 extension.
                                                        // Also ignored if mesh shader stage included in pStages.
        info.pTessellationState = pTessellationState;   // Can be null if the VK_DYNAMIC_STATE_PATCH_CONTROL_POINTS_EXT
                                                        // is set in the pDynamicState below.
        info.pViewportState = pViewportState;           // Can be null if rasterization not enabled.
                                                        // Can be null if VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT
                                                        // and VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT are set in pDynamicState
                                                        // below, and also requires the VK_EXT_extended_dynamic_state3
                                                        // extension enabled.
        info.pRasterizationState = pRasterizationState; // I guess this could be null if rasterization is not enabled.
                                                        // Can be null if VK_DYNAMIC_STATE_DEPTH_CLAMP_ENABLE_EXT,
                                                        // VK_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE,
                                                        // VK_DYNAMIC_STATE_POLYGON_MODE_EXT,
                                                        // VK_DYNAMIC_STATE_CULL_MODE, VK_DYNAMIC_STATE_FRONT_FACE,
                                                        // VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE, VK_DYNAMIC_STATE_DEPTH_BIAS,
                                                        // and VK_DYNAMIC_STATE_LINE_WIDTH are set in pDynamicState below,
                                                        // which also requires the VK_EXT_extended_dynamic_state3
                                                        // extension enabled.
        info.pMultisampleState = pMultisampleState;     // Can be null if rasterization not enabled.
                                                        //  Can be null if VK_DYNAMIC_STATE_RASTERIZATION_SAMPLES_EXT,
                                                        //  VK_DYNAMIC_STATE_SAMPLE_MASK_EXT, and
                                                        //  VK_DYNAMIC_STATE_ALPHA_TO_COVERAGE_ENABLE_EXT are set in
                                                        //  pDynamicState below, and
                                                        //  ((alphaToOne feature not enabled)
                                                        //  or
                                                        //  (VK_DYNAMIC_STATE_ALPHA_TO_ONE_ENABLE_EXT is set and
                                                        //  VkPipelineMultisampleStateCreateInfo::sampleShadingEnable
                                                        //  is VK_False).
                                                        //   VK_EXT_extended_dynamic_state3 extension needs to be enabled
                                                        //   to do this.
        info.pDepthStencilState = pDepthStencilState;   // Can be null if rasterization not enabled.
                                                        // Can be null if VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE,
                                                        // VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE, VK_DYNAMIC_STATE_DEPTH_COMPARE_OP,
                                                        // VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE,
                                                        // VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE, VK_DYNAMIC_STATE_STENCIL_OP,
                                                        // and VK_DYNAMIC_STATE_DEPTH_BOUNDS are set in pDynamicState below
                                                        // Also requires the  VK_EXT_extended_dynamic_state3 extension.
        info.pColorBlendState = pColorBlendState;       // Can be null if rasterization not enabled.
                                                        // Can be null if VK_DYNAMIC_STATE_LOGIC_OP_ENABLE_EXT,
                                                        // VK_DYNAMIC_STATE_LOGIC_OP_EXT,
                                                        // VK_DYNAMIC_STATE_COLOR_BLEND_ENABLE_EXT,
                                                        // VK_DYNAMIC_STATE_COLOR_BLEND_EQUATION_EXT,
                                                        // VK_DYNAMIC_STATE_COLOR_WRITE_MASK_EXT, and
                                                        // VK_DYNAMIC_STATE_BLEND_CONSTANTS are set in pDynamicState below
                                                        // Requires the VK_EXT_extended_dynamic_state3 extension.
        info.pDynamicState = pDynamicState;             // Could be null if no state in the pipeline needs to be dynamic.
        info.layout = layout;                           // Need fill
        info.renderPass = renderPass;                   // Need fill
        info.subpass = subpass;                         // Need fill
        info.basePipelineHandle = basePipelineHandle;   // Need fill
        info.basePipelineIndex = basePipelineIndex;     // Need fill

        return info;
    }

    VkRenderPassCreateInfo &fillRenderPassCreateInfo(
        std::vector<VkAttachmentDescription> attachments,
        std::vector<VkSubpassDescription> subpasses,
        std::vector<VkSubpassDependency> dependencies,
        VkRenderPassCreateFlags flags = VkRenderPassCreateFlags{})
    {
        m_renderPassCreateInfos.push_back(VkRenderPassCreateInfo{});
        VkRenderPassCreateInfo &renderPassInfo = m_renderPassCreateInfos.back();

        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.pNext = nullptr;
        renderPassInfo.flags = flags;
        renderPassInfo.attachmentCount = attachments.size();
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = subpasses.size();
        renderPassInfo.pSubpasses = subpasses.data();
        renderPassInfo.dependencyCount = dependencies.size();
        renderPassInfo.pDependencies = dependencies.data();
    }

    VkRenderPassCreateInfo &fillDefaultRenderPass(
        std::vector<VkAttachmentDescription> attachments,
        std::vector<VkSubpassDescription> subpasses,
        std::vector<VkSubpassDependency> dependencies)
    {
        m_renderPassCreateInfos.push_back(VkRenderPassCreateInfo{});
        VkRenderPassCreateInfo &info = m_renderPassCreateInfos.back();

        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = attachments.size();
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = subpasses.size();
        renderPassInfo.pSubpasses = subpasses.data();
        renderPassInfo.dependencyCount = dependencies.size();
        renderPassInfo.pDependencies = dependencies.data();
    }

    VkShaderModuleCreateInfo &VknInfos::fillShaderModuleCreateInfo(
        std::vector<char> &code, VkShaderModuleCreateFlags flags)
    {
        m_shaderModuleCreateInfos.push_back(VkShaderModuleCreateInfo{});
        VkShaderModuleCreateInfo &info = m_shaderModuleCreateInfos.back();
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        info.codeSize = code.size();
        info.pCode = reinterpret_cast<const uint32_t *>(code.data());
        return info;
    }

    VkPipelineShaderStageCreateInfo &VknInfos::fillShaderStageCreateInfo(
        VkShaderModule module, VkShaderStageFlagBits stage,
        VkSpecializationInfo *pSpecializationInfo, VkPipelineShaderStageCreateFlags flags)
    {
        m_shaderStageCreateInfos.push_back(VkPipelineShaderStageCreateInfo{});
        VkPipelineShaderStageCreateInfo &info = m_shaderStageCreateInfos.back();
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags; // need fill
        info.stage = stage; // need fill
        info.module = module;
        info.pName = m_mainEntry;
        info.pSpecializationInfo = pSpecializationInfo; // need fill
        return info;
    }

    VkPipelineVertexInputStateCreateInfo &VknInfos::fillVertexInputStateCreateInfo(
        std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions,
        std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions)
    {
        m_vertexInputStateCreateInfos.push_back(VkPipelineVertexInputStateCreateInfo{});
        VkPipelineVertexInputStateCreateInfo &info = m_vertexInputStateCreateInfos.back();
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0; // reserved for future use
        info.vertexBindingDescriptionCount = vertexBindingDescriptions.size();
        info.pVertexBindingDescriptions = vertexBindingDescriptions.data();
        info.vertexAttributeDescriptionCount = vertexAttributeDescriptions.size();
        info.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
        return info;
    }

    VkPipelineVertexInputStateCreateInfo &VknInfos::fillDefaultVertexInputState()
    {
        std::vector<VkVertexInputBindingDescription> vertexInputBinding;
        vertexInputBinding.push_back(VkVertexInputBindingDescription{});
        vertexInputBinding[0].binding = 0;
        vertexInputBinding[0].stride = 0; // No actual vertex data
        vertexInputBinding[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        // Minimal vertex input attribute description
        std::vector<VkVertexInputAttributeDescription> vertexInputAttribute;
        vertexInputAttribute.push_back(VkVertexInputAttributeDescription{});
        vertexInputAttribute[0].binding = 0;
        vertexInputAttribute[0].location = 0;
        vertexInputAttribute[0].format = VK_FORMAT_UNDEFINED;
        vertexInputAttribute[0].offset = 0;

        return this->fillVertexInputStateCreateInfo(vertexInputBinding, vertexInputAttribute);
    }

    VkPipelineInputAssemblyStateCreateInfo &VknInfos::fillInputAssemblyStateCreateInfo(
        VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable)
    {
        m_inputAssemblyStateCreateInfos.push_back(VkPipelineInputAssemblyStateCreateInfo{});
        VkPipelineInputAssemblyStateCreateInfo &info = m_inputAssemblyStateCreateInfos.back();
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0; // reserved for future use
        info.topology = topology;
        info.primitiveRestartEnable = primitiveRestartEnable;
        return info;
    }

    VkPipelineTessellationStateCreateInfo &VknInfos::fillTessellationStateCreateInfo(uint32_t patchControlPoints)
    {
        m_tessellationStateCreateInfos.push_back(VkPipelineTessellationStateCreateInfo{});
        VkPipelineTessellationStateCreateInfo &info = m_tessellationStateCreateInfos.back();
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0; // reserved for future use
        info.patchControlPoints = patchControlPoints;
        return info;
    }

    VkPipelineViewportStateCreateInfo &VknInfos::fillViewportStateCreateInfo(
        std::vector<VkViewport> viewports, std::vector<VkRect2D> scissors)
    {
        m_viewportStateCreateInfos.push_back(VkPipelineViewportStateCreateInfo{});
        VkPipelineViewportStateCreateInfo &info = m_viewportStateCreateInfos.back();
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0; // reserved for future use
        info.viewportCount = viewports.size();
        info.pViewports = viewports.data();
        info.scissorCount = scissors.size();
        info.pScissors = scissors.data();
        return info;
    }

    VkPipelineRasterizationStateCreateInfo &VknInfos::fillRasterizationStateCreateInfo(
        VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace,
        float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor,
        float lineWidth, VkBool32 depthClampEnable,
        VkBool32 rasterizerDiscardEnable, VkBool32 depthBiasEnable)
    {
        m_rasterizationStateCreateInfos.push_back(VkPipelineRasterizationStateCreateInfo{});
        VkPipelineRasterizationStateCreateInfo &info = m_rasterizationStateCreateInfos.back();
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0; // reserved for future use
        info.depthClampEnable = depthClampEnable;
        info.rasterizerDiscardEnable = rasterizerDiscardEnable;
        info.polygonMode = polygonMode;
        info.cullMode = cullMode;
        info.frontFace = frontFace;
        info.depthBiasEnable = depthBiasEnable;
        info.depthBiasConstantFactor = depthBiasConstantFactor;
        info.depthBiasClamp = depthBiasClamp;
        info.depthBiasSlopeFactor = depthBiasSlopeFactor;
        info.lineWidth = lineWidth;
        return info;
    }

    VkPipelineMultisampleStateCreateInfo &VknInfos::fillMultisampleStateCreateInfo(
        float minSampleShading, VkSampleMask *pSampleMask,
        VkSampleCountFlagBits rasterizationSamples,
        VkBool32 sampleShadingEnable, VkBool32 alphaToCoverageEnable,
        VkBool32 alphaToOneEnable)
    {
        m_multisampleStateCreateInfos.push_back(VkPipelineMultisampleStateCreateInfo{});
        VkPipelineMultisampleStateCreateInfo &info = m_multisampleStateCreateInfos.back();
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0; // reserved for future use
        info.rasterizationSamples = rasterizationSamples;
        info.sampleShadingEnable = sampleShadingEnable;
        info.minSampleShading = minSampleShading;
        info.pSampleMask = pSampleMask;
        info.alphaToCoverageEnable = alphaToCoverageEnable;
        info.alphaToOneEnable = alphaToOneEnable;
        return info;
    }

    VkPipelineDepthStencilStateCreateInfo &VknInfos::fillDepthStencilStateCreateInfo(
        VkCompareOp depthCompareOp, VkStencilOpState front, VkStencilOpState back,
        float minDepthBounds, float maxDepthBounds,
        VkPipelineDepthStencilStateCreateFlags flags,
        VkBool32 depthTestEnable, VkBool32 depthWriteEnable,
        VkBool32 depthBoundsTestEnable, VkBool32 stencilTestEnable)
    {
        m_depthStencilStateCreateInfos.push_back(VkPipelineDepthStencilStateCreateInfo{});
        VkPipelineDepthStencilStateCreateInfo &info = m_depthStencilStateCreateInfos.back();
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        info.depthTestEnable = depthTestEnable;
        info.depthWriteEnable = depthWriteEnable;
        info.depthCompareOp = depthCompareOp;
        info.depthBoundsTestEnable = depthBoundsTestEnable;
        info.stencilTestEnable = stencilTestEnable;
        info.front = front;
        info.back = back;
        info.minDepthBounds = minDepthBounds;
        info.maxDepthBounds = maxDepthBounds;
        return info;
    }

    VkPipelineColorBlendStateCreateInfo &VknInfos::fillColorBlendStateCreateInfo(
        VkLogicOp logicOp, std::vector<VkPipelineColorBlendAttachmentState> attachments,
        float blendConstants[4], VkBool32 logicOpEnable,
        VkPipelineColorBlendStateCreateFlags flags)
    {
        m_colorBlendStateCreateInfos.push_back(VkPipelineColorBlendStateCreateInfo{});
        VkPipelineColorBlendStateCreateInfo &info = m_colorBlendStateCreateInfos.back();
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        info.logicOpEnable = logicOpEnable;
        info.logicOp = logicOp;
        info.attachmentCount = attachments.size();
        info.pAttachments = attachments.data();
        info.blendConstants[4] = {};
        return info;
    }

    VkPipelineDynamicStateCreateInfo &VknInfos::fillDynamicStateCreateInfo(std::vector<VkDynamicState> dynamicStates)
    {
        m_dynamicStateCreateInfos.push_back(VkPipelineDynamicStateCreateInfo{});
        VkPipelineDynamicStateCreateInfo &info = m_dynamicStateCreateInfos.back();
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0; // reserved for future use
        info.dynamicStateCount = dynamicStates.size();
        info.pDynamicStates = dynamicStates.data();
        return info;
    }

    VkDescriptorSetLayoutCreateInfo &VknInfos::fillDescriptorSetLayoutCreateInfo(
        std::vector<VkDescriptorSetLayoutBinding> bindings, VkDescriptorSetLayoutCreateFlags flags)
    {
        m_descriptorSetLayoutCreateInfos.push_back(VkDescriptorSetLayoutCreateInfo{});
        VkDescriptorSetLayoutCreateInfo &info = m_descriptorSetLayoutCreateInfos.back();
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        info.bindingCount = bindings.size();
        info.pBindings = bindings.data();
        return info;
    }

    VkPipelineLayoutCreateInfo &VknInfos::fillPipelineLayoutCreateInfo(
        std::vector<VkDescriptorSetLayout> setLayouts,
        std::vector<VkPushConstantRange> pushConstantRanges,
        VkPipelineLayoutCreateFlags flags)
    {
        m_layoutCreateInfos.push_back(VkPipelineLayoutCreateInfo{});
        VkPipelineLayoutCreateInfo &info = m_layoutCreateInfos.back();
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        info.setLayoutCount = setLayouts.size();
        info.pSetLayouts = setLayouts.data();
        info.pushConstantRangeCount = pushConstantRanges.size();
        info.pPushConstantRanges = pushConstantRanges.data();

        return info;
    }

    VkPipelineCacheCreateInfo &VknInfos::fillPipelineCacheCreateInfo(
        size_t initialDataSize,
        const void *pInitialData,
        VkPipelineCacheCreateFlags flags)
    {
        m_cacheCreateInfos.push_back(VkPipelineCacheCreateInfo{});
        VkPipelineCacheCreateInfo &info = m_cacheCreateInfos.back();
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        info.initialDataSize = initialDataSize;
        info.pInitialData = pInitialData;

        return info;
    }

    bool VknInfos::checkFill(checkFillFunctions functionName)
    {
        switch (functionName)
        {
        case APP_INFO:
            if (!m_filledAppInfo)
                throw std::runtime_error("AppInfo not filled before get.");
        case DEVICE_QUEUE_CREATE_INFO:
            if (!m_filledDeviceQueueCreateInfo)
                throw std::runtime_error("DeviceQueueCreateInfo not filled before get.");
        case DEVICE_CREATE_INFO:
            if (!m_filledDeviceCreateInfo)
                throw std::runtime_error("DeviceCreateInfo not filled before get.");
        }
        return true;
    }

    void VknInfos::fillAppInfo(uint32_t apiVersion, std::string appName,
                               std::string engineName, VkApplicationInfo *pNext,
                               uint32_t applicationVersion, uint32_t engineVersion)
    {
        m_appName = appName;
        m_engineName = engineName;

        m_appInfo.pApplicationName = m_appName.c_str();
        m_appInfo.pEngineName = m_engineName.c_str();

        m_appInfo.pNext = nullptr;
        m_appInfo.pApplicationName = m_appName.c_str();
        m_appInfo.applicationVersion = 0;
        m_appInfo.pEngineName = m_engineName.c_str();
        m_appInfo.engineVersion = 0;
        // Vulkan 1.0 only compatible with 1.0 (maybe stick with 1.1 at least)
        m_appInfo.apiVersion = VK_API_VERSION_1_1;

        m_filledAppInfo = true;
        this->fillInstanceCreateInfo();
    }

    void VknInfos::fillInstanceCreateInfo(VkInstanceCreateInfo *pNext,
                                          VkInstanceCreateFlags flags,
                                          std::vector<std::string> enabledLayerNames,
                                          std::vector<std::string> enabledExtensionNames)
    {
        m_instanceCreateInfo.pNext = nullptr;
        m_instanceCreateInfo.flags = VkInstanceCreateFlagBits{};
        m_instanceCreateInfo.pApplicationInfo = &m_appInfo;
        m_instanceCreateInfo.enabledLayerCount = enabledLayerNames.size();
        std::vector<const char *> layerNames;
        for (auto name : enabledLayerNames)
            layerNames.push_back(name.c_str());
        m_instanceCreateInfo.ppEnabledLayerNames = layerNames.data();
        m_instanceCreateInfo.enabledExtensionCount = enabledExtensionNames.size();
        std::vector<const char *> extNames;
        for (auto name : enabledExtensionNames)
            extNames.push_back(name.c_str());
        m_instanceCreateInfo.ppEnabledExtensionNames = extNames.data();
    }

    void VknInfos::fillDeviceQueueCreateInfo(uint32_t queueFamilyIdx, uint32_t queueCount,
                                             VkApplicationInfo *pNext,
                                             VkDeviceQueueCreateFlags flags,
                                             float queuePriorities)
    {
        m_queueCreateInfos[queueFamilyIdx].queueFamilyIndex = queueFamilyIdx;
        if (pNext != nullptr)
            m_queueCreateInfos[queueFamilyIdx].pNext = pNext;
        if (flags != INT_MAX)
            m_queueCreateInfos[queueFamilyIdx].flags = flags; // Only flag is a protected memory bit, for a queue family that supports it
        m_queueCreateInfos[queueFamilyIdx].queueCount = queueCount;
        if (queuePriorities != -1)
            m_queuePriorities[queueFamilyIdx] = queuePriorities;
        m_queueCreateInfos[queueFamilyIdx].pQueuePriorities = &(m_queuePriorities[queueFamilyIdx]);
        if (queueFamilyIdx == (m_queueCreateInfos.size() - 1))
        {
            m_filledDeviceQueueCreateInfo = true;
            this->fillDeviceCreateInfo();
        }
    }

    void VknInfos::fillDeviceCreateInfo()
    {
        m_deviceCreateInfo.queueCreateInfoCount = m_queueCreateInfos.size();
        m_deviceCreateInfo.pQueueCreateInfos = m_queueCreateInfos.data();
        m_filledDeviceCreateInfo = true;
        // m_deviceCreateInfo.enabledExtensionCount = 0;
        // m_deviceCreateInfo.ppEnabledExtensionNames = nullptr;
        // m_deviceCreateInfo.pEnabledFeatures = nullptr;
    }

}