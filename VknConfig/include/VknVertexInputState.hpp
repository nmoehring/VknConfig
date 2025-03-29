#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "VknInfos.hpp"

namespace vkn
{
    class VknVertexInputState
    {
    public:
        VknVertexInputState() = default;
        VknVertexInputState(
            VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        void fillVertexBindingDescription(uint32_t binding = 0, uint32_t stride = 0,
                                          VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);
        void fillVertexAttributeDescription(uint32_t binding = 0, uint32_t location = 0,
                                            VkFormat format = VK_FORMAT_R32G32_SFLOAT, uint32_t offset = 0);
        void fillVertexInputStateCreateInfo();

    private:
        // Engine
        VknEngine *m_engine{nullptr};
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos{nullptr};

        // State
        uint32_t m_numBindings{0};
        uint32_t m_numAttributes{0};
        bool m_attributesFilled{false};
        bool m_bindingsFilled{false};
        bool m_filled{false};
    };
} // namespace vkn