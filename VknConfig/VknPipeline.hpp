#pragma once

#include "VknDevice.hpp"

namespace vkn
{
    class VknPipeline
    {
    public:
        VknPipeline();
        VknPipeline(VknDevice &dev, VknInfos &infos);
        ~VknPipeline();

    private:
        VknDevice *m_device{nullptr};
        VknInfos *m_infos{nullptr};
        std::vector<VkPipeline> m_pipelines{};
        std::vector<VkPipelineLayout> m_layouts{};

        void setVertexInput();
        void setInputAssembly();
        void setTessellation();
        void setViewport();
        void setRasterization();
        void setMultisampling();
        void setDepthStencil();
        void setColorBlend();

        void createLayout(int idx,
                          std::vector<VkDescriptorSetLayout> layouts = std::vector<VkDescriptorSetLayout>{},
                          std::vector<VkPushConstantRange> ranges = std::vector<VkPushConstantRange>{});
        void createPipeline(int idx, std::vector<VkPipelineShaderStageCreateInfo> &stages,
                            VkPipelineLayout layout, VkRenderPass renderPass, uint32_t subpass,
                            VkPipeline basePipelineHandle, int32_t basePipelineIndex,
                            VkPipelineCreateFlags flags = VkPipelineCreateFlags{});
    };

}