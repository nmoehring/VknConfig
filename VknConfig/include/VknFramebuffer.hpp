#pragma once

#include <vulkan/vulkan.h>

#include "VknInfos.hpp"
#include "VknResult.hpp"

namespace vkn
{
    class VknFramebuffer
    {
    public:
        VknFramebuffer() = default;
        VknFramebuffer(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        void fillDimensions(uint32_t width, uint32_t height);
        void fillNumLayers(uint32_t numLayers);
        void setCreateFlags(VkFramebufferCreateFlags createFlags);
        void fillFramebufferCreateInfo();
        void createFramebuffer();
        void setAttachments(std::vector<VkImageView> *vkImageViews);

    private:
        // Engine
        VknEngine *m_engine{nullptr};
        VknIdxs m_relIdxs{};
        VknIdxs m_absIdxs{};
        VknInfos *m_infos{nullptr};

        // Wrapped object
        VkFramebuffer m_buffer{VkFramebuffer{}};

        // Params
        std::vector<VkImageView> *m_attachments{};
        uint32_t m_width{800};
        uint32_t m_height{600};
        uint32_t m_numLayers{1};
        VkFramebufferCreateFlags m_createFlags{0};

        // State
        bool m_filledCreateInfo{false};
        bool m_createdFramebuffer{false};
        bool m_attachmentsSet{false};
    };
}