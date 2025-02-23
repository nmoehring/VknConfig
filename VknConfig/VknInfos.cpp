#include <stdexcept>
#include <limits>
#include <algorithm>

#include "VknInfos.hpp"
#include <vulkan/vulkan.h>

namespace vkn
{
    VknInfos::VknInfos()
    {
    }

    VkGraphicsPipelineCreateInfo *VknInfos::fillGfxPipelineCreateInfo(
        uint32_t pipelineIdx,
        std::vector<VkPipelineShaderStageCreateInfo *> &stages,
        VkPipelineLayout *layout, VkRenderPass *renderPass, uint32_t subpass,
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
        info.stageCount = m_shaderStageCreateInfos[pipelineIdx].size(); // Need fill
        info.pStages = m_shaderStageCreateInfos[pipelineIdx].data();    // Need fill (other struct)
        info.pVertexInputState = pVertexInputState;                     // Can be null if VK_DYNAMIC_STATE_VERTEX_INPUT_EXT
                                                                        // set in pDynamicState below. Ignored if mesh shader stage
                                                                        // included in pStages.
        info.pInputAssemblyState = pInputAssemblyState;                 // Can be null if VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE
                                                                        // and VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY are set in pDynamicState,
                                                                        // below and dynamicPrimitiveTopologyUnrestricted is VK_TRUE in the
                                                                        // VkPhysicalDeviceExtendedDynamicState3PropertiesEXT struct,
                                                                        // which requires the VK_EXT_extended_dynamic_state3 extension.
                                                                        // Also ignored if mesh shader stage included in pStages.
        info.pTessellationState = pTessellationState;                   // Can be null if the VK_DYNAMIC_STATE_PATCH_CONTROL_POINTS_EXT
                                                                        // is set in the pDynamicState below.
        info.pViewportState = pViewportState;                           // Can be null if rasterization not enabled.
                                                                        // Can be null if VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT
                                                                        // and VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT are set in pDynamicState
                                                                        // below, and also requires the VK_EXT_extended_dynamic_state3
                                                                        // extension enabled.
        info.pRasterizationState = pRasterizationState;                 // I guess this could be null if rasterization is not enabled.
                                                                        // Can be null if VK_DYNAMIC_STATE_DEPTH_CLAMP_ENABLE_EXT,
                                                                        // VK_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE,
                                                                        // VK_DYNAMIC_STATE_POLYGON_MODE_EXT,
                                                                        // VK_DYNAMIC_STATE_CULL_MODE, VK_DYNAMIC_STATE_FRONT_FACE,
                                                                        // VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE, VK_DYNAMIC_STATE_DEPTH_BIAS,
                                                                        // and VK_DYNAMIC_STATE_LINE_WIDTH are set in pDynamicState below,
                                                                        // which also requires the VK_EXT_extended_dynamic_state3
                                                                        // extension enabled.
        info.pMultisampleState = pMultisampleState;                     // Can be null if rasterization not enabled.
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
        info.pDepthStencilState = pDepthStencilState;                   // Can be null if rasterization not enabled.
                                                                        // Can be null if VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE,
                                                                        // VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE, VK_DYNAMIC_STATE_DEPTH_COMPARE_OP,
                                                                        // VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE,
                                                                        // VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE, VK_DYNAMIC_STATE_STENCIL_OP,
                                                                        // and VK_DYNAMIC_STATE_DEPTH_BOUNDS are set in pDynamicState below
                                                                        // Also requires the  VK_EXT_extended_dynamic_state3 extension.
        info.pColorBlendState = pColorBlendState;                       // Can be null if rasterization not enabled.
                                                                        // Can be null if VK_DYNAMIC_STATE_LOGIC_OP_ENABLE_EXT,
                                                                        // VK_DYNAMIC_STATE_LOGIC_OP_EXT,
                                                                        // VK_DYNAMIC_STATE_COLOR_BLEND_ENABLE_EXT,
                                                                        // VK_DYNAMIC_STATE_COLOR_BLEND_EQUATION_EXT,
                                                                        // VK_DYNAMIC_STATE_COLOR_WRITE_MASK_EXT, and
                                                                        // VK_DYNAMIC_STATE_BLEND_CONSTANTS are set in pDynamicState below
                                                                        // Requires the VK_EXT_extended_dynamic_state3 extension.
        info.pDynamicState = pDynamicState;                             // Could be null if no state in the pipeline needs to be dynamic.
        if (layout == nullptr)
            info.layout = VkPipelineLayout{};
        else
            info.layout = *layout; // Need fill
        if (renderPass == nullptr)
            info.renderPass = VkRenderPass{};
        else
            info.renderPass = *renderPass;            // Need fill
        info.subpass = subpass;                       // Need fill
        info.basePipelineHandle = basePipelineHandle; // Need fill
        info.basePipelineIndex = basePipelineIndex;   // Need fill

        return &info;
    }

    VkRenderPassCreateInfo *VknInfos::fillRenderPassCreateInfo(VkRenderPassCreateFlags flags)
    {
        m_renderPassCreateInfos.push_back(VkRenderPassCreateInfo{});
        VkRenderPassCreateInfo &renderPassInfo = m_renderPassCreateInfos.back();

        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.pNext = nullptr;
        renderPassInfo.flags = flags;
        renderPassInfo.attachmentCount = m_attachmentDescriptions.size();
        if (m_attachmentDescriptions.size() == 0)
            renderPassInfo.pAttachments = VK_NULL_HANDLE;
        else
            renderPassInfo.pAttachments = m_attachmentDescriptions.data();
        renderPassInfo.subpassCount = m_subpassDescriptions.size();
        if (m_subpassDescriptions.size() == 0)
            renderPassInfo.pSubpasses = VK_NULL_HANDLE;
        else
            renderPassInfo.pSubpasses = m_subpassDescriptions.data();
        renderPassInfo.dependencyCount = m_subpassDependencies.size();
        if (m_subpassDependencies.size() == 0)
            renderPassInfo.pDependencies = VK_NULL_HANDLE;
        else
            renderPassInfo.pDependencies = m_subpassDependencies.data();

        return &renderPassInfo;
    }

