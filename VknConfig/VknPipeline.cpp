#include "include/VknPipeline.hpp"

namespace vkn
{
    VknPipeline::VknPipeline(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
        m_instanceLock = this;
        m_engine->addNewVknObject<VknPipelineLayout, VkPipelineLayout, VkDevice>(
            0, m_layouts, m_relIdxs, m_absIdxs, m_infos);
        m_vertexInputState = VknVertexInputState{engine, relIdxs, absIdxs, infos};
        m_inputAssemblyState = VknInputAssemblyState{engine, relIdxs, absIdxs, infos};
        m_multisampleState = VknMultisampleState{engine, relIdxs, absIdxs, infos};
        m_rasterizationState = VknRasterizationState{engine, relIdxs, absIdxs, infos};
        m_colorBlendState = VknColorBlendState{engine, relIdxs, absIdxs, infos}; // Initialize
        m_viewportState = VknViewportState{engine, relIdxs, absIdxs, infos};
        m_dynamicState = VknDynamicState{engine, relIdxs, absIdxs, infos};
    }

    VknShaderStage *VknPipeline::addShaderStage(uint32_t shaderIdx,
                                                VknShaderStageType stageType,
                                                std::string filename, VkPipelineShaderStageCreateFlags flags)
    {
        m_instanceLock(this);
        VknShaderStage &shaderStage = m_engine->addNewVknObject<VknShaderStage, VkShaderModule, VkDevice>(
            shaderIdx, m_shaderStages, m_relIdxs, m_absIdxs, m_infos);
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

    VkGraphicsPipelineCreateInfo *VknPipeline::_fillPipelineCreateInfo()
    {
        VkPipelineLayout *layout = this->getLayout()->getVkLayout();
        return m_infos->fillGfxPipelineCreateInfo(
            m_relIdxs, m_engine->getObject<VkRenderPass>(m_absIdxs),
            layout, m_basePipelineHandle, m_basePipelineIndex, m_createFlags);
    }
}