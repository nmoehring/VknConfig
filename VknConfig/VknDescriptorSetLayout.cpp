#include "include/VknDescriptorSetLayout.hpp"

namespace vkn
{
    VknDescriptorSetLayout::VknDescriptorSetLayout(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
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
        VkDescriptorSetLayoutCreateInfo *createInfo = m_infos->fileDescriptorSetLayoutCreateInfo(
            m_bindings, m_createFlags);
        VknResult res{
            vkCreateDescriptorSetLayout(
                m_engine->getObject<VkDevice>(m_absIdxs),
                createInfo, nullptr,
                &m_engine->getObject<VkDescriptorSetLayout>(m_absIdxs)),
            "Create descriptor set layout."};
    }
}