    VkShaderModuleCreateInfo *VknInfos::fillShaderModuleCreateInfo(
        uint32_t pipelineIdx, std::vector<char> &code, VkShaderModuleCreateFlags flags)
    {
        for (int i = 0; i < (pipelineIdx - (m_shaderModuleCreateInfos.size() - 1)); ++i)
            m_shaderModuleCreateInfos.push_back(std::vector<VkShaderModuleCreateInfo>{});
        m_shaderModuleCreateInfos[pipelineIdx].push_back(VkShaderModuleCreateInfo{});
        VkShaderModuleCreateInfo *info = &(m_shaderModuleCreateInfos[pipelineIdx].back());
        info->sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = flags;
        info->codeSize = code.size();
        info->pCode = reinterpret_cast<const uint32_t *>(code.data());
        return info;
    }

    VkPipelineShaderStageCreateInfo *VknInfos::fillShaderStageCreateInfo(
        uint32_t pipelineIdx, VkShaderModule module, VkShaderStageFlagBits stage,
        VkPipelineShaderStageCreateFlags flags, VkSpecializationInfo *pSpecializationInfo)
    {
        for (int i = 0; i < (pipelineIdx - (m_shaderStageCreateInfos.size() - 1)); ++i)
            m_shaderStageCreateInfos.push_back(std::vector<VkPipelineShaderStageCreateInfo>{});
        m_shaderStageCreateInfos[pipelineIdx].push_back(VkPipelineShaderStageCreateInfo{});
        VkPipelineShaderStageCreateInfo *info = &(m_shaderStageCreateInfos[pipelineIdx].back());
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = flags; // need fill
        info->stage = stage; // need fill
        info->module = module;
        info->pName = m_mainEntry;
        info->pSpecializationInfo = pSpecializationInfo; // need fill
        return info;
    }

    VkPipelineVertexInputStateCreateInfo *VknInfos::fillVertexInputStateCreateInfo(uint32_t pipelineIdx)
    {
        for (int i = 0; i < (pipelineIdx - (m_vertexInputStateCreateInfos.size() - 1)); ++i)
            m_vertexInputStateCreateInfos.push_back(std::vector<VkPipelineVertexInputStateCreateInfo>{});
        if ((m_vertexInputBindings.size() - 1) < pipelineIdx || m_vertexInputBindings[pipelineIdx].size() == 0)
            throw std::runtime_error("Vertex input bindings not filled before filling vertex input state create info.");
        if ((m_vertexInputAttributes.size() - 1) < pipelineIdx || m_vertexInputAttributes[pipelineIdx].size() == 0)
            throw std::runtime_error("Vertex input attributes not filled before filling vertex input state create info.");

        std::vector<VkVertexInputBindingDescription> *vertexBindingDescriptions = &(m_vertexInputBindings[pipelineIdx]);
        std::vector<VkVertexInputAttributeDescription> *vertexAttributeDescriptions = &(m_vertexInputAttributes[pipelineIdx]);

        m_vertexInputStateCreateInfos[pipelineIdx].push_back(VkPipelineVertexInputStateCreateInfo{});
        VkPipelineVertexInputStateCreateInfo *info = &(m_vertexInputStateCreateInfos[pipelineIdx].back());
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        info->pNext = VK_NULL_HANDLE;
        info->flags = 0; // reserved for future use
        info->vertexBindingDescriptionCount = vertexBindingDescriptions->size();
        if (vertexBindingDescriptions->size() == 0)
            info->pVertexBindingDescriptions = VK_NULL_HANDLE;
        else
            info->pVertexBindingDescriptions = vertexBindingDescriptions->data();
        info->vertexAttributeDescriptionCount = vertexAttributeDescriptions->size();
        if (vertexAttributeDescriptions->size() == 0)
            info->pVertexAttributeDescriptions = VK_NULL_HANDLE;
        else
            info->pVertexAttributeDescriptions = vertexAttributeDescriptions->data();
        return info;
    }

    VkPipelineInputAssemblyStateCreateInfo *VknInfos::fillInputAssemblyStateCreateInfo(
        uint32_t pipelineIdx, VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable)
    {
        for (int i = 0; i < (pipelineIdx - (m_inputAssemblyStateCreateInfos.size() - 1)); ++i)
            m_inputAssemblyStateCreateInfos.push_back(std::vector<VkPipelineInputAssemblyStateCreateInfo>{});
        m_inputAssemblyStateCreateInfos[pipelineIdx].push_back(VkPipelineInputAssemblyStateCreateInfo{});
        VkPipelineInputAssemblyStateCreateInfo *info = &(m_inputAssemblyStateCreateInfos[pipelineIdx].back());
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = 0; // reserved for future use
        info->topology = topology;
        info->primitiveRestartEnable = primitiveRestartEnable;
        return info;
    }

