#include "include/VknDescriptorSetLayout.hpp"

namespace vkn
{
    VknDescriptorSetLayout *VknDescriptorSetLayout::s_editable{nullptr};

    VknDescriptorSetLayout::VknDescriptorSetLayout(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
        s_editable = this;
    }

    // Config
    void VknDescriptorSetLayout::addBinding(
        uint32_t binding, VkDescriptorType descriptorType, uint32_t descriptorCount,
        VkShaderStageFlags stageFlags, const VkSampler *pImmutableSamplers)
    {
        testEditability();
        m_bindings.push_back(VkDescriptorSetLayoutBinding{});
        m_bindings.back().binding = binding;
        m_bindings.back().descriptorType = descriptorType;
        m_bindings.back().descriptorCount = descriptorCount;
        m_bindings.back().stageFlags = stageFlags;
        m_bindings.back().pImmutableSamplers = pImmutableSamplers;
    }

    // Create
    void VknDescriptorSetLayout::createDescriptorSetLayout()
    {
        testEditability();
        VkDescriptorSetLayoutCreateInfo *createInfo = m_infos->fillDescriptorSetLayoutCreateInfo(
            m_bindings, m_createFlags);
        VknResult res{
            vkCreateDescriptorSetLayout(
                m_engine->getObject<VkDevice>(m_absIdxs),
                createInfo, nullptr,
                &m_engine->getObject<VkDescriptorSetLayout>(m_absIdxs)),
            "Create descriptor set layout."};
    }

    void VknDescriptorSetLayout::testEditability()
    {
        if (s_editable != this)
            throw std::runtime_error("Members of a VknPipeline must be added all at once so that they are stored contiguously.");
    }
}