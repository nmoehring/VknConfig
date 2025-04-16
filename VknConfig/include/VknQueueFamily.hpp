/**
 * @file VknQueueFamily.hpp
 * @brief Represents a Vulkan queue family and its properties.
 *
 * VknQueueFamily is a hierarchy-bound leaf class within the VknConfig project.
 * It is used by VknPhysicalDevice to represent a Vulkan queue family.
 * VknQueueFamily does not depend on any other Vkn classes.
 * It does not have any classes that depend on it.
 *
 * Hierarchy Graph:
 * [VknConfig] (Top-Level)
 *     |
 *     +-- [VknDevice]
 *         |
 *         +-- [VknPhysicalDevice]
 *         |   |
 *         |   +-- [VknQueueFamily] ^ / \ <<=== YOU ARE HERE
 *         |
 *         +-- [VknSwapchain]
 *         |   |
 *         |   +-- [VknImageView] ^ / \
 *         |
 *         +-- [VknRenderpass]
 *             |
 *             +-- [VknFramebuffer] ^ / \
 *             |
 *             +-- [VknPipeline]
 *                 |
 *                 +-- [VknPipelineLayout]
 *                 |   |
 *                 |   +-- [VknDescriptorSetLayout]
 *                 |
 *                 +-- [VknVertexInputState] ^ / \
 *                 +-- [VknInputAssemblyState] ^ / \
 *                 +-- [VknMultisampleState] ^ / \
 *                 +-- [VknRasterizationState] ^ / \
 *                 +-- [VknShaderStage] ^ / \
 *                 +-- [VknViewportState] ^ / \
 *
 * [VknEngine] (Free/Top-Level)
 * [VknInfos] (Free/Top-Level)
 * [VknResult] (Free/Top-Level)
 */

#pragma once

#include <stdexcept>

#include <vulkan/vulkan.h>

#include "VknResult.hpp"
#include "VknInfos.hpp"

namespace vkn
{
    class VknQueueFamily
    {
    public:
        // Overloads
        VknQueueFamily() = default;
        VknQueueFamily(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        // Config
        void setNumSelected(int num);
        void setMemoryProtection();

        // Getters
        int getNumSelected();
        uint32_t getNumAvailable();
        VkDeviceQueueCreateFlags getFlags();
        bool supportsGraphics();
        bool supportsCompute();
        bool supportsTransfer();
        bool supportsSparseBinding();
        bool supportsMemoryProtection();

    private:
        // Engine
        VknEngine *m_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Params
        VkDeviceQueueCreateFlags m_createFlags{0};

        // State
        int m_numSelected{0};
    };
}