    VkPipelineTessellationStateCreateInfo *VknInfos::fillTessellationStateCreateInfo(uint32_t pipelineIdx, uint32_t patchControlPoints)
    {
        for (int i = 0; i < (pipelineIdx - (m_tessellationStateCreateInfos.size() - 1)); ++i)
            m_tessellationStateCreateInfos.push_back(std::vector<VkPipelineTessellationStateCreateInfo>{});
        m_tessellationStateCreateInfos[pipelineIdx].push_back(VkPipelineTessellationStateCreateInfo{});
        VkPipelineTessellationStateCreateInfo *info = &(m_tessellationStateCreateInfos[pipelineIdx].back());
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = 0; // reserved for future use
        info->patchControlPoints = patchControlPoints;
        return info;
    }

    VkPipelineViewportStateCreateInfo *VknInfos::fillViewportStateCreateInfo(
        uint32_t pipelineIdx, std::vector<VkViewport> viewports, std::vector<VkRect2D> scissors)
    {
        for (int i = 0; i < (pipelineIdx - (m_viewportStateCreateInfos.size() - 1)); ++i)
            m_viewportStateCreateInfos.push_back(std::vector<VkPipelineViewportStateCreateInfo>{});
        m_viewportStateCreateInfos[pipelineIdx].push_back(VkPipelineViewportStateCreateInfo{});
        VkPipelineViewportStateCreateInfo *info = &(m_viewportStateCreateInfos[pipelineIdx].back());
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = 0; // reserved for future use
        info->viewportCount = viewports.size();
        if (viewports.size() == 0)
            info->pViewports = VK_NULL_HANDLE;
        else
            info->pViewports = viewports.data();
        info->scissorCount = scissors.size();
        if (scissors.size() == 0)
            info->pScissors = VK_NULL_HANDLE;
        else
            info->pScissors = scissors.data();
        return info;
    }

    VkPipelineRasterizationStateCreateInfo *VknInfos::fillRasterizationStateCreateInfo(
        uint32_t pipelineIdx, VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace,
        float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor,
        float lineWidth, VkBool32 depthClampEnable,
        VkBool32 rasterizerDiscardEnable, VkBool32 depthBiasEnable)
    {
        for (int i = 0; i < (pipelineIdx - (m_rasterizationStateCreateInfos.size() - 1)); ++i)
            m_rasterizationStateCreateInfos.push_back(std::vector<VkPipelineRasterizationStateCreateInfo>{});
        m_rasterizationStateCreateInfos[pipelineIdx].push_back(VkPipelineRasterizationStateCreateInfo{});
        VkPipelineRasterizationStateCreateInfo *info = &(m_rasterizationStateCreateInfos[pipelineIdx].back());
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = 0; // reserved for future use
        info->depthClampEnable = depthClampEnable;
        info->rasterizerDiscardEnable = rasterizerDiscardEnable;
        info->polygonMode = polygonMode;
        info->cullMode = cullMode;
        info->frontFace = frontFace;
        info->depthBiasEnable = depthBiasEnable;
        info->depthBiasConstantFactor = depthBiasConstantFactor;
        info->depthBiasClamp = depthBiasClamp;
        info->depthBiasSlopeFactor = depthBiasSlopeFactor;
        info->lineWidth = lineWidth;
        return info;
    }

    VkPipelineMultisampleStateCreateInfo *VknInfos::fillMultisampleStateCreateInfo(
        uint32_t pipelineIdx, float minSampleShading, VkSampleMask *pSampleMask,
        VkSampleCountFlagBits rasterizationSamples,
        VkBool32 sampleShadingEnable, VkBool32 alphaToCoverageEnable,
        VkBool32 alphaToOneEnable)
    {
        for (int i = 0; i < (pipelineIdx - (m_multisampleStateCreateInfos.size() - 1)); ++i)
            m_multisampleStateCreateInfos.push_back(std::vector<VkPipelineMultisampleStateCreateInfo>{});
        m_multisampleStateCreateInfos[pipelineIdx].push_back(VkPipelineMultisampleStateCreateInfo{});
        VkPipelineMultisampleStateCreateInfo *info = &(m_multisampleStateCreateInfos[pipelineIdx].back());
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = 0; // reserved for future use
        info->rasterizationSamples = rasterizationSamples;
        info->sampleShadingEnable = sampleShadingEnable;
        info->minSampleShading = minSampleShading;
        info->pSampleMask = pSampleMask;
        info->alphaToCoverageEnable = alphaToCoverageEnable;
        info->alphaToOneEnable = alphaToOneEnable;
        return info;
    }

    VkPipelineDepthStencilStateCreateInfo *VknInfos::fillDepthStencilStateCreateInfo(
        uint32_t pipelineIdx, VkCompareOp depthCompareOp, VkStencilOpState front, VkStencilOpState back,
        float minDepthBounds, float maxDepthBounds,
        VkPipelineDepthStencilStateCreateFlags flags,
        VkBool32 depthTestEnable, VkBool32 depthWriteEnable,
        VkBool32 depthBoundsTestEnable, VkBool32 stencilTestEnable)
    {
        for (int i = 0; i < (pipelineIdx - (m_depthStencilStateCreateInfos.size() - 1)); ++i)
            m_depthStencilStateCreateInfos.push_back(std::vector<VkPipelineDepthStencilStateCreateInfo>{});
        m_depthStencilStateCreateInfos[pipelineIdx].push_back(VkPipelineDepthStencilStateCreateInfo{});
        VkPipelineDepthStencilStateCreateInfo *info = &(m_depthStencilStateCreateInfos[pipelineIdx].back());
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = flags;
        info->depthTestEnable = depthTestEnable;
        info->depthWriteEnable = depthWriteEnable;
        info->depthCompareOp = depthCompareOp;
        info->depthBoundsTestEnable = depthBoundsTestEnable;
        info->stencilTestEnable = stencilTestEnable;
        info->front = front;
        info->back = back;
        info->minDepthBounds = minDepthBounds;
        info->maxDepthBounds = maxDepthBounds;
        return info;
    }

