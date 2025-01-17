#include <filesystem>

#include "VknPipeline.hpp"
#include "VknInfos.hpp"
#include "../Utilities/Utilities.hpp"

namespace vkn
{
    VknPipeline::VknPipeline() {}

    VknPipeline::VknPipeline(VknDevice *dev, VknInfos *infos, VknResultArchive *archive)
        : m_device{dev}, m_infos{infos}, m_archive{archive} {}

    VknPipeline::~VknPipeline()
    {
        if (!m_destroyed)
            this->destroy();
    }

    void VknPipeline::destroy()
    {
        for (auto pipeline : m_pipelines)
            vkDestroyPipeline(*(m_device->getVkDevice()), pipeline, nullptr);
        for (auto layout : m_layouts)
            vkDestroyPipelineLayout(*(m_device->getVkDevice()), layout, nullptr);
        for (auto module : m_shaderModules)
            vkDestroyShaderModule(*(m_device->getVkDevice()), module, nullptr);
        m_destroyed = true;
    }

    int VknPipeline::createPipeline(VkGraphicsPipelineCreateInfo pipelineCreateInfo)
    {
        m_pipelines.push_back(VkPipeline{});
        VknResult res{vkCreateGraphicsPipelines(
                          *m_device->getVkDevice(), nullptr, 1, &(m_pipelineCreateInfos[0]), nullptr, &(m_pipelines.back())),
                      "Create pipeline."};
        if (!res.isSuccess())
            throw std::runtime_error(res.toErr("Error creating pipeline."));
        m_archive->store(res);
        return m_pipelines.size() - 1;
    }

    void VknPipeline::createLayout(
        int idx, std::vector<VkDescriptorSetLayout> layouts, std::vector<VkPushConstantRange> ranges)
    {
        auto layoutInfo = m_infos->fillPipelineLayoutCreateInfo(layouts, ranges);
        vkCreatePipelineLayout(*(m_device->getVkDevice()), &layoutInfo, nullptr, &(m_layouts[idx]));
    }

    int VknPipeline::createShaderModule(const std::string &filename)
    {
        std::filesystem::path shaderDir = std::filesystem::current_path() / "resources";
        std::vector<char> code{CCUtilities::readBinaryFile(shaderDir / filename)};
        auto createInfo{m_infos->fillShaderModuleCreateInfo(code)};
        VkShaderModule shaderModule;
        if (vkCreateShaderModule(*(m_device->getVkDevice()), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create shader module!");
        }
        m_shaderModules.push_back(shaderModule);
        return m_shaderModules.size() - 1;
    }

    int VknPipeline::createShaderStage(ShaderStage shaderStage, std::string filename)
    {
        int module_idx{this->createShaderModule(filename)};
        VkShaderStageFlagBits shaderStageFlagBits{};
        switch (shaderStage)
        {
        case VKN_VERTEX_STAGE:
            shaderStageFlagBits = VK_SHADER_STAGE_VERTEX_BIT;
        case VKN_FRAGMENT_STAGE:
            shaderStageFlagBits = VK_SHADER_STAGE_FRAGMENT_BIT;
        default:
            throw std::runtime_error("Shader stage not recognized.");
        }
        m_shaderStageInfos.push_back(
            m_infos->fillShaderStageCreateInfo(m_shaderModules[module_idx], shaderStageFlagBits));
        return m_shaderStageInfos.size() - 1;
    }

    std::vector<VkPipelineShaderStageCreateInfo> VknPipeline::getShaderStageInfos(std::vector<int> idxs)
    {
        std::vector<VkPipelineShaderStageCreateInfo> infos;
        for (int idx : idxs)
            infos.push_back(m_shaderStageInfos[idx]);
        return infos;
    }
}