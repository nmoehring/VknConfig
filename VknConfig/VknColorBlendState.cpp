#include "include/VknColorBlendState.hpp"

namespace vkn
{
    VknColorBlendState::VknColorBlendState(
        VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos)
        : m_engine{engine}, m_relIdxs{relIdxs}, m_absIdxs{absIdxs}, m_infos{infos}
    {
        // Initialize with one default attachment state (overwrite)
        setDefaultAttachmentState();
    }

    void VknColorBlendState::setLogicOpEnable(VkBool32 enable)
    {
        m_logicOpEnable = enable;
    }

    void VknColorBlendState::setLogicOp(VkLogicOp logicOp)
    {
        m_logicOp = logicOp;
    }

    void VknColorBlendState::setBlendConstants(float r, float g, float b, float a)
    {
        m_blendConstants[0] = r;
        m_blendConstants[1] = g;
        m_blendConstants[2] = b;
        m_blendConstants[3] = a;
    }

    void VknColorBlendState::setDefaultAttachmentState(
        VkBool32 blendEnable, VkColorComponentFlags colorWriteMask,
        VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor, VkBlendOp colorBlendOp,
        VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor, VkBlendOp alphaBlendOp)
    {
        m_attachmentBlendStates.clear(); // Assuming only one for default
        VkPipelineColorBlendAttachmentState defaultState{};
        defaultState.blendEnable = blendEnable;
        defaultState.srcColorBlendFactor = srcColorBlendFactor;
        defaultState.dstColorBlendFactor = dstColorBlendFactor;
        defaultState.colorBlendOp = colorBlendOp;
        defaultState.srcAlphaBlendFactor = srcAlphaBlendFactor;
        defaultState.dstAlphaBlendFactor = dstAlphaBlendFactor;
        defaultState.alphaBlendOp = alphaBlendOp;
        defaultState.colorWriteMask = colorWriteMask;
        m_attachmentBlendStates.append(defaultState);
    }

    void VknColorBlendState::_fileColorBlendStateCreateInfo()
    {
        if (m_filedCreateInfo)
            throw std::runtime_error("Color blend state create info already filed.");

        // This assumes m_attachmentBlendStates is already populated (e.g., by setDefaultAttachmentState or other methods)
        // You'll need to ensure the number of attachments here matches the subpass's color attachment count.
        // For now, this is a simplified version.
        if (m_attachmentBlendStates.isEmpty())
        {
            // Add a default if nothing was configured, to prevent pAttachments from being null if attachmentCount > 0
            setDefaultAttachmentState();
        }

        m_infos->fileColorBlendStateCreateInfo(
            m_relIdxs.get<VkDevice>(),
            m_relIdxs.get<VkRenderPass>(),
            m_relIdxs.get<VkPipeline>(), // This is the subpass index for pipeline
            m_logicOp,
            m_attachmentBlendStates, // Pass the VknVector
            m_blendConstants,
            m_logicOpEnable,
            0); // flags for VkPipelineColorBlendStateCreateInfo (usually 0)
        m_filedCreateInfo = true;
    }
}