    VkPipelineColorBlendStateCreateInfo *VknInfos::fillColorBlendStateCreateInfo(
        uint32_t pipelineIdx, VkLogicOp logicOp, std::vector<VkPipelineColorBlendAttachmentState> attachments,
        float blendConstants[4], VkBool32 logicOpEnable,
        VkPipelineColorBlendStateCreateFlags flags)
    {
        for (int i = 0; i < (pipelineIdx - (m_colorBlendStateCreateInfos.size() - 1)); ++i)
            m_colorBlendStateCreateInfos.push_back(std::vector<VkPipelineColorBlendStateCreateInfo>{});
        m_colorBlendStateCreateInfos[pipelineIdx].push_back(VkPipelineColorBlendStateCreateInfo{});
        VkPipelineColorBlendStateCreateInfo *info = &(m_colorBlendStateCreateInfos[pipelineIdx].back());
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = flags;
        info->logicOpEnable = logicOpEnable;
        info->logicOp = logicOp;
        info->attachmentCount = attachments.size();
        if (attachments.size() == 0)
            info->pAttachments = VK_NULL_HANDLE;
        else
            info->pAttachments = attachments.data();
        info->blendConstants[4] = {};
        return info;
    }

    VkPipelineDynamicStateCreateInfo *VknInfos::fillDynamicStateCreateInfo(
        uint32_t pipelineIdx, std::vector<VkDynamicState> dynamicStates)
    {
        for (int i = 0; i < (pipelineIdx - (m_dynamicStateCreateInfos.size() - 1)); ++i)
            m_dynamicStateCreateInfos.push_back(std::vector<VkPipelineDynamicStateCreateInfo>{});
        m_dynamicStateCreateInfos[pipelineIdx].push_back(VkPipelineDynamicStateCreateInfo{});
        VkPipelineDynamicStateCreateInfo *info = &(m_dynamicStateCreateInfos[pipelineIdx].back());
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = 0; // reserved for future use
        info->dynamicStateCount = dynamicStates.size();
        if (dynamicStates.size() == 0)
            info->pDynamicStates = VK_NULL_HANDLE;
        else
            info->pDynamicStates = dynamicStates.data();
        return info;
    }

    VkDescriptorSetLayoutCreateInfo *VknInfos::fillDescriptorSetLayoutCreateInfo(
        std::vector<VkDescriptorSetLayoutBinding> bindings, VkDescriptorSetLayoutCreateFlags flags)
    {
        m_descriptorSetLayoutCreateInfos.push_back(VkDescriptorSetLayoutCreateInfo{});
        VkDescriptorSetLayoutCreateInfo &info = m_descriptorSetLayoutCreateInfos.back();
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        info.bindingCount = bindings.size();
        if (bindings.size() == 0)
            info.pBindings = VK_NULL_HANDLE;
        else
            info.pBindings = bindings.data();
        return &info;
    }

    VkPipelineLayoutCreateInfo *VknInfos::fillPipelineLayoutCreateInfo(
        uint32_t pipelineIdx,
        std::vector<VkDescriptorSetLayout> setLayouts,
        std::vector<VkPushConstantRange> pushConstantRanges,
        VkPipelineLayoutCreateFlags flags)
    {
        for (int i = 0; i < (pipelineIdx - (m_layoutCreateInfos.size() - 1)); ++i)
            m_layoutCreateInfos.push_back(std::vector<VkPipelineLayoutCreateInfo>{});
        m_layoutCreateInfos[pipelineIdx].push_back(VkPipelineLayoutCreateInfo{});
        VkPipelineLayoutCreateInfo *info = &(m_layoutCreateInfos[pipelineIdx].back());
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = flags;
        info->setLayoutCount = setLayouts.size();
        if (setLayouts.size() == 0)
            info->pSetLayouts = VK_NULL_HANDLE;
        else
            info->pSetLayouts = setLayouts.data();
        info->pushConstantRangeCount = pushConstantRanges.size();
        if (pushConstantRanges.size() == 0)
            info->pPushConstantRanges = VK_NULL_HANDLE;
        else
            info->pPushConstantRanges = pushConstantRanges.data();

        return info;
    }

    VkPipelineCacheCreateInfo *VknInfos::fillPipelineCacheCreateInfo(
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

        return &info;
    }

    bool VknInfos::checkFill(checkFillFunctions functionName)
    {
        switch (functionName)
        {
        case APP_INFO:
            if (!m_filledAppInfo)
                throw std::runtime_error("AppInfo not filled before get.");
            break;
        case DEVICE_QUEUE_CREATE_INFO:
            if (!m_filledDeviceQueueCreateInfo)
                throw std::runtime_error("DeviceQueueCreateInfo not filled before get.");
            break;
        case DEVICE_CREATE_INFO:
            if (!m_filledDeviceCreateInfo)
                throw std::runtime_error("DeviceCreateInfo not filled before get.");
            break;
        case INSTANCE_CREATE_INFO:
            if (!m_filledInstanceCreateInfo)
                throw std::runtime_error("InstanceCreateInfo not filled before get.");
            break;
        }
        return true;
    }

