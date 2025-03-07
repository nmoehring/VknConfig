#pragma once

#include <vulkan/vulkan.h>

#include "VknInfos.hpp"
#include "VknResult.hpp"

namespace vkn
{
    class VknFramebuffer
    {
    public:
        VknFramebuffer();
        VknFramebuffer(uint32_t deviceIdx, uint32_t renderpassIdx, uint32_t framebufferIdx,
                       VknResultArchive *archive, VknInfos *infos, VkRenderPass *renderpass);

        void fillDimensions(uint32_t width, uint32_t height);
        void fillNumLayers(uint32_t numLayers);
        void setCreateFlags(VkFramebufferCreateFlags createFlags);
        void fillFramebufferCreateInfo();
        void createFramebuffer();
        void setAttachments(std::vector<VkImageView> *vkImageViews);

    private:
        // Filled in ctor
        uint32_t m_deviceIdx;
        uint32_t m_renderpassIdx;
        uint32_t m_framebufferIdx;
        VkDevice *m_vkDevice;
        const bool *m_vkDeviceCreated;
        bool m_placeholder;
        VknInfos *m_infos;
        VknResultArchive *m_archive;
        VkRenderPass *m_renderpass;

        // Fill defaults here
        VkFramebuffer m_buffer{VkFramebuffer{}};

        std::vector<VkImageView> *m_attachments{};
        uint32_t m_width{800};
        uint32_t m_height{600};
        uint32_t m_numLayers{1};
        VkFramebufferCreateFlags m_createFlags{0};

        bool m_filledCreateInfo{false};
        bool m_createdFramebuffer{false};
        bool m_attachmentsSet{false};
    };
}