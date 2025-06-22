#include "include/VknPipelineLayout.hpp"

namespace vkn
{
    VknPipelineLayout::VknPipelineLayout(VknIdxs relIdxs, VknIdxs absIdxs)
        : VknObject(relIdxs, absIdxs)
    {
        m_instanceLock = this;
    }

    // Config
    VknDescriptorSetLayout *VknPipelineLayout::addDescriptorSetLayout()
    {
        m_instanceLock(this);
        m_descriptorSetLayoutStartIdx = s_engine->getVectorSize<VkDescriptorSetLayout>();
        return &s_engine->addNewVknObject<VknDescriptorSetLayout, VkDescriptorSetLayout, VkDevice>(
            uint32_t{0}, m_descriptorSetLayouts,
            m_relIdxs, m_absIdxs);
    }

    void VknPipelineLayout::addPushConstantRange(VkShaderStageFlags stageFlags,
                                                 uint32_t offset, uint32_t size)
    {
        VkPushConstantRange &element = m_pushConstantRanges.appendOne(VkPushConstantRange{});
        element.stageFlags = stageFlags;
        element.offset = offset;
        element.size = size;
    }

    // Create
    void VknPipelineLayout::_createPipelineLayout()
    {
        if (m_createdPipelineLayout)
            throw std::runtime_error("Already created the pipeline layout.");
        VknVector<VkDescriptorSetLayout> descriptorSetLayouts;

        VkPipelineLayoutCreateInfo *createInfo = s_infos->filePipelineLayoutCreateInfo(
            m_relIdxs, s_engine->getVectorSlice<VkDescriptorSetLayout>(m_descriptorSetLayoutStartIdx, m_descriptorSetLayouts.size()),
            m_pushConstantRanges, m_createFlags);

        VknResult res{
            vkCreatePipelineLayout(
                s_engine->getObject<VkDevice>(m_absIdxs), createInfo,
                nullptr, &s_engine->getObject<VkPipelineLayout>(m_absIdxs)),
            "Create pipeline layout."};
        m_createdPipelineLayout = true;
    }

    VkPipelineLayout *VknPipelineLayout::getVkLayout()
    {
        return &s_engine->getObject<VkPipelineLayout>(m_absIdxs);
    }
}