    void VknInfos::fillAppName(std::string name)
    {
        m_appName = new std::vector<char>(name.size());
        for (int i = 0; i <= name.size(); ++i)
            m_appName->at(i) = name.c_str()[i];
        m_filledAppName = true;
    }

    void VknInfos::fillEngineName(std::string name)
    {
        m_engineName = new std::vector<char>(name.size());
        for (int i = 0; i <= name.size(); ++i)
            m_engineName->at(i) = name.c_str()[i];
        m_filledEngineName = true;
    }

    void VknInfos::fillInstanceExtensionNames(std::vector<std::string> names)
    {
        m_enabledInstanceExtensionNames = new std::vector<char[100]>(names.size());
        for (int nameIdx = 0; nameIdx < names.size(); ++nameIdx)
        {
            for (int charIdx = 0; charIdx < 100; ++charIdx)
                if (charIdx < names[nameIdx].size())
                    m_enabledInstanceExtensionNames->at(nameIdx)[charIdx] = names[nameIdx][charIdx];
                else
                    m_enabledInstanceExtensionNames->at(nameIdx)[charIdx] = char("\0");
        }
    }

    void VknInfos::fillDeviceExtensionNames(std::vector<std::string> names)
    {
        m_enabledDeviceExtensionNames.push_back(new std::vector<char[100]>(names.size()));
        for (int nameIdx = 0; nameIdx < names.size(); ++nameIdx)
        {
            for (int charIdx = 0; charIdx < 100; ++charIdx)
                if (charIdx < names[nameIdx].size(); ++nameIdx)
                    m_enabledDeviceExtensionNames.back()->at(nameIdx)[charIdx] = names[nameIdx][charIdx];
                else
                    m_enabledDeviceExtensionNames.back()->at(nameIdx)[charIdx] = char("\0");
        }
    }

    void VknInfos::fillEnabledLayerNames(std::vector<std::string> names)
    {
        m_enabledLayerNames = new std::vector<char[100]>(names.size());
        for (int nameIdx = 0; nameIdx < names.size(); ++nameIdx)
        {
            for (int charIdx = 0; charIdx < 100; ++charIdx)
                if (charIdx < names[nameIdx].size())
                    m_enabledLayerNames->at(nameIdx)[charIdx] = names[nameIdx][charIdx];
                else
                    m_enabledLayerNames->at(nameIdx)[charIdx] = char("\0");
        }
    }

    void VknInfos::fillDeviceFeatures(bool robustBufferAccess, bool fullDrawIndexUint32, bool imageCubeArray,
                                      bool independentBlend, bool geometryShader, bool tessellationShader,
                                      bool sampleRateShading, bool dualSrcBlend, bool logicOp,
                                      bool multiDrawIndirect, bool drawIndirectFirstInstance, bool depthClamp,
                                      bool depthBiasClamp, bool fillModeNonSolid, bool depthBounds,
                                      bool wideLines, bool largePoints, bool alphaToOne,
                                      bool multiViewport, bool samplerAnisotropy, bool textureCompressionETC2,
                                      bool textureCompressionASTC_LDR, bool textureCompressionBC, bool occlusionQueryPrecise,
                                      bool pipelineStatisticsQuery, bool vertexPipelineStoresAndAtomics,
                                      bool fragmentStoresAndAtomics, bool shaderTessellationAndGeometryPointSize,
                                      bool shaderImageGatherExtended, bool shaderStorageImageExtendedFormats,
                                      bool shaderStorageImageMultisample, bool shaderStorageImageReadWithoutFormat,
                                      bool shaderStorageImageWriteWithoutFormat, bool shaderUniformBufferArrayDynamicIndexing,
                                      bool shaderSampledImageArrayDynamicIndexing, bool shaderStorageBufferArrayDynamicIndexing,
                                      bool shaderStorageImageArrayDynamicIndexing, bool shaderClipDistance,
                                      bool shaderCullDistance, bool shaderFloat64, bool shaderInt64,
                                      bool shaderInt16, bool shaderResourceResidency, bool shaderResourceMinLod,
                                      bool sparseBinding, bool sparseResidencyBuffer, bool sparseResidencyImage2D,
                                      bool sparseResidencyImage3D, bool sparseResidency2Samples,
                                      bool sparseResidency4Samples, bool sparseResidency8Samples, bool sparseResidency16Samples,
                                      bool sparseResidencyAliased, bool variableMultisampleRate, bool inheritedQueries)
    {
        m_enabledFeatures.push_back(VkPhysicalDeviceFeatures{});
        VkPhysicalDeviceFeatures *info = &(m_enabledFeatures.back());
        info->robustBufferAccess = robustBufferAccess;
        info->fullDrawIndexUint32 = fullDrawIndexUint32;
        info->imageCubeArray = imageCubeArray;
        info->independentBlend = independentBlend;
        info->geometryShader = geometryShader;
        info->tessellationShader = tessellationShader;
        info->sampleRateShading = sampleRateShading;
        info->dualSrcBlend = dualSrcBlend;
        info->logicOp = logicOp;
        info->multiDrawIndirect = multiDrawIndirect;
        info->drawIndirectFirstInstance = drawIndirectFirstInstance;
        info->depthClamp = depthClamp;
        info->depthBiasClamp = depthBiasClamp;
        info->fillModeNonSolid = fillModeNonSolid;
        info->depthBounds = depthBounds;
        info->wideLines = wideLines;
        info->largePoints = largePoints;
        info->alphaToOne = alphaToOne;
        info->multiViewport = multiViewport;
        info->samplerAnisotropy = samplerAnisotropy;
        info->textureCompressionETC2 = textureCompressionETC2;
        info->textureCompressionASTC_LDR = textureCompressionASTC_LDR;
        info->textureCompressionBC = textureCompressionBC;
        info->occlusionQueryPrecise = occlusionQueryPrecise;
        info->pipelineStatisticsQuery = pipelineStatisticsQuery;
        info->vertexPipelineStoresAndAtomics = vertexPipelineStoresAndAtomics;
        info->fragmentStoresAndAtomics = fragmentStoresAndAtomics;
        info->shaderTessellationAndGeometryPointSize = shaderTessellationAndGeometryPointSize;
        info->shaderImageGatherExtended = shaderImageGatherExtended;
        info->shaderStorageImageExtendedFormats = shaderStorageImageExtendedFormats;
        info->shaderStorageImageMultisample = shaderStorageImageMultisample;
        info->shaderStorageImageReadWithoutFormat = shaderStorageImageReadWithoutFormat;
        info->shaderStorageImageWriteWithoutFormat = shaderStorageImageWriteWithoutFormat;
        info->shaderUniformBufferArrayDynamicIndexing = shaderUniformBufferArrayDynamicIndexing;
        info->shaderSampledImageArrayDynamicIndexing = shaderSampledImageArrayDynamicIndexing;
        info->shaderStorageBufferArrayDynamicIndexing = shaderStorageBufferArrayDynamicIndexing;
        info->shaderStorageImageArrayDynamicIndexing = shaderStorageImageArrayDynamicIndexing;
        info->shaderClipDistance = shaderClipDistance;
        info->shaderCullDistance = shaderCullDistance;
        info->shaderFloat64 = shaderFloat64;
        info->shaderInt64 = shaderInt64;
        info->shaderInt16 = shaderInt16;
        info->shaderResourceResidency = shaderResourceResidency;
        info->shaderResourceMinLod = shaderResourceMinLod;
        info->sparseBinding = sparseBinding;
        info->sparseResidencyBuffer = sparseResidencyBuffer;
        info->sparseResidencyImage2D = sparseResidencyImage2D;
        info->sparseResidencyImage3D = sparseResidencyImage3D;
        info->sparseResidency2Samples = sparseResidency2Samples;
        info->sparseResidency4Samples = sparseResidency4Samples;
        info->sparseResidency8Samples = sparseResidency8Samples;
        info->sparseResidency16Samples = sparseResidency16Samples;
        info->sparseResidencyAliased = sparseResidencyAliased;
        info->variableMultisampleRate = variableMultisampleRate;
        info->inheritedQueries = inheritedQueries;
    }

