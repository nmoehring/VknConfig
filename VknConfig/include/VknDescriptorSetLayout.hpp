/**
 * @file VknDescriptorSetLayout.hpp
 * @brief Manages a Vulkan VkDescriptorSetLayout object.
 *
 * VknDescriptorSetLayout is a hierarchy-bound leaf class within the VknConfig project.
 * It is used by VknPipelineLayout to define the layout of bindings within a
 * single descriptor set.
 * VknDescriptorSetLayout depends on VknEngine, VknInfos, and VknIdxs.
 * It does not have any classes that depend on it.
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
 *                 +-- [VknPipelineLayout]
 *                 |   |
 *                 |   +-- [VknDescriptorSetLayout] ^ / \ <<=== YOU ARE HERE
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

#include "VknEngine.hpp"
#include "VknInfos.hpp"
#include "VknResult.hpp"

namespace vkn
{
    class VknDescriptorSetLayout
    {
    public:
        // Overloads
        VknDescriptorSetLayout() = default;
        VknDescriptorSetLayout(
            VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        // Config
        void addBinding(
            uint32_t binding, VkDescriptorType descriptorType, uint32_t descriptorCount,
            VkShaderStageFlags stageFlags, const VkSampler *pImmutableSamplers = VK_NULL_HANDLE);
        void setCreateFlags(VkDescriptorSetLayoutCreateFlags createFlags) { m_createFlags = createFlags; }

        // Create
        void createDescriptorSetLayout();

        // Get
        VkDescriptorSetLayout *getVkDescriptorSetLayout();

    private:
        // Engine
        VknEngine *m_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Params
        VknVector<VkDescriptorSetLayoutBinding> m_bindings{};
        VkDescriptorSetLayoutCreateFlags m_createFlags{0};

        // State
        bool m_createdDescriptorSetLayout{false};

        uint32_t getStartIdx();
    };
} // namespace vkn