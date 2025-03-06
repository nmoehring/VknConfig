#include <stdexcept>
#include <limits>
#include <algorithm>

#include "VknInfos.hpp"

namespace vkn
{
    VknInfos::VknInfos()
    {
    }

    void VknInfos::fillDeviceQueuePriorities(uint32_t deviceIdx, uint32_t queueFamilyIdx,
                                             std::vector<float> priorities)
    {
        this->initVectors<float>(deviceIdx, queueFamilyIdx, priorities.size() - 1, m_queuePriorities);
        for (auto &priority : priorities)
            m_queuePriorities[deviceIdx][queueFamilyIdx].push_back(priority);
        m_deviceQueuePrioritiesFilled = true;
    }

    void VknInfos::setNumDeviceQueueFamilies(int num, uint32_t deviceIdx)
    {
        if (m_numQueueFamilies.size() < deviceIdx + 1)
            m_numQueueFamilies.resize(deviceIdx + 1);
        m_numQueueFamilies[deviceIdx] = num;
    }

    void VknInfos::fillRenderPassPtrs(
        uint32_t deviceIdx, uint32_t renderPassIdx, VkRenderPass *renderPass,
        const bool *renderPassCreated)
    {
        this->initVectors<VkRenderPass *>(deviceIdx, renderPassIdx, m_renderPasses);
        m_renderPasses[deviceIdx][renderPassIdx] = renderPass;

        this->initVectors<const bool *>(deviceIdx, renderPassIdx, m_renderPassCreatedPtrs);
        m_renderPassCreatedPtrs[deviceIdx][renderPassIdx] = renderPassCreated;
    }