    void VknInfos::fillAppInfo(uint32_t apiVersion, uint32_t applicationVersion, uint32_t engineVersion)
    {
        if (!m_filledAppName)
            throw std::runtime_error("App name not filled before filling app info.");
        if (!m_filledEngineName)
            throw std::runtime_error("Engine name not filled before filling app info.");
        m_appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        m_appInfo.pNext = VK_NULL_HANDLE;
        m_appInfo.pApplicationName = m_appName->data();
        m_appInfo.applicationVersion = applicationVersion;
        m_appInfo.pEngineName = m_engineName->data();
        m_appInfo.engineVersion = engineVersion;
        m_appInfo.apiVersion = apiVersion;

        m_filledAppInfo = true;
    }

    const char *const *VknInfos::getNamesPointer(std::vector<char[100]> *names)
    {
        return reinterpret_cast<const char *const *>(names->data());
    }

    void VknInfos::fillInstanceCreateInfo(VkInstanceCreateFlags flags)
    {
        if (!m_filledLayerNames)
            throw std::runtime_error("Layer names not filled before filling instance create info.");
        if (!m_filledInstanceExtensionNames)
            throw std::runtime_error("Instance extension names not filled before filling instance create info.");
        if (!m_filledAppInfo)
            throw std::runtime_error("AppInfo not filled before InstanceCreateInfo.");

        m_instanceCreateInfo.pNext = VK_NULL_HANDLE;
        m_instanceCreateInfo.flags = flags;
        m_instanceCreateInfo.pApplicationInfo = &m_appInfo;
        m_instanceCreateInfo.enabledLayerCount = m_enabledLayerNames->size();
        if (m_enabledLayerNames->size() == 0)
            m_instanceCreateInfo.ppEnabledLayerNames = VK_NULL_HANDLE;
        else
            m_instanceCreateInfo.ppEnabledLayerNames = this->getNamesPointer(m_enabledLayerNames);
        m_instanceCreateInfo.enabledExtensionCount = m_enabledInstanceExtensionNames->size();
        if (m_enabledInstanceExtensionNames->size() == 0)
            m_instanceCreateInfo.ppEnabledExtensionNames = VK_NULL_HANDLE;
        else
            m_instanceCreateInfo.ppEnabledExtensionNames = this->getNamesPointer(m_enabledInstanceExtensionNames);
        m_filledDeviceQueueCreateInfo = true;
    }

    void VknInfos::fillDeviceQueueCreateInfo(uint32_t queueFamilyIdx, uint32_t queueCount,
                                             VkApplicationInfo *pNext,
                                             VkDeviceQueueCreateFlags flags)
    {
        m_queueCreateInfos.push_back(VkDeviceQueueCreateInfo{});
        VkDeviceQueueCreateInfo &info = m_queueCreateInfos.back();
        info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info.queueFamilyIndex = queueFamilyIdx;
        info.queueCount = queueCount;
        info.pNext = pNext;
        info.flags = flags; // Only flag is a protected memory bit, for a queue family that supports it
        m_queuePriorities.push_back(std::vector<float>{});
        for (int i = 0; i < queueCount; ++i)
            m_queuePriorities.back().push_back(1.0f);
        if (m_queuePriorities.size() == 0)
            throw std::runtime_error("No queue priorities set for device queue create info.");
        info.pQueuePriorities = m_queuePriorities.back().data();
        m_filledDeviceQueueCreateInfo = true;
    }

