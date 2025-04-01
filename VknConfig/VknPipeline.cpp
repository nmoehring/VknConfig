#include <filesystem>
#include <iterator>

#include "include/VknCommon.hpp"
#include "include/VknPipeline.hpp"

namespace vkn
{
    VknPipeline::VknPipeline(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
        m_vertexInputState = VknVertexInputState{engine, relIdxs, absIdxs, infos};
        m_inputAssemblyState = VknInputAssemblyState{engine, relIdxs, absIdxs, infos};
        m_multisampleState = VknMultisampleState{engine, relIdxs, absIdxs, infos};
        m_rasterizationState = VknRasterizationState{engine, relIdxs, absIdxs, infos};
        m_viewportState = VknViewportState{engine, relIdxs, absIdxs, infos};
    }

    VknShaderStage *VknPipeline::addShaderStage(uint32_t shaderIdx,
                                                VknShaderStageType stageType,
                                                std::string filename, VkPipelineShaderStageCreateFlags flags)
    {
        VknShaderStage &shaderStage = addNewVknObject<VknShaderStage, VkShaderModule>(
            shaderIdx, m_shaderStages, m_engine, m_relIdxs, m_absIdxs, m_infos);
        shaderStage.setFilename(filename);
        shaderStage.setShaderStageType(stageType);
        shaderStage.setFlags(flags);
        return &shaderStage;
    }

    VknShaderStage *VknPipeline::getShaderStage(uint32_t shaderIdx)
    {
        return getListElement(shaderIdx, m_shaderStages);
    }

    void VknPipeline::_fillPipelineCreateInfo()
    {
        m_infos->fillGfxPipelineCreateInfo(
            m_relIdxs, m_engine->getObject<VkRenderPass>(m_absIdxs.get<VkRenderPass>()),
            &m_layout, m_basePipelineHandle, m_basePipelineIndex, m_createFlags);
    }

    void VknPipeline::addDescriptorSetLayoutBinding(
        uint32_t binding, VkDescriptorType descriptorType, uint32_t descriptorCount,
        VkShaderStageFlags stageFlags, const VkSampler *pImmutableSamplers)
    {
        m_bindings.push_back(VkDescriptorSetLayoutBinding{});
        m_bindings.back().binding = binding;
        m_bindings.back().descriptorType = descriptorType;
        m_bindings.back().descriptorCount = descriptorCount;
        m_bindings.back().stageFlags = stageFlags;
        m_bindings.back().pImmutableSamplers = pImmutableSamplers;
    }

    void VknPipeline::createDescriptorSetLayout()
    {
        VkDescriptorSetLayoutCreateInfo *createInfo = m_infos->fillDescriptorSetLayoutCreateInfo(
            m_bindings, m_descriptorSetLayoutCreateFlags);
        m_descriptorSetLayoutIdxs.push_back(m_engine->push_back(VkDescriptorSetLayout{}));
        VknResult res{
            vkCreateDescriptorSetLayout(
                m_engine->getObject<VkDevice>(m_absIdxs.get<VkDevice>()),
                createInfo, nullptr,
                &m_engine->getObject<VkDescriptorSetLayout>(m_descriptorSetLayoutIdxs.back())),
            "Create descriptor set layout."};
    }

    void VknPipeline::addPushConstantRange(VkShaderStageFlags stageFlags, uint32_t offset,
                                           uint32_t size)
    {
        m_pushConstantRanges.push_back(VkPushConstantRange{});
        m_pushConstantRanges.back().stageFlags = stageFlags;
        m_pushConstantRanges.back().offset = offset;
        m_pushConstantRanges.back().size = size;
    }

    void VknPipeline::createLayout()
    {
        if (m_createdPipelineLayout)
            throw std::runtime_error("Already created the pipeline layout.");
        VkPipelineLayoutCreateInfo *createInfo = m_infos->fillPipelineLayoutCreateInfo(m_relIdxs, m_descriptorSetLayouts,
                                                                                       m_pushConstantRanges, m_layoutCreateFlags);
        m_pipelineLayoutIdxs.push_back(m_engine->push_back(VkPipelineLayout{}));
        vkCreatePipelineLayout(
            m_engine->getObject<VkDevice>(m_absIdxs.get<VkDevice>()), createInfo,
            nullptr, &m_engine->getObject<VkPipelineLayout>(m_pipelineLayoutIdxs.back()));
        m_createdPipelineLayout = true;
    }
}