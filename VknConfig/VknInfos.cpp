#include "include/VknInfos.hpp"

namespace vkn
{
    VknInfos::VknInfos()
    {
    }

    void VknInfos::fillDeviceQueuePriorities(VknIdxs &relIdxs, uint32_t queueFamilyIdx,
                                             std::vector<float> priorities)
    {
        this->initVectors<float>(relIdxs.get<VkDevice>(), queueFamilyIdx,
                                 priorities.size() - 1, m_queuePriorities);
        for (auto &priority : priorities)
            m_queuePriorities[relIdxs.get<VkDevice>()][queueFamilyIdx].push_back(priority);
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
        if (relIdxs.get<VkDevice>() + 1 <= m_gfxPipelineCreateInfos.size() &&
            relIdxs.get<VkRenderPass>() + 1 <= m_gfxPipelineCreateInfos[relIdxs.get<VkDevice>()].size() &&
            relIdxs.get<VkPipeline>() + 1 <= m_gfxPipelineCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()].size() &&
            m_gfxPipelineCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()].stageCount > 0)
            throw std::runtime_error("Pipeline create info has already been filled.");
        this->initVectors<VkGraphicsPipelineCreateInfo>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), relIdxs.get<VkPipeline>(), m_gfxPipelineCreateInfos);
        m_gfxPipelineCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()] =
            VkGraphicsPipelineCreateInfo{};
        VkGraphicsPipelineCreateInfo &info =
            m_gfxPipelineCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()];
        info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        info.pNext = VK_NULL_HANDLE;
        info.flags = flags;
        info.stageCount = m_shaderStageCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()].size(); // Need fill
        info.pStages = m_shaderStageCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()].data();    // Need fill
        if (m_filledVertexInputStateInfo)
            info.pVertexInputState =
                &m_vertexInputStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()]; // Can be null if VK_DYNAMIC_STATE_VERTEX_INPUT_EXT
                                                                                                                                 // set in pDynamicState below. Ignored if mesh shader stage
                                                                                                                                 // included in pStages.
        if (m_filledInputAssemblyStateInfo)
            info.pInputAssemblyState =
                &m_inputAssemblyStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()]; // Can be null if VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE
                                                                                                                                   // and VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY are set in pDynamicState,
                                                                                                                                   // below and dynamicPrimitiveTopologyUnrestricted is VK_TRUE in the
                                                                                                                                   // VkPhysicalDeviceExtendedDynamicState3PropertiesEXT struct,
                                                                                                                                   // which requires the VK_EXT_extended_dynamic_state3 extension.
                                                                                                                                   // Also ignored if mesh shader stage included in pStages.
        if (m_filledTessellationStateInfo)
            info.pTessellationState =
                &m_tessellationStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()]; // Can be null if the VK_DYNAMIC_STATE_PATCH_CONTROL_POINTS_EXT
                                                                                                                                  // is set in the pDynamicState below.
        if (m_filledViewportStateInfo)
            info.pViewportState =
                &m_viewportStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()]; // Can be null if rasterization not enabled.
                                                                                                                              // Can be null if VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT
                                                                                                                              // and VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT are set in pDynamicState
                                                                                                                              // below, and also requires the VK_EXT_extended_dynamic_state3
                                                                                                                              // extension enabled.
        if (m_filledRasterizationStateInfo)
            info.pRasterizationState =
                &m_rasterizationStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()]; // I guess this could be null if rasterization is not enabled.
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
                &m_multisampleStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()]; // Can be null if rasterization not enabled.
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
                &m_depthStencilStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()]; // Can be null if rasterization not enabled.
                                                                                                                                  // Can be null if VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE,
                                                                                                                                  // VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE, VK_DYNAMIC_STATE_DEPTH_COMPARE_OP,
                                                                                                                                  // VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE,
                                                                                                                                  // VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE, VK_DYNAMIC_STATE_STENCIL_OP,
                                                                                                                                  // and VK_DYNAMIC_STATE_DEPTH_BOUNDS are set in pDynamicState below
                                                                                                                                  // Also requires the  VK_EXT_extended_dynamic_state3 extension.
        if (m_filledColorBlendStateInfo)
            info.pColorBlendState =
                &m_colorBlendStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()]; // Can be null if rasterization not enabled.
                                                                                                                                // Can be null if VK_DYNAMIC_STATE_LOGIC_OP_ENABLE_EXT,
                                                                                                                                // VK_DYNAMIC_STATE_LOGIC_OP_EXT,
                                                                                                                                // VK_DYNAMIC_STATE_COLOR_BLEND_ENABLE_EXT,
                                                                                                                                // VK_DYNAMIC_STATE_COLOR_BLEND_EQUATION_EXT,
                                                                                                                                // VK_DYNAMIC_STATE_COLOR_WRITE_MASK_EXT, and
                                                                                                                                // VK_DYNAMIC_STATE_BLEND_CONSTANTS are set in pDynamicState below
                                                                                                                                // Requires the VK_EXT_extended_dynamic_state3 extension.
        if (m_filledDynamicStateInfo)
            info.pDynamicState =
                &m_dynamicStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()]; // Could be null if no state in the pipeline needs to be dynamic.
        info.layout = *layout;                                                                                               // Need fill
        info.renderPass = renderpass;                                                                                        // Need fill
        info.subpass = relIdxs.get<VkPipeline>();                                                                            // Need fill
        info.basePipelineHandle = basePipelineHandle;                                                                        // Need fill
        info.basePipelineIndex = basePipelineIndex;                                                                          // Need fill

        return &info;
    }

    void VknInfos::initRenderpass(VknIdxs &relIdxs)
    {
        this->initVectors<VkRenderPassCreateInfo>(relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(),
                                                  m_renderpassCreateInfos);
        this->initVectors<VkAttachmentDescription>(relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(),
                                                   0, m_attachmentDescriptions);
        this->initVectors<VkSubpassDescription>(relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(),
                                                0, m_subpassDescriptions);
        this->initVectors<VkSubpassDependency>(relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(),
                                               0, m_subpassDependencies);
        this->initVectors<VkAttachmentReference>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), 0,
            NUM_ATTACHMENT_TYPES - 1, 0, m_attachmentReferences);
    }

    VkRenderPassCreateInfo *VknInfos::fillRenderpassCreateInfo(VknIdxs &relIdxs,
                                                               uint32_t numAttachments,
                                                               uint32_t numSubpasses,
                                                               uint32_t numSubpassDeps,
                                                               VkRenderPassCreateFlags flags)
    {
        this->initVectors<VkRenderPassCreateInfo>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), m_renderpassCreateInfos);
        m_renderpassCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()] = VkRenderPassCreateInfo{};
        VkRenderPassCreateInfo &renderpassInfo = m_renderpassCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()];
        std::vector<VkAttachmentDescription> &attachmentDescriptions =
            m_attachmentDescriptions[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()];
        std::vector<VkSubpassDescription> &subpassDescriptions =
            m_subpassDescriptions[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()];
        std::vector<VkSubpassDependency> &subpassDependencies =
            m_subpassDependencies[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()];

        renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderpassInfo.pNext = VK_NULL_HANDLE;
        renderpassInfo.flags = flags;
        renderpassInfo.attachmentCount = numAttachments;
        if (numAttachments == 0)
            renderpassInfo.pAttachments = VK_NULL_HANDLE;
        else
            renderpassInfo.pAttachments = attachmentDescriptions.data();
        renderpassInfo.subpassCount = numSubpasses;
        if (numSubpasses == 0)
            renderpassInfo.pSubpasses = VK_NULL_HANDLE;
        else
            renderpassInfo.pSubpasses = subpassDescriptions.data();
        renderpassInfo.dependencyCount = numSubpassDeps;
        if (numSubpassDeps == 0)
            renderpassInfo.pDependencies = VK_NULL_HANDLE;
        else
            renderpassInfo.pDependencies = subpassDependencies.data();

        return &renderpassInfo;
    }

    VkShaderModuleCreateInfo *VknInfos::fillShaderModuleCreateInfo(
        VknIdxs &relIdxs, std::vector<char> *code)
    {
        this->initVectors<VkShaderModuleCreateInfo>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(),
            relIdxs.get<VkPipeline>(), relIdxs.get<VkShaderModule>(), m_shaderModuleCreateInfos);
        m_shaderModuleCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()][relIdxs.get<VkShaderModule>()] =
            VkShaderModuleCreateInfo{};
        VkShaderModuleCreateInfo *info =
            &m_shaderModuleCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()][relIdxs.get<VkShaderModule>()];
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
        this->initVectors<VkPipelineShaderStageCreateInfo>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), relIdxs.get<VkPipeline>(),
            relIdxs.get<VkShaderModule>(), m_shaderStageCreateInfos);
        m_shaderStageCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()][relIdxs.get<VkShaderModule>()] =
            VkPipelineShaderStageCreateInfo{};
        VkPipelineShaderStageCreateInfo *info =
            &m_shaderStageCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()][relIdxs.get<VkShaderModule>()];
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
        this->initVectors<VkPipelineVertexInputStateCreateInfo>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), relIdxs.get<VkPipeline>(), m_vertexInputStateCreateInfos);
        this->initVectors<VkVertexInputBindingDescription>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), relIdxs.get<VkPipeline>(), 0, m_vertexInputBindings);
        this->initVectors<VkVertexInputAttributeDescription>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), relIdxs.get<VkPipeline>(), 0, m_vertexInputAttributes);

        std::vector<VkVertexInputBindingDescription> *vertexBindingDescriptions =
            &m_vertexInputBindings[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()];
        std::vector<VkVertexInputAttributeDescription> *vertexAttributeDescriptions =
            &m_vertexInputAttributes[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()];

        m_vertexInputStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()] =
            VkPipelineVertexInputStateCreateInfo{};
        VkPipelineVertexInputStateCreateInfo *info =
            &m_vertexInputStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()];
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        info->pNext = VK_NULL_HANDLE;
        info->flags = 0; // reserved for future use
        info->vertexBindingDescriptionCount = numBindings;
        if (numBindings == 0)
            info->pVertexBindingDescriptions = VK_NULL_HANDLE;
        else
            info->pVertexBindingDescriptions = vertexBindingDescriptions->data();
        info->vertexAttributeDescriptionCount = numAttributes;
        if (numAttributes == 0)
            info->pVertexAttributeDescriptions = VK_NULL_HANDLE;
        else
            info->pVertexAttributeDescriptions = vertexAttributeDescriptions->data();
        m_filledVertexInputStateInfo = true;
        return info;
    }

    VkPipelineInputAssemblyStateCreateInfo *VknInfos::fillInputAssemblyStateCreateInfo(
        VknIdxs &relIdxs, VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable)
    {
        this->initVectors<VkPipelineInputAssemblyStateCreateInfo>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), relIdxs.get<VkPipeline>(),
            m_inputAssemblyStateCreateInfos);
        m_inputAssemblyStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()] =
            VkPipelineInputAssemblyStateCreateInfo{};
        VkPipelineInputAssemblyStateCreateInfo *info =
            &m_inputAssemblyStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()];
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
        this->initVectors<VkPipelineTessellationStateCreateInfo>(
            deviceIdx, renderpassIdx, subpassIdx, m_tessellationStateCreateInfos);
        m_tessellationStateCreateInfos[deviceIdx][renderpassIdx][subpassIdx] =
            VkPipelineTessellationStateCreateInfo{};
        VkPipelineTessellationStateCreateInfo *info =
            &m_tessellationStateCreateInfos[deviceIdx][renderpassIdx][subpassIdx];
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = 0; // reserved for future use
        info->patchControlPoints = patchControlPoints;
        return info;
    }

    VkPipelineViewportStateCreateInfo *VknInfos::fillViewportStateCreateInfo(
        VknIdxs &relIdxs,
        std::vector<VkViewport> *viewports, std::vector<VkRect2D> *scissors)
    {
        this->initVectors<VkPipelineViewportStateCreateInfo>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), relIdxs.get<VkPipeline>(),
            m_viewportStateCreateInfos);
        m_viewportStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()] =
            VkPipelineViewportStateCreateInfo{};
        VkPipelineViewportStateCreateInfo *info =
            &m_viewportStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()];
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = 0; // reserved for future use
        info->viewportCount = viewports->size();
        if (viewports->size() == 0)
            info->pViewports = VK_NULL_HANDLE;
        else
            info->pViewports = viewports->data();
        info->scissorCount = scissors->size();
        if (scissors->size() == 0)
            info->pScissors = VK_NULL_HANDLE;
        else
            info->pScissors = scissors->data();
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
        this->initVectors<VkPipelineRasterizationStateCreateInfo>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), relIdxs.get<VkPipeline>(), m_rasterizationStateCreateInfos);
        m_rasterizationStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()] =
            VkPipelineRasterizationStateCreateInfo{};
        VkPipelineRasterizationStateCreateInfo *info =
            &m_rasterizationStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()];
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
        this->initVectors<VkPipelineMultisampleStateCreateInfo>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), relIdxs.get<VkPipeline>(), m_multisampleStateCreateInfos);
        m_multisampleStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()] =
            VkPipelineMultisampleStateCreateInfo{};
        VkPipelineMultisampleStateCreateInfo *info =
            &m_multisampleStateCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()];
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
        this->initVectors<VkPipelineDepthStencilStateCreateInfo>(
            deviceIdx, renderpassIdx, subpassIdx, m_depthStencilStateCreateInfos);
        m_depthStencilStateCreateInfos[deviceIdx][renderpassIdx][subpassIdx] =
            VkPipelineDepthStencilStateCreateInfo{};
        VkPipelineDepthStencilStateCreateInfo *info =
            &m_depthStencilStateCreateInfos[deviceIdx][renderpassIdx][subpassIdx];
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
        std::vector<VkPipelineColorBlendAttachmentState> attachments,
        float blendConstants[4], VkBool32 logicOpEnable,
        VkPipelineColorBlendStateCreateFlags flags)
    {
        this->initVectors<VkPipelineColorBlendStateCreateInfo>(
            deviceIdx, renderpassIdx, subpassIdx, m_colorBlendStateCreateInfos);
        m_colorBlendStateCreateInfos[deviceIdx][renderpassIdx][subpassIdx] =
            VkPipelineColorBlendStateCreateInfo{};
        VkPipelineColorBlendStateCreateInfo *info =
            &m_colorBlendStateCreateInfos[deviceIdx][renderpassIdx][subpassIdx];
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
        uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx, std::vector<VkDynamicState> dynamicStates)
    {
        this->initVectors<VkPipelineDynamicStateCreateInfo>(
            deviceIdx, renderpassIdx, subpassIdx, m_dynamicStateCreateInfos);
        m_dynamicStateCreateInfos[deviceIdx][renderpassIdx][subpassIdx] =
            VkPipelineDynamicStateCreateInfo{};
        VkPipelineDynamicStateCreateInfo *info =
            &m_dynamicStateCreateInfos[deviceIdx][renderpassIdx][subpassIdx];
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
        std::vector<VkDescriptorSetLayoutBinding> bindings,
        VkDescriptorSetLayoutCreateFlags flags)
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
        VknIdxs &relIdxs, std::span<VkDescriptorSetLayout> setLayouts,
        std::vector<VkPushConstantRange> pushConstantRanges,
        VkPipelineLayoutCreateFlags flags)
    {
        this->initVectors(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), relIdxs.get<VkPipeline>(), m_layoutCreateInfos);
        m_layoutCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()] =
            VkPipelineLayoutCreateInfo{};
        VkPipelineLayoutCreateInfo *info =
            &m_layoutCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()];
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info->pNext = VK_NULL_HANDLE;
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
        this->initVectors<VkDeviceQueueCreateInfo>(
            relIdxs.get<VkDevice>(), queueFamilyIdx, m_queueCreateInfos);
        m_queueCreateInfos[relIdxs.get<VkDevice>()][queueFamilyIdx] = VkDeviceQueueCreateInfo{};
        VkDeviceQueueCreateInfo &info = m_queueCreateInfos[relIdxs.get<VkDevice>()][queueFamilyIdx];
        info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info.queueFamilyIndex = queueFamilyIdx;
        info.queueCount = queueCount;
        info.pNext = VK_NULL_HANDLE;
        info.flags = flags; // Only flag is a protected memory bit, for a queue family that supports it
        if (!m_deviceQueuePrioritiesFilled)
            throw std::runtime_error("Queue priorities not filled before filling device queue create info.");
        info.pQueuePriorities = m_queuePriorities[relIdxs.get<VkDevice>()][queueFamilyIdx].data();
        m_filledDeviceQueueCreateInfo = true;
    }

    VkDeviceCreateInfo *VknInfos::fillDeviceCreateInfo(uint32_t deviceIdx)
    {
        this->initVectors<VkDeviceCreateInfo>(deviceIdx, m_deviceCreateInfos);
        if (m_queueCreateInfos[deviceIdx].size() == 0) // filled function?
            throw std::runtime_error("Queues not selected before filling device create info.");
        m_deviceCreateInfos[deviceIdx] = VkDeviceCreateInfo{};
        VkDeviceCreateInfo &info = m_deviceCreateInfos[deviceIdx];
        // default
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext = VK_NULL_HANDLE;
        info.flags = 0; // flags reserved, must = 0
        info.queueCreateInfoCount = m_queueCreateInfos[deviceIdx].size();
        info.pQueueCreateInfos = m_queueCreateInfos[deviceIdx].data();
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
        this->initVectors<VkSwapchainCreateInfoKHR>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkSwapchainKHR>(), m_swapchainCreateInfos);
        m_swapchainCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkSwapchainKHR>()] = VkSwapchainCreateInfoKHR{};
        VkSwapchainCreateInfoKHR &swapchainInfo = m_swapchainCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkSwapchainKHR>()];
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
        this->initVectors<VkAttachmentDescription>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), attachIdx, m_attachmentDescriptions);
        m_attachmentDescriptions[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][attachIdx] = VkAttachmentDescription{};
        VkAttachmentDescription &description =
            m_attachmentDescriptions[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][attachIdx];
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

        this->initVectors<VkAttachmentReference>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), subpassIdx, attachmentType,
            refIdx, m_attachmentReferences);
        this->initVectors<uint32_t>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), subpassIdx,
            refIdx, m_preserveAttachments);
        if (attachmentType == PRESERVE_ATTACHMENT)
            m_preserveAttachments[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()][refIdx] = attachmentIdx;
        else
            m_attachmentReferences[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][subpassIdx][attachmentType][refIdx] = VkAttachmentReference{};
        VkAttachmentReference &ref = m_attachmentReferences[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][subpassIdx][attachmentType][refIdx];
        ref.attachment = attachmentIdx;
        ref.layout = layout;
    }

    std::vector<std::vector<std::vector<VkAttachmentReference>>> *VknInfos::getRenderpassAttachmentReferences(
        VknIdxs &relIdxs)
    {
        this->initVectors<VkAttachmentReference>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), 0, NUM_ATTACHMENT_TYPES - 1, 0, m_attachmentReferences);
        return &m_attachmentReferences[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()];
    }

    std::vector<VkAttachmentDescription> *VknInfos::getRenderpassAttachmentDescriptions(
        VknIdxs &relIdxs)
    {
        this->initVectors<VkAttachmentDescription>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), 0, m_attachmentDescriptions);
        return &m_attachmentDescriptions[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()];
    }

    std::vector<std::vector<VkAttachmentReference>> *VknInfos::getSubpassAttachmentReferences(
        uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx)
    {
        this->initVectors<VkAttachmentReference>(
            deviceIdx, renderpassIdx, subpassIdx, NUM_ATTACHMENT_TYPES - 1, 0, m_attachmentReferences);
        return &m_attachmentReferences[deviceIdx][renderpassIdx][subpassIdx];
    }

    std::vector<uint32_t> *VknInfos::getSubpassPreserveAttachments(
        uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx)
    {
        this->initVectors<uint32_t>(deviceIdx, renderpassIdx, subpassIdx, 0, m_preserveAttachments);
        return &m_preserveAttachments[deviceIdx][renderpassIdx][subpassIdx];
    }

    std::vector<std::vector<uint32_t>> *VknInfos::getRenderpassPreserveAttachments(
        uint32_t deviceIdx, uint32_t renderpassIdx)
    {
        this->initVectors<uint32_t>(deviceIdx, renderpassIdx, 0, 0, m_preserveAttachments);
        return &m_preserveAttachments[deviceIdx][renderpassIdx];
    }

    VkSubpassDescription *VknInfos::fillSubpassDescription(
        uint32_t numColor, uint32_t numInput, uint32_t numResolve, uint32_t numDepthStencil,
        uint32_t numPreserve, VknIdxs &relIdxs, uint32_t subpassIdx,
        VkPipelineBindPoint pipelineBindPoint, VkSubpassDescriptionFlags flags)
    {
        this->initVectors<VkSubpassDescription>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), subpassIdx, m_subpassDescriptions);
        m_subpassDescriptions[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][subpassIdx] = VkSubpassDescription{};
        VkSubpassDescription &description = m_subpassDescriptions[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][subpassIdx];

        description.flags = flags;
        description.pipelineBindPoint = pipelineBindPoint;
        std::vector<VkAttachmentReference> &inputAttachments = m_attachmentReferences[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][subpassIdx][INPUT_ATTACHMENT];
        std::vector<VkAttachmentReference> &colorAttachments = m_attachmentReferences[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][subpassIdx][COLOR_ATTACHMENT];
        std::vector<VkAttachmentReference> &resolveAttachments = m_attachmentReferences[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][subpassIdx][RESOLVE_ATTACHMENT];
        std::vector<VkAttachmentReference> &depthStencilAttachments = m_attachmentReferences[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][subpassIdx][DEPTH_STENCIL_ATTACHMENT];
        std::vector<uint32_t> &preserveAttachments = m_preserveAttachments[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][subpassIdx];
        if (numInput == 0)
        {
            description.inputAttachmentCount = 0;
            description.pInputAttachments = VK_NULL_HANDLE;
        }
        else
        {
            description.inputAttachmentCount = numInput;
            description.pInputAttachments = inputAttachments.data();
        }
        if (numColor == 0)
        {
            description.colorAttachmentCount = 0;
            description.pColorAttachments = VK_NULL_HANDLE;
        }
        else
        {
            description.colorAttachmentCount = numColor;
            description.pColorAttachments = colorAttachments.data();
        }
        if (numPreserve == 0)
        {
            description.colorAttachmentCount = 0;
            description.pPreserveAttachments = VK_NULL_HANDLE;
        }
        else
        {
            description.preserveAttachmentCount = numPreserve;
            description.pPreserveAttachments = preserveAttachments.data();
        }
        if (numResolve > numColor)
            throw std::runtime_error("Resolve attachments must be less than or equal to color attachments.");
        else if (numResolve == 0)
            description.pResolveAttachments = VK_NULL_HANDLE;
        else
            description.pResolveAttachments = resolveAttachments.data();
        if (numDepthStencil > 1)
            throw std::runtime_error("Too many depth stencil attachments in subpass description.");
        if (numDepthStencil == 0)
            description.pDepthStencilAttachment = VK_NULL_HANDLE;
        else
            description.pDepthStencilAttachment = &depthStencilAttachments[0];

        return &description;
    }

    VkSubpassDependency *VknInfos::fillSubpassDependency(
        VknIdxs &relIdxs,
        uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask,
        VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStageMask, VkAccessFlags dstAccessMask)
    {
        this->initVectors<VkSubpassDependency>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), 0,
            m_subpassDependencies);
        m_subpassDependencies[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()].push_back(VkSubpassDependency{});
        VkSubpassDependency &dependency = m_subpassDependencies[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()].back();
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
        this->initVectors<VkVertexInputBindingDescription>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), relIdxs.get<VkPipeline>(), bindIdx, m_vertexInputBindings);
        m_vertexInputBindings[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()][bindIdx] =
            VkVertexInputBindingDescription{};
        VkVertexInputBindingDescription *description =
            &m_vertexInputBindings[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()][bindIdx];
        description->binding = binding;
        description->stride = stride;
        description->inputRate = inputRate;
        return description;
    }

    VkVertexInputAttributeDescription *VknInfos::fillVertexInputAttributeDescription(
        VknIdxs &relIdxs, uint32_t attributeIdx, uint32_t binding,
        uint32_t location, VkFormat format, uint32_t offset)
    {
        this->initVectors<VkVertexInputAttributeDescription>(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), relIdxs.get<VkPipeline>(), attributeIdx, m_vertexInputAttributes);
        m_vertexInputAttributes[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()][attributeIdx] =
            VkVertexInputAttributeDescription{};
        VkVertexInputAttributeDescription *description =
            &m_vertexInputAttributes[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkPipeline>()][attributeIdx];
        description->binding = binding;
        description->location = location;
        description->format = format;
        description->offset = offset;
        return description;
    }

    std::vector<VkVertexInputBindingDescription> *VknInfos::getVertexInputBindings(
        uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx)
    {
        return &m_vertexInputBindings[deviceIdx][renderpassIdx][subpassIdx];
    }

    std::vector<VkVertexInputAttributeDescription> *VknInfos::getVertexInputAttributes(
        uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t subpassIdx)
    {
        return &m_vertexInputAttributes[deviceIdx][renderpassIdx][subpassIdx];
    }

    VkImageViewCreateInfo *VknInfos::getImageViewCreateInfo(VknIdxs &relIdxs)
    {
        return &m_imageViewCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkSwapchainKHR>()][relIdxs.get<VkImageView>()];
    }

    VkSwapchainCreateInfoKHR *VknInfos::getSwapchainCreateInfo(VknIdxs &relIdxs)
    {
        return &m_swapchainCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkSwapchainKHR>()];
    }

    VkFramebufferCreateInfo *VknInfos::getFramebufferCreateInfo(VknIdxs &relIdxs)
    {
        return &m_framebufferCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkFramebuffer>()];
    }

    VkFramebufferCreateInfo *VknInfos::fillFramebufferCreateInfo(VknIdxs &relIdxs,
                                                                 VkRenderPass *renderpass, std::span<VkImageView> attachments,
                                                                 uint32_t width, uint32_t height, uint32_t numLayers,
                                                                 VkFramebufferCreateFlags &flags)
    {
        this->initVectors(
            relIdxs.get<VkDevice>(), relIdxs.get<VkRenderPass>(), relIdxs.get<VkFramebuffer>(),
            m_framebufferCreateInfos);
        m_framebufferCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkFramebuffer>()] = VkFramebufferCreateInfo{};
        VkFramebufferCreateInfo &info =
            m_framebufferCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkRenderPass>()][relIdxs.get<VkFramebuffer>()];
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.pNext = VK_NULL_HANDLE;
        info.renderPass = *renderpass;
        info.attachmentCount = attachments.size();
        if (info.attachmentCount == 0)
            info.pAttachments = VK_NULL_HANDLE;
        else
            info.pAttachments = attachments.data();
        info.width = width;
        info.height = height;
        info.layers = numLayers;
        info.flags = flags;
        return &info;
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
        this->initVectors(relIdxs.get<VkDevice>(), relIdxs.get<VkSwapchainKHR>(),
                          relIdxs.get<VkImageView>(), m_imageViewCreateInfos);
        m_imageViewCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkSwapchainKHR>()][relIdxs.get<VkImageView>()] = VkImageViewCreateInfo{};
        VkImageViewCreateInfo &info = m_imageViewCreateInfos[relIdxs.get<VkDevice>()][relIdxs.get<VkSwapchainKHR>()][relIdxs.get<VkImageView>()];
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