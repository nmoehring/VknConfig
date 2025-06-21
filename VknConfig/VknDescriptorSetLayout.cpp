#include "include/VknDescriptorSetLayout.hpp"

namespace vkn
{
    VknDescriptorSetLayout::VknDescriptorSetLayout(VknIdxs relIdxs, VknIdxs absIdxs) : VknObject(relIdxs, absIdxs)
    {
    }

    // Config
    void VknDescriptorSetLayout::addBinding(
        uint32_t binding, VkDescriptorType descriptorType, uint32_t descriptorCount,
        VkShaderStageFlags stageFlags, const VkSampler *pImmutableSamplers)
    {
        VkDescriptorSetLayoutBinding &descSetLayoutBinding =
            m_bindings.appendOne(VkDescriptorSetLayoutBinding{});
        descSetLayoutBinding.binding = binding;
        descSetLayoutBinding.descriptorType = descriptorType;
        descSetLayoutBinding.descriptorCount = descriptorCount;
        descSetLayoutBinding.stageFlags = stageFlags;
        descSetLayoutBinding.pImmutableSamplers = pImmutableSamplers;
    }

    // Create
    void VknDescriptorSetLayout::createDescriptorSetLayout()
    {
        VkDescriptorSetLayoutCreateInfo *createInfo = s_infos.fileDescriptorSetLayoutCreateInfo(
            m_bindings, m_createFlags);
        VknResult res{
            vkCreateDescriptorSetLayout(
                s_engine.getObject<VkDevice>(m_absIdxs),
                createInfo, nullptr,
                &s_engine.getObject<VkDescriptorSetLayout>(m_absIdxs)),
            "Create descriptor set layout."};
    }
}