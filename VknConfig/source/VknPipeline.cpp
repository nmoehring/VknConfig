#include <filesystem>

#include "../include/VknPipeline.hpp"
#include <iterator>

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

    void VknPipeline::addShaderStage(uint32_t shaderIdx,
                                     VknShaderStageType stageType, std::string filename, VkPipelineShaderStageCreateFlags flags)
    {
        if (shaderIdx != m_numShaderStages)
            throw std::runtime_error("ShaderIdx passed to addShaderStage is invalid. Should be next idx.");
        VknIdxs relIdxs = m_relIdxs;
        relIdxs.shaderIdx = shaderIdx;
        m_shaderStages.emplace_back(m_engine, relIdxs, m_absIdxs, m_infos);
        m_shaderStages[shaderIdx].setFilename(filename);
        m_shaderStages[shaderIdx].setShaderStageType(stageType);
        m_shaderStages[shaderIdx].setFlags(flags);
    }

    VknShaderStage *VknPipeline::getShaderStage(uint32_t shaderIdx)
    {
        return &m_shaderStages[shaderIdx];
    }

    void VknPipeline::fillPipelineCreateInfo(
        VkPipeline basePipelineHandle, int32_t basePipelineIndex, VkPipelineCreateFlags flags)
    {
        if (m_filledCreateInfo)
            throw std::runtime_error("Pipeline create info already filled.");
        m_infos->fillGfxPipelineCreateInfo(
            m_relIdxs, &m_layout, basePipelineHandle,
            basePipelineIndex, flags);
        m_filledCreateInfo = true;
    }

    void VknPipeline::fillDescriptorSetLayoutCreateInfo(
        VkDescriptorSetLayoutCreateFlags flags)
    {
        m_infos->fillDescriptorSetLayoutCreateInfo(m_bindings, flags);
    }

    void VknPipeline::createDescriptorSetLayoutBinding(
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

    void VknPipeline::createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo *descriptorSetLayoutCreateInfo)
    {
        if (m_createdPipelineLayout)
            throw std::runtime_error("Pipeline layout already created, no need to create the descriptor set layout.");
        m_descriptorSetLayouts.push_back(VkDescriptorSetLayout{});
        VknResult res{
            vkCreateDescriptorSetLayout(
                m_engine->getObject<VkDevice>(m_absIdxs.deviceIdx),
                descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayouts.back()),
            "Create descriptor set layout."};
    }

    void VknPipeline::createPushConstantRange(VkShaderStageFlags stageFlags, uint32_t offset,
                                              uint32_t size)
    {
        m_pushConstantRanges.push_back(VkPushConstantRange{});
        m_pushConstantRanges.back().stageFlags = stageFlags;
        m_pushConstantRanges.back().offset = offset;
        m_pushConstantRanges.back().size = size;
    }

    void VknPipeline::fillPipelineLayoutCreateInfo(VkPipelineLayoutCreateFlags flags)
    {
        m_infos->fillPipelineLayoutCreateInfo(m_relIdxs, m_descriptorSetLayouts,
                                              m_pushConstantRanges, flags);
    }

    void VknPipeline::createLayout()
    {
        if (m_createdPipelineLayout)
            throw std::runtime_error("Already created the pipeline layout.");
        VkPipelineLayoutCreateInfo *layoutCreateInfo = m_infos->getPipelineLayoutCreateInfo(
            m_relIdxs);
        vkCreatePipelineLayout(m_engine->getObject<VkDevice>(m_absIdxs.deviceIdx), layoutCreateInfo, nullptr, &m_layout);
        m_createdPipelineLayout = true;
    }

}