#pragma once

#include <vulkan/vulkan.h>
#include "VknInfos.hpp" // For VknEngine, VknIdxs, VknInfos
#include "VknData.hpp"  // For VknVector

namespace vkn
{
    class VknColorBlendState
    {
    public:
        VknColorBlendState() = default;
        VknColorBlendState(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        // Configuration
        void setLogicOpEnable(VkBool32 enable);
        void setLogicOp(VkLogicOp logicOp);
        void setBlendConstants(float r, float g, float b, float a);

        // Per-attachment configuration
        // You'll need to manage a VknVector<VkPipelineColorBlendAttachmentState>
        // and provide methods to add/get/modify these.
        // For simplicity, a common default for one attachment:
        void setDefaultAttachmentState(
            VkBool32 blendEnable = VK_FALSE,
            VkColorComponentFlags colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
            VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            VkBlendOp colorBlendOp = VK_BLEND_OP_ADD,
            VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD);

        void _fileColorBlendStateCreateInfo();

    private:
        // Engine
        VknEngine *m_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Params for VkPipelineColorBlendStateCreateInfo
        VkBool32 m_logicOpEnable{VK_FALSE};
        VkLogicOp m_logicOp{VK_LOGIC_OP_COPY}; // Default if logicOpEnable is true
        float m_blendConstants[4]{0.0f, 0.0f, 0.0f, 0.0f};
        VknVector<VkPipelineColorBlendAttachmentState> m_attachmentBlendStates{};
        VkPipelineColorBlendStateCreateFlags flags{0};

        // State
        bool m_filedCreateInfo{false};
    };
}