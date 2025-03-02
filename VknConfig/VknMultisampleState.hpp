#pragma once

#include <vulkan/vulkan.h>
#include "VknInfos.hpp"

namespace vkn
{
    class VknMultisampleState
    {
    public:
        VknMultisampleState();
        VknMultisampleState(uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx, VknInfos *infos);
        //~VknMultisampleState();

        void setRasterizationSamples(VkSampleCountFlagBits rasterizationSamples);
        void setSampleShadingEnable(VkBool32 sampleShadingEnable);
        void setMinSampleShading(float minSampleShading);
        void setSampleMask(VkSampleMask sampleMask);
        void setAlphaToCoverageEnable(VkBool32 alphaToCoverageEnable);
        void setAlphaToOneEnable(VkBool32 alphaToOneEnable);

        void fillMultisampleStateCreateInfo();

    private:
        uint32_t m_deviceIdx{};
        uint32_t m_renderPassIdx{};
        uint32_t m_subpassIdx{};

        VknInfos *m_infos{nullptr};

        VkPipelineMultisampleStateCreateInfo *m_createInfo{VK_NULL_HANDLE};
        VkSampleCountFlagBits m_rasterizationSamples{VK_SAMPLE_COUNT_1_BIT};
        VkBool32 m_sampleShadingEnable{VK_FALSE};
        float m_minSampleShading{0.0f};
        VkSampleMask m_sampleMask{0};
        VkBool32 m_alphaToCoverageEnable{VK_FALSE};
        VkBool32 m_alphaToOneEnable{VK_FALSE};

        bool m_filled{false};
    };
}