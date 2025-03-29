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
 *     +-- [VknDevice] (Hierarchy-Bound)
 *         |
 *         +-- [VknPhysicalDevice] (Hierarchy-Bound)
 *         |   |
 *         |   +-- [VknQueueFamily] (Hierarchy-Bound Leaf) <<=== YOU ARE HERE
 *         |
 *         +-- [VknSwapchain] (Hierarchy-Bound)
 *         |   |
 *         |   +-- [VknImageView] (Hierarchy-Bound Leaf)
 *         |
 *         +-- [VknRenderpass] (Hierarchy-Bound)
 *             |
 *             +-- [VknPipeline] (Hierarchy-Bound)
 *                 |
 *                 +-- [VknVertexInputState] (Hierarchy-Bound Leaf)
 *                 +-- [VknInputAssemblyState] (Hierarchy-Bound Leaf)
 *                 +-- [VknMultisampleState] (Hierarchy-Bound Leaf)
 *                 +-- [VknRasterizationState] (Hierarchy-Bound Leaf)
 *                 +-- [VknShaderStage] (Hierarchy-Bound Leaf)
 *                 +-- [VknViewportState] (Hierarchy-Bound Leaf)
 *
 * [VknEngine] (Free/Top-Level)
 * [VknInfos] (Free/Top-Level)
 * [VknResult] (Free/Top-Level)
 */

#pragma once

#include "VknResult.hpp"
#include "VknInfos.hpp"

namespace vkn
{
    class VknQueueFamily
    {
    public:
        VknQueueFamily() = default;
        VknQueueFamily(VkQueueFamilyProperties properties);

        bool supportsGraphics();
        bool supportsCompute();
        bool supportsTransfer();
        bool supportsSparseBinding();
        bool supportsMemoryProtection();
        int getNumSelected();
        void setNumSelected(int num);
        uint32_t getNumAvailable();

    private:
        // Members
        VkQueueFamilyProperties m_properties{};

        // State
        int m_numSelected{0};
    };
}