#include "include/VknPipeline.hpp"

namespace vkn
{
    VknPipeline::VknPipeline(VknIdxs relIdxs, VknIdxs absIdxs)
        : VknObject(relIdxs, absIdxs)
    {
        m_instanceLock = this;
        s_engine.addNewVknObject<VknPipelineLayout, VkPipelineLayout, VkDevice>(
            0, m_layouts, m_relIdxs, m_absIdxs);
        m_vertexInputState = VknVertexInputState{relIdxs, absIdxs};
        m_inputAssemblyState = VknInputAssemblyState{relIdxs, absIdxs};
        m_multisampleState = VknMultisampleState{relIdxs, absIdxs};
        m_rasterizationState = VknRasterizationState{relIdxs, absIdxs};
        m_colorBlendState = VknColorBlendState{relIdxs, absIdxs}; // Initialize
        m_viewportState = VknViewportState{relIdxs, absIdxs};
        m_dynamicState = VknDynamicState{relIdxs, absIdxs};
    }

    VknShaderStage *VknPipeline::addShaderStage(uint32_t shaderIdx,
                                                VknShaderStageType stageType,
                                                std::string filename, VkPipelineShaderStageCreateFlags flags)
    {
        m_instanceLock(this);
        VknShaderStage &shaderStage = s_engine.addNewVknObject<VknShaderStage, VkShaderModule, VkDevice>(
            shaderIdx, m_shaderStages, m_relIdxs, m_absIdxs);
        shaderStage.setFilename(filename);
        shaderStage.setShaderStageType(stageType);
        shaderStage.setFlags(flags);
        return &shaderStage;
    }

    VknShaderStage *VknPipeline::getShaderStage(uint32_t shaderIdx)
    {
        return getListElement(shaderIdx, m_shaderStages);
    }

    VknPipelineLayout *VknPipeline::getLayout()
    {
        return &m_layouts.front();
    }

    VkGraphicsPipelineCreateInfo *VknPipeline::_filePipelineCreateInfo()
    {
        VkPipelineLayout *layout = this->getLayout()->getVkLayout();
        return s_infos.fileGfxPipelineCreateInfo(
            m_relIdxs, s_engine.getObject<VkRenderPass>(m_absIdxs),
            layout, m_basePipelineHandle, m_basePipelineIndex, m_createFlags);
    }
}