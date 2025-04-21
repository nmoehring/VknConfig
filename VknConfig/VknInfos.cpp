#include "include/VknInfos.hpp"

namespace vkn
{
    VknInfos::VknInfos()
    {
    }

    void VknInfos::fillDeviceQueuePriorities(VknIdxs &relIdxs, uint32_t queueFamilyIdx,
                                             std::vector<float> priorities)
    {
        for (auto &priority : priorities)
            m_queuePriorities[relIdxs.get<VkDevice>()][queueFamilyIdx].append(priority);
        m_deviceQueuePrioritiesFilled = true;
    }

    void VknInfos::setNumDeviceQueueFamilies(int num, uint32_t deviceIdx)
    {
        if (m_numQueueFamilies.size() < deviceIdx + 1)
            m_numQueueFamilies.resize(deviceIdx + 1);
        m_numQueueFamilies[deviceIdx] = num;
    }

    VkGraphicsPipelineCreateInfo *VknInfos::fillGfxPipelineCreateInfo(
        VknIdxs &relIdxs, VkRenderPass &renderpass,
        VkPipelineLayout *layout, VkPipeline basePipelineHandle,
        int32_t basePipelineIndex, VkPipelineCreateFlags flags)
    {
        VkGraphicsPipelineCreateInfo &info =
            m_gfxPipelineCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()]
                .insert(VkGraphicsPipelineCreateInfo{}, relIdxs.get<VkPipeline>());
        info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        info.pNext = VK_NULL_HANDLE;
        info.flags = flags;
        info.stageCount = m_shaderStageCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()].getDataSize(); // Need fill
        info.pStages = m_shaderStageCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()].getData();        // Need fill
        if (m_filledVertexInputStateInfo)
            info.pVertexInputState =
                &m_vertexInputStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()](relIdxs.get<VkPipeline>()); // Can be null if VK_DYNAMIC_STATE_VERTEX_INPUT_EXT
                                                                                                                                 // set in pDynamicState below. Ignored if mesh shader stage
                                                                                                                                 // included in pStages.
        if (m_filledInputAssemblyStateInfo)
            info.pInputAssemblyState =
                &m_inputAssemblyStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()](relIdxs.get<VkPipeline>()); // Can be null if VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE
                                                                                                                                   // and VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY are set in pDynamicState,
                                                                                                                                   // below and dynamicPrimitiveTopologyUnrestricted is VK_TRUE in the
                                                                                                                                   // VkPhysicalDeviceExtendedDynamicState3PropertiesEXT struct,
                                                                                                                                   // which requires the VK_EXT_extended_dynamic_state3 extension.
                                                                                                                                   // Also ignored if mesh shader stage included in pStages.
        if (m_filledTessellationStateInfo)
            info.pTessellationState =
                &m_tessellationStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()](relIdxs.get<VkPipeline>()); // Can be null if the VK_DYNAMIC_STATE_PATCH_CONTROL_POINTS_EXT
                                                                                                                                  // is set in the pDynamicState below.
        if (m_filledViewportStateInfo)
            info.pViewportState =
                &m_viewportStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()](relIdxs.get<VkPipeline>()); // Can be null if rasterization not enabled.
                                                                                                                              // Can be null if VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT
                                                                                                                              // and VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT are set in pDynamicState
                                                                                                                              // below, and also requires the VK_EXT_extended_dynamic_state3
                                                                                                                              // extension enabled.
        if (m_filledRasterizationStateInfo)
            info.pRasterizationState =
                &m_rasterizationStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()](relIdxs.get<VkPipeline>()); // I guess this could be null if rasterization is not enabled.
                                                                                                                                   // Can be null if VK_DYNAMIC_STATE_DEPTH_CLAMP_ENABLE_EXT,
                                                                                                                                   // VK_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE,
                                                                                                                                   // VK_DYNAMIC_STATE_POLYGON_MODE_EXT,
                                                                                                                                   // VK_DYNAMIC_STATE_CULL_MODE, VK_DYNAMIC_STATE_FRONT_FACE,
                                                                                                                                   // VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE, VK_DYNAMIC_STATE_DEPTH_BIAS,
                                                                                                                                   // and VK_DYNAMIC_STATE_LINE_WIDTH are set in pDynamicState below,
                                                                                                                                   // which also requires the VK_EXT_extended_dynamic_state3
                                                                                                                                   // extension enabled.
        if (m_filledMultisampleStateInfo)
            info.pMultisampleState =
                &m_multisampleStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()](relIdxs.get<VkPipeline>()); // Can be null if rasterization not enabled.
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
        if (m_filledDepthStencilStateInfo)
            info.pDepthStencilState =
                &m_depthStencilStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()](relIdxs.get<VkPipeline>()); // Can be null if rasterization not enabled.
                                                                                                                                  // Can be null if VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE,
                                                                                                                                  // VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE, VK_DYNAMIC_STATE_DEPTH_COMPARE_OP,
                                                                                                                                  // VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE,
                                                                                                                                  // VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE, VK_DYNAMIC_STATE_STENCIL_OP,
                                                                                                                                  // and VK_DYNAMIC_STATE_DEPTH_BOUNDS are set in pDynamicState below
                                                                                                                                  // Also requires the  VK_EXT_extended_dynamic_state3 extension.
        if (m_filledColorBlendStateInfo)
            info.pColorBlendState =
                &m_colorBlendStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()](relIdxs.get<VkPipeline>()); // Can be null if rasterization not enabled.
                                                                                                                                // Can be null if VK_DYNAMIC_STATE_LOGIC_OP_ENABLE_EXT,
                                                                                                                                // VK_DYNAMIC_STATE_LOGIC_OP_EXT,
                                                                                                                                // VK_DYNAMIC_STATE_COLOR_BLEND_ENABLE_EXT,
                                                                                                                                // VK_DYNAMIC_STATE_COLOR_BLEND_EQUATION_EXT,
                                                                                                                                // VK_DYNAMIC_STATE_COLOR_WRITE_MASK_EXT, and
                                                                                                                                // VK_DYNAMIC_STATE_BLEND_CONSTANTS are set in pDynamicState below
                                                                                                                                // Requires the VK_EXT_extended_dynamic_state3 extension.
        if (m_filledDynamicStateInfo)
            info.pDynamicState =
                &m_dynamicStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()](relIdxs.get<VkPipeline>()); // Could be null if no state in the pipeline needs to be dynamic.
        info.layout = *layout;                                                                                               // Need fill
        info.renderPass = renderpass;                                                                                        // Need fill
        info.subpass = relIdxs.get<VkPipeline>();                                                                            // Need fill
        info.basePipelineHandle = basePipelineHandle;                                                                        // Need fill
        info.basePipelineIndex = basePipelineIndex;                                                                          // Need fill

        return &info;
    }

    VkRenderPassCreateInfo *VknInfos::fillRenderpassCreateInfo(VknIdxs &relIdxs,
                                                               uint32_t numAttachments,
                                                               uint32_t numSubpasses,
                                                               uint32_t numSubpassDeps,
                                                               VkRenderPassCreateFlags flags)
    {
        VkRenderPassCreateInfo &renderpassInfo =
            m_renderpassCreateInfos[relIdxs.get<VkDevice>()]
                .insert(VkRenderPassCreateInfo{}, relIdxs.get<VkRenderPass>());
        VknSpace<VkAttachmentDescription> &attachmentDescriptions =
            m_attachmentDescriptions[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()];
        VknSpace<VkSubpassDescription> &subpassDescriptions =
            m_subpassDescriptions[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()];
        VknSpace<VkSubpassDependency> &subpassDependencies =
            m_subpassDependencies[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()];

        renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderpassInfo.pNext = VK_NULL_HANDLE;
        renderpassInfo.flags = flags;
        renderpassInfo.attachmentCount = numAttachments;
        renderpassInfo.pAttachments = numAttachments == 0 ? VK_NULL_HANDLE : attachmentDescriptions.getData();
        renderpassInfo.subpassCount = numSubpasses;
        renderpassInfo.pSubpasses = numSubpasses == 0 ? VK_NULL_HANDLE : subpassDescriptions.getData();
        renderpassInfo.dependencyCount = numSubpassDeps;
        renderpassInfo.pDependencies = numSubpassDeps == 0 ? VK_NULL_HANDLE : subpassDependencies.getData();

        return &renderpassInfo;
    }

    VkShaderModuleCreateInfo *VknInfos::fillShaderModuleCreateInfo(
        VknIdxs &relIdxs, std::vector<char> *code)
    {
        VkShaderModuleCreateInfo *info =
            &m_shaderModuleCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()]
                 .insert(VkShaderModuleCreateInfo{}, relIdxs.get<VkShaderModule>());
        info->sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = 0;
        info->codeSize = code->size();
        info->pCode = reinterpret_cast<const uint32_t *>(code->data());
        return info;
    }

    VkPipelineShaderStageCreateInfo *VknInfos::fillShaderStageCreateInfo(
        VknIdxs &relIdxs,
        VkShaderModule *module, VkShaderStageFlagBits *stage,
        VkPipelineShaderStageCreateFlags *flags, VkSpecializationInfo *pSpecializationInfo)
    {
        VkPipelineShaderStageCreateInfo *info =
            &m_shaderStageCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()]
                 .insert(VkPipelineShaderStageCreateInfo{}, relIdxs.get<VkShaderModule>());
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        info->pNext = VK_NULL_HANDLE;
        info->flags = *flags; // need fill
        info->stage = *stage; // need fill
        info->module = *module;
        info->pName = m_mainEntry;
        info->pSpecializationInfo = pSpecializationInfo; // need fill
        return info;
    }

    VkPipelineVertexInputStateCreateInfo *VknInfos::fillVertexInputStateCreateInfo(
        VknIdxs &relIdxs, uint32_t numBindings, uint32_t numAttributes)
    {
        VknSpace<VkVertexInputBindingDescription> *vertexBindingDescriptions =
            &m_vertexInputBindings[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()];
        VknSpace<VkVertexInputAttributeDescription> *vertexAttributeDescriptions =
            &m_vertexInputAttributes[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()];

        VkPipelineVertexInputStateCreateInfo *info =
            &m_vertexInputStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()]
                 .insert(VkPipelineVertexInputStateCreateInfo{}, relIdxs.get<VkPipeline>());
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        info->pNext = VK_NULL_HANDLE;
        info->flags = 0; // reserved for future use
        info->vertexBindingDescriptionCount = numBindings;
        if (numBindings == 0)
            info->pVertexBindingDescriptions = VK_NULL_HANDLE;
        else
            info->pVertexBindingDescriptions = vertexBindingDescriptions->getData();
        info->vertexAttributeDescriptionCount = numAttributes;
        if (numAttributes == 0)
            info->pVertexAttributeDescriptions = VK_NULL_HANDLE;
        else
            info->pVertexAttributeDescriptions = vertexAttributeDescriptions->getData();
        m_filledVertexInputStateInfo = true;
        return info;
    }

    VkPipelineInputAssemblyStateCreateInfo *VknInfos::fillInputAssemblyStateCreateInfo(
        VknIdxs &relIdxs, VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable)
    {
        VkPipelineInputAssemblyStateCreateInfo *info =
            &m_inputAssemblyStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()]
                 .insert(VkPipelineInputAssemblyStateCreateInfo{}, relIdxs.get<VkPipeline>());
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = 0; // reserved for future use
        info->topology = topology;
        info->primitiveRestartEnable = primitiveRestartEnable;
        m_filledInputAssemblyStateInfo = true;
        return info;
    }

    VkPipelineTessellationStateCreateInfo *VknInfos::fillTessellationStateCreateInfo(
        uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx, uint32_t patchControlPoints)
    {
        VkPipelineTessellationStateCreateInfo *info =
            &m_tessellationStateCreateInfos[deviceIdx][renderpassIdx]
                 .insert(VkPipelineTessellationStateCreateInfo{}, subpassIdx);
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = 0; // reserved for future use
        info->patchControlPoints = patchControlPoints;
        return info;
    }

    VkPipelineViewportStateCreateInfo *VknInfos::fillViewportStateCreateInfo(
        VknIdxs &relIdxs,
        VknVector<VkViewport> *viewports, VknVector<VkRect2D> *scissors)
    {
        VkPipelineViewportStateCreateInfo *info =
            &m_viewportStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()]
                 .insert(VkPipelineViewportStateCreateInfo{}, relIdxs.get<VkPipeline>());
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = 0; // reserved for future use
        info->viewportCount = viewports->getSize();
        info->pViewports = viewports->getData();
        info->scissorCount = scissors->getSize();
        info->pScissors = scissors->getData();
        m_filledViewportStateInfo = true;
        return info;
    }

    VkPipelineRasterizationStateCreateInfo *VknInfos::fillRasterizationStateCreateInfo(
        VknIdxs &relIdxs, VkPolygonMode polygonMode,
        VkCullModeFlags cullMode, VkFrontFace frontFace,
        float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor,
        float lineWidth, VkBool32 depthClampEnable,
        VkBool32 rasterizerDiscardEnable, VkBool32 depthBiasEnable)
    {
        VkPipelineRasterizationStateCreateInfo *info =
            &m_rasterizationStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()]
                 .insert(VkPipelineRasterizationStateCreateInfo{}, relIdxs.get<VkPipeline>());
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
        m_filledRasterizationStateInfo = true;
        return info;
    }

    VkPipelineMultisampleStateCreateInfo *VknInfos::fillMultisampleStateCreateInfo(
        VknIdxs &relIdxs, float minSampleShading,
        VkSampleMask *pSampleMask, VkSampleCountFlagBits rasterizationSamples,
        VkBool32 sampleShadingEnable, VkBool32 alphaToCoverageEnable,
        VkBool32 alphaToOneEnable)
    {
        VkPipelineMultisampleStateCreateInfo *info =
            &m_multisampleStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()]
                 .insert(VkPipelineMultisampleStateCreateInfo{}, relIdxs.get<VkPipeline>());
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = 0; // reserved for future use
        info->rasterizationSamples = rasterizationSamples;
        info->sampleShadingEnable = sampleShadingEnable;
        info->minSampleShading = minSampleShading;
        info->pSampleMask = pSampleMask;
        info->alphaToCoverageEnable = alphaToCoverageEnable;
        info->alphaToOneEnable = alphaToOneEnable;
        m_filledMultisampleStateInfo = true;
        return info;
    }

    VkPipelineDepthStencilStateCreateInfo *VknInfos::fillDepthStencilStateCreateInfo(
        uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx,
        VkCompareOp depthCompareOp, VkStencilOpState front, VkStencilOpState back,
        float minDepthBounds, float maxDepthBounds,
        VkPipelineDepthStencilStateCreateFlags flags,
        VkBool32 depthTestEnable, VkBool32 depthWriteEnable,
        VkBool32 depthBoundsTestEnable, VkBool32 stencilTestEnable)
    {
        VkPipelineDepthStencilStateCreateInfo *info =
            &m_depthStencilStateCreateInfos[deviceIdx][renderpassIdx]
                 .insert(VkPipelineDepthStencilStateCreateInfo{}, subpassIdx);
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
        uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx, VkLogicOp logicOp,
        VknVector<VkPipelineColorBlendAttachmentState> attachments,
        float blendConstants[4], VkBool32 logicOpEnable,
        VkPipelineColorBlendStateCreateFlags flags)
    {
        VkPipelineColorBlendStateCreateInfo *info =
            &m_colorBlendStateCreateInfos[deviceIdx][renderpassIdx]
                 .insert(VkPipelineColorBlendStateCreateInfo{}, subpassIdx);
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = flags;
        info->logicOpEnable = logicOpEnable;
        info->logicOp = logicOp;
        info->attachmentCount = attachments.getSize();
        info->pAttachments = attachments.getData();
        std::copy(&blendConstants[0], &blendConstants[4], info->blendConstants);
        return info;
    }

    VkPipelineDynamicStateCreateInfo *VknInfos::fillDynamicStateCreateInfo(
        uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx, VknVector<VkDynamicState> dynamicStates)
    {
        VkPipelineDynamicStateCreateInfo *info =
            &m_dynamicStateCreateInfos[deviceIdx][renderpassIdx]
                 .insert(VkPipelineDynamicStateCreateInfo{}, subpassIdx);
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = 0; // reserved for future use
        info->dynamicStateCount = dynamicStates.getSize();
        info->pDynamicStates = dynamicStates.getData();
        return info;
    }

    VkDescriptorSetLayoutCreateInfo *VknInfos::fillDescriptorSetLayoutCreateInfo(
        VknVector<VkDescriptorSetLayoutBinding> bindings,
        VkDescriptorSetLayoutCreateFlags flags)
    {
        VkDescriptorSetLayoutCreateInfo &info =
            m_descriptorSetLayoutCreateInfos.append(VkDescriptorSetLayoutCreateInfo{});
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = flags;
        info.bindingCount = bindings.getSize();
        info.pBindings = bindings.getData();
        return &info;
    }

    VkPipelineLayoutCreateInfo *VknInfos::fillPipelineLayoutCreateInfo(
        VknIdxs &relIdxs, std::span<VkDescriptorSetLayout> setLayouts,
        VknVector<VkPushConstantRange> pushConstantRanges,
        VkPipelineLayoutCreateFlags flags)
    {
        VkPipelineLayoutCreateInfo *info =
            &m_layoutCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()]
                 .insert(VkPipelineLayoutCreateInfo{}, relIdxs.get<VkPipeline>());
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info->pNext = VK_NULL_HANDLE;
        info->flags = flags;
        info->setLayoutCount = setLayouts.size();
        info->pSetLayouts = setLayouts.empty() ? VK_NULL_HANDLE : setLayouts.data();
        info->pushConstantRangeCount = pushConstantRanges.getSize();
        info->pPushConstantRanges = pushConstantRanges.getData();
        return info;
    }

    VkPipelineCacheCreateInfo *VknInfos::fillPipelineCacheCreateInfo(
        size_t initialDataSize,
        const void *pInitialData,
        VkPipelineCacheCreateFlags flags)
    {
        VkPipelineCacheCreateInfo &info = m_cacheCreateInfos.append(VkPipelineCacheCreateInfo{});
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
        m_appName = name;
        m_filledAppName = true;
    }

    void VknInfos::fillEngineName(std::string name)
    {
        m_engineName = name;
        m_filledEngineName = true;
    }

    void VknInfos::fillInstanceExtensionNames(const char *const *names, uint32_t size)
    {
        m_enabledInstanceExtensionNames = names;
        m_enabledInstanceExtensionNamesSize = size;
        m_filledInstanceExtensionNames = true;
    }

    void VknInfos::fillDeviceExtensionNames(uint32_t deviceIdx, const char *const *names, uint32_t size)
    {
        if (m_enabledDeviceExtensionNames.size() < deviceIdx + 1)
            m_enabledDeviceExtensionNames.resize(deviceIdx + 1);
        m_enabledDeviceExtensionNames[deviceIdx] = names;
        m_enabledDeviceExtensionNamesSize = size;
    }

    void VknInfos::fillEnabledLayerNames(const char *const *names, uint32_t size)
    {
        m_enabledLayerNames = names;
        m_enabledLayerNamesSize = size;
        m_filledLayerNames = true;
    }

    void VknInfos::fillDeviceFeatures(VknFeatures features)
    {
        m_enabledFeatures.push_back(features.createInfo());
    }

    void VknInfos::fillAppInfo(uint32_t apiVersion, uint32_t applicationVersion, uint32_t engineVersion)
    {
        if (!m_filledAppName)
            throw std::runtime_error("App name not filled before filling app info.");
        if (!m_filledEngineName)
            throw std::runtime_error("Engine name not filled before filling app info.");
        m_appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        m_appInfo.pNext = VK_NULL_HANDLE;
        m_appInfo.pApplicationName = m_appName.c_str();
        m_appInfo.applicationVersion = applicationVersion;
        m_appInfo.pEngineName = m_engineName.c_str();
        m_appInfo.engineVersion = engineVersion;
        m_appInfo.apiVersion = apiVersion;

        m_filledAppInfo = true;
    }

    void VknInfos::fillInstanceCreateInfo(VkInstanceCreateFlags flags)
    {
        if (!m_filledLayerNames)
            throw std::runtime_error("Layer names not filled before filling instance create info.");
        if (!m_filledInstanceExtensionNames)
            throw std::runtime_error("Instance extension names not filled before filling instance create info.");
        if (!m_filledAppInfo)
            throw std::runtime_error("AppInfo not filled before InstanceCreateInfo.");

        m_instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        m_instanceCreateInfo.pNext = VK_NULL_HANDLE;
        m_instanceCreateInfo.flags = flags;
        m_instanceCreateInfo.pApplicationInfo = &m_appInfo;
        m_instanceCreateInfo.enabledLayerCount = m_enabledLayerNamesSize;
        if (m_enabledLayerNamesSize == 0)
            m_instanceCreateInfo.ppEnabledLayerNames = VK_NULL_HANDLE;
        else
            m_instanceCreateInfo.ppEnabledLayerNames = m_enabledLayerNames;
        m_instanceCreateInfo.enabledExtensionCount = m_enabledInstanceExtensionNamesSize;
        if (m_enabledInstanceExtensionNamesSize == 0)
            m_instanceCreateInfo.ppEnabledExtensionNames = VK_NULL_HANDLE;
        else
            m_instanceCreateInfo.ppEnabledExtensionNames = m_enabledInstanceExtensionNames;
        m_filledInstanceCreateInfo = true;
    }

    void VknInfos::fillDeviceQueueCreateInfo(VknIdxs &relIdxs, uint32_t queueFamilyIdx,
                                             uint32_t queueCount,
                                             VkDeviceQueueCreateFlags flags)
    {
        VkDeviceQueueCreateInfo &info =
            m_queueCreateInfos[relIdxs.get<VkDevice>()]
                .insert(VkDeviceQueueCreateInfo{}, queueFamilyIdx);
        info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info.queueFamilyIndex = queueFamilyIdx;
        info.queueCount = queueCount;
        info.pNext = VK_NULL_HANDLE;
        info.flags = flags; // Only flag is a protected memory bit, for a queue family that supports it
        if (!m_deviceQueuePrioritiesFilled)
            throw std::runtime_error("Queue priorities not filled before filling device queue create info.");
        info.pQueuePriorities = m_queuePriorities[relIdxs.get<VkDevice>()][queueFamilyIdx].getData();
        m_filledDeviceQueueCreateInfo = true;
    }

    VkDeviceCreateInfo *VknInfos::fillDeviceCreateInfo(uint32_t deviceIdx)
    {
        if (m_queueCreateInfos[deviceIdx].getDataSize() == 0) // filled function?
            throw std::runtime_error("Queues not selected before filling device create info.");
        VkDeviceCreateInfo &info =
            m_deviceCreateInfos.insert(deviceIdx, VkDeviceCreateInfo{});
        // default
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext = VK_NULL_HANDLE;
        info.flags = 0; // flags reserved, must = 0
        info.queueCreateInfoCount = m_queueCreateInfos[deviceIdx].getDataSize();
        info.pQueueCreateInfos = m_queueCreateInfos[deviceIdx].getData();
        // enabledLayerCount is deprecated and should not be used
        info.enabledLayerCount = 0; // ignored, value doesn't matter
        // ppEnabledLayerNames is deprecated and should not be used
        info.ppEnabledLayerNames = VK_NULL_HANDLE; // ignored, value doesn't matter
        info.enabledExtensionCount = m_enabledDeviceExtensionNamesSize;
        if (m_enabledDeviceExtensionNamesSize == 0)
            info.ppEnabledExtensionNames = VK_NULL_HANDLE;
        else
            info.ppEnabledExtensionNames = m_enabledDeviceExtensionNames[deviceIdx];
        info.pEnabledFeatures = &m_enabledFeatures[deviceIdx];

        m_filledDeviceCreateInfo = true;
        return &info;
    }

    VkSwapchainCreateInfoKHR *VknInfos::fillSwapchainCreateInfo(
        VknIdxs &relIdxs,
        VkSurfaceKHR *surface, uint32_t imageCount, VkExtent2D dimensions,
        VkSurfaceFormatKHR surfaceFormat, uint32_t numImageArrayLayers, VkImageUsageFlags usage,
        VkSharingMode sharingMode, VkSurfaceTransformFlagBitsKHR preTransform,
        VkCompositeAlphaFlagBitsKHR compositeAlpha, VkPresentModeKHR presentMode, VkBool32 clipped,
        VkSwapchainKHR oldSwapchain)
    {
        VkSwapchainCreateInfoKHR &swapchainInfo =
            m_swapchainCreateInfos[relIdxs.get<VkDevice>()]
                .insert(VkSwapchainCreateInfoKHR{}, relIdxs.get<VkSwapchainKHR>());
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.surface = *surface;                 // The surface you created
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
        VknIdxs &relIdxs, uint32_t attachIdx,
        VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp,
        VkAttachmentStoreOp storeOp, VkAttachmentLoadOp stencilLoadOp,
        VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout,
        VkImageLayout finalLayout, VkAttachmentDescriptionFlags flags)
    {
        VkAttachmentDescription &description =
            m_attachmentDescriptions[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()]
                .insert(VkAttachmentDescription{}, attachIdx);

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
        VknIdxs &relIdxs, uint32_t subpassIdx,
        uint32_t refIdx, VknAttachmentType attachmentType, uint32_t attachmentIdx,
        VkImageLayout layout)
    {
        if (attachmentType == PRESERVE_ATTACHMENT)
            m_preserveAttachments[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()]
                .insert(attachmentIdx, refIdx);
        else
        {
            VkAttachmentReference &ref =
                m_attachmentReferences[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][subpassIdx][attachmentType]
                    .insert(VkAttachmentReference{}, refIdx);
            ref.attachment = attachmentIdx;
            ref.layout = layout;
        }
    }

    // Subpass>RefType>Refs
    VknSpace<VkAttachmentReference> *VknInfos::getRenderpassAttachmentReferences(
        VknIdxs &relIdxs)
    {
        return &m_attachmentReferences[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()];
    }

    VknSpace<VkAttachmentDescription> *VknInfos::getRenderpassAttachmentDescriptions(
        VknIdxs &relIdxs)
    {
        return &m_attachmentDescriptions[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()];
    }

    VknSpace<VkAttachmentReference> *VknInfos::getSubpassAttachmentReferences(
        uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx)
    {
        return &m_attachmentReferences[deviceIdx][renderpassIdx][subpassIdx];
    }

    VknSpace<uint32_t> *VknInfos::getSubpassPreserveAttachments(
        uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx)
    {
        return &m_preserveAttachments[deviceIdx][renderpassIdx][subpassIdx];
    }

    VknSpace<uint32_t> *VknInfos::getRenderpassPreserveAttachments(
        uint32_t deviceIdx, uint32_t renderpassIdx)
    {
        return &m_preserveAttachments[deviceIdx][renderpassIdx];
    }

    VkSubpassDescription *VknInfos::fillSubpassDescription(
        uint32_t numColor, uint32_t numInput, uint32_t numResolve, uint32_t numDepthStencil,
        uint32_t numPreserve, VknIdxs &relIdxs, uint32_t subpassIdx,
        VkPipelineBindPoint pipelineBindPoint, VkSubpassDescriptionFlags flags)
    {
        m_subpassDescriptions[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()].insert(VkSubpassDescription{}, subpassIdx);
        VkSubpassDescription &description = m_subpassDescriptions[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()](subpassIdx);

        description.flags = flags;
        description.pipelineBindPoint = pipelineBindPoint;
        VknSpace<VkAttachmentReference> &inputAttachments = m_attachmentReferences[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][subpassIdx][INPUT_ATTACHMENT];
        VknSpace<VkAttachmentReference> &colorAttachments = m_attachmentReferences[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][subpassIdx][COLOR_ATTACHMENT];
        VknSpace<VkAttachmentReference> &resolveAttachments = m_attachmentReferences[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][subpassIdx][RESOLVE_ATTACHMENT];
        VknSpace<VkAttachmentReference> &depthStencilAttachments = m_attachmentReferences[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][subpassIdx][DEPTH_STENCIL_ATTACHMENT];
        VknSpace<uint32_t> &preserveAttachments = m_preserveAttachments[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][subpassIdx];

        description.inputAttachmentCount = numInput;
        description.pInputAttachments = inputAttachments.getData();

        description.colorAttachmentCount = numColor;
        description.pColorAttachments = colorAttachments.getData();

        description.preserveAttachmentCount = numPreserve;
        description.pPreserveAttachments = preserveAttachments.getData();

        if (numResolve > numColor)
            throw std::runtime_error("Resolve attachments must be less than or equal to color attachments.");

        description.pResolveAttachments = resolveAttachments.getData();
        if (numDepthStencil > 1)
            throw std::runtime_error("Too many depth stencil attachments in subpass description.");

        description.pDepthStencilAttachment = &depthStencilAttachments(0);

        return &description;
    }

    VkSubpassDependency *VknInfos::fillSubpassDependency(
        VknIdxs &relIdxs,
        uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask,
        VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStageMask, VkAccessFlags dstAccessMask)
    {
        VkSubpassDependency &dependency =
            m_subpassDependencies[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()]
                .append(VkSubpassDependency{});
        dependency.srcSubpass = srcSubpass;
        dependency.dstSubpass = dstSubpass;
        dependency.srcStageMask = srcStageMask;
        dependency.srcAccessMask = srcAccessMask;
        dependency.dstStageMask = dstStageMask;
        dependency.dstAccessMask = dstAccessMask;

        return &dependency;
    }

    VkVertexInputBindingDescription *VknInfos::fillVertexInputBindingDescription(
        VknIdxs &relIdxs, uint32_t bindIdx,
        uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)

    {
        VkVertexInputBindingDescription *description =
            &m_vertexInputBindings[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()]
                 .insert(VkVertexInputBindingDescription{}, bindIdx);

        description->binding = binding;
        description->stride = stride;
        description->inputRate = inputRate;
        return description;
    }

    VkVertexInputAttributeDescription *VknInfos::fillVertexInputAttributeDescription(
        VknIdxs &relIdxs, uint32_t attributeIdx, uint32_t binding,
        uint32_t location, VkFormat format, uint32_t offset)
    {
        VkVertexInputAttributeDescription *description =
            &m_vertexInputAttributes[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()]
                 .insert(VkVertexInputAttributeDescription{}, attributeIdx);

        description->binding = binding;
        description->location = location;
        description->format = format;
        description->offset = offset;
        return description;
    }

    VknSpace<VkVertexInputBindingDescription> *VknInfos::getVertexInputBindings(
        uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx)
    {
        return &m_vertexInputBindings[deviceIdx][renderpassIdx][subpassIdx];
    }

    VknSpace<VkVertexInputAttributeDescription> *VknInfos::getVertexInputAttributes(
        uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx)
    {
        return &m_vertexInputAttributes[deviceIdx][renderpassIdx][subpassIdx];
    }

    VkImageViewCreateInfo *VknInfos::getImageViewCreateInfo(VknIdxs &relIdxs)
    {
        return &m_imageViewCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkSwapchainKHR>()](relIdxs.get<VkImageView>());
    }

    VkSwapchainCreateInfoKHR *VknInfos::getSwapchainCreateInfo(VknIdxs &relIdxs)
    {
        return &m_swapchainCreateInfos[relIdxs.get<VkDevice>()](relIdxs.get<VkSwapchainKHR>());
    }

    VkFramebufferCreateInfo *VknInfos::getFramebufferCreateInfo(VknIdxs &relIdxs)
    {
        return &m_framebufferCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()](relIdxs.get<VkFramebuffer>());
    }

    VkFramebufferCreateInfo *VknInfos::fillFramebufferCreateInfo(VknIdxs &relIdxs,
                                                                 VkRenderPass *renderpass, std::span<VkImageView> attachments,
                                                                 uint32_t width, uint32_t height, uint32_t numLayers,
                                                                 VkFramebufferCreateFlags &flags)
    {
        VkFramebufferCreateInfo *info =
            &m_framebufferCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()]
                 .insert(VkFramebufferCreateInfo{}, relIdxs.get<VkFramebuffer>());
        info->sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info->pNext = VK_NULL_HANDLE;
        info->renderPass = *renderpass;
        info->attachmentCount = attachments.size();
        info->pAttachments = attachments.empty() ? VK_NULL_HANDLE : attachments.data();
        info->width = width;
        info->height = height;
        info->layers = numLayers;
        info->flags = flags;
        return info;
    }

    VkImageCreateInfo *VknInfos::fillImageCreateInfo(
        VknIdxs &relId, VkImageType imageType, VkFormat format, VkExtent3D extent,
        uint32_t mipLevels, uint32_t arrayLayers, VkSampleCountFlagBits samples, VkImageTiling tiling,
        VkImageUsageFlags usage, VkImageLayout initialLayout,
        VkImageCreateFlags flags)
    {
        m_imageCreateInfos.push_back(VkImageCreateInfo{});
        VkImageCreateInfo *info = &m_imageCreateInfos.back();
        info->sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info->pNext = VK_NULL_HANDLE;
        info->flags = flags;
        info->imageType = imageType;
        info->format = format;
        info->extent = extent;
        info->mipLevels = mipLevels;
        info->arrayLayers = arrayLayers;
        info->samples = samples;
        info->tiling = tiling;
        info->usage = usage;
        info->sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info->queueFamilyIndexCount = 0;
        info->pQueueFamilyIndices = VK_NULL_HANDLE;
        info->initialLayout = initialLayout;
        return info;
    }

    VkImageViewCreateInfo *VknInfos::fillImageViewCreateInfo(VknIdxs &relIdxs,
                                                             VkImage &image, VkImageViewType &viewType, VkFormat &format,
                                                             VkComponentMapping &components, VkImageSubresourceRange &subresourceRange,
                                                             VkImageViewCreateFlags &flags)

    {
        VkImageViewCreateInfo &info =
            m_imageViewCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkSwapchainKHR>()]
                .insert(VkImageViewCreateInfo{}, relIdxs.get<VkImageView>());
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.pNext = VK_NULL_HANDLE;
        info.image = image;
        info.viewType = viewType;
        info.format = format;
        info.components = components;
        info.subresourceRange = subresourceRange;
        info.flags = flags;
        return &info;
    }
}