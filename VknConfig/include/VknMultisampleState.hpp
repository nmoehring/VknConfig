#pragma once

#include <vulkan/vulkan.h>
#include "VknInfos.hpp"
#include "VknEngine.hpp"

namespace vkn
{
    class VknMultisampleState
    {
    public:
        VknMultisampleState() = delete;
        VknMultisampleState(
            VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        void setRasterizationSamples(VkSampleCountFlagBits rasterizationSamples);
        void setSampleShadingEnable(VkBool32 sampleShadingEnable);
        void setMinSampleShading(float minSampleShading);
        void setSampleMask(VkSampleMask sampleMask);
        void setAlphaToCoverageEnable(VkBool32 alphaToCoverageEnable);
        void setAlphaToOneEnable(VkBool32 alphaToOneEnable);

        void fillMultisampleStateCreateInfo();

    private:
        // Engine
        VknEngine *m_engine{nullptr};
        VknIdxs m_relIdxs{};
        VknIdxs m_absIdxs{};
        VknInfos *m_infos{nullptr};

        // Params
        VkSampleCountFlagBits m_rasterizationSamples{VK_SAMPLE_COUNT_1_BIT};
        VkBool32 m_sampleShadingEnable{VK_FALSE};
        float m_minSampleShading{0.0f};
        VkSampleMask m_sampleMask{0};
        VkBool32 m_alphaToCoverageEnable{VK_FALSE};
        VkBool32 m_alphaToOneEnable{VK_FALSE};

        // State
        bool m_filled{false};
    };
}