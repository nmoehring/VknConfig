#include "VknPipeline.hpp"
#include "VknInfos.hpp"

namespace vkn
{
    VknPipeline::VknPipeline() {}

    VknPipeline::VknPipeline(VknDevice &dev, VknInfos &infos) : m_device{&dev}, m_infos{&infos}
    {

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    }

    VknPipeline::~VknPipeline()
    {
        for (auto layout : m_layouts)
            vkDestroyPipelineLayout(m_device->getDevice(), layout, nullptr);
        for (auto pipeline : m_pipelines)
            vkDestroyPipeline(m_device->getDevice(), pipeline, nullptr);
    }

    void VknPipeline::createPipeline(int idx, std::vector<VkPipelineShaderStageCreateInfo> &stages,
                                     VkPipelineLayout layout, VkRenderPass renderPass, uint32_t subpass,
                                     VkPipeline basePipelineHandle, int32_t basePipelineIndex,
                                     VkPipelineCreateFlags flags = VkPipelineCreateFlags{})
    {
        auto pipelineInfo = m_infos->fillGfxPipelineCreateInfo(
            stages, m_layouts[idx], renderPass, subpass, basePipelineHandle, basePipelineIndex);
        auto res = vkCreateGraphicsPipelines(
            m_device->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &(m_pipelines[idx]));
    }

    void VknPipeline::createLayout(
        int idx, std::vector<VkDescriptorSetLayout> layouts, std::vector<VkPushConstantRange> ranges)
    {
        auto layoutInfo = m_infos->fillPipelineLayoutCreateInfo(layouts, ranges);
        vkCreatePipelineLayout(m_device->getDevice(), &layoutInfo, nullptr, &(m_layouts[idx]));
    }

}