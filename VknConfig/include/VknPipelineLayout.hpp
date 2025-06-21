/**
 * @file VknPipelineLayout.hpp
 * @brief Manages a Vulkan VkPipelineLayout object.
 *
 * VknPipelineLayout is a hierarchy-bound class within the VknConfig project.
 * It is used by VknPipeline to define the layout of resources (descriptor sets
 * and push constants) accessible by a pipeline.
 * VknPipelineLayout depends on VknEngine, VknInfos, VknIdxs, and VknDescriptorSetLayout.
 * VknPipelineLayout is a child of VknPipeline.
 *
 * Hierarchy Graph:
 * [VknConfig] (Top-Level)
 *     |
 *     +-- [VknDevice]
 *         |
 *         +-- [VknPhysicalDevice]
 *         |   |
 *         |   +-- [VknQueueFamily] ^ / \
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
 *                 +-- [VknPipelineLayout] <<=== YOU ARE HERE
 *                 |   |
 *                 |   +-- [VknDescriptorSetLayout] ^ / \
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
 * [VknImage] (Free/Top-Level)
 */

#pragma once

#include "VknData.hpp"
#include "VknObject.hpp"
#include "VknDescriptorSetLayout.hpp"

namespace vkn
{
    class VknPipelineLayout : public VknObject
    {
    public:
        VknPipelineLayout() = default;
        VknPipelineLayout(VknIdxs relIdxs, VknIdxs absIdxs);

        // Vkn Members
        VknDescriptorSetLayout *addDescriptorSetLayout();

        // Config
        void addPushConstantRange(VkShaderStageFlags stageFlags = 0, uint32_t offset = 0,
                                  uint32_t size = 0);
        void setCreateFlags(VkPipelineLayoutCreateFlags flags);

        // Create
        void _createPipelineLayout();

        // Get
        VkPipelineLayout *getVkLayout();

    private:
        // Members
        std::list<VknDescriptorSetLayout> m_descriptorSetLayouts{};
        VknVector<VkPushConstantRange> m_pushConstantRanges{};

        // Params
        VkPipelineLayoutCreateFlags m_createFlags{0};

        // State
        bool m_createdPipelineLayout{false};
        uint32_t m_descriptorSetLayoutStartIdx{0};
        VknInstanceLock<VknPipelineLayout> m_instanceLock;
    };
}