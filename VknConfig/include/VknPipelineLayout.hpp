#pragma once

#include "VknCommon.hpp"
#include "VknEngine.hpp"
#include "VknInfos.hpp"
#include "VknDescriptorSetLayout.hpp"

namespace vkn
{
    class VknPipelineLayout
    {
    public:
        VknPipelineLayout() = default;
        VknPipelineLayout(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        // Add Vkn Members
        VknDescriptorSetLayout *addDescriptorSetLayout();

        // Config
        void addPushConstantRange(VkShaderStageFlags stageFlags = 0, uint32_t offset = 0,
                                  uint32_t size = 0);

        // Create
        void createPipelineLayout();

        // Get
        VkPipelineLayout *getVkLayout();

    private:
        // Engine
        VknEngine *m_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Params
        std::list<VknDescriptorSetLayout> m_descriptorSetLayouts{};
        std::vector<VkPushConstantRange> m_pushConstantRanges{};
        VkPipelineLayoutCreateFlags m_createFlags{0};

        // State
        bool m_createdPipelineLayout{false};
        uint32_t m_descriptorSetLayoutStartIdx{0};
    };
}