    VkGraphicsPipelineCreateInfo *VknInfos::fillGfxPipelineCreateInfo(
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
        VkPipelineLayout *layout, VkPipeline basePipelineHandle,
        int32_t basePipelineIndex, VkPipelineCreateFlags flags)
    {
        if (deviceIdx + 1 <= m_gfxPipelineCreateInfos.size() &&
            renderPassIdx + 1 <= m_gfxPipelineCreateInfos[deviceIdx].size() &&
            subpassIdx + 1 <= m_gfxPipelineCreateInfos[deviceIdx][renderPassIdx].size() &&
            m_gfxPipelineCreateInfos[deviceIdx][renderPassIdx][subpassIdx].stageCount > 0)
            throw std::runtime_error("Pipeline create info has already been filled.");
        this->initVectors<VkGraphicsPipelineCreateInfo>(
            deviceIdx, renderPassIdx, subpassIdx, m_gfxPipelineCreateInfos);
        VkRenderPass *renderPass = m_renderPasses[deviceIdx][renderPassIdx];
        m_gfxPipelineCreateInfos[deviceIdx][renderPassIdx][subpassIdx] =
            VkGraphicsPipelineCreateInfo{};
        VkGraphicsPipelineCreateInfo &info =
            m_gfxPipelineCreateInfos[deviceIdx][renderPassIdx][subpassIdx];
        info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        info.pNext = VK_NULL_HANDLE;
        info.flags = flags;
        info.stageCount = m_shaderStageCreateInfos[deviceIdx][renderPassIdx][subpassIdx].size(); // Need fill
        info.pStages = m_shaderStageCreateInfos[deviceIdx][renderPassIdx][subpassIdx].data();    // Need fill
        if (m_filledVertexInputStateInfo)
            info.pVertexInputState =
                &m_vertexInputStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx]; // Can be null if VK_DYNAMIC_STATE_VERTEX_INPUT_EXT
                                                                                      // set in pDynamicState below. Ignored if mesh shader stage
                                                                                      // included in pStages.
        if (m_filledInputAssemblyStateInfo)
            info.pInputAssemblyState =
                &m_inputAssemblyStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx]; // Can be null if VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE
                                                                                        // and VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY are set in pDynamicState,
                                                                                        // below and dynamicPrimitiveTopologyUnrestricted is VK_TRUE in the
                                                                                        // VkPhysicalDeviceExtendedDynamicState3PropertiesEXT struct,
                                                                                        // which requires the VK_EXT_extended_dynamic_state3 extension.
                                                                                        // Also ignored if mesh shader stage included in pStages.
        if (m_filledTessellationStateInfo)
            info.pTessellationState =
                &m_tessellationStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx]; // Can be null if the VK_DYNAMIC_STATE_PATCH_CONTROL_POINTS_EXT
                                                                                       // is set in the pDynamicState below.
        if (m_filledViewportStateInfo)
            info.pViewportState =
                &m_viewportStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx]; // Can be null if rasterization not enabled.
                                                                                   // Can be null if VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT
                                                                                   // and VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT are set in pDynamicState
                                                                                   // below, and also requires the VK_EXT_extended_dynamic_state3
                                                                                   // extension enabled.
        if (m_filledRasterizationStateInfo)
            info.pRasterizationState =
                &m_rasterizationStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx]; // I guess this could be null if rasterization is not enabled.
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
                &m_multisampleStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx]; // Can be null if rasterization not enabled.
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
                &m_depthStencilStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx]; // Can be null if rasterization not enabled.
                                                                                       // Can be null if VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE,
                                                                                       // VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE, VK_DYNAMIC_STATE_DEPTH_COMPARE_OP,
                                                                                       // VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE,
                                                                                       // VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE, VK_DYNAMIC_STATE_STENCIL_OP,
                                                                                       // and VK_DYNAMIC_STATE_DEPTH_BOUNDS are set in pDynamicState below
                                                                                       // Also requires the  VK_EXT_extended_dynamic_state3 extension.
        if (m_filledColorBlendStateInfo)
            info.pColorBlendState =
                &m_colorBlendStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx]; // Can be null if rasterization not enabled.
                                                                                     // Can be null if VK_DYNAMIC_STATE_LOGIC_OP_ENABLE_EXT,
                                                                                     // VK_DYNAMIC_STATE_LOGIC_OP_EXT,
                                                                                     // VK_DYNAMIC_STATE_COLOR_BLEND_ENABLE_EXT,
                                                                                     // VK_DYNAMIC_STATE_COLOR_BLEND_EQUATION_EXT,
                                                                                     // VK_DYNAMIC_STATE_COLOR_WRITE_MASK_EXT, and
                                                                                     // VK_DYNAMIC_STATE_BLEND_CONSTANTS are set in pDynamicState below
                                                                                     // Requires the VK_EXT_extended_dynamic_state3 extension.
        if (m_filledDynamicStateInfo)
            info.pDynamicState =
                &m_dynamicStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx]; // Could be null if no state in the pipeline needs to be dynamic.
        info.layout = *layout;                                                    // Need fill
        info.renderPass = *renderPass;                                            // Need fill
        info.subpass = subpassIdx;                                                // Need fill
        info.basePipelineHandle = basePipelineHandle;                             // Need fill
        info.basePipelineIndex = basePipelineIndex;                               // Need fill

        return &info;
    }

    void VknInfos::initRenderPass(uint32_t deviceIdx, uint32_t renderPassIdx)
    {
        this->initVectors<VkRenderPassCreateInfo>(deviceIdx, renderPassIdx,
                                                  m_renderPassCreateInfos);
        this->initVectors<VkAttachmentDescription>(deviceIdx, renderPassIdx,
                                                   0, m_attachmentDescriptions);
        this->initVectors<VkSubpassDescription>(deviceIdx, renderPassIdx,
                                                0, m_subpassDescriptions);
        this->initVectors<VkSubpassDependency>(deviceIdx, renderPassIdx,
                                               0, m_subpassDependencies);
        this->initVectors<VkAttachmentReference>(
            deviceIdx, renderPassIdx, 0, NUM_ATTACHMENT_TYPES - 1, 0, m_attachmentReferences);
    }

    VkRenderPassCreateInfo *VknInfos::fillRenderPassCreateInfo(uint32_t deviceIdx,
                                                               uint32_t renderPassIdx,
                                                               uint32_t numAttachments,
                                                               uint32_t numSubpasses,
                                                               uint32_t numSubpassDeps,
                                                               VkRenderPassCreateFlags flags)
    {
        this->initVectors<VkRenderPassCreateInfo>(deviceIdx, renderPassIdx, m_renderPassCreateInfos);
        m_renderPassCreateInfos[deviceIdx][renderPassIdx] = VkRenderPassCreateInfo{};
        VkRenderPassCreateInfo &renderPassInfo = m_renderPassCreateInfos[deviceIdx][renderPassIdx];
        std::vector<VkAttachmentDescription> &attachmentDescriptions =
            m_attachmentDescriptions[deviceIdx][renderPassIdx];
        std::vector<VkSubpassDescription> &subpassDescriptions =
            m_subpassDescriptions[deviceIdx][renderPassIdx];
        std::vector<VkSubpassDependency> &subpassDependencies =
            m_subpassDependencies[deviceIdx][renderPassIdx];

        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.pNext = VK_NULL_HANDLE;
        renderPassInfo.flags = flags;
        renderPassInfo.attachmentCount = numAttachments;
        if (numAttachments == 0)
            renderPassInfo.pAttachments = VK_NULL_HANDLE;
        else
            renderPassInfo.pAttachments = attachmentDescriptions.data();
        renderPassInfo.subpassCount = numSubpasses;
        if (numSubpasses == 0)
            renderPassInfo.pSubpasses = VK_NULL_HANDLE;
        else
            renderPassInfo.pSubpasses = subpassDescriptions.data();
        renderPassInfo.dependencyCount = numSubpassDeps;
        if (numSubpassDeps == 0)
            renderPassInfo.pDependencies = VK_NULL_HANDLE;
        else
            renderPassInfo.pDependencies = subpassDependencies.data();

        return &renderPassInfo;
    }

    VkShaderModuleCreateInfo *VknInfos::fillShaderModuleCreateInfo(
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx, uint32_t shaderIdx,
        std::vector<char> *code, VkShaderModuleCreateFlags flags)
    {
        this->initVectors<VkShaderModuleCreateInfo>(
            deviceIdx, renderPassIdx, subpassIdx, shaderIdx, m_shaderModuleCreateInfos);
        m_shaderModuleCreateInfos[deviceIdx][renderPassIdx][subpassIdx][shaderIdx] =
            VkShaderModuleCreateInfo{};
        VkShaderModuleCreateInfo *info =
            &m_shaderModuleCreateInfos[deviceIdx][renderPassIdx][subpassIdx][shaderIdx];
        info->sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = flags;
        info->codeSize = code->size();
        info->pCode = reinterpret_cast<const uint32_t *>(code->data());
        return info;
    }

    VkPipelineShaderStageCreateInfo *VknInfos::fillShaderStageCreateInfo(
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx, uint32_t shaderIdx,
        VkShaderModule *module, VkShaderStageFlagBits *stage,
        VkPipelineShaderStageCreateFlags *flags, VkSpecializationInfo *pSpecializationInfo)
    {
        this->initVectors<VkPipelineShaderStageCreateInfo>(
            deviceIdx, renderPassIdx, subpassIdx, shaderIdx, m_shaderStageCreateInfos);
        m_shaderStageCreateInfos[deviceIdx][renderPassIdx][subpassIdx][shaderIdx] =
            VkPipelineShaderStageCreateInfo{};
        VkPipelineShaderStageCreateInfo *info =
            &m_shaderStageCreateInfos[deviceIdx][renderPassIdx][subpassIdx][shaderIdx];
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
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx, uint32_t numBindings, uint32_t numAttributes)
    {
        this->initVectors<VkPipelineVertexInputStateCreateInfo>(
            deviceIdx, renderPassIdx, subpassIdx, m_vertexInputStateCreateInfos);
        this->initVectors<VkVertexInputBindingDescription>(
            deviceIdx, renderPassIdx, subpassIdx, 0, m_vertexInputBindings);
        this->initVectors<VkVertexInputAttributeDescription>(
            deviceIdx, renderPassIdx, subpassIdx, 0, m_vertexInputAttributes);

        std::vector<VkVertexInputBindingDescription> *vertexBindingDescriptions =
            &m_vertexInputBindings[deviceIdx][renderPassIdx][subpassIdx];
        std::vector<VkVertexInputAttributeDescription> *vertexAttributeDescriptions =
            &m_vertexInputAttributes[deviceIdx][renderPassIdx][subpassIdx];

        m_vertexInputStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx] =
            VkPipelineVertexInputStateCreateInfo{};
        VkPipelineVertexInputStateCreateInfo *info =
            &m_vertexInputStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx];
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
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
        VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable)
    {
        this->initVectors<VkPipelineInputAssemblyStateCreateInfo>(
            deviceIdx, renderPassIdx, subpassIdx, m_inputAssemblyStateCreateInfos);
        m_inputAssemblyStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx] =
            VkPipelineInputAssemblyStateCreateInfo{};
        VkPipelineInputAssemblyStateCreateInfo *info =
            &m_inputAssemblyStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx];
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = 0; // reserved for future use
        info->topology = topology;
        info->primitiveRestartEnable = primitiveRestartEnable;
        m_filledInputAssemblyStateInfo = true;
        return info;
    }

    VkPipelineTessellationStateCreateInfo *VknInfos::fillTessellationStateCreateInfo(
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx, uint32_t patchControlPoints)
    {
        this->initVectors<VkPipelineTessellationStateCreateInfo>(
            deviceIdx, renderPassIdx, subpassIdx, m_tessellationStateCreateInfos);
        m_tessellationStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx] =
            VkPipelineTessellationStateCreateInfo{};
        VkPipelineTessellationStateCreateInfo *info =
            &m_tessellationStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx];
        info->sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        info->pNext = nullptr;
        info->flags = 0; // reserved for future use
        info->patchControlPoints = patchControlPoints;
        return info;
    }

    VkPipelineViewportStateCreateInfo *VknInfos::fillViewportStateCreateInfo(
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
        std::vector<VkViewport> *viewports, std::vector<VkRect2D> *scissors)
    {
        this->initVectors<VkPipelineViewportStateCreateInfo>(
            deviceIdx, renderPassIdx, subpassIdx, m_viewportStateCreateInfos);
        m_viewportStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx] =
            VkPipelineViewportStateCreateInfo{};
        VkPipelineViewportStateCreateInfo *info =
            &m_viewportStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx];
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
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx, VkPolygonMode polygonMode,
        VkCullModeFlags cullMode, VkFrontFace frontFace,
        float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor,
        float lineWidth, VkBool32 depthClampEnable,
        VkBool32 rasterizerDiscardEnable, VkBool32 depthBiasEnable)
    {
        this->initVectors<VkPipelineRasterizationStateCreateInfo>(
            deviceIdx, renderPassIdx, subpassIdx, m_rasterizationStateCreateInfos);
        m_rasterizationStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx] =
            VkPipelineRasterizationStateCreateInfo{};
        VkPipelineRasterizationStateCreateInfo *info =
            &m_rasterizationStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx];
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
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx, float minSampleShading,
        VkSampleMask *pSampleMask, VkSampleCountFlagBits rasterizationSamples,
        VkBool32 sampleShadingEnable, VkBool32 alphaToCoverageEnable,
        VkBool32 alphaToOneEnable)
    {
        this->initVectors<VkPipelineMultisampleStateCreateInfo>(
            deviceIdx, renderPassIdx, subpassIdx, m_multisampleStateCreateInfos);
        m_multisampleStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx] =
            VkPipelineMultisampleStateCreateInfo{};
        VkPipelineMultisampleStateCreateInfo *info =
            &m_multisampleStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx];
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
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
        VkCompareOp depthCompareOp, VkStencilOpState front, VkStencilOpState back,
        float minDepthBounds, float maxDepthBounds,
        VkPipelineDepthStencilStateCreateFlags flags,
        VkBool32 depthTestEnable, VkBool32 depthWriteEnable,
        VkBool32 depthBoundsTestEnable, VkBool32 stencilTestEnable)
    {
        this->initVectors<VkPipelineDepthStencilStateCreateInfo>(
            deviceIdx, renderPassIdx, subpassIdx, m_depthStencilStateCreateInfos);
        m_depthStencilStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx] =
            VkPipelineDepthStencilStateCreateInfo{};
        VkPipelineDepthStencilStateCreateInfo *info =
            &m_depthStencilStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx];
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
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx, VkLogicOp logicOp,
        std::vector<VkPipelineColorBlendAttachmentState> attachments,
        float blendConstants[4], VkBool32 logicOpEnable,
        VkPipelineColorBlendStateCreateFlags flags)
    {
        this->initVectors<VkPipelineColorBlendStateCreateInfo>(
            deviceIdx, renderPassIdx, subpassIdx, m_colorBlendStateCreateInfos);
        m_colorBlendStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx] =
            VkPipelineColorBlendStateCreateInfo{};
        VkPipelineColorBlendStateCreateInfo *info =
            &m_colorBlendStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx];
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
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx, std::vector<VkDynamicState> dynamicStates)
    {
        this->initVectors<VkPipelineDynamicStateCreateInfo>(
            deviceIdx, renderPassIdx, subpassIdx, m_dynamicStateCreateInfos);
        m_dynamicStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx] =
            VkPipelineDynamicStateCreateInfo{};
        VkPipelineDynamicStateCreateInfo *info =
            &m_dynamicStateCreateInfos[deviceIdx][renderPassIdx][subpassIdx];
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
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
        std::vector<VkDescriptorSetLayout> setLayouts,
        std::vector<VkPushConstantRange> pushConstantRanges,
        VkPipelineLayoutCreateFlags flags)
    {
        this->initVectors(deviceIdx, renderPassIdx, subpassIdx, m_layoutCreateInfos);
        m_layoutCreateInfos[deviceIdx][renderPassIdx][subpassIdx] =
            VkPipelineLayoutCreateInfo{};
        VkPipelineLayoutCreateInfo *info =
            &m_layoutCreateInfos[deviceIdx][renderPassIdx][subpassIdx];
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
        VkPhysicalDeviceFeatures *info = &m_enabledFeatures.back();
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

    void VknInfos::fillDeviceQueueCreateInfo(uint32_t deviceIdx, uint32_t queueFamilyIdx,
                                             uint32_t queueCount, VkApplicationInfo *pNext,
                                             VkDeviceQueueCreateFlags flags)
    {
        this->initVectors<VkDeviceQueueCreateInfo>(deviceIdx, queueFamilyIdx, m_queueCreateInfos);
        m_queueCreateInfos[deviceIdx][queueFamilyIdx] = VkDeviceQueueCreateInfo{};
        VkDeviceQueueCreateInfo &info = m_queueCreateInfos[deviceIdx][queueFamilyIdx];
        info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info.queueFamilyIndex = queueFamilyIdx;
        info.queueCount = queueCount;
        info.pNext = pNext;
        info.flags = flags; // Only flag is a protected memory bit, for a queue family that supports it
        if (!m_deviceQueuePrioritiesFilled)
            throw std::runtime_error("Queue priorities not filled before filling device queue create info.");
        info.pQueuePriorities = m_queuePriorities[deviceIdx][queueFamilyIdx].data();
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
        uint32_t deviceIdx, uint32_t swapchainIdx,
        VkSurfaceKHR surface, uint32_t imageCount, VkExtent2D dimensions,
        VkSurfaceFormatKHR surfaceFormat, uint32_t numImageArrayLayers, VkImageUsageFlags usage,
        VkSharingMode sharingMode, VkSurfaceTransformFlagBitsKHR preTransform,
        VkCompositeAlphaFlagBitsKHR compositeAlpha, VkPresentModeKHR presentMode, VkBool32 clipped,
        VkSwapchainKHR oldSwapchain)
    {
        this->initVectors<VkSwapchainCreateInfoKHR>(deviceIdx, swapchainIdx, m_swapchainCreateInfos);
        m_swapchainCreateInfos[deviceIdx][swapchainIdx] = VkSwapchainCreateInfoKHR{};
        VkSwapchainCreateInfoKHR &swapchainInfo = m_swapchainCreateInfos[deviceIdx][swapchainIdx];
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
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t attachIdx,
        VkFormat format, VkSampleCountFlagBits samples, VkAttachmentLoadOp loadOp,
        VkAttachmentStoreOp storeOp, VkAttachmentLoadOp stencilLoadOp,
        VkAttachmentStoreOp stencilStoreOp, VkImageLayout initialLayout,
        VkImageLayout finalLayout, VkAttachmentDescriptionFlags flags)
    {
        this->initVectors<VkAttachmentDescription>(
            deviceIdx, renderPassIdx, attachIdx, m_attachmentDescriptions);
        m_attachmentDescriptions[deviceIdx][renderPassIdx][attachIdx] = VkAttachmentDescription{};
        VkAttachmentDescription &description =
            m_attachmentDescriptions[deviceIdx][renderPassIdx][attachIdx];
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
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
        uint32_t refIdx, VknAttachmentType attachmentType, uint32_t attachmentIdx,
        VkImageLayout layout)
    {
        this->initVectors<VkAttachmentReference>(
            deviceIdx, renderPassIdx, subpassIdx, attachmentType,
            refIdx, m_attachmentReferences);
        this->initVectors<uint32_t>(deviceIdx, renderPassIdx, subpassIdx,
                                    refIdx, m_preserveAttachments);
        if (attachmentType == PRESERVE_ATTACHMENT)
            m_preserveAttachments[deviceIdx][renderPassIdx][subpassIdx][refIdx] = attachmentIdx;
        else
        {
            m_attachmentReferences[deviceIdx][renderPassIdx][subpassIdx][attachmentType][refIdx] = VkAttachmentReference{};
            VkAttachmentReference &ref = m_attachmentReferences[deviceIdx][renderPassIdx][subpassIdx][attachmentType][refIdx];
            ref.attachment = attachmentIdx;
            ref.layout = layout;
        }
    }

    std::vector<std::vector<std::vector<VkAttachmentReference>>> *VknInfos::getRenderPassAttachmentReferences(
        uint32_t deviceIdx, uint32_t renderPassIdx)
    {
        this->initVectors<VkAttachmentReference>(
            deviceIdx, renderPassIdx, 0, NUM_ATTACHMENT_TYPES - 1, 0, m_attachmentReferences);
        return &m_attachmentReferences[deviceIdx][renderPassIdx];
    }

    std::vector<std::vector<VkAttachmentReference>> *VknInfos::getSubpassAttachmentReferences(
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx)
    {
        this->initVectors<VkAttachmentReference>(
            deviceIdx, renderPassIdx, subpassIdx, NUM_ATTACHMENT_TYPES - 1, 0, m_attachmentReferences);
        return &m_attachmentReferences[deviceIdx][renderPassIdx][subpassIdx];
    }

    std::vector<uint32_t> *VknInfos::getSubpassPreserveAttachments(
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx)
    {
        this->initVectors<uint32_t>(deviceIdx, renderPassIdx, subpassIdx, 0, m_preserveAttachments);
        return &m_preserveAttachments[deviceIdx][renderPassIdx][subpassIdx];
    }

    std::vector<std::vector<uint32_t>> *VknInfos::getRenderPassPreserveAttachments(
        uint32_t deviceIdx, uint32_t renderPassIdx)
    {
        this->initVectors<uint32_t>(deviceIdx, renderPassIdx, 0, 0, m_preserveAttachments);
        return &m_preserveAttachments[deviceIdx][renderPassIdx];
    }

    VkSubpassDescription *VknInfos::fillSubpassDescription(
        uint32_t numColor, uint32_t numInput, uint32_t numResolve, uint32_t numDepthStencil,
        uint32_t numPreserve, uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
        VkPipelineBindPoint pipelineBindPoint, VkSubpassDescriptionFlags flags)
    {
        this->initVectors<VkSubpassDescription>(
            deviceIdx, renderPassIdx, subpassIdx, m_subpassDescriptions);
        m_subpassDescriptions[deviceIdx][renderPassIdx][subpassIdx] = VkSubpassDescription{};
        VkSubpassDescription &description = m_subpassDescriptions[deviceIdx][renderPassIdx][subpassIdx];

        description.flags = flags;
        description.pipelineBindPoint = pipelineBindPoint;
        std::vector<VkAttachmentReference> &inputAttachments = m_attachmentReferences[deviceIdx][renderPassIdx][subpassIdx][INPUT_ATTACHMENT];
        std::vector<VkAttachmentReference> &colorAttachments = m_attachmentReferences[deviceIdx][renderPassIdx][subpassIdx][COLOR_ATTACHMENT];
        std::vector<VkAttachmentReference> &resolveAttachments = m_attachmentReferences[deviceIdx][renderPassIdx][subpassIdx][RESOLVE_ATTACHMENT];
        std::vector<VkAttachmentReference> &depthStencilAttachments = m_attachmentReferences[deviceIdx][renderPassIdx][subpassIdx][DEPTH_STENCIL_ATTACHMENT];
        std::vector<uint32_t> &preserveAttachments = m_preserveAttachments[deviceIdx][renderPassIdx][subpassIdx];
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
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassDepIdx,
        uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask,
        VkAccessFlags srcAccessMask, VkPipelineStageFlags dstStageMask, VkAccessFlags dstAccessMask)
    {
        this->initVectors<VkSubpassDependency>(
            deviceIdx, renderPassIdx, subpassDepIdx, m_subpassDependencies);
        m_subpassDependencies[deviceIdx][renderPassIdx][subpassDepIdx] = VkSubpassDependency{};
        VkSubpassDependency &dependency = m_subpassDependencies[deviceIdx][renderPassIdx][subpassDepIdx];
        dependency.srcSubpass = srcSubpass;
        dependency.dstSubpass = dstSubpass;
        dependency.srcStageMask = srcStageMask;
        dependency.srcAccessMask = srcAccessMask;
        dependency.dstStageMask = dstStageMask;
        dependency.dstAccessMask = dstAccessMask;

        return &dependency;
    }

    VkVertexInputBindingDescription *VknInfos::fillVertexInputBindingDescription(
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx, uint32_t bindIdx,
        uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)

    {
        this->initVectors<VkVertexInputBindingDescription>(
            deviceIdx, renderPassIdx, subpassIdx, bindIdx, m_vertexInputBindings);
        m_vertexInputBindings[deviceIdx][renderPassIdx][subpassIdx][bindIdx] =
            VkVertexInputBindingDescription{};
        VkVertexInputBindingDescription *description =
            &m_vertexInputBindings[deviceIdx][renderPassIdx][subpassIdx][bindIdx];
        description->binding = binding;
        description->stride = stride;
        description->inputRate = inputRate;
        return description;
    }

    VkVertexInputAttributeDescription *VknInfos::fillVertexInputAttributeDescription(
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
        uint32_t attributeIdx, uint32_t binding, uint32_t location, VkFormat format, uint32_t offset)
    {
        this->initVectors<VkVertexInputAttributeDescription>(
            deviceIdx, renderPassIdx, subpassIdx, attributeIdx, m_vertexInputAttributes);
        m_vertexInputAttributes[deviceIdx][renderPassIdx][subpassIdx][attributeIdx] =
            VkVertexInputAttributeDescription{};
        VkVertexInputAttributeDescription *description =
            &m_vertexInputAttributes[deviceIdx][renderPassIdx][subpassIdx][attributeIdx];
        description->binding = binding;
        description->location = location;
        description->format = format;
        description->offset = offset;
        return description;
    }

    std::vector<VkVertexInputBindingDescription> *VknInfos::getVertexInputBindings(
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx)
    {
        return &m_vertexInputBindings[deviceIdx][renderPassIdx][subpassIdx];
    }

    std::vector<VkVertexInputAttributeDescription> *VknInfos::getVertexInputAttributes(
        uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx)
    {
        return &m_vertexInputAttributes[deviceIdx][renderPassIdx][subpassIdx];
    }

    VkSwapchainCreateInfoKHR *VknInfos::getSwapchainCreateInfo(uint32_t deviceIdx, uint32_t swapchainIdx)
    {
        return &m_swapchainCreateInfos[deviceIdx][swapchainIdx];
    }
}