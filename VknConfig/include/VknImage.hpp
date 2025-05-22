/**
 * @file VknImage.hpp
 * @brief Manages a Vulkan VkImage object.
 *
 * VknImage is a free/top-level class within the VknConfig project.
 * It is responsible for configuring and creating a Vulkan VkImage resource.
 * The created VkImage can then be used, for example, by a VknImageView.
 * VknImage depends on VknEngine, VknInfos, and VknIdxs.
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
 * [VknImage] (Free/Top-Level) <<=== YOU ARE HERE
 */

#pragma once

#include "VknEngine.hpp"
#include "VknInfos.hpp"

namespace vkn
{
    class VknImage
    {
    public:
        // Overloads
        VknImage() = default;
        VknImage(VknEngine *engine, VknIdxs relIdxs, VknIdxs absIdxs, VknInfos *infos);

        // Create
        void createImage();
        void demolishImage();
        void addMemory();
        void allocateAndBindMemory(
            VkMemoryPropertyFlags requiredMemoryProperties =
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        void deallocateMemory();
        // uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties); // Removed member function

        // Config
        void setImageType(VkImageType imageType);
        void setFormat(VkFormat format);
        void setExtent(VkExtent3D extent);
        void setMipLevels(uint32_t mipLevels);
        void setArrayLayers(uint32_t arrayLayers);
        void setSamples(VkSampleCountFlagBits samples);
        void setTiling(VkImageTiling tiling);
        void setUsage(VkImageUsageFlags usage);
        void setInitialLayout(VkImageLayout initialLayout);
        void setFlags(VkImageCreateFlags flags);

        // Get
        VkImage *getVkImage();
        VkImageUsageFlags &getUsage() { return m_usage; }

    private:
        // Engine
        VknEngine *m_engine;
        VknIdxs m_relIdxs;
        VknIdxs m_absIdxs;
        VknInfos *m_infos;

        // Members
        VkDeviceMemory *m_deviceMemory = nullptr;

        // Params
        VkImageType m_imageType = VK_IMAGE_TYPE_2D;
        VkFormat m_format = VK_FORMAT_R8G8B8A8_UNORM;
        VkExtent3D m_extent = VkExtent3D{0, 0, 1};
        uint32_t m_mipLevels = 1;
        uint32_t m_arrayLayers = 1;
        VkSampleCountFlagBits m_samples = VK_SAMPLE_COUNT_1_BIT;
        VkImageTiling m_tiling = VK_IMAGE_TILING_OPTIMAL;
        VkImageUsageFlags m_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        VkImageLayout m_initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageCreateFlags m_flags = 0;

        // State
        bool m_createdVkImage{false};
        bool m_memoryBound{false};
        bool m_memoryAdded{false};
        VknInstanceLock<VknImage> m_instanceLock;

        uint32_t findSuitableMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    };

}