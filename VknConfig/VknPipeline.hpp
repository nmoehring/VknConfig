#pragma once

#include <unordered_map>

#include "VknDevice.hpp"

namespace vkn
{
    enum ShaderStage
    {
        VKN_VERTEX_STAGE = VK_SHADER_STAGE_VERTEX_BIT,
        VKN_FRAGMENT_STAGE = VK_SHADER_STAGE_FRAGMENT_BIT
    };

    class VknPipeline
    {
    public:
        VknPipeline();
        VknPipeline(VknDevice *dev, VknInfos *infos, VknResultArchive *archive);
        ~VknPipeline();
        int createShaderStage(ShaderStage, std::string);
        void destroy();

        void createLayout(int idx,
                          std::vector<VkDescriptorSetLayout> layouts = std::vector<VkDescriptorSetLayout>{},
                          std::vector<VkPushConstantRange> ranges = std::vector<VkPushConstantRange>{});
        std::vector<VkPipelineShaderStageCreateInfo> getShaderStageInfos(std::vector<int> idxs);
        int createPipeline(VkGraphicsPipelineCreateInfo pipelineCreateInfo);

    private:
        VknDevice *m_device{nullptr};
        VknInfos *m_infos{nullptr};
        VknResultArchive *m_archive{nullptr};
        std::vector<VkGraphicsPipelineCreateInfo> m_pipelineCreateInfos;
        std::vector<VkPipeline> m_pipelines{};
        std::vector<VkPipelineLayout> m_layouts{};
        std::vector<VkShaderModule> m_shaderModules{};
        std::vector<VkPipelineShaderStageCreateInfo> m_shaderStageInfos{};
        bool m_destroyed = false;

        int createShaderModule(const std::string &filename);

        void setVertexInput();
        void setInputAssembly();
        void setTessellation();
        void setViewport();
        void setRasterization();
        void setMultisampling();
        void setDepthStencil();
        void setColorBlend();
    };

}