    VkDeviceCreateInfo *VknInfos::fillDeviceCreateInfo(uint32_t deviceIdx)
    {
        if (m_queueCreateInfos.size() == 0)
            throw std::runtime_error("Queues not selected before filling device create info.");
        VkDeviceCreateInfo &info = m_deviceCreateInfo;
        // default
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext = VK_NULL_HANDLE;
        info.flags = 0; // flags reserved, must = 0
        info.queueCreateInfoCount = m_queueCreateInfos.size();
        info.pQueueCreateInfos = m_queueCreateInfos.data();
        // enabledLayerCount is deprecated and should not be used
        info.enabledLayerCount = 0; // ignored, value doesn't matter
        // ppEnabledLayerNames is deprecated and should not be used
        info.ppEnabledLayerNames = VK_NULL_HANDLE; // ignored, value doesn't matter
        m_deviceCreateInfo.enabledExtensionCount = m_enabledDeviceExtensionNames[deviceIdx]->size();
        if (m_enabledDeviceExtensionNames[deviceIdx]->size() == 0)
            m_instanceCreateInfo.ppEnabledExtensionNames = VK_NULL_HANDLE;
        else
            m_instanceCreateInfo.ppEnabledExtensionNames = this->getNamesPointer(m_enabledDeviceExtensionNames[deviceIdx]);
        m_deviceCreateInfo.pEnabledFeatures = &(m_enabledFeatures[deviceIdx]);

        m_filledDeviceCreateInfo = true;
        return &m_deviceCreateInfo;
    }

    VkSwapchainCreateInfoKHR *VknInfos::fillSwapChainCreateInfo(
        VkSurfaceKHR surface, uint32_t imageCount, VkExtent2D dimensions,
        VkSurfaceFormatKHR surfaceFormat, uint32_t numImageArrayLayers, VkImageUsageFlags usage,
        VkSharingMode sharingMode, VkSurfaceTransformFlagBitsKHR preTransform,
        VkCompositeAlphaFlagBitsKHR compositeAlpha, VkPresentModeKHR presentMode, VkBool32 clipped,
        VkSwapchainKHR oldSwapchain)
    {
        m_swapChainCreateInfos.push_back(VkSwapchainCreateInfoKHR{});
        VkSwapchainCreateInfoKHR &swapchainInfo = m_swapChainCreateInfos.back();
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.surface = surface;                  // The surface you created
        swapchainInfo.minImageCount = imageCount;         // Number of images in the swapchain
        swapchainInfo.imageFormat = surfaceFormat.format; // Format of the images
        swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainInfo.imageExtent = dimensions; // Dimensions of the images
        swapchainInfo.imageArrayLayers = numImageArrayLayers;
        swapchainInfo.imageUsage = usage;
        swapchainInfo.imageSharingMode = sharingMode;
        swapchainInfo.preTransform = preTransform;
        swapchainInfo.compositeAlpha = compositeAlpha;
        swapchainInfo.presentMode = presentMode;
        swapchainInfo.clipped = clipped;
        swapchainInfo.oldSwapchain = oldSwapchain;

        return &swapchainInfo;
    }

    VkAttachmentDescription *VknInfos::fillAttachmentDescription(
        VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp,
        VkAttachmentStoreOp storeOp, VkAttachmentLoadOp stencilLoadOp,
        VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout,
        VkImageLayout finalLayout, VkAttachmentDescriptionFlags flags)
    {
        m_attachmentDescriptions.push_back(VkAttachmentDescription{});
        VkAttachmentDescription &description = m_attachmentDescriptions.back();
        description.format = format; // Set to your swapchain image format
        description.samples = samples;
        description.loadOp = loadOp;
        description.storeOp = storeOp;
        description.stencilLoadOp = stencilLoadOp;
        description.stencilStoreOp = stencilStoreOp;
        description.initialLayout = initialLayout;
        description.finalLayout = finalLayout;
        description.flags = flags;

        return &description;
    }

    void VknInfos::fillAttachmentReference(
        uint32_t subpassIdx, VknAttachmentType attachmentType, uint32_t attachmentIdx, VkImageLayout layout)
    {
        for (int i = 0; i < (subpassIdx - (m_attachmentReferences.size() - 1)); ++i)
        {
            m_attachmentReferences.push_back(std::vector<std::vector<VkAttachmentReference>>{});
            m_attachmentReferences.back().push_back(std::vector<VkAttachmentReference>{}); // color
            m_attachmentReferences.back().push_back(std::vector<VkAttachmentReference>{}); // depth-stencil
            m_attachmentReferences.back().push_back(std::vector<VkAttachmentReference>{}); // resolve
            m_attachmentReferences.back().push_back(std::vector<VkAttachmentReference>{}); // input
            m_preserveAttachments.push_back(std::vector<uint32_t>{});
        }
        if (attachmentType == PRESERVE_ATTACHMENT)
            m_preserveAttachments.back().push_back(attachmentIdx);
        else
        {
            m_attachmentReferences.back()[attachmentType].push_back(VkAttachmentReference{});
            VkAttachmentReference &ref = m_attachmentReferences[subpassIdx][attachmentType].back();
            ref.attachment = attachmentIdx;
            ref.layout = layout;
        }
    }

    std::vector<std::vector<std::vector<VkAttachmentReference>>> *VknInfos::getAllAttachmentReferences()
    {
        return &m_attachmentReferences;
    }

    std::vector<std::vector<VkAttachmentReference>> *VknInfos::getAttachmentReferences(uint32_t subpassIdx)
    {
        return &(m_attachmentReferences[subpassIdx]);
    }

    std::vector<uint32_t> *VknInfos::getPreserveAttachments(uint32_t subpassIdx)
    {
        return &(m_preserveAttachments[subpassIdx]);
    }

    std::vector<std::vector<uint32_t>> *VknInfos::getAllPreserveAttachments()
    {
        return &m_preserveAttachments;
    }

    VkSubpassDescription *VknInfos::fillSubpassDescription(
        uint32_t subpassIdx,
        VkPipelineBindPoint pipelineBindPoint,
        VkSubpassDescriptionFlags flags)
    {
        m_subpassDescriptions.push_back(VkSubpassDescription{});
        VkSubpassDescription &description = m_subpassDescriptions.back();
        description.flags = flags;
        description.pipelineBindPoint = pipelineBindPoint;
        std::vector<VkAttachmentReference> &inputAttachments = m_attachmentReferences[subpassIdx][INPUT_ATTACHMENT];
        std::vector<VkAttachmentReference> &colorAttachments = m_attachmentReferences[subpassIdx][COLOR_ATTACHMENT];
        std::vector<VkAttachmentReference> &resolveAttachments = m_attachmentReferences[subpassIdx][RESOLVE_ATTACHMENT];
        std::vector<VkAttachmentReference> &depthStencilAttachments = m_attachmentReferences[subpassIdx][DEPTH_STENCIL_ATTACHMENT];
        std::vector<uint32_t> &preserveAttachments = m_preserveAttachments[subpassIdx];

        if (inputAttachments.size() == 0)
        {
            description.inputAttachmentCount = 0;
            description.pInputAttachments = VK_NULL_HANDLE;
        }
        else
        {
            description.inputAttachmentCount = inputAttachments.size();
            description.pInputAttachments = inputAttachments.data();
        }
        if (colorAttachments.size() == 0)
        {
            description.colorAttachmentCount = 0;
            description.pColorAttachments = VK_NULL_HANDLE;
        }
        else
        {
            description.colorAttachmentCount = colorAttachments.size();
            description.pColorAttachments = colorAttachments.data();
        }
        if (preserveAttachments.size() == 0)
        {
            description.colorAttachmentCount = 0;
            description.pPreserveAttachments = VK_NULL_HANDLE;
        }
        else
        {
            description.preserveAttachmentCount = preserveAttachments.size();
            description.pPreserveAttachments = preserveAttachments.data();
        }
        if ((!resolveAttachments.size() == 0 && colorAttachments.size() == 0) || (!resolveAttachments.size() == 0 && (resolveAttachments.size() > colorAttachments.size())))
            throw std::runtime_error("Resolve attachments must be less than or equal to color attachments.");
        else if (resolveAttachments.size() == 0)
            description.pResolveAttachments = VK_NULL_HANDLE;
        else
            description.pResolveAttachments = resolveAttachments.data();
        if (!depthStencilAttachments.size() == 0 && depthStencilAttachments.size() > 1)
            throw std::runtime_error("Too many depth stencil attachments in subpass description.");
        if (depthStencilAttachments.size() == 0)
            description.pDepthStencilAttachment = VK_NULL_HANDLE;
        else
            description.pDepthStencilAttachment = &(depthStencilAttachments.back());

        return &description;
    }

    VkSubpassDependency *VknInfos::fillSubpassDependency(
        uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask,
        VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStageMask, VkAccessFlags dstAccessMask)
    {
        m_subpassDependencies.push_back(VkSubpassDependency{});
        VkSubpassDependency &dependency = m_subpassDependencies.back();
        dependency.srcSubpass = srcSubpass;
        dependency.dstSubpass = dstSubpass;
        dependency.srcStageMask = srcStageMask;
        dependency.srcAccessMask = srcAccessMask;
        dependency.dstStageMask = dstStageMask;
        dependency.dstAccessMask = dstAccessMask;

        return &dependency;
    }

    void VknInfos::fillVertexInputBindingDescription(uint32_t idx, uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)
    {
        for (int i = 0; i < (idx - (m_vertexInputBindings.size() - 1)); ++i)
            m_vertexInputBindings.push_back(std::vector<VkVertexInputBindingDescription>{});
        m_vertexInputBindings[idx].push_back(VkVertexInputBindingDescription{});
        VkVertexInputBindingDescription *description = &(m_vertexInputBindings[idx].back());
        description->binding = binding;
        description->stride = stride;
        description->inputRate = inputRate;
    }

    void VknInfos::fillVertexInputAttributeDescription(uint32_t idx, uint32_t binding, uint32_t location, VkFormat format, uint32_t offset)
    {
        for (int i = 0; i < (idx - (m_vertexInputAttributes.size() - 1)); ++i)
            m_vertexInputAttributes.push_back(std::vector<VkVertexInputAttributeDescription>{});
        m_vertexInputAttributes[idx].push_back(VkVertexInputAttributeDescription{});
        VkVertexInputAttributeDescription *description = &(m_vertexInputAttributes[idx].back());
        description->binding = binding;
        description->location = location;
        description->format = format;
        description->offset = offset;
    }
}