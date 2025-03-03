#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "VknInfos.hpp"

namespace vkn
{
    class VknVertexInputState
    {
    public:
        VknVertexInputState();
        VknVertexInputState(uint32_t deviceIdx, uint32_t renderPassIdx, uint32_t subpassIdx,
                            VknInfos *infos);
        //~VknVertexInputState();

        void fillVertexBindingDescription(uint32_t binding = 0, uint32_t stride = 0,
                                          VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);
        void fillVertexAttributeDescription(uint32_t binding = 0, uint32_t location = 0,
                                            VkFormat format = VK_FORMAT_UNDEFINED, uint32_t offset = 0);
        void fillVertexInputStateCreateInfo();

    private:
        uint32_t m_deviceIdx;
        uint32_t m_renderPassIdx;
        uint32_t m_subpassIdx;
        VknInfos *m_infos;
        bool m_placeholder;

        bool m_filled{false};
        uint32_t m_numBindings{0};
        uint32_t m_numAttributes{0};
    };
} // namespace vkn