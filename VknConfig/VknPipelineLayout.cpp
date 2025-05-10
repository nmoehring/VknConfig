#include "include/VknPipelineLayout.hpp"

namespace vkn
{
    VknPipelineLayout::VknPipelineLayout(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
    }

    // Config
    VknDescriptorSetLayout *VknPipelineLayout::addDescriptorSetLayout()
    {
        m_descriptorSetLayoutStartIdx = m_engine->getVectorSize<VkDescriptorSetLayout>();
        return &m_engine->addNewVknObject<VknDescriptorSetLayout, VkDescriptorSetLayout, VkDevice>(
            uint32_t{0}, m_descriptorSetLayouts,
            m_relIdxs, m_absIdxs, m_infos);
    }

    void VknPipelineLayout::addPushConstantRange(VkShaderStageFlags stageFlags,
                                                 uint32_t offset, uint32_t size)
    {
        VkPushConstantRange &element = m_pushConstantRanges.append(VkPushConstantRange{});
        element.stageFlags = stageFlags;
        element.offset = offset;
        element.size = size;
    }

    // Create
    void VknPipelineLayout::createPipelineLayout()
    {
        if (m_createdPipelineLayout)
            throw std::runtime_error("Already created the pipeline layout.");
        VknVector<VkDescriptorSetLayout> descriptorSetLayouts;

        VkPipelineLayoutCreateInfo *createInfo = m_infos->fillPipelineLayoutCreateInfo(
            m_relIdxs, m_engine->getVectorSlice<VkDescriptorSetLayout>(m_descriptorSetLayoutStartIdx, m_descriptorSetLayouts.size()),
            m_pushConstantRanges, m_createFlags);
        vkCreatePipelineLayout(
            m_engine->getObject<VkDevice>(m_absIdxs), createInfo,
            nullptr, &m_engine->getObject<VkPipelineLayout>(m_absIdxs));
        m_createdPipelineLayout = true;
    }

    VkPipelineLayout *VknPipelineLayout::getVkLayout()
    {
        return &m_engine->getObject<VkPipelineLayout>(m_absIdxs);